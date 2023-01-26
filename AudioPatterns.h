uint8_t bri[NUM_BANDS],
        dfm[NUM_BANDS],
        dfa[NUM_BANDS],
        avgs[SAMPLES],
        smp[NUM_BANDS][SAMPLES],
        dif[NUM_BANDS][SAMPLES],
        bass[NUM_BANDS],
        treb[NUM_BANDS];
        
uint8_t averageSpectrumAverage = 0;

uint8_t getAverage255(uint8_t in[]){
  unsigned int out = 0;
  for(int i=0; i<SAMPLES; ++i){
    out+=in[i];
    //Serial.print(in[i]);
    //Serial.print(" ");
  }
  //Serial.println();
  //Serial.println(out/SAMPLES);
  return out/SAMPLES;
}
uint8_t getMax255(uint8_t in[]){
  uint8_t out = 0;
  for(int i=0; i<SAMPLES; ++i)
    out=out<in[i]?in[i]:out;
  return out;
}
uint8_t getMaxBass(uint8_t in[]){
  uint8_t out = 0, mx = in[out];
  for(int i=out+1; i<=6; ++i)
    if(mx<in[i]){
      mx = in[i];
      out = i;
    }
  return out;
}
uint8_t getMaxTreb(uint8_t in[]){
  uint8_t out = 6, mx = in[out];
  for(int i=out-1; i>=0; --i)
    if(mx<in[i]){
      mx = in[i];
      out = i;
    }
  return out;
}
void saveBands(uint16_t in){
  for(byte b=0; b<NUM_BANDS; ++b)
    //smp[b][in] = pow(spectrumByte[b]>>4,2);
    smp[b][in] = spectrumByte[b];
  for(byte b=0; b<NUM_BANDS; ++b)
    dif[b][in] = abs(smp[b][in+1]-smp[b][in]);
  avgs[in] = spectrumAvg;
}
void analyzeBands(){
  bool bb = true;
  bool bt = true;
  static byte newBass = DEFAULT_BASS;
  static byte newTreb = DEFAULT_TREB;
  // Get Average spectrumAvgs
  averageSpectrumAverage = getAverage255(avgs);
  // Get Average
  for(byte b=0; b<NUM_BANDS; ++b)
    bri[b] = getAverage255(smp[b]);
  // Get Average Difference
  for(byte b=0; b<NUM_BANDS; ++b)
    dfa[b] = getAverage255(dif[b]);
  // Get Biggest Difference
  for(byte b=0; b<NUM_BANDS; ++b)
    dfm[b] = getMax255(dif[b]);

    
  // Bass Analysis
  for(byte b=0; b<NUM_BANDS; ++b)
    bass[b] = (bri[b]>>1)>dfm[b]? 0: dfm[b]-(bri[b]>>1);
  // Treb Analysis
  for(byte b=0; b<NUM_BANDS; ++b)
    treb[b] = (dfm[b] + bri[b]>>3)>>1;

  
  // Weighted Current Band
  //bass[bassBand] *= 9>>3;
  //treb[trebBand] *= 9>>3;
  // Weighted Default Band
  bass[DEFAULT_BASS] *= 9>>3;
  treb[DEFAULT_TREB] *= 9>>3;
  // Band Assignment
  newTreb = getMaxTreb(treb);
  newBass = getMaxBass(bass);
  
  // 2 Strikes Against The Treb Band
  if(bt&&newTreb!=trebBand) bt = false;
  else if(!bt&&newTreb!=trebBand){
    trebBand = newTreb;
    bt = true;
  }
  // 2 Strikes Against The Bass Band
  if(bb&&(newBass!=bassBand)) bb = false;
  else if(!bb&&newBass!=bassBand){
    bassBand = newBass;
    bb = true;
  }
}
void rainFlow(){
  buff[0] = 1;
  buff[1] = speed;
  buff[2] = brightness;
  CRGB c;
  static uint16_t idx;
  static uint8_t rgbRate, huey;
  //EVERY_N_SECONDS(2){ if(spectrumAvg>32) analyzeBands(); else {bassBand=1; trebBand=5;} }
  saveBands(idx);
  idx = idx==SAMPLES-1? 0: idx+1;
  analyzeBands();
  uint8_t bright = spectrumByte[trebBand];
  //bright = smp[trebBand][idx];
  //uint8_t bright = spectrumAvg;
  //uint8_t bright = (spectrumByte[trebBand] + spectrumByte[trebBand == 6 ? trebBand-1:trebBand+1]) / 2;
  
  //Serial.println(averageSpectrumAverage);
  if(averageSpectrumAverage >= 157){
    Serial.println("MAXIMUM DANCE!!!");
    rgbRate += 3;
  }
   
   //Serial.println(bright);
  //if(spectrumAvg < ZERO_THRESHOLD || bri[trebBand] < ZERO_THRESHOLD){
  if(averageSpectrumAverage < ZERO_THRESHOLD)
    bright = 0;
  //}
  if(rgbRate>3) rgbRate-=rgbRate>>1;
  if(rgbRate>0)        --rgbRate;
  if(beatDetect()){
    //rgbRate+=16;
    hueDirect ? huey+=rgbRate:huey-=rgbRate;
    //bright = 255;
    c = CRGB::White;
  } else{ // beatDetect()
    if(trebDetect())
      rgbRate+=12;
    hueDirect ? huey+=rgbRate:huey-=rgbRate;
    c = ColorFromPalette(palettes[currentPaletteIndex], huey, bright, LINEARBLEND);
  } // beatDetect()
  buff[3] = c.r;
  buff[4] = c.g;
  buff[5] = c.b;
}

