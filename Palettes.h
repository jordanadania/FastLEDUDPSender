extern const TProgmemRGBGradientPalettePtr gGradientPalettes[];

TBlendType currentBlending = LINEARBLEND;

const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM ={
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, 
   CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, 
   CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };

#define Holly_Green 0x00580c
#define Holly_Red   0xB00402
const TProgmemRGBPalette16 Holly_p FL_PROGMEM ={
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Green, 
   Holly_Green, Holly_Green, Holly_Green, Holly_Red 
};

const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM ={
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray,
   CRGB::Red,  CRGB::Red,  CRGB::Red,  CRGB::Red, 
   CRGB::Gray, CRGB::Gray, CRGB::Gray, CRGB::Gray };

const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM ={
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, 
   CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };

#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM ={
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, 
   HALFFAIRY,        HALFFAIRY,        CRGB::FairyLight, CRGB::FairyLight, 
   QUARTERFAIRY,     QUARTERFAIRY,     CRGB::FairyLight, CRGB::FairyLight, 
   CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };

// A palette of soft snowflakes with the occasional bright one
const TProgmemRGBPalette16 Snow_p FL_PROGMEM ={
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0x304048,
   0x304048, 0x304048, 0x304048, 0xE0F0FF };

#define C9_Red    0xB80400
#define C9_Orange 0x902C02
#define C9_Green  0x046002
#define C9_Blue   0x070758
#define C9_White  0x606820
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM ={
   C9_Red,    C9_Orange, C9_Red,    C9_Orange,
   C9_Orange, C9_Red,    C9_Orange, C9_Red,
   C9_Green,  C9_Green,  C9_Green,  C9_Green,
   C9_Blue,   C9_Blue,   C9_Blue,
   C9_White
};

#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 Ice_p FL_PROGMEM ={
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3
};
//typedef PaletteAndName PaletteAndNameList[];

const CRGBPalette16 palettes[] = {
  RainbowColors_p, RainbowStripeColors_p,
  RedWhiteBlue_p, RedGreenWhite_p, Holly_p,
  RedWhite_p, BlueWhite_p, RedBlue_p,
  FairyLight_p, RetroC9_p, PartyColors_p,
  CloudColors_p, OceanColors_p, ForestColors_p, 
  LavaColors_p, HeatColors_p, IceColors_p, Ice_p, Snow_p,
  Sunset_Real_gp, es_rivendell_15_gp, es_ocean_breeze_036_gp,
  rgi_15_gp, retro2_16_gp, Analogous_1_gp,
  es_pinksplash_08_gp, Coral_reef_gp, es_ocean_breeze_068_gp,
  es_pinksplash_07_gp, es_vintage_01_gp, departure_gp,
  es_landscape_64_gp, es_landscape_33_gp, rainbowsherbet_gp,
  gr65_hult_gp, gr64_hult_gp, GMT_drywet_gp,
  ib_jul01_gp, es_vintage_57_gp, ib15_gp,
  Fuschia_7_gp, es_emerald_dragon_08_gp, lava_gp,
  fire_gp, Colorfull_gp, Magenta_Evening_gp,
  Pink_Purple_gp, es_autumn_19_gp, BlacK_Blue_Magenta_White_gp,
  BlacK_Magenta_Red_gp, BlacK_Red_Magenta_Yellow_gp, Blue_Cyan_Yellow_gp
};

const uint8_t paletteCount = ARRAY_SIZE2(palettes);

const String paletteNames[paletteCount] = {
  "Rainbow",  "Rainbow Stripe",
  "Red White Blue", "Red Green White", "Holly",
  "Red White", "Blue White", "Red Blue",
  "Fairy Light", "Retro C9", "Party",
  "Cloud", "Ocean", "Forest",
  "Lava", "Heat", "Ice 1", "Ice 2", "Snow",
  "Sunset_Real_gp", "es_rivendell_15_gp", "es_ocean_breeze_036_gp",
  "rgi_15_gp", "retro2_16_gp", "Analogous_1_gp",
  "es_pinksplash_08_gp", "Coral_reef_gp", "es_ocean_breeze_068_gp",
  "es_pinksplash_07_gp", "es_vintage_01_gp", "departure_gp",
  "es_landscape_64_gp", "es_landscape_33_gp", "rainbowsherbet_gp",
  "gr65_hult_gp", "gr64_hult_gp", "GMT_drywet_gp",
  "ib_jul01_gp", "es_vintage_57_gp", "ib15_gp",
  "Fuschia_7_gp", "es_emerald_dragon_08_gp", "lava_gp",
  "fire_gp", "Colorfull_gp", "Magenta_Evening_gp",
  "Pink_Purple_gp", "es_autumn_19_gp", "BlacK_Blue_Magenta_White_gp",
  "BlacK_Magenta_Red_gp", "BlacK_Red_Magenta_Yellow_gp", "Blue_Cyan_Yellow_gp"
};
