/* Fazenda 360 - arquivo unico (libxenon). */
#include <stdio.h>
#include <string.h>

/* ===== game.h ===== */

#define MAP_W 20
#define MAP_H 11
#define DAY_LEN 60.0f   /* segundos por dia */

typedef struct { int up, down, left, right, a, lb, rb, start; } pad_t;

typedef struct {
    int active, x, y, dx, dy, tool, water;
    float cool;
    pad_t prev;
} player_t;

typedef struct {
    unsigned char tile[MAP_H][MAP_W];
    unsigned char crop[MAP_H][MAP_W];
    int gold, seeds, crops, day;
    float daytime;
    char msg[64];
    float msg_timer;
    player_t p[2];
} game_t;

extern game_t G;

void game_init(void);
void game_update(const pad_t pads[2], float dt);
void game_render(void);
void game_next_day(void);   /* exposto para testes */


/* ===== plat.h ===== */
/* Camada de plataforma: o jogo so conhece estas funcoes. */
void plat_rect(int x, int y, int w, int h, int r, int g, int b);
void plat_text(int row, const char *s);   /* linha de texto do HUD (0..2) */
void plat_present(void);                  /* envia o frame para a tela   */
int  plat_screen_w(void);
int  plat_screen_h(void);
/* font.c (portavel): texto 5x7 desenhado com plat_rect */
void font_draw(int x, int y, int scale, const char *s, int r, int g, int b);

/* ===== game.c ===== */
#include <stdio.h>
#include <string.h>

enum { T_GRASS, T_SOIL, T_WET, T_WATER, T_PATH, T_HOUSE, T_SHOP, T_BOX, T_TREE };
enum { TOOL_HOE, TOOL_CAN, TOOL_SEED, TOOL_HARVEST, TOOL_COUNT };

#define HUD_H 48
#define WATER_MAX 15
#define SEED_PACK 5
#define SEED_PRICE 10
#define CROP_PRICE 15
#define WIN_GOLD 500

static const char *LAYOUT[MAP_H] = {
    "TTTTTTTTTTTTTTTTTTTT",
    "T.HH..S..B.........T",
    "T.pp..pppp.........T",
    "T..p...........ww..T",
    "T..p...........ww..T",
    "T..pppppp..........T",
    "T.......p..........T",
    "T.......p......T...T",
    "T.......pppp.......T",
    "T..................T",
    "TTTTTTTTTTTTTTTTTTTT",
};
static const char *TOOLS[TOOL_COUNT] = { "Enxada", "Regador", "Sementes", "Colher" };

game_t G;
static unsigned char dirty[MAP_H][MAP_W];
static int tile_px, org_x, org_y, layout_ok;
static char last_hud[3][96];

/* ------------------------------------------------------------ logica */

static void mark(int x, int y) {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) dirty[y][x] = 1;
}
static void mark_player(const player_t *p) {
    mark(p->x, p->y);
    mark(p->x + p->dx, p->y + p->dy);
}
static void say(const char *s, float secs) {
    snprintf(G.msg, sizeof G.msg, "%s", s);
    G.msg_timer = secs;
}
static int walkable(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return 0;
    int t = G.tile[y][x];
    return t == T_GRASS || t == T_SOIL || t == T_WET || t == T_PATH;
}
static int occupied(int x, int y, int self) {
    for (int i = 0; i < 2; i++)
        if (i != self && G.p[i].active && G.p[i].x == x && G.p[i].y == y) return 1;
    return 0;
}

