/* Fazenda 360 - arquivo unico (libxenon). */
#include <stdio.h>
#include <string.h>

/* ===== game.h ===== */

#define MAP_W 20
#define MAP_H 11
#define DAY_LEN 60.0f   /* segundos por dia */

typedef struct { int up, down, left, right, a, lb, rb, start; } pad_t;

typedef struct {
    int active, x, y, dx, dy, tool, water, moving, frame;
    float cool, anim, swing;
    pad_t prev;
} player_t;

typedef struct {
    unsigned char tile[MAP_H][MAP_W];
    unsigned char crop[MAP_H][MAP_W];
    int gold, seeds, crops, wood, stone, day;
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

/* ===== sprites (extraidos da folha do personagem; 22x40, 60 cores) ===== */
#define SPR_W 22
#define SPR_H 40
/* direcoes: 0 baixo, 1 cima, 2 esquerda, 3 direita; quadros: 0 parado, 1-3 andando */
static const unsigned char PAL_BOY[60][3] = {
    {9,10,18}, {174,67,52}, {44,88,174}, {14,42,97}, {229,136,95}, {159,114,48}, {65,29,18}, {135,56,29}, {180,47,47}, {73,35,25}, {249,190,145}, {88,30,20}, {7,24,60}, {248,179,131}, {59,40,41}, {157,43,41}, {28,17,15}, {54,18,14}, {225,162,122}, {26,24,39}, {25,64,135}, {51,27,29}, {243,161,113}, {37,27,20}, {22,57,122}, {18,49,111}, {247,246,240}, {5,14,37}, {76,25,17}, {44,39,49}, {215,107,75}, {109,41,24}, {105,23,26}, {68,50,56}, {121,49,26}, {82,41,38}, {45,14,10}, {243,224,206}, {244,202,168}, {53,73,115}, {138,80,63}, {31,72,147}, {198,54,52}, {121,58,57}, {134,30,32}, {45,52,90}, {149,67,34}, {89,68,70}, {35,11,7}, {116,104,92}, {27,40,74}, {64,19,15}, {177,142,118}, {10,33,80}, {99,36,22}, {178,107,79}, {203,81,59}, {24,9,6}, {86,44,23}, {37,80,161}
};
static const unsigned char PAL_GIRL[60][3] = {
    {133,47,24}, {34,21,16}, {247,207,175}, {24,70,149}, {112,51,22}, {207,96,64}, {66,39,34}, {31,43,80}, {45,31,28}, {38,10,6}, {243,239,237}, {107,98,107}, {106,33,19}, {96,78,79}, {63,71,100}, {182,174,181}, {9,17,40}, {14,52,121}, {213,149,120}, {186,77,41}, {63,50,57}, {30,79,164}, {174,121,95}, {8,30,76}, {50,14,9}, {6,10,24}, {90,46,35}, {63,20,13}, {234,140,98}, {211,202,205}, {145,54,26}, {122,66,50}, {21,10,8}, {47,37,47}, {136,134,154}, {93,41,20}, {123,116,129}, {231,223,220}, {8,22,58}, {248,178,130}, {169,69,33}, {19,60,134}, {93,27,16}, {78,33,18}, {120,40,21}, {155,109,73}, {9,37,93}, {32,60,114}, {243,158,114}, {224,119,80}, {81,64,65}, {12,45,106}, {134,91,74}, {68,90,147}, {78,22,14}, {175,85,61}, {249,191,146}, {155,61,30}, {217,173,161}, {161,151,158}
};
static const char *SPR[2][4][4] = {
  {
    {
      "......wwwwVm6wwmmm........wwmmm9VsaamBBa......w6mmmaaSBspaSVp6w.....maBVYYBSppBBVBmvvw....9HaHSsY7YYSBBVYYBvH...wHaSVYYYVV7Y77Bam69...6mSs77YVY77V77spa69..wmaBYkYssk7YSBkYSSv9..aasVkYBHBkYpapskYSaawwasB7VpH8VspaFFaBYYSm9wm6BVpVuuspp1u1kpVSpm9.aHSssWYVuuUUe7YsSHmaw..vppiN00UDAMGvG1ppv.....mmub00AAAA00b4smZ.....mW4Q00bDDb00QMim......HW4Aq0ADDcNMbMFa.......pB4MDDDDDDDMBS.........ZHuAADDAMUaa...........mvGF118vvv...........aW3o118833ia.........aFgO3uuuu33g8p.......ZS8gfOggg8ff8uS.......aigFKfrrrrff8uFH......HFgWPnOxxfndig8H......pggaP5f22fnOJggp......SgFNOK222xPPCF8S.....H8uFGOKf22fPPC1u1p....H4DuHrPPP3r3rJUD4pp...HcAcBPKfxffK3HAcApp...HAADGPffxxxxO0ADApp...peee0OK3rrPxKRZhe9.....aaaRffORR3ffCaHH........XRKffRRPffCE..........XRPxP003fO0E..........EvVksvvB7YvE..........EvBVVvvBBwvN.........Lvsk7VvvBkksmL........NvB7YwvvSY7svG.........LvwBvNGm6wvN...........NGGv..vvGv......",
      "......................................................................................................................wmmawwmw.............6mBVaHaHm...........9aamHsVaSBmw.........9HBsVsSBSBSmm6........waassVVBBVsVsmw.......wmBVYVYYssYBa69.......mSs7BBY7ss7YSv9......9HsY7SH7YBSVYsmm......mBYYBpFVYFFHSsSm......mHspWguBuuuFaSpa.......mmspv1MDu0HYaH.........mFc0ZAAq0I1m..........HUQ0lcAc0Q4S..........HtMnDDDDqD4S...........6sDADMDMea.............p74DMt7m..............6oE11VJLH............6i3luu13FWp..........ai8OduuhKg8pZ.........H8iOPooOfi8iZ.........p8NjKx2xnT88.........aggJOf22fOR8uBZ.......aDMJOO2xOPJ4ASZ.......aAIC333333JMcpZ.......HIeCOxxxxKCLA.9.......XamCxfPPfxP0aX.........XTCxfrrf2PRX...........TRx23C22rJ............ERKKrRKKCT............E033R03d0T............EvwsHvVVmE............vsYYSm77svE...........vskkSvs7Bv............NvssvGvvvN.............NGGNNNNN.......",
      "......................................................................................................................wmmwwww..............w6ssm9am9...........w6mmSsHmYs9..........9aBsVSSSsVmvmw........6mpBVYsssssSSv........9apsYYVVVVV6H6........mBs7YVVYBYVBm9.......HmB7YpskYHpYsHaw......mBYsapBVS1VSsSaw......mSVpY1upuuuSBpm9......9maVaGU44ZGSsmaZ......ZHm7l04DDL01VvZ........ZaUb0cccT0buH..........p4cGDAAneAtp...........HMDDMDADhH.............HuMMDUka..............G0iFFGG9.............airuugX3iH...........HggKFg8jP8pH..........pgFf3ooKKFFa..........S895K22llW8a.........pg8JK222fPL4e9........aDMJK222KPeDtL........HDDR333PPPlMZLX.......LIlrOfxxxPR76X.........LGOx3r3fKRJX..........X0K2frCPPRX............Jr2xC3OrRT............TCxKR3PCJ.............EGY90RrRJ.............LvYYaGsaT.............GSYwaHsSv.............NawBvB7Bv..............vppvG6vN..............NvvNNGG........",
      "......................................................................................................................wvmww9w..............w6sBHpmv6...........96ampsppsBaw.........9HsVVS6SBSmvmw........6aSVVVBBBBBsSm........9aSVYYVsVVsBa9........aSs7VsYsV7kVa9.......6aBkYSB7VpBk7Sv9......mpVBaHBVSgppBsaH......vaSaV8uVuU1BHSm9......9ma7avuMMtvHBmL.........a1qvtDAXGtFv..........pUQ0qAcl0QUa..........6MblADAAqc46...........6MDAMDAD1H.............ae4AAusH..............aJVFFNvS.............HFPeUuXoFa...........H8gKhggjjgWa..........p8FfjooKd88m..........W8EndxfdXigH.........Hu8JOx22Or9gFH.......XHDMCOx22O3LDAH.......XaAtC3333PrJAAH.......XND03OfxxxPRDAH........XH0PfO3PxfRaHX.........T0K2f3rOKRTX...........0OxxrrKO0T............TrKKRCO3RT............T0wVGR3CRE............Gp6Sm6YwGE............Gmp6mBVsmL.............vppmwk7HN.............Nvvvasav................NNvvvN.......",
    },
    {
      "......HH6S6m6vmmw...........6maBBmvsVpw........w9vm6pBssSsBvaaw......mHBsVsSBsBBBssHmw.....6HapssssBV7VVpHa9.....9maSsVssYY77Ypm9......mmBsVYVY7V777Baa9....9mpSVk7V777V77sSm6....mSSs777V77777YBVBm9..9aYSS7YVV77s7YsBSmv9...6SaSsVVVYYBVsSSHm6.....mHpSBBSVsBsSSpm6......maHHBSpBSBSSpHvH.......HmapSpppBHHav9.........9mHHHHpHHav9..........ZZammmammm9............ZGvmmamvNZ............aCCuuuuoJaZ.........ZHio388FFrjFa........Z9pgdO88giOdgWZ.......9ai8dKo33oxjg8aZ......9a88jddxxxxj8gaZ......9Hgijj22xxPoigH6......6HgWjKxddxPoWgH9......9W8WPfxxdxPoWgSZ.....HBu4HC33333rJp44pH....phDAGCr333PrTpcApa....HhDMJPf2xx2K3JDApa....LEee0PKxKKxK3Ret9......Xam0PKOCC3KPRmaX........XRPKPCR3KPRX...........ROfKrR3fPR............RrKOC0PO3R............0CjjR033oJ............0B7YavVkwv...........NvsYsHmsVsmN..........GSVYVHmY7V6v..........NvB7YmvVYsmG...........NG66GGH6GN.............GGGNNGGG.......",
      ".......................................................................................................................w....................wvwwmw...............wmsamsmH............wmvaSBBsmavw.........9apSSSBsBBSpm6........9mpssBSBVVsSa6........96HBYVsBVYsaH9........9mBV77VY77YBv6.......HaBBYkYY777VsBm6......HaBBY7Y7777VsBmS.......9mSYYVY7VYspvH6.......6mHsBVYYBVSav66........6apSBsBBSpHH96........6mHpHSSppaa996.........6mHHHHHav6999.........96vvvmmv69999.........9aL9iiiaaZZZ9........ZHWjF8FFoZpZZZ........6igOhgg83gi9ZZ.......ZHg8OoFFTO88a..........pgidjooodWga..........WgWXd222lSgW.........H8gGKx222K088a........H4MGKffffKRM4a........HAIR3OffKKrMDH........HDLCOfx22x3lMHX.......XaGR3PPOPOPRHX..........TRrrCC3P3RX...........TRCrCRKfrR............TJCr0R3K0T............TvNHvvVwvT............Lv6SmaYYBG.............mHpm67YHG.............NvmvGwBv................GNGvvN.......",
      "................................................................................................w6ww6w................6a66HH9.............9HasHHVmaH9..........9mHpaBsSmpv6..........vSsVBSSsssBmH.........6mBVVsBsYsm66.........vSsYVsVV7YBa6.......6amSs7YVYYkYBpm9......SaBSYkYY7Y7VVB66......6HaBY7VY7YYVppa6......66vHssVVYVVspm66......66HHSBBsBBBSpv66......669appSBSSSpm666......9996appSpppHa666......9999HaHppHam9999......9ZZ99Gmaamva9999......ZZZ9aNiiiiNG9ZZZ......ZZZSiP8g883hpZZZ........HigOjggh31WaZZ........Hg8fPTojdh8mZZ........BgVXdfxdligHZ........LF8pOx222KJgF6........aguJf222xKRuUH........pAIJOfxKOOCcDLl......lHAtRPffKKOoDDHX.......X6arKx222fCHHXX........ETrKOP3PfRLT...........TCP3rCOfRT............TR33CCffCT............GNCCRCffCT............NN6HvR3PRT.............GHHvmVYGT.............LvvvSYYHN................vwY7aN................GGSmm..................vGGN.......",
      "...................................................................................................www................waH9pmw.............wmmBHasaHw...........wvHSpSVBmmvH..........mssVBBSBSBBaH.........9mBYYBBsVBmmw.........mSsVVssYYVSm9.......96vpV7VVY77YSma.......6mpSs7V7kYkVBBa9......6aHSYYV77YYYSam9......66mpVVsVYVYspv6.......66apSsBsVsBSpm........666HpBSSBSSHa9........669aHpppSpHav.........9999HaHHHHHm9.........99999vvmmmv9..........9999maiiiiLaZ.........ZZZHiP8g883iaZ........ZZHigPX88Z38ia........Z.p8gdjZETO88a..........p8ildjjOKigH..........W8WOx222PTgFZ........H7gGKx222KGggH.......lHtDROf222KJAIH.......XHtMRrOKKKOCDIH........XELCfx2xxKCZEX.........XLrfKPPPOCLE...........XCffPRrPRX............TR3xKRrCRT.............JrOORCRT..............JREV0GGN..............JHVYHvvN..............GHs7wvN................v6w9v.................Nvpmv..................GGN..........",
    },
    {
      "......wmBaaBvwwmm..........9mvpHsVBaHBvw.......w6pHppSBsBSssm66w....waHSSV7VVBSVYSaamHw...waaBVV7Vsss7VBYsBmH...w6HVY7YVV7YYYVBHma9...HHSY77VsY7YYYYBpm69...aHS7kYBV7VY7VVYBma6..wSSV7sSHssS7sVBYYpaH..6asYsHpFBap7BBVsVBHH..HaSSSFuu1FVsBpsBSpaa...Z6aaUY0GV1SpaHSSHma.....9m4I0nIUpSuHSpm69.....ZmDQ0qb4SeDpSpv6......SAAQ0qbD4DDHHm9.......9sDDAADDM7ppv9.........6tAAADU1Waa6...........ZqqIU1Wvm6.............Ha7FhGGp..............Xa81O88S9.............NWgOOg8iHZ...........XGijK1gg8aZ...........XRPOfhFg8m............XR2d53Vu8S9...........XRfxfPGggW9..........X6JfxxPGu8WH.........EHt0PKKTtD4VH.........THc03PPLcAAkH.........TLq0KxxJAD4.H..........TG0PKfxGHHa............TRC3KxOrJE............TR3rfxK3RE............T0PRfKPr0E............T0o03OrC0E............EmVm9V96mG...........Naswv6wVwpv...........GvBHkkYVwpv...........Gvma7kVB6mv............NvvpHHamG...............GGGGGN........",
      "...............................wm9.................w9Hpaww6w............w6vHsH6Hmm..........wavHssYSpsSm66........H6ssVSsssBBSpaH.......HpBVVsVVBsVVsSv.......pBV7Vs7YsYYBHm6......mpBYY77kYY7VsSm6......mBVYBkkVYkYYYBHv......aVVBS7BSsYYssVpm......9HppsVu1pssBSSSa.......6amusapkBSBSpHa........ZmUwv1uHpSBpHm........ZvAnGb4aUpppm9........pIcnGQAMApHavZ........p9ADDDAUBppm...........StDMD41HaH9............Zeet19vv9..............mmiFPNH................puddWSp..............HFtKgg8p9............LJFOdgg8Wa............LoPPdFg8ia...........LSlf5PEFgga..........9u1oKxKrBggFH........H1McJKxKrRUDAH........ptAtC3OPrJMAAp........EZtTRr33oYAcAH.........XpGRPf23LYcUH..........XXRC22PC0maX..........XTR22OCRRTE...........E0f2f3RCRRE...........E0KOPrCrCJvE.........ENNoO3CCrN6HG........EN6Y79J00aBSHv........GHkkYVSvvSBpvN........NNV7YSHvv6Hmv..........LvapHvNNvvG.............NGGv...........",
      ".........9H..................wwHm6w66w............wmmSsvHav6...........vvSBsVBaVHm9w........HSSSVBVVsssS6a9......9ppBsVsVYVsVsBH9......6HHs7ssVYYYsSaH6......HaB77s777s7VBpm6......HaV7skYsYkY7VBpa......HmVpVspps7ssVsSa......HaSpBYFVSYsssSam......w6mm1apBYVBBSpHm........6vU0vuupBSSpam.......ZZmc0Jc4mUHSSa9........pAb0JQDDDHHHH.........ShADDAAUpama...........pIDDAUYpmm.............Yq44FBvm...............aaFFjJp...............9BuOd8WS.............EH1jOggFS9............LJ8OOgg8WH............NoOOKF88ip...........Eaff5OTWg8W...........p1rfffPJggiH.........p4cCfxf30gMFa.........pDIRPP3rSDD4H.........HII0r33r7AD4H.........XpHROxxPLAMtL...........XRCOxfraaaX...........TRK2x3rRLE............JCf2f3RRTE............R3fxPCCC0L............RrOf30CCGvL...........G6VoC006pmG..........LvwwYBv9SpaG.........LvYkksSvwBmv..........NGV7VsHvavvN............GaaavGvv...............GGvv..........",
      ".....................................................wa..................w96aHw96w...........wwHvBBmaBm9..........9mmpSVVSpVpm9w.......6mSBBBSVVVsSpmmw......9a6BsBBYVVsssSm6......99SVYsYYVVVspm69......aHBVY77kVY7sSHa9......apVVs77V77V7YSmH......aSVSBVpBVYYsYsaH......mpSpSFupsVVBBBaH......9ama8BSB7BSSBpaH.......9Gm4GvUuSspppma.......Z96AT0cUpUmHa6.........HDcT0bDDAmam6.........p7DDDDA4ppaHZ..........6DDDDUiaaH..............ZYFiWGL..............HmaFFjLH...............pp8jh8Bp..............68Odgg8SZ............JJFKlgggiH............Jr3KdFggFH............LPf5OjW88SB..........XpofxfPJiuFa.........XpIRKffPrGADH.........XHM03333rtcDH.........XaI0POfKoecDp..........XL03fxKrRI49...........XRf2KPrRaaE...........0f22PrRr0EE..........EGKfOrRCCCvE.........EvBwo3C0CJ96vE........m77YVL0006S6vL........vYkVVSGNm6wpvL........GvBV6mNGBsSvL..........NvHHvNNvvvN............NGvG..GNN......",
    },
    {
      "......www9mp6H6www.........pm66aBsmSamm9........wmYBaBsYHmp7Ymw......99vpYBsVV7Y7Vamw.....6mmBSBBV77VsVsBBa6....aBBssssYY7YsVVBsBm9...mmBsYsVYVsk7VkYBaaw...9maSV7YB77k7YVkVm9w...6aSVkssVk7VsVHsYBm6..6vSB7VSB7YBaBm8Hssm9..6mSBVBSSYsSi11gaSSpw..9mSSspppBBW7khUvmm....9HppBHHaHkUmvGMvm......6mHpHUuauMb0QAvh.......6vaBDMUDAQ0QAD6.......ZHmSkt4DDDIcAtp........ZaHHY4MDDAAD9..........ZmmHF4DDDDa.............6mLF8ama..............apjO1ia..............HSFgjOgH..............pFguhOhFR............HS8gFXfP3R............aW8gWj5xxRT...........aWggLOxxx0X...........HWguJOfxfRpX..........HF4493POORYEX.........HMDAUL3330DpE.........HUDDALKxfRIp6..........aaHaOxKORLE...........XGJoOx3CRLE............R3PxfC3RT............ER3PxfCP0T............E0CrPOR30T............Lv6s7kmVvL...........Em6SVkBvSsNL..........Em6BYYkk6svN...........Gm6sYk7HavN............vaHH6SvvG..............NGGGGG........",
      "..........9ww.................wwm9wawmw............wmmVsmamaaw.........wmpSssYSBVVm9.........aHSssYsBVsam6........waBssV7sssVsBHa......9mBssY7V7ss77Sm6......9mSB77Vk7kVB7Vm6......mpBYVVY1VsSHSY6v......mSVYVVYYppuiaBpm......mSVVSBspSuUUaHH9......mHBSppB77m04Gm.Z......mappHaHUcn0Dv..Z......9vHpUtpMQnGcv9.........mapIDMDbqGccp.........ZmHSkMDDDAAtp...........mmpF4DDD4H............ZwvHF1aaa..............NTjhFm...............HWFgKhS9.............HpFggdPF0.............HSggFdPr0.............pWggWj5f0.............SigFpOdfR9...........HSggiJKxKRhX..........HSDDtJOfKRDHX.........HSDAASrrr0MaX.........HpDAA6fK30eLX..........9HpLOxOrRH............XHGoOx2RRX............XJRrPx2PRT............ERRCrOxxCJ............JRrrCPKPo0E..........EGRrrRr3Y7vvE.........LvS9C00sVVk7v.........vBsBGG0BY776v........LvBsV6GGHsYHvN.........GaBYBvGG6vvN...........vm66vNGvN..............vvGG............",
      "......................................................mw6w................wwv6a666w...........wHamspmHSSmw.........9v6SsYBssYpm9........9mSssYVBBsBvp6.......9HmSsVYVBBssssmw......9mSsB7YVYVY7YaH9......paSB7VY77kYS7Sm9......mSB7skkYVBpHSBv9......mSsVB77Vps1smSv9......vpBsSsBS71Uumm99......vaSSpBS1p094m..Z......vaSpSpHuc0lIm..........mpp4VsMQ0nba..........6mHIMMAbnAcw6..........map4MDDDDMw............amV1UDDD9..............mmYFVmm..............LaZjjia...............piggPhZ..............HBgg8KjVG.............HBggiOOPG.............SiuFLPdOG............6p8uLCPK3BS...........9p4A9JOf3tWp..........LS4AAL3OrtAH...........BMAAL3PCeUH...........ZphEl1jJGpX...........XLaTl1l0GX.............E0rX1hCRE.............0RRoFef3T............L0CCCjdKP0...........EvHJCRrOjEaG..........vaS9G0RowY76G.........vaSBmmmSV7kVv..........GmS6vGpVYBaN...........mmmNGmpHvN.............vG..vvG........",
      ".....................................................6aHwm6ww............w9msBmmmmHw..........wamaVVmaVVHH.........wmBVVVBVVBam9........9mSVVs7sSSVBBa6......w6HSssYYYsBYYsHa......9mHBsVYY77V77Yaw......6aHB7YskYkspVYSv......aaBYYVYkBSpkaSVv......aaSVSsYYSY11HHpm......amSSpSVSFieUamm.......amppaHp11v04vH........9mHH7uvUcn0cvL.........maH7DeMbq0bIp..........aaSkMAADIAAp..........6HppUDDDDD6.............amB1uehh..............6pEh8Ha..............ZpFFd1B...............a8ggXOip.............HWgg8gKjRE...........9pFgFWXnOCHZ..........pWggHCO5fC8pp.........HUMGC3fxKCUDep........HDcGrPxxORIAtp........HAAZr333rR44hH........6AM03O2x3RGHE.........Xp6ROf2fCCXX............XRrPx2CRX.............ERR3O2fRX............E0CCRP2fCT............NvJC03OPCT...........Lmp600r3hNaE..........NmSSHvNVYsaLE..........NvSHvpY7k7vN..........NmmvvaskYsvL............vGNGpS6vL................NGGGL.......",
    },
  },
  {
    {
      "...RsRh4Z9999R.RRssZs...OJJUgORCCZZRsOUJJvO..9eveW9gCi0viCCRWiU5U99UeeWRgi0viiUvvgsROUJ99UUi9gi0eU0vvvvUgOOiU9900ROi0ev0eee0vvCROC099iiOgiJeviJJ0s0JisOs099CiOiee0CseesvOUJigRiOh9iOivUsgisRvJJgUigsRhhRi9CCssJ5ig5nnUgsRgRh90C9ggRR05nSnnvCCROgis9Ug9R00WWVuumWWWeROgiR9ggRWvIxPquuuPWb5sOgsO.9gsOnuFWqud2PWAmgRsOR.6RhRvddqddduIq2SssgO...OQRC5dddddddudeZRgO...O...ReSdddummCW..R9...6...89WvJJvO99...........X.7Nt55n7NB8..........DaEkImSmkNFBo........XxAYfbAAbpkAFKX.......oTTrffErrfHFbBo......6aAFrDkkpHq7xAYD......8FbYl73LLLlkEbTo......hnSjkpfLLLHp8nnCQ....RSuStNpHLL3HH8SdS6R...RmuutNHHpkpppOmuuRR...Ruuu8pfLL3LfH1uu2RR...RuudGpfLLLL3HXjuu6R....OOONfffkkp3LN1OO........oGf33NPpLLNco.........KGH3fcPp33NX..........KP7llPP7llP6..........6W440OWh44W8..........WOhZZOWhZhh16........8WZevZ9WR0eZ91.........WO4ZZWWRZZh18..........19OW18199W8...........8111..8188....................hh......",
      "............................................................................................................................................OO99O...............OORCggs99s...........9sCiiiUiggRR.........OgCi0Ci0U0CCh.........OiiUUii00UiCO........hRCUviiU0Cee0OZ.......OsiU0geUCRveUg9Z......Oiv0gR00iJsiUi9Q......OCigsJsv55ggCg9Q......OggsRtnSSt0sss9.......ORR0WPmdmWWesRR........OCtxPuuuWaSgR.........OnSFW2uAWMutO.........RRtmddduudiR...........RRUduduS0O..............11J5U1W6............KKEknnnNEo............oBak2dmNEbX..........8BTYpraapabKo.........XFT7qp3HD7bT8..........Iw7o3LLENIIh.D......R4mScHLL3pcnmS6D......R.2ScppkkkGmuuR.......R.dQN3LLLLNGudR........RRPH3LLLL3POO..........RP33pkH33PKK..........KPfLHGfLfPK............GN3fPH3NG.............KPpNPNlGK.............6RZhWZ4R8............6WC4ZWZU4W6...........8W4vZWh4ZW8............19ZR1WWW8..............111............",
      ".............................................................................................................................................OOOR................OOssgsO9R...........ZOCCCCiCCiOZ.........hRgiigCivUC0OZ........hOg0Ui0UUUig9Z........hg0e00ev0e0C9ZZ......ZOC0egvJigveCs9Z......QOivisi0CUsUUgOZ......hOiis5sR5JCCig9Z.......OgsR5nnS5UsCR9Q.......Oss9WSumPWURRhQ.......OOJFPuuuPBnRR.........ORdbPbu2WBuvO..........ORmddduud0O............OgnddumCO..............RWvee19..............XKNtn5c7K.............KBkSSSkEoo...........XYAkYTFprTXDDD.......D8TBDNppjEbXDDD......DD6wKofLLE7wQDDD......D1nSVpLLLpGSSRDD.......OdutkpppkGuuRDD.......RddPf3333GxuRoD.......KhRcfff33p1s6..........KXc3pNkfp16............Xc3pGH3kG.............XG3HGf3PX..............GpkPNHPX..............WZZWhZWX.............894RWZ0h1.............1O4hWR411..............1RRWWWW...............8WW1...........",
      "..........................................................................................................................................hO.O9OOhh............ZRiRRi0iRO9..........ZRgCiCgCiiCsR.........Z9siUiCC000i9.........ZOivevCe0evi9.........Og0evgevsUJvi9........OC00is00CCUei9........9CiggesCJCgUi9........9gCOv555S5ssi9........9sRgWWSdSWW0sW.........OvMEPuuuPbt9..........O5bDP2u2PAm9..........6RmmIduuImeO...........RgSdumuSVs.............RCmudSRR...............XXe5CPo.............KYkkSSnkF6...........oBAlpAbbkAaK..........oTbplkNkpYbK.........D.Fx7jf3fjkb1.........6vm5NHLL3HcmSR........6tu5Np33fHGmuR........RMdJNppHHpGuuR........R.jPp3LLLLcsjR.........ROPfff3ffp1R...........KG33pPpHkK............KPHLfPpfPX.............XkffPkpPK.............1R0hW16WX.............1h4ZWhh98............8W4v49hh98.............W9ZZWWW1..............8W99W88................8118..........",
    },
    {
      ".....O9Z.......OOO........O99OZR99RZOOROR......ROUeRWRggsOsJJe9R....RZUeR9gg0iih9gvJisO..ZOeU0OC00ii0CsOs0vCO..ZOU0sC00UU000CsOCUCO..Z9CgRC0UUvU0vigOsig9..Z9gsR00eUJU0J0iRRgs9...ZORg0e0eJeeJeiRRs9h...h99C0JUeJeU0eCRRs9....hRsCUvveeeUvvCssOh...QhCCgUUvveU0vUCssOh...RgiCRCv0UeU0viisgi9...RsCsRg0Cie0C0CsRRO9....699RgiiiUCCisRO9h.......OOsgCCCCCgsRR6.........ORsggCCgsR96...........9RsgigsOW.............KWOsss9WKo...........8Bk7RROXka8..........KKANkw9oMkbBX.........XFANHp7kkNbFXK........XTTNEHff3HFTKo........KAxNDLLLLlxboK.......8.TYNfLLLLkBwIR......RtSdJNHLL33p6mmmO.....sM2uecpkpppkgduuO.....RMumXkfHfffHGIuuO.....RqddGHLLLLL3c6uuO......OO9Gf33HfLHHPOO.........KcHHkGkkfpX...........KGH3HPkHfPG...........KGpfpPNpHPX...........6GN7NPG77PX...........6W4vhWhV0O6...........1OZ4hWZ4CZW6.........8WZ0vZWCvvhW8..........8940hWh44O18...........199W1WWWW8............888888118.......",
      ".............................................................................................................................................9..OO................RRRRRhRh...........Z9ROCCCshROZ..........90iRgCiCRUU9Z........RC0siCiiiCsU09........O4RgUi00iiRgi9Z.......9ssCv0UU00CRs9h.......hOs0eUee0viRRhh.......Z9gUeeeeUUiR9hh.......hOi0e0vv00CO9hh......QRigCU0evi0CRg9h......hRCRC00UUi0gRR6Q.......69RCi0U0iCR9966.........9giCiCCsO9666..........OssCgss9..............O9ssssO6.............8X8RRR9WoXooo........K8Fk8RO1Gx8ooo........8FApl8XpNTY8DD........KbF3pkkHHYAXDD........DA73LLLLHEFoDD.......OnIGHLLL3pXS56D.......RddGk333pphddOD.......OuuGkpppHpcuuOD.......RMVPHLLLL3cqj6o........ROPfHHHffc88...........KPpkcNpHGK............KGppGp3fPX............XGkpPNfpPX............X1RhWOQhWX............8WZZWRZZW8.............WhhWZU4O1.............8W9WOgRW...............118WWW1.......",
      "...........................................................................................................................................ZZ9RZR9h.............ZO9sCROhsOZ...........9ggsgCgR4CO..........ZRgiC0iCgg0RR.......ZhRgs00000isCC9Z......hhOsCUiUvUUCOg9h......hhOR0e0vvUUCOsOh......hhRR0evveUUCO9Zh......hh9siv0UeU0iOOhh......QhRgiU0Uv0iggOhh......6RRRCUiUeiigsO66......6699siii0igs9666.........WsCCiigsO966Q..........OsgCCsR96.............XOsssRR9..........ooo8aP9RsR1DXoo.......DDXaTcosR8GbBKoo......DDKbFkp71lpTFXoo......DDXABffkpf3ab8oo......o6VIDH3LLLHDwqKD......DRSmRHLLL3pKSnRD......DR2uRHHHpkkQ2uRD.......6VVPHLLLLHGuuOo........ROcfL3fL3NOOK..........KcHHpppHc8X...........XGppNNHHcK.............GNHNk33GX.............GcNPkffcX.............WshW1QQP..............1OhWOZZWX..............WWWZ4Z98...............8WZ4498................W9Z9W..................1W1........",
      "..............................................................................................................................................O..9.................RROORhO.............ROOggORgOZ...........Ogggs0gssU9Z.........hRsC0i0iisgC9Z.......Z9gsC00vi0isC9Z.......hOggUv0eUv0ss9Z.......hh9gvvve0e0g9h........hh9giv0eUUUg9h........hh9CCvieUUisOh........h9Csi0iU00isg9........6ORsi0iUiiCRRh........6.OOgCgiggs96............9RggiggR9.............69RsCsR96.............K8ORgsO16............KxYGOROGBK..........ooKxT7Q9XcTa8.........ooBTxlBBacFT8.........ooBT7fkpklabX.........D8Sn8H3L3kVmnR........D8uS1pf3fktdmR........o1utGppkpp7udR........o6R1N3LLLHG6.6.........KXGk3fHfHG8K...........KGN3fGkpPK.............GPp3NNNPK.............XPN3pPGGX..............Pc78W98...............W844991...............8W449W................8W4Z98.................1RR9..................8111.........",
    },
    {
      "...............OOR..........O999OhZZO99s......Z99OgC00COh9UJJU9R...ZZgsCiiCe0iCO9UUJUsh..hgCC0iiivvUisOO0vvRR..hOCiU00UUUU0gs9R0eR9.ZROieUi0J00UvCg99C0R9.ORCeegC0eeiUeCCh9CUs9.Os0vCsssCev00eiCRCCO9.Og0Cs0eCs0eUiviCOgCO9.9RiggJ55CiUiCiiCOgiR9.9RgR5tO9siiCgCigOgUCRh.OO9vS1WMesRsCisOg0iC9..V6qb8Wbnsvdsss9ggii9...guAXWAddmSRR99g0CRO...guuuu2ddngsRORgisRh...RVuuuddngssOOOsCO9.....Rduuu5ZOO9..ORgR6......ROO5eV81....9sh.......D8enwYBK6...O96.......D8I2kFAFX.............8XbH3bAFoX............8BY33FATaX............DxfapYbbaK...........XKLLok7FAaK...........6Xf33pkMm5...........KRGk33pc5mmR6.........KRGkHfpQuumRQ.........6RGkLLXVuumRh.........K6cHLLp8ju566..........KcNf33k1OO.............GNc3LfpPK.............Pkc3LfpP6.............1cNcfHcP6.............1hZRUZhW1............8RZRWZ4ZhW6...........WsZRv44ZsW8...........19WCv04hOW.............WW9RRRO98...............81111..........",
      "...................................999..............hRs4hORRh...........ZZ99Os9iJvi94........OORiiiCs94vJiO.......RsC0Ci0CCsOi0UO.......OgCU0ivUigsgi09......hRCiUiiUeeigssC9......9CUeiUUJeiU0sRs9......90UUCJeUeUeigs9h......9iisgev0veUUCg9h......9CssUvvi0UiiCgOZ......9ssi5iUii0CiggRh.......OOO5WWVegOiCsRO........O92W8mnRSRgsRW........hdA18AumSRsR9.........gdudduuStRRO..........h.ddduu5RROO...........hqudmJOOOX8............R9OJJK1.................6nTYEX...............8wxHAAoK.............X1bHHAAaK.............XEHfHTAxK.............XF3jkaAbK............8qp3ffcTTK...........R.mG3fp8mS8O..........RS2Gff7VumsR..........RuwGpH6u2dsO...........R1GfL62unhO...........KXckLL8sR6............KXk3LHNGGK............Kc3LfkPNPP............1H33HccN8W6..........8Rh7HkPN8hRW6........68Z44QG1WhZhW8........W4e04hOWRhZW8.........9RZ4Zh9W9OW1...........W1RRW981W1.............81111...........",
      "........................................................hhh..............ZZ..Z999R............ZOO999ZvvWZ..........hOs0iiRW0UJRh........OggCg0UCsR0evRZ.......OgCCg0UCgss0U9Z......hRii0i0UUigsRiWh......OCUvC0JJie0sRg9h......OUU0seveU0vCsOOh......90isv0iUeUUisRRZ......9ss0UJ0ive0isss9......ORRvCe0i0UiCRRR9......Q6W5WWUegsigROWh.......Q9dxWw5Rnsgs9OQ.......sSuYWAdddssORR........RtddIuumqRRRR..........OMduudURROO6............IIm5OW9OR.............O9eeDX886..............95xYKX...............8FFkAbaK..............GbHpAABX.............XapHpTAFK.............WYfjlaATK............RtN33HkTT............RRdcf3pGnmnR..........R.dckkk6uudR...........oMcpHHU2umR...........KRGHL3cSdSO...........KXGL33kP9OK...........KPfL3kcNGXK...........XPLLpccpkP6..........8W8pfkPNpXRW6........W1Z448cP1hZZO8........9Uv4ZhW88Z4Z98........18ZZhOW8WvZO1..........81OOW881WW98...........8111...11........",
      "...............................................................................RR...............OORZRORR............RRgCsO9CUgO..........hOgC0CRsCee0RZ.......hOgggU0CgOiJvC9.......OCC0iCv0igRgU09......OhC0Uiv0viCgOCg9......Ogi00UeJvUiCOgC9......OiUigveJ0UviROgO......O00iCvgv0vvvCOgO......9isCivJgUU0UCOCg......9ssUJ9ig0i00COgs.......O9vtWt5isgCgOR9........1t2WwngtissOR9........huAWbdSmeR99R6........hudddumSgR9sO.........QRddudtsROsR9..........RMISJCW9OsRO...........ROitBX86OR.............KqIaEKoQ.............D82lAbaKo.............8XFfAATKo.............KrffFAAaK.............KrBHHxbT66...........hQk33k7xwt...........8.dNf3pcvudVR.........8.dckHpNdudVR..........6WNHLHNVuSh6..........KPHL3HNc1O6...........KNLLfkcNP16..........KGHLfkGckc996........6W1lfHGPcNQZR9.......8Ov04KNP8W1ZZR9.......99Vv44O18WRZR16.......61h44RW18WORW8.........891hOW...WW8............111.............",
    },
    {
      "................RR.........OOORZZ9O..R99O......4OUeeO9h9ORRR0JJ9R....RCeeZ9RsCUUigsivv9h..RsUevssgC00v0iggveCR.ZOC0v0OgCUveeUiiCCJiO.ZOC0i9sC0UeJeiviCg009.ZOC0gOg0ieeUUeiCsRgU9.hOgiCRi0eUUeUisRgCR0O.hORCCsivUi0eigeJ5eRCO.hRs0Cs0U0ii0gC5nneOOO.hO0igs0CCiiiUeWWn5O9.h90UissiCgCssJwW12t9..9iU0gsOgsUnRRmAW12M9..ORCCggRss0SmSdAW12uR..hOsgggsRss0Smddduunh...R9RgggORRRvSddddd.....69RssOO999CJSdddh1.....69RR968Wax5J99OO.......OORoKKxYYwmj1D.........66oKxAAFpwToK...........8obAAxHrbEX...........6YbAFrLlkrx1..........6bATBljLLrY1..........6TbF7H3LL3k1.........RsImj7HfLLfNRR........RUdmVXkpH3pNnR........sjuuuskpppkcuR........Rtmuu83LLLHGMR.........6OOOHLLffHG9K..........RPNHLLHkkGK............GkHLLNNkP............KPkH3fGNPX............6WcNpkP7WX...........6WWh40UW4WW...........6WhZ44sW4Z91..........6WhZ40vURZ91...........9RhZ4vUWO98............W9OORRWW8..............111111.......",
      "....................................................999R.999...........h9sRssR9iJe9.........Z9RC00v0ggsevOZ......ZRgCi0eeU00s0esO......Z9gi0eevvvUCCvi9......OsC0eevUvevigiC9......9giUeUvJeUiRsgi9......9g0e0veJ0sRegsC9......9gieiUv0CJJ5gRRh......OCiiCi0CvJtSQORQ......sCiissi05OWm6OQQ......CsigJUgnuBWb86QQ......CRsgnm5mbBWb2R........ssRgg5mdddu2us........RgsssstSdduuV.........OsgOOsinduudO.........RssO9VtJJ1OO...........9RKoYYI58..............6KFATH2BX.............oKbAAfrTX.............KaAbxfHkN.............XTbxED33L.............XTbElELLc6...........R6nSK7fLfGQ...........RVdm6Xf3HGdR..........RVuu2RpppPdO..........OVmuu833fPh8...........RORRfLpNP1K...........68GkfL3cPK.............XGkp3LpGK............KGGcNHLLPX...........61WcPcf33PX...........1RhhGGk74s16..........9hZh9OZ44ZO1..........8OZh99h4vtvW8..........19RWWh400hW............1W11WRRRW6...............81111........",
      ".........................................................99s.............h9OOR9veOR..........Z99CvUiRUvvOh........Z9gCivU0igCvUOZ......4OggUeJJvvisUURR......ZOgiUvJUvJ0si0OR......9sCiee0vvviRs0OR......9si0vvvvUigiRgOR......9sivU0JUissJ9sOh......9sgUiCegi5nn9OQQ......WsCigg0i5W1m9V........WRgCsJRn2W829V........9RgggnRmA18bO6.........RsssnSd2MM2mO..........9RRCSdduudC............9ROCndddV6.............1O5JJW96..............8aYwe1...............KBbbYIXD..............XFAApbD8..............XTAFHfHG..............XbAYEBLc..............aAbElLLG.............RawI7fL31h............O5SmWf3H8IR...........OSuuVXkN8dR...........RSuutXfkPqR...........6QVV8ffcP8............KOR873fGG..............KGkH33PX..............XPGkHLNG.............6WccGHHkP............89Rh1GN48O6...........8Whh9WZ4ZR96..........89hh9WZ4ee49...........8WhRWg444ZW............8WW19ssR98...............11111........",
      "....................................................9OOs.99R............ORRRsZO0eZO.........ZOORCvv0gievOR........h9gi0Uee0ssveO.......OOhi0Uev0eUCsvC9......hOsiUeUUee0sg0g9......9RC0eeUee0gssis9......9gCUJUeeeis09CRO......9gCvvCevCC5J99RQ......OsCiiC00vsvS99........ORsCsss0JgW2W.........OORgsJs0SaP29Q........Q9Rgs55SdaW2MR.........89RsemddduuuR...........OssvSdddIO............h99ie5RRh..............6WqwJOD...............ooYFTXD..............KXTbTFo8..............KaAAYfFP..............XxATr3fG..............XAbxEELc..............YbbEll3NK............6xbx7p33XC............RmmMNH33KuR...........Od2dXpppK2R...........Rudd8pLfXmO...........6qjK7f3pPRK...........KOR7H3kNGK............KKGkH3LcG.............KGGcp3LPK.............8WGGHfHPK............81hRPc77WX............8OhO9h4ZW6............81h9R44U4W6............1WWOg4U4W6.............889WWWW8................11888.........",
    },
  },
};

/* ===== game.c ===== */
#include <stdio.h>
#include <string.h>

enum { T_GRASS, T_SOIL, T_WET, T_WATER, T_PATH, T_HOUSE, T_SHOP, T_BOX, T_TREE, T_TREE2, T_ROCK };
/* Inventario (barra lateral): 6 ferramentas + madeira + pedra */
enum { TOOL_AXE, TOOL_HOE, TOOL_PICK, TOOL_CAN, TOOL_SEED, TOOL_HARVEST, TOOL_COUNT };
enum { SLOT_WOOD = TOOL_COUNT, SLOT_STONE, SLOT_COUNT };
#define WOOD_PRICE 3
#define STONE_PRICE 5

#define HUD_H 48
#define SWING_T 0.32f   /* duracao do golpe da ferramenta (s) */
#define WATER_MAX 15
#define SEED_PACK 5
#define SEED_PRICE 10
#define CROP_PRICE 15
#define WIN_GOLD 500

static const char *LAYOUT[MAP_H] = {
    "TTTTTTTTTTTTTTTTTTTT",
    "T.HH..S..B..t..r...T",
    "T.pp..pppp.r.......T",
    "T..p...........ww..T",
    "T..p...........ww..T",
    "T..pppppp..........T",
    "T.......p.....r....T",
    "T.......p......t...T",
    "T.......pppp...r...T",
    "T..t...............T",
    "TTTTTTTTTTTTTTTTTTTT",
};
static const char *TOOLS[TOOL_COUNT] = { "Machado", "Enxada", "Picareta", "Regador", "Sementes", "Foice" };

game_t G;
static unsigned char dirty[MAP_H][MAP_W];
static int tile_px, org_x, org_y, layout_ok;
static char last_hud[3][96];
static int last_side[10], side_ok, side_x;

/* ------------------------------------------------------------ logica */

static void mark(int x, int y) {
    if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) dirty[y][x] = 1;
}
static void mark_player(const player_t *p) {
    mark(p->x, p->y);
    mark(p->x, p->y - 1);            /* sprite invade metade do tile de cima */
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
                case 't': G.tile[y][x] = T_TREE2; break;
                case 'r': G.tile[y][x] = T_ROCK;  break;
                case 'H': G.tile[y][x] = T_HOUSE; break;
                case 'S': G.tile[y][x] = T_SHOP;  break;
                case 'B': G.tile[y][x] = T_BOX;   break;
                case 'p': G.tile[y][x] = T_PATH;  break;
                case 'w': G.tile[y][x] = T_WATER; break;
                default:  G.tile[y][x] = T_GRASS; break;
            }
        }
    G.gold = 50; G.seeds = 10; G.day = 1;
    G.p[0] = (player_t){ .active = 1, .x = 4, .y = 5, .dy = 1, .tool = TOOL_HOE, .water = WATER_MAX };
    G.p[1] = (player_t){ .active = 0, .x = 5, .y = 5, .dy = 1, .tool = TOOL_HOE, .water = WATER_MAX };
    layout_ok = 0; side_ok = 0;
    memset(last_hud, 0, sizeof last_hud);
    say("Bem-vindo! Troque de ferramenta com LB e RB.", 6.0f);
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
        int total = G.crops * CROP_PRICE + G.wood * WOOD_PRICE + G.stone * STONE_PRICE;
        if (total > 0) {
            char b[64]; snprintf(b, sizeof b, "Vendeu tudo por %d de ouro!", total);
            G.gold += total; G.crops = 0; G.wood = 0; G.stone = 0; say(b, 4.0f);
            if (G.gold >= WIN_GOLD) say("A fazenda prosperou! Continue jogando.", 8.0f);
        } else say("Nada para vender.", 2.0f);
        return;
    }
    if (P->tool == TOOL_AXE || P->tool == TOOL_HOE || P->tool == TOOL_PICK || P->tool == TOOL_HARVEST)
        P->swing = SWING_T;
    switch (P->tool) {
    case TOOL_AXE:   /* arvores com 't' levam 3 golpes */
        if (t == T_TREE2) {
            if (++G.crop[fy][fx] >= 3) {
                G.tile[fy][fx] = T_GRASS; G.crop[fy][fx] = 0; G.wood += 4;
                say("+4 madeira", 2.0f);
            }
            mark(fx, fy);
        }
        break;
    case TOOL_HOE:
        if (t == T_GRASS) { G.tile[fy][fx] = T_SOIL; mark(fx, fy); }
        break;
    case TOOL_PICK:  /* pedras levam 2 golpes; tambem desfaz terra arada vazia */
        if (t == T_ROCK) {
            if (++G.crop[fy][fx] >= 2) {
                G.tile[fy][fx] = T_GRASS; G.crop[fy][fx] = 0; G.stone += 3;
                say("+3 pedra", 2.0f);
            }
            mark(fx, fy);
        } else if ((t == T_SOIL || t == T_WET) && G.crop[fy][fx] == 0) {
            G.tile[fy][fx] = T_GRASS; mark(fx, fy);
        }
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

        /* animacao de andar: quadros 1,2,3,2 enquanto o direcional/analogico esta ativo */
        P->moving = (mx || my);
        if (P->moving) P->anim += dt * 9.0f; else P->anim = 0;
        {
            static const int seq[4] = { 1, 2, 3, 2 };
            int fr = P->moving ? seq[(int)P->anim & 3] : 0;
            if (fr != P->frame) { P->frame = fr; mark_player(P); }
        }

        if (in->a  && !P->prev.a)  act(i);
        if (in->rb && !P->prev.rb) { P->tool = (P->tool + 1) % TOOL_COUNT; mark_player(P); }
        if (in->lb && !P->prev.lb) { P->tool = (P->tool + TOOL_COUNT - 1) % TOOL_COUNT; mark_player(P); }
        if (P->swing > 0) {
            P->swing -= dt; if (P->swing < 0) P->swing = 0;
            for (int yy = P->y - 1; yy <= P->y + 1; yy++)
                for (int xx = P->x - 1; xx <= P->x + 1; xx++) mark(xx, yy);
        }
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
static void draw_sprite(int px, int py, int who, int dir, int fr) {
    const char *sp = SPR[who][dir][fr];
    const unsigned char (*pal)[3] = who ? PAL_GIRL : PAL_BOY;
    int T = tile_px, HD = T * 3 / 2;             /* altura final = 1,5 tile */
    int w = SPR_W * HD / SPR_H, ox = px + (T - w) / 2, top = py + T - HD;
    for (int j = 0; j < SPR_H; j++) {
        int y0 = top + j * HD / SPR_H, y1 = top + (j + 1) * HD / SPR_H;
        if (y1 <= y0) y1 = y0 + 1;
        for (int i = 0; i < SPR_W; ) {
            char c = sp[j * SPR_W + i];
            if (c == '.') { i++; continue; }
            int k = i;
            while (k < SPR_W && sp[j * SPR_W + k] == c) k++;
            int idx = c <= '9' ? c - '0' : (c <= 'Z' ? c - 'A' + 10 : c - 'a' + 36);
            int x0 = ox + i * HD / SPR_H, x1 = ox + k * HD / SPR_H;
            if (x1 <= x0) x1 = x0 + 1;
            plat_rect(x0, y0, x1 - x0, y1 - y0, pal[idx][0], pal[idx][1], pal[idx][2]);
            i = k;
        }
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
    case T_TREE2: draw_grass(px, py, u); R(7, 9, 2, 6, 120, 80, 40); R(3, 1, 10, 9, 70, 170, 70);
                  R(5, 2, 5, 3, 110, 200, 100); R(4, 6, 2, 2, 230, 90, 60); R(10, 5, 2, 2, 230, 90, 60); break;
    case T_ROCK:  draw_grass(px, py, u); R(2, 7, 12, 8, 120, 120, 130); R(4, 5, 8, 4, 150, 150, 160);
                  R(5, 6, 3, 1, 200, 200, 210); R(3, 13, 10, 2, 90, 90, 100); break;
    case T_HOUSE: draw_grass(px, py, u); R(1, 5, 14, 10, 200, 170, 130); R(0, 2, 16, 4, 170, 50, 40);
                  R(6, 10, 4, 5, 110, 70, 40); R(2, 7, 3, 3, 150, 200, 240); R(11, 7, 3, 3, 150, 200, 240); break;
    case T_SHOP:  draw_grass(px, py, u); R(1, 8, 14, 7, 180, 120, 60);
                  R(0, 3, 4, 4, 220, 60, 60); R(4, 3, 4, 4, 240, 240, 240);
                  R(8, 3, 4, 4, 220, 60, 60); R(12, 3, 4, 4, 240, 240, 240); R(6, 9, 4, 4, 230, 210, 120); break;
    case T_BOX:   draw_grass(px, py, u); R(2, 6, 12, 8, 150, 100, 50); R(2, 6, 12, 2, 120, 80, 40); R(7, 6, 2, 8, 230, 200, 60); break;
    }
    for (int i = 0; i < 2; i++) {      /* cursor da tile apontada */
        const player_t *P = &G.p[i];
        if (!P->active || P->x + P->dx != x || P->y + P->dy != y) continue;
        int cr = i ? 255 : 255, cg = i ? 170 : 255, cb = i ? 40 : 255;
        R(0, 0, 16, 1, cr, cg, cb); R(0, 15, 16, 1, cr, cg, cb);
        R(0, 0, 1, 16, cr, cg, cb); R(15, 0, 1, 16, cr, cg, cb);
    }
}

/* ---- inventario lateral (estilo Stardew Valley) ---- */

/* ===== icones das ferramentas (extraidos da imagem de ferramentas, 24x24) ===== */
#define ICO_N 24
static const unsigned char PAL_PICK[14][3] = { {180,179,182}, {163,149,135}, {165,117,74}, {110,106,116}, {167,84,11}, {139,72,23}, {85,82,96}, {67,65,80}, {90,42,16}, {61,34,18}, {47,41,45}, {46,23,11}, {31,28,38}, {18,15,19} };
static const char *ICO_PICK[ICO_N] = {
    "........................",
    "........................",
    "......cccaaca...........",
    "....a7100116accaddd.....",
    "....c6000000136c858d....",
    ".....77763310067758d....",
    "......cca776333367b.....",
    ".........aacc73257c.....",
    "...........bca4263677...",
    "..........d854973017c...",
    ".........d8545ba6106c...",
    "........98445b9a73017c..",
    ".......994449d.da6013c..",
    "......bb4448d..dc7113c..",
    ".....b8445b9.....c301c..",
    "....b9444bb......d301c..",
    "...b94448d.......d601c..",
    "...b5458b.........a33c..",
    ".bb5448bb.........c77d..",
    "d94458b............ac...",
    "d9548b..................",
    "db88b9..................",
    ".ddd....................",
    "........................"
};
static const unsigned char PAL_AXE[14][3] = { {187,183,184}, {176,121,60}, {174,95,21}, {91,88,99}, {147,69,12}, {85,82,95}, {81,79,92}, {68,67,82}, {115,48,12}, {75,33,11}, {56,40,35}, {36,33,38}, {34,25,25}, {26,15,16} };
static const char *ICO_AXE[ICO_N] = {
    "........................",
    "...............bbb......",
    ".............cbb78d.....",
    ".............c7631d.....",
    "............d76553bd....",
    "...........b76665677d...",
    "...........b755566637bb.",
    "...........cb7655533303a",
    "...........aca765333303b",
    "..........a988c75333003b",
    ".........d9428db7350006.",
    "........d8424cab730006..",
    "........c2229d.c7000b...",
    ".......d4229c..c600c....",
    ".....c94249a....aba.....",
    "....a942299.............",
    "...a942289..............",
    "...d8228c...............",
    ".cd8248d................",
    "d94249d.................",
    "d94489..................",
    "c9889c..................",
    "2.dd....................",
    "........................"
};
static const unsigned char PAL_HOE[14][3] = { {184,153,120}, {179,111,41}, {145,92,46}, {95,93,104}, {142,70,14}, {82,80,94}, {72,69,81}, {96,42,11}, {66,40,25}, {54,39,33}, {41,37,41}, {31,26,26}, {26,18,19}, {18,10,7} };
static const char *ICO_HOE[ICO_N] = {
    "........................",
    "..............bcca......",
    "............9aa668b.....",
    "............ba6556b.....",
    "...........b666533a9....",
    "..........db6655556ab...",
    "..........dca6555555a6..",
    "............c665555336a.",
    "...........8b8665533303c",
    "..........d844895333003b",
    ".........b8414cb6333003c",
    "........97414b7ba530036.",
    "........71148..aa30056..",
    ".......c41478...9666a...",
    ".....984147d.....9a8....",
    "....b841478.............",
    "....841188..............",
    "...d4117d...............",
    "..c7114d................",
    "d84147d.................",
    "d8117c8.................",
    "d877cc..................",
    ".8dd8...................",
    "........................"
};

static void draw_icon_map(int px, int py, int sz, const char *const *map, const unsigned char (*pal)[3]) {
    for (int j = 0; j < ICO_N; j++) {
        int y0 = py + j * sz / ICO_N, y1 = py + (j + 1) * sz / ICO_N;
        if (y1 <= y0) y1 = y0 + 1;
        for (int i = 0; i < ICO_N; ) {
            char c = map[j][i];
            if (c == '.') { i++; continue; }
            int k = i;
            while (k < ICO_N && map[j][k] == c) k++;
            int idx = c <= '9' ? c - '0' : c - 'a' + 10;
            int x0 = px + i * sz / ICO_N, x1 = px + k * sz / ICO_N;
            if (x1 <= x0) x1 = x0 + 1;
            plat_rect(x0, y0, x1 - x0, y1 - y0, pal[idx][0], pal[idx][1], pal[idx][2]);
            i = k;
        }
    }
}

static void draw_icon(int px, int py, int u, int id) {
    switch (id) {
    case TOOL_AXE:  draw_icon_map(px + u, py + u, 14 * u, ICO_AXE,  PAL_AXE);  break;
    case TOOL_HOE:  draw_icon_map(px + u, py + u, 14 * u, ICO_HOE,  PAL_HOE);  break;
    case TOOL_PICK: draw_icon_map(px + u, py + u, 14 * u, ICO_PICK, PAL_PICK); break;
    case TOOL_CAN:
        R(4, 6, 8, 7, 70, 130, 210); R(5, 5, 6, 1, 110, 170, 240);
        R(11, 4, 3, 3, 70, 130, 210); R(13, 3, 2, 2, 70, 130, 210); R(2, 7, 2, 4, 50, 100, 170); break;
    case TOOL_SEED:
        R(4, 5, 8, 9, 220, 200, 140); R(5, 3, 6, 3, 150, 110, 60);
        R(6, 8, 1, 1, 90, 60, 30); R(9, 10, 1, 1, 90, 60, 30); R(7, 12, 1, 1, 90, 60, 30); break;
    case TOOL_HARVEST:
        R(11, 2, 2, 13, 140, 90, 50);
        R(3, 2, 9, 2, 200, 205, 215); R(2, 4, 2, 3, 200, 205, 215); break;
    case SLOT_WOOD:
        R(2, 6, 12, 6, 140, 90, 50); R(11, 7, 3, 4, 205, 165, 115); R(3, 8, 7, 1, 105, 65, 35); break;
    case SLOT_STONE:
        R(3, 7, 10, 6, 130, 130, 140); R(5, 5, 6, 3, 155, 155, 165); R(6, 6, 2, 1, 205, 205, 215); break;
    }
}

static int slot_count(int id) {
    switch (id) {
    case TOOL_CAN:     return G.p[0].water;
    case TOOL_SEED:    return G.seeds;
    case TOOL_HARVEST: return G.crops;
    case SLOT_WOOD:    return G.wood;
    case SLOT_STONE:   return G.stone;
    }
    return -1;   /* ferramenta sem contador */
}

static void side_signature(int *s) {
    s[0] = G.p[0].tool; s[1] = G.p[1].tool; s[2] = G.p[1].active;
    s[3] = G.seeds; s[4] = G.crops; s[5] = G.wood; s[6] = G.stone;
    s[7] = G.p[0].water; s[8] = 0; s[9] = 0;
}

static void draw_side(void) {
    int u = tile_px / 16, gap = tile_px / 8 + 1, sz = tile_px;
    int sc = tile_px >= 32 ? 2 : 1;
    for (int i = 0; i < SLOT_COUNT; i++) {
        int px = side_x, py = org_y + i * (sz + gap);
        R(0, 0, 16, 16, 45, 40, 55);                       /* fundo do slot */
        R(0, 0, 16, 1, 90, 80, 100); R(0, 15, 16, 1, 20, 18, 28);
        draw_icon(px, py, u, i);
        int sel0 = (i == G.p[0].tool);
        int sel1 = G.p[1].active && (i == G.p[1].tool);
        if (sel1) {                                         /* J2: laranja, por dentro */
            R(1, 1, 14, 1, 255, 160, 40); R(1, 14, 14, 1, 255, 160, 40);
            R(1, 1, 1, 14, 255, 160, 40); R(14, 1, 1, 14, 255, 160, 40);
        }
        if (sel0) {                                         /* J1: branco/azul, por fora */
            R(0, 0, 16, 2, 240, 240, 255); R(0, 14, 16, 2, 240, 240, 255);
            R(0, 0, 2, 16, 240, 240, 255); R(14, 0, 2, 16, 240, 240, 255);
        }
        int n = slot_count(i);
        if (n >= 0) {
            char b[8]; snprintf(b, sizeof b, "%d", n > 999 ? 999 : n);
            int w = (int)strlen(b) * 6 * sc;
            int tx = px + sz - w - 2, ty = py + sz - 7 * sc - 2;
            plat_rect(tx - 2, ty - 1, w + 3, 7 * sc + 2, 20, 18, 28);
            font_draw(tx, ty, sc, b, 255, 255, 255);
        }
    }
}

/* ---- ferramenta na mao e animacao de golpe (estilo da folha de ferramentas) ---- */
static void tool_line(int x0, int y0, int x1, int y1, int th, int tool) {
    for (int i = 0; i <= 10; i++) {
        int x = x0 + (x1 - x0) * i / 10, y = y0 + (y1 - y0) * i / 10;
        plat_rect(x - th / 2, y - th / 2, th, th, 140, 90, 50);
    }
    int hs = th * 3;
    switch (tool) {
    case TOOL_AXE:
        plat_rect(x1 - hs, y1 - hs, hs * 2, hs * 2 - th, 175, 180, 192);
        plat_rect(x1 + hs - th, y1 - hs, th, hs * 2 - th, 235, 235, 245); break;
    case TOOL_PICK:
        plat_rect(x1 - hs * 2, y1 - th, hs * 4, th * 2, 175, 180, 190);
        plat_rect(x1 - hs * 2, y1, th * 2, hs, 175, 180, 190);
        plat_rect(x1 + hs * 2 - th * 2, y1, th * 2, hs, 175, 180, 190); break;
    case TOOL_HOE:
        plat_rect(x1 - th, y1 - th, hs * 2, th * 2, 170, 175, 185);
        plat_rect(x1 + hs - th, y1, th * 2, hs, 170, 175, 185); break;
    default:   /* foice */
        plat_rect(x1 - hs * 2, y1 - th, hs * 3, th, 205, 210, 220);
        plat_rect(x1 - hs * 2, y1, th, hs, 205, 210, 220); break;
    }
}

static int has_hand_tool(int tool) {
    return tool == TOOL_AXE || tool == TOOL_HOE || tool == TOOL_PICK || tool == TOOL_HARVEST;
}

static void draw_held(int who) {          /* ferramenta parada/andando, na mao */
    const player_t *P = &G.p[who];
    if (!has_hand_tool(P->tool) || P->swing > 0) return;
    int T = tile_px, th = T / 16 > 0 ? T / 16 : 1;
    int px = org_x + P->x * T, py = org_y + P->y * T;
    int hx = px + T / 2 + (P->dx ? P->dx * T / 5 : T / 5), hy = py + T * 45 / 100;
    tool_line(hx, hy, hx + (P->dx ? P->dx * T / 6 : T / 8), hy + T / 3, th, P->tool);
}

static void draw_swing(int who) {         /* golpe: levanta, desce e risca o arco */
    const player_t *P = &G.p[who];
    if (P->swing <= 0 || !has_hand_tool(P->tool)) return;
    int T = tile_px, th = T / 16 > 0 ? T / 16 : 1;
    int px = org_x + P->x * T, py = org_y + P->y * T;
    float p = 1.0f - P->swing / SWING_T;
    int hx = px + T / 2, hy = py + T * 45 / 100;
    int k0x = hx - P->dx * T / 5, k0y = py - T * 45 / 100;          /* levantada */
    int k1x = hx + P->dx * T / 3, k1y = py - T / 3;                 /* topo do arco */
    int k2x = px + T / 2 + P->dx * T * 9 / 10, k2y = py + T / 2 + P->dy * T * 9 / 10 + (P->dy ? 0 : T / 6);
    int ex, ey;
    if (p < 0.35f) { float t = p / 0.35f; ex = k0x + (int)((k1x - k0x) * t); ey = k0y + (int)((k1y - k0y) * t); }
    else           { float t = (p - 0.35f) / 0.65f; t = t * t;
                     ex = k1x + (int)((k2x - k1x) * t); ey = k1y + (int)((k2y - k1y) * t); }
    if (p >= 0.35f) {                       /* rastro claro do golpe */
        int vx = k2x - k1x, vy = k2y - k1y;
        int len = (vx < 0 ? -vx : vx) + (vy < 0 ? -vy : vy) + 1;
        float reach = (p - 0.35f) / 0.65f;
        for (int a = 0; a <= 6; a++) {
            float f = a / 6.0f * reach; if (f > 1) f = 1;
            int off = (int)(T / 5 * (f * (1.0f - f) * 4.0f));
            int x = k1x + (int)(vx * f) - vy * off / len, y = k1y + (int)(vy * f) + vx * off / len;
            plat_rect(x - th, y - th, th * 2, th * 2, 230, 235, 245);
        }
    }
    tool_line(hx, hy, ex, ey, th, P->tool);
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
        /* mapa + barra lateral (1 slot de largura + margem) precisam caber na tela */
        for (int k = 0; k < 3; k++)
            if (MAP_W * sizes[k] + sizes[k] + 24 <= sw &&
                SLOT_COUNT * (sizes[k] + sizes[k] / 8 + 1) <= MAP_H * sizes[k] &&
                MAP_H * sizes[k] + HUD_H <= sh) { tile_px = sizes[k]; break; }
        int total_w = MAP_W * tile_px + tile_px + 24;
        org_x = (sw - total_w) / 2;
        org_y = HUD_H;
        side_x = org_x + MAP_W * tile_px + 16;
        plat_rect(0, 0, sw, sh, 0, 0, 0);
        memset(dirty, 1, sizeof dirty);
        side_ok = 0;
        layout_ok = 1;
    }
    unsigned char redrawn[MAP_H][MAP_W];
    memset(redrawn, 0, sizeof redrawn);
    for (int y = 0; y < MAP_H; y++)
        for (int x = 0; x < MAP_W; x++)
            if (dirty[y][x]) { draw_tile(x, y); redrawn[y][x] = 1; dirty[y][x] = 0; }

    /* personagens por cima, de cima para baixo; redesenha se o tile deles ou o de cima mudou */
    {
        int order[2] = { 0, 1 };
        if (G.p[1].active && G.p[1].y < G.p[0].y) { order[0] = 1; order[1] = 0; }
        for (int n = 0; n < 2; n++) {
            const player_t *P = &G.p[order[n]];
            if (!P->active) continue;
            int up = P->y > 0 ? redrawn[P->y - 1][P->x] : 0;
            if (!redrawn[P->y][P->x] && !up) continue;
            int dir = P->dy > 0 ? 0 : P->dy < 0 ? 1 : P->dx < 0 ? 2 : 3;
            draw_sprite(org_x + P->x * tile_px, org_y + P->y * tile_px, order[n], dir, P->frame);
            draw_held(order[n]);
        }
        for (int n = 0; n < 2; n++) if (G.p[n].active) draw_swing(n);
    }

    {   /* inventario: so redesenha quando algo mudou */
        int cur[10];
        side_signature(cur);
        if (!side_ok || memcmp(cur, last_side, sizeof cur) != 0) {
            draw_side();
            memcpy(last_side, cur, sizeof cur);
            side_ok = 1;
        }
    }

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
    console_set_cursor(0, 0);
    console_putch(' ');           /* um unico flush por quadro */
    need_flush = 0;
}