void plasma(){
  buff[0] = 2;
  buff[1] = brightness;
  buff[2] = spectrumAvg;
  buff[3] = currentPaletteIndex;
}
void mappedPlasma(){
  buff[0] = 3;
  buff[1] = brightness;
  buff[2] = spectrumAvg;
  buff[3] = currentPaletteIndex;
}

void paletteG() {
  buff[0] = 4;
  buff[1] = brightness;
  buff[2] = spectrumAvg;
  buff[3] = currentPaletteIndex;
}
void mappedPaletteG(){
  buff[0] = 5;
  buff[1] = brightness;
  buff[2] = spectrumAvg;
  buff[3] = currentPaletteIndex;
}

void vuPalette(){
  buff[0] = 6;
  buff[1] = speed;
  buff[2] = brightness;
  buff[3] = spectrumAvg;
  buff[4] = currentPaletteIndex;
}

void sevenBands(){
  buff[0] = 7;
  buff[1] = brightness;
  // once you figure out custom structures
  //for(byte b = 0; b < 7; ++b){
  //  buff[b+1] = 
  buff[2] = spectrumByte[0];
  blueWinPacket();
  buff[2] = spectrumByte[1];
  denWin01Packet();
  buff[2] = spectrumByte[2];
  denWin02Packet();
  buff[2] = spectrumByte[3];
  bigDenWinPacket();
  buff[2] = spectrumByte[4];
  laundryWinPacket();
  buff[2] = spectrumByte[5];
  deckWin01Packet();
  buff[2] = spectrumByte[6];
  deckWin02Packet();
  buff[2] = 9;
  tv55Packet();
  buff[2] = 9;
  fibo256Packet();
}

/*
void peakAnalysis(){
  fill_solid(leds, NUM_PIXELS, CRGB::Black);
  const uint8_t columnSize = NUM_PIXELS/7;
  for(uint8_t i=0; i<7; i++){
    uint8_t columnHeight = map(spectrumValue[i], 0, 1865, 0, columnSize);
    uint8_t   peakHeight = map8(spectrumByte[i], 2, columnSize);
    uint16_t columnStart = i * columnSize+NUM_PIXELS;
    uint16_t   columnEnd = columnStart + columnSize;
    if(columnEnd>=NUM_PIXELS*2) columnEnd = NUM_PIXELS*2 - 1;
    for(uint16_t j=columnStart; j<columnStart+columnHeight; j++)
      if(j<NUM_PIXELS && j<=columnEnd){
        leds[j] = rBow[i];
        leds[j] %= 106;
      }
    uint16_t k = columnStart + peakHeight;
    if(k<NUM_PIXELS && k<=columnEnd)
      leds[k] = rBow[i];
  }
}
*/


/*
#define VUFadeFactor 5
#define VUScaleFactor 2.0
#define VUPaletteFactor 1.5
void drawVU() { audio = true;
  CRGB pixelColor;

  const float xScale = 255.0 / (NUM_PIXELS / 2);
  float specCombo = (spectrumDecay[0] + spectrumDecay[1] + spectrumDecay[2] + spectrumDecay[3]) / 4.0;

  for (byte x = 0; x < NUM_PIXELS / 2; x++) {
    int senseValue = specCombo / VUScaleFactor - xScale * x;
    int pixelBrightness = senseValue * VUFadeFactor;
    if (pixelBrightness > 255) pixelBrightness = 255;
    if (pixelBrightness < 0) pixelBrightness = 0;

    int pixelPaletteIndex = senseValue / VUPaletteFactor - 15;
    if (pixelPaletteIndex > 240) pixelPaletteIndex = 240;
    if (pixelPaletteIndex < 0) pixelPaletteIndex = 0;

    pixelColor = ColorFromPalette(palettes[currentPaletteIndex], pixelPaletteIndex, pixelBrightness);

    leds[x] = pixelColor;
    leds[NUM_PIXELS - x - 1] = pixelColor;
  }
}
*/