void game_init(void) {
    memset(&G, 0, sizeof G);
    memset(dirty, 1, sizeof dirty);
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++) {
            switch (LAYOUT[y][x]) {
                case 'T': G.tile[y][x] = T_TREE;  break;
                case 'H': G.tile[y][x] = T_HOUSE; break;
                case 'S': G.tile[y][x] = T_SHOP;  break;
                case 'B': G.tile[y][x] = T_BOX;   break;
                case 'p': G.tile[y][x] = T_PATH;  break;
                case 'w': G.tile[y][x] = T_WATER; break;
                default:  G.tile[y][x] = T_GRASS; break;
            }
        }
    G.gold = 50; G.seeds = 10; G.day = 1;
    G.p[0] = (player_t){ .active = 1, .x = 4, .y = 5, .dy = 1, .water = WATER_MAX };
    G.p[1] = (player_t){ .active = 0, .x = 5, .y = 5, .dy = 1, .water = WATER_MAX };
    layout_ok = 0;
    memset(last_hud, 0, sizeof last_hud);
    say("Bem-vindo! Compre sementes na barraca (A).", 6.0f);
}

void game_next_day(void) {
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++) {
            if (G.tile[y][x] == T_WET) {
                if (G.crop[y][x] >= 1 && G.crop[y][x] < 4) G.crop[y][x]++;
                G.tile[y][x] = T_SOIL;
            }
        }
    G.day++;
    G.daytime = 0;
    for (int i = 0; i < 2; i++) G.p[i].water = WATER_MAX;
    memset(dirty, 1, sizeof dirty);
    char b[64]; snprintf(b, sizeof b, "Amanheceu! Dia %d. Regador cheio.", G.day);
    say(b, 5.0f);
}

static void act(int i) {
    player_t *P = &G.p[i];
    int fx = P->x + P->dx, fy = P->y + P->dy;
    if (fx < 0 || fx >= MAP_W || fy < 0 || fy >= MAP_H) return;
    int t = G.tile[fy][fx];

    if (t == T_SHOP) {
        if (G.gold >= SEED_PRICE) {
            G.gold -= SEED_PRICE; G.seeds += SEED_PACK;
            say("Comprou 5 sementes por 10 de ouro.", 3.0f);
        } else say("Ouro insuficiente (10 por 5 sementes).", 3.0f);
        return;
    }
    if (t == T_BOX) {
        if (G.crops > 0) {
            char b[64]; snprintf(b, sizeof b, "Vendeu %d colheitas por %d de ouro!", G.crops, G.crops * CROP_PRICE);
            G.gold += G.crops * CROP_PRICE; G.crops = 0; say(b, 4.0f);
            if (G.gold >= WIN_GOLD) say("A fazenda prosperou! Continue jogando.", 8.0f);
        } else say("Nada para vender.", 2.0f);
        return;
    }
    switch (P->tool) {
    case TOOL_HOE:
        if (t == T_GRASS) { G.tile[fy][fx] = T_SOIL; mark(fx, fy); }
        break;
    case TOOL_CAN:
        if (t == T_WATER) { P->water = WATER_MAX; say("Regador cheio.", 2.0f); }
        else if ((t == T_SOIL) && P->water > 0) { G.tile[fy][fx] = T_WET; P->water--; mark(fx, fy); }
        else if (t == T_SOIL) say("Regador vazio! Encha no lago.", 3.0f);
        break;
    case TOOL_SEED:
        if ((t == T_SOIL || t == T_WET) && G.crop[fy][fx] == 0) {
            if (G.seeds > 0) { G.seeds--; G.crop[fy][fx] = 1; mark(fx, fy); }
            else say("Sem sementes! Compre na barraca.", 3.0f);
        }
        break;
    case TOOL_HARVEST:
        if (G.crop[fy][fx] == 4) { G.crop[fy][fx] = 0; G.crops++; mark(fx, fy); }
        break;
    }
}