int plat_screen_w(void) { return scr_w; }
int plat_screen_h(void) { return scr_h; }

/* Analogico esquerdo: zona morta pequena e so o eixo dominante vale (anda reto, sem diagonal).
 * Se cima/baixo do analogico ficarem invertidos no seu controle, segure LB + RB juntos
 * por um instante para inverter o eixo vertical (cada controle guarda a sua escolha). */
#define STICK_DEADZONE 9000
static int invert_y[2], inv_prev[2];

static int read_pad(int port, pad_t *p) {
    struct controller_data_s c;
    memset(p, 0, sizeof *p);
    memset(&c, 0, sizeof c);
    int fresh = get_controller_data(&c, port);

    int both = c.lb && c.rb;
    if (both && !inv_prev[port]) invert_y[port] = !invert_y[port];
    inv_prev[port] = both;

    int sx = c.s1_x, sy = invert_y[port] ? -c.s1_y : c.s1_y;
    int ax = sx < 0 ? -sx : sx, ay = sy < 0 ? -sy : sy;
    int l = 0, r = 0, u = 0, d = 0;
    if (ax > STICK_DEADZONE || ay > STICK_DEADZONE) {
        if (ax >= ay) { l = sx < 0; r = sx > 0; }
        else          { u = sy > 0; d = sy < 0; }
    }
    p->left  = c.left  || l;
    p->right = c.right || r;
    p->up    = c.up    || u;
    p->down  = c.down  || d;
    p->a = c.a; p->start = c.start;
    if (!both) { p->lb = c.lb; p->rb = c.rb; }   /* o atalho LB+RB nao troca ferramenta */
    return fresh;
}