void game_update(const pad_t pads[2], float dt) {
    for (int i = 0; i < 2; i++) {
        player_t *P = &G.p[i];
        const pad_t *in = &pads[i];
        if (!P->active) {
            if (in->start && !P->prev.start && !occupied(P->x, P->y, i)) {
                P->active = 1; mark_player(P); say("Jogador 2 entrou!", 3.0f);
            }
            P->prev = *in; continue;
        }
        int mx = in->right - in->left, my = in->down - in->up;
        if (mx || my) {
            P->cool -= dt;
            if (P->cool <= 0) {
                mark_player(P);
                if (mx) { P->dx = mx > 0 ? 1 : -1; P->dy = 0; }
                else    { P->dx = 0; P->dy = my > 0 ? 1 : -1; }
                int nx = P->x + P->dx, ny = P->y + P->dy;
                if (walkable(nx, ny) && !occupied(nx, ny, i)) { P->x = nx; P->y = ny; }
                mark_player(P);
                P->cool = 0.14f;
            }
        } else P->cool = 0;

        if (in->a  && !P->prev.a)  act(i);
        if (in->rb && !P->prev.rb) { P->tool = (P->tool + 1) % TOOL_COUNT; mark_player(P); }
        if (in->lb && !P->prev.lb) { P->tool = (P->tool + TOOL_COUNT - 1) % TOOL_COUNT; mark_player(P); }
        P->prev = *in;
    }
    G.daytime += dt;
    if (G.daytime >= DAY_LEN) game_next_day();
    if (G.msg_timer > 0) { G.msg_timer -= dt; if (G.msg_timer <= 0) G.msg[0] = 0; }
}

/* ------------------------------------------------------------ desenho */

#define R(x, y, w, h, r, g, b) plat_rect(px + (x) * u, py + (y) * u, (w) * u, (h) * u, r, g, b)

static void draw_grass(int px, int py, int u) {
    R(0, 0, 16, 16, 88, 170, 70);
    R(3, 4, 1, 1, 70, 150, 55); R(10, 9, 1, 1, 70, 150, 55);
    R(12, 3, 1, 1, 70, 150, 55); R(6, 13, 1, 1, 70, 150, 55);
}
static void draw_soil(int px, int py, int u, int wet) {
    int r = wet ? 80 : 120, g = wet ? 52 : 80, b = wet ? 33 : 50;
    int r2 = wet ? 60 : 95, g2 = wet ? 40 : 62, b2 = wet ? 26 : 38;
    R(0, 0, 16, 16, r, g, b);
    R(0, 4, 16, 1, r2, g2, b2); R(0, 8, 16, 1, r2, g2, b2); R(0, 12, 16, 1, r2, g2, b2);
}
static void draw_crop(int px, int py, int u, int s) {
    switch (s) {
    case 1: R(7, 9, 2, 2, 150, 110, 70); break;
    case 2: R(7, 8, 2, 5, 90, 200, 70); R(5, 7, 2, 2, 90, 200, 70); R(9, 7, 2, 2, 90, 200, 70); break;
    case 3: R(7, 5, 2, 8, 60, 180, 60); R(4, 5, 3, 3, 60, 180, 60); R(9, 5, 3, 3, 60, 180, 60);
            R(5, 8, 2, 2, 60, 180, 60); R(9, 8, 2, 2, 60, 180, 60); break;
    case 4: R(7, 5, 2, 8, 60, 180, 60); R(4, 4, 3, 3, 230, 50, 50); R(9, 4, 3, 3, 230, 50, 50);
            R(6, 8, 4, 4, 230, 50, 50); break;
    }
}
static void draw_tile(int x, int y) {
    int u = tile_px / 16, px = org_x + x * tile_px, py = org_y + y * tile_px;
    switch (G.tile[y][x]) {
    case T_GRASS: draw_grass(px, py, u); break;
    case T_SOIL:  draw_soil(px, py, u, 0); draw_crop(px, py, u, G.crop[y][x]); break;
    case T_WET:   draw_soil(px, py, u, 1); draw_crop(px, py, u, G.crop[y][x]); break;
    case T_PATH:  R(0, 0, 16, 16, 200, 180, 130); R(3, 5, 1, 1, 180, 160, 110); R(11, 10, 1, 1, 180, 160, 110); break;
    case T_WATER: R(0, 0, 16, 16, 50, 110, 200); R(3, 5, 5, 1, 110, 170, 240); R(8, 11, 5, 1, 110, 170, 240); break;
    case T_TREE:  draw_grass(px, py, u); R(7, 10, 2, 5, 100, 65, 35); R(3, 2, 10, 9, 30, 110, 40); R(5, 3, 4, 3, 50, 140, 55); break;
    case T_HOUSE: draw_grass(px, py, u); R(1, 5, 14, 10, 200, 170, 130); R(0, 2, 16, 4, 170, 50, 40);
                  R(6, 10, 4, 5, 110, 70, 40); R(2, 7, 3, 3, 150, 200, 240); R(11, 7, 3, 3, 150, 200, 240); break;
    case T_SHOP:  draw_grass(px, py, u); R(1, 8, 14, 7, 180, 120, 60);
                  R(0, 3, 4, 4, 220, 60, 60); R(4, 3, 4, 4, 240, 240, 240);
                  R(8, 3, 4, 4, 220, 60, 60); R(12, 3, 4, 4, 240, 240, 240); R(6, 9, 4, 4, 230, 210, 120); break;
    case T_BOX:   draw_grass(px, py, u); R(2, 6, 12, 8, 150, 100, 50); R(2, 6, 12, 2, 120, 80, 40); R(7, 6, 2, 8, 230, 200, 60); break;
    }
    for (int i = 0; i < 2; i++) {
        const player_t *P = &G.p[i];
        if (!P->active) continue;
        if (P->x == x && P->y == y) {
            int sr = i ? 240 : 60, sg = i ? 150 : 120, sb = i ? 40 : 230;
            R(5, 1, 6, 2, 60, 40, 30); R(5, 2, 6, 5, 240, 210, 170);
            R(4, 7, 8, 6, sr, sg, sb); R(5, 13, 2, 3, 50, 50, 70); R(9, 13, 2, 3, 50, 50, 70);
        }
    }
    for (int i = 0; i < 2; i++) {      /* cursor da tile apontada */
        const player_t *P = &G.p[i];
        if (!P->active || P->x + P->dx != x || P->y + P->dy != y) continue;
        int cr = i ? 255 : 255, cg = i ? 170 : 255, cb = i ? 40 : 255;
        R(0, 0, 16, 1, cr, cg, cb); R(0, 15, 16, 1, cr, cg, cb);
        R(0, 0, 1, 16, cr, cg, cb); R(15, 0, 1, 16, cr, cg, cb);
    }
}

static void hud_line(int row, const char *s) {
    if (strncmp(last_hud[row], s, sizeof last_hud[row]) == 0) return;
    snprintf(last_hud[row], sizeof last_hud[row], "%s", s);
    plat_text(row, s);
}

void game_render(void) {
    if (!layout_ok) {
        int sw = plat_screen_w(), sh = plat_screen_h();
        static const int sizes[3] = { 48, 32, 16 };
        tile_px = 16;
        for (int k = 0; k < 3; k++)
            if (MAP_W * sizes[k] <= sw && MAP_H * sizes[k] + HUD_H <= sh) { tile_px = sizes[k]; break; }
        org_x = (sw - MAP_W * tile_px) / 2;
        org_y = HUD_H;
        plat_rect(0, 0, sw, sh, 0, 0, 0);
        memset(dirty, 1, sizeof dirty);
        layout_ok = 1;
    }
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            if (dirty[y][x]) { draw_tile(x, y); dirty[y][x] = 0; }

    char b[96];
    snprintf(b, sizeof b, "Dia %d  Ouro:%d  Sementes:%d  Colheita:%d  Tempo:%d%%",
             G.day, G.gold, G.seeds, G.crops, (int)(G.daytime * 100 / DAY_LEN));
    hud_line(0, b);
    snprintf(b, sizeof b, "J1 %s Agua:%d/%d   %s", TOOLS[G.p[0].tool], G.p[0].water, WATER_MAX,
             G.p[1].active ? "" : "");
    if (G.p[1].active) {
        char c[192]; snprintf(c, sizeof c, "%s | J2 %s Agua:%d/%d", b, TOOLS[G.p[1].tool], G.p[1].water, WATER_MAX);
        hud_line(1, c);
    } else {
        snprintf(b + strlen(b), sizeof b - strlen(b), " | J2: aperte START");
        hud_line(1, b);
    }
    hud_line(2, G.msg);
    plat_present();
}