int main(void) {
    unsigned cols = 0, rows = 0;
    xenos_init(VIDEO_MODE_AUTO);
    console_set_colors(CONSOLE_COLOR_BLACK, CONSOLE_COLOR_WHITE);
    console_init();
    console_get_dimensions(&cols, &rows);
    scr_w = (int)cols * 8;
    scr_h = (int)rows * 16;
    usb_init();
    usb_do_poll();

    game_init();
    unsigned long long last = mftb(), stat_t = last;
    int frames = 0, pad_seen = 0;
    double worst_poll = 0;
    for (;;) {
        unsigned long long p0 = mftb();
        usb_do_poll();
        double poll_ms = (double)(mftb() - p0) * 1000.0 / (double)PPC_TIMEBASE_FREQ;
        if (poll_ms > worst_poll) worst_poll = poll_ms;
        pad_t pads[2];
        if (read_pad(0, &pads[0])) pad_seen = 1;
        read_pad(1, &pads[1]);
        unsigned long long now = mftb();
        float dt = (float)((double)(now - last) / (double)PPC_TIMEBASE_FREQ);
        last = now;
        if (dt > 0.25f) dt = 0.25f;
        game_update(pads, dt);
        game_render();
        frames++;
        if (now - stat_t >= (unsigned long long)PPC_TIMEBASE_FREQ) {   /* diagnostico, 1x por segundo */
            char b[64];
            snprintf(b, sizeof b, "FPS:%d USB MAX:%dMS PAD:%s", frames, (int)worst_poll,
                     pad_seen ? "OK" : "SEM DADOS");
            plat_rect(HUD_X, scr_h - 18, scr_w - HUD_X, 16, 0, 0, 0);
            font_draw(HUD_X, scr_h - 18, 2, b, 200, 200, 200);
            plat_present();
            frames = 0; worst_poll = 0; stat_t = now;
        }
        mdelay(2);
    }
    return 0;
}