#undef R

/* ===== font.c ===== */
/* Fonte 5x7 propria (so maiusculas/numeros/pontuacao) desenhada com plat_rect.
 * Independe do console de texto do libxenon, que nao permite posicionar o cursor. */
#include <string.h>

static const char GLYPH_CHARS[] = " !%'()+,-./0123456789:;?|ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static const unsigned char GLYPHS[][7] = {
    {0,0,0,0,0,0,0},                                   /* espaco */
    {4,4,4,4,4,0,4},                                   /* ! */
    {0x18,0x19,2,4,8,0x13,3},                          /* % */
    {4,4,8,0,0,0,0},                                   /* ' */
    {2,4,8,8,8,4,2},                                   /* ( */
    {8,4,2,2,2,4,8},                                   /* ) */
    {0,4,4,0x1F,4,4,0},                                /* + */
    {0,0,0,0,0x0C,4,8},                                /* , */
    {0,0,0,0x1F,0,0,0},                                /* - */
    {0,0,0,0,0,0x0C,0x0C},                             /* . */
    {1,1,2,4,8,0x10,0x10},                             /* / */
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},              /* 0 */
    {4,0x0C,4,4,4,4,0x0E},                             /* 1 */
    {0x0E,0x11,1,2,4,8,0x1F},                          /* 2 */
    {0x1F,2,4,2,1,0x11,0x0E},                          /* 3 */
    {2,6,0x0A,0x12,0x1F,2,2},                          /* 4 */
    {0x1F,0x10,0x1E,1,1,0x11,0x0E},                    /* 5 */
    {6,8,0x10,0x1E,0x11,0x11,0x0E},                    /* 6 */
    {0x1F,1,2,4,8,8,8},                                /* 7 */
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},              /* 8 */
    {0x0E,0x11,0x11,0x0F,1,2,0x0C},                    /* 9 */
    {0,0x0C,0x0C,0,0x0C,0x0C,0},                       /* : */
    {0,0x0C,0x0C,0,0x0C,4,8},                          /* ; */
    {0x0E,0x11,1,2,4,0,4},                             /* ? */
    {4,4,4,4,4,4,4},                                   /* | */
    {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11},              /* A */
    {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E},              /* B */
    {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E},              /* C */
    {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E},              /* D */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F},              /* E */
    {0x1F,0x10,0x10,0x1E,0x10,0x10,0x10},              /* F */
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},              /* G */
    {0x11,0x11,0x11,0x1F,0x11,0x11,0x11},              /* H */
    {0x0E,4,4,4,4,4,0x0E},                             /* I */
    {7,2,2,2,2,0x12,0x0C},                             /* J */
    {0x11,0x12,0x14,0x18,0x14,0x12,0x11},              /* K */
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},              /* L */
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},              /* M */
    {0x11,0x19,0x15,0x13,0x11,0x11,0x11},              /* N */
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},              /* O */
    {0x1E,0x11,0x11,0x1E,0x10,0x10,0x10},              /* P */
    {0x0E,0x11,0x11,0x11,0x15,0x12,0x0D},              /* Q */
    {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11},              /* R */
    {0x0F,0x10,0x10,0x0E,1,1,0x1E},                    /* S */
    {0x1F,4,4,4,4,4,4},                                /* T */
    {0x11,0x11,0x11,0x11,0x11,0x11,0x0E},              /* U */
    {0x11,0x11,0x11,0x11,0x11,0x0A,4},                 /* V */
    {0x11,0x11,0x11,0x15,0x15,0x15,0x0A},              /* W */
    {0x11,0x11,0x0A,4,0x0A,0x11,0x11},                 /* X */
    {0x11,0x11,0x0A,4,4,4,4},                          /* Y */
    {0x1F,1,2,4,8,0x10,0x1F},                          /* Z */
};

void font_draw(int x, int y, int scale, const char *s, int r, int g, int b) {
    for (; *s; s++, x += 6 * scale) {
        char c = *s;
        if (c >= 'a' && c <= 'z') c -= 32;
        const char *p = strchr(GLYPH_CHARS, c);
        const unsigned char *gl = GLYPHS[p && c ? p - GLYPH_CHARS : 23]; /* '?' */
        for (int row = 0; row < 7; row++)
            for (int col = 0; col < 5; col++)
                if (gl[row] & (0x10 >> col))
                    plat_rect(x + col * scale, y + row * scale, scale, scale, r, g, b);
    }
}

/* ===== main_xenon.c ===== */
/* Fazenda 360 - ponto de entrada do Xbox 360 (libxenon, roda via XeLL). */
#include <stdio.h>
#include <string.h>
#include <xenos/xenos.h>
#include <console/console.h>
#include <input/input.h>
#include <usb/usbmain.h>
#include <time/time.h>
#include <ppc/timebase.h>
#include <xenon_soc/xenon_power.h>

/* Area util do console: colunas*8 x linhas*16 pixels (sempre dentro do framebuffer). */
static int scr_w, scr_h, need_flush;

#define HUD_X 64   /* a celula (0,0) do console e redesenhada no flush; HUD fica fora dela */
#define HUD_Y 0
#define HUD_LINE_H 16

void plat_rect(int x, int y, int w, int h, int r, int g, int b) {
    /* console_pset nao tem checagem de limites: recortar aqui evita corromper memoria */
    int x1 = x + w, y1 = y + h;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x1 > scr_w) x1 = scr_w;
    if (y1 > scr_h) y1 = scr_h;
    for (int yy = y; yy < y1; yy++)
        for (int xx = x; xx < x1; xx++)
            console_pset(xx, yy, r, g, b);
    need_flush = 1;
}

void plat_text(int row, const char *s) {
    int y = HUD_Y + row * HUD_LINE_H;
    plat_rect(HUD_X, y, scr_w - HUD_X, HUD_LINE_H - 1, 0, 0, 0);
    font_draw(HUD_X, y, 2, s, 255, 255, 255);
}

void plat_present(void) {
    if (!need_flush) return;
    /* console_putch termina com memdcbst() no framebuffer inteiro (unico flush exportado).
     * Imprime um espaco na celula (0,0) e volta o cursor com '\r' para nunca rolar a tela. */
    console_putch(' ');
    console_putch('\r');
    need_flush = 0;
}

int plat_screen_w(void) { return scr_w; }
int plat_screen_h(void) { return scr_h; }

static void read_pad(int port, pad_t *p) {
    struct controller_data_s c;
    memset(p, 0, sizeof *p);
    memset(&c, 0, sizeof c);
    get_controller_data(&c, port);
    p->left  = c.left  || c.s1_x < -16000;
    p->right = c.right || c.s1_x >  16000;
    p->up    = c.up    || c.s1_y >  16000;
    p->down  = c.down  || c.s1_y < -16000;
    p->a = c.a; p->lb = c.lb; p->rb = c.rb; p->start = c.start;
}

int main(void) {
    unsigned cols = 0, rows = 0;
    xenos_init(VIDEO_MODE_AUTO);
    console_set_colors(CONSOLE_COLOR_BLACK, CONSOLE_COLOR_WHITE);
    console_init();
    console_get_dimensions(&cols, &rows);
    scr_w = (int)cols * 8;
    scr_h = (int)rows * 16;
    xenon_make_it_faster(XENON_SPEED_FULL);
    usb_init();
    usb_do_poll();

    game_init();
    unsigned long long last = mftb();
    for (;;) {
        usb_do_poll();
        pad_t pads[2];
        read_pad(0, &pads[0]);
        read_pad(1, &pads[1]);
        unsigned long long now = mftb();
        float dt = (float)((double)(now - last) / (double)PPC_TIMEBASE_FREQ);
        last = now;
        if (dt > 0.25f) dt = 0.25f;
        game_update(pads, dt);
        game_render();
        mdelay(8);
    }
    return 0;
}
