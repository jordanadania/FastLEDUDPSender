// Portions of this file are adapted from the work of Stefan Petrick:
// https://plus.google.com/u/0/115124694226931502095

// Portions of this file are adapted from RGB Shades Audio Demo Code by Garrett Mace:
// https://github.com/macetech/RGBShadesAudio
#ifdef ESP8266
  #define AUDIO_PIN  A0
  #define RESET_PIN  D4
  #define STROBE_PIN D3
#else
  #define AUDIO_PIN   36
  #define RESET_PIN   33
  #define STROBE_PIN  26
  #ifndef LED_BUILTIN
    #define LED_BUILTIN 3
  #endif
#endif

#define RGB_DECAY       64
#define ZERO_THRESHOLD  64

#define NUM_BANDS 7
#define SAMPLES 100

#define DEFAULT_BASS 1
#define DEFAULT_TREB 4

#define AUDIODELAY 10

#define SPECTRUMSMOOTH 0.1
#define PEAKDECAY 0.05
#define NOISEFLOOR 65

#define AGCSMOOTH 0.004
#define GAINUPPERLIMIT 20.0
#define GAINLOWERLIMIT 0.1


byte bassBand = DEFAULT_BASS;
byte trebBand = DEFAULT_TREB;

unsigned int spectrumValue[NUM_BANDS];
float spectrumDecay[NUM_BANDS] = {0};
float spectrumPeaks[NUM_BANDS] = {0};
float audioAvg = 270.0;
float gainAGC = 0.0;

uint8_t spectrumByte[NUM_BANDS];
uint8_t spectrumAvg;

unsigned long lastFrameMillis,
              lastDecayMillis;

void initializeAudio() {
  pinMode(AUDIO_PIN, INPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(STROBE_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  digitalWrite(STROBE_PIN, HIGH);
}

void readAudio() {
  // Jason Coon WebServer FACTORS
  static PROGMEM const byte spectrumFactors[7] = {9, 11, 13, 13, 12, 12, 13};
  // RGB SHADES FACTORS
  //static PROGMEM const byte spectrumFactors[7] = {6, 8, 8, 8, 7, 7, 10};
  digitalWrite(RESET_PIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(RESET_PIN, LOW);
  delayMicroseconds(10);
  int analogsum = 0;
  for (int i = 0; i < NUM_BANDS; i++) {
    digitalWrite(STROBE_PIN, LOW);
    delayMicroseconds(25);
    // RGB SHADES ANALOGREADx3
    spectrumValue[i] = (analogRead(AUDIO_PIN)+analogRead(AUDIO_PIN)+analogRead(AUDIO_PIN))/3;
    //spectrumValue[i] = analogRead(AUDIO_PIN);
    digitalWrite(STROBE_PIN, HIGH);
    delayMicroseconds(30);
    //Serial.println(spectrumValue[i]);
    if (spectrumValue[i] < NOISEFLOOR) {
      spectrumValue[i] = 0;
    } else {
      spectrumValue[i] -= NOISEFLOOR;
    }
    spectrumValue[i] = (spectrumValue[i]*pgm_read_byte(spectrumFactors+i));
    spectrumValue[i] /= 10;
    analogsum += spectrumValue[i];
    spectrumValue[i] *= gainAGC;
    spectrumDecay[i] = (1.0 - SPECTRUMSMOOTH) * spectrumDecay[i] + SPECTRUMSMOOTH * spectrumValue[i];
    if (spectrumPeaks[i] < spectrumDecay[i]) spectrumPeaks[i] = spectrumDecay[i];
    spectrumPeaks[i] = spectrumPeaks[i] * (1.0 - PEAKDECAY);
    spectrumByte[i] = spectrumValue[i]>>2>255?255:spectrumValue[i]>>2;
    //Serial.print(spectrumByte[i]);
    //Serial.print(" ");
  }
  //Serial.println();
  audioAvg = (1.0 - AGCSMOOTH) * audioAvg + AGCSMOOTH * (analogsum / 7.0);
  spectrumAvg = (analogsum / 7.0) / 4;
  //Serial.println(spectrumAvg);
  gainAGC = 270.0 / audioAvg;
  if (gainAGC > GAINUPPERLIMIT) gainAGC = GAINUPPERLIMIT;
  if (gainAGC < GAINLOWERLIMIT) gainAGC = GAINLOWERLIMIT;
}

// Attempt at beat detection
byte beatTriggered = 0;
#define beatLevel 36.0
#define beatDeadzone 18.0
#define beatDelay 150
float lastBeatVal = 0;
byte beatDetect() {
  static float beatAvg = 0;
  static unsigned long lastBeatMillis;
  //byte specCombo = spectrumByte[bassBand];
  //byte specCombo = (spectrumByte[0] + spectrumByte[1]) / 2;
  //float specCombo = (spectrumDecay[0] + spectrumDecay[1]) / 2 ;
  float specCombo = (spectrumDecay[bassBand] + spectrumDecay[bassBand < 6 ? bassBand+1 : bassBand-1]) / 2;
  beatAvg = (1.0 - AGCSMOOTH) * beatAvg + AGCSMOOTH * specCombo;
  //Serial.println(specCombo - beatAvg);
  if (lastBeatVal < beatAvg) lastBeatVal = beatAvg;
  if (specCombo - beatAvg > beatLevel && beatTriggered == 0 && currentMillis - lastBeatMillis > beatDelay) {
    beatTriggered = 1;
    lastBeatVal = specCombo;
    lastBeatMillis = currentMillis;
      //Serial.print(bassBand);
      //Serial.print(" ");
      //Serial.println(trebBand);
    return 1;
  } else if ((lastBeatVal - specCombo) > beatDeadzone) {
    beatTriggered = 0;
    return 0;
  } else {
    return 0;
  }
}

byte trebTriggered = 0;
#define trebLevel 24.0
#define trebDeadzone 16.0
#define trebDelay 0
float lastTrebVal = 0;
byte trebDetect() {
  static float trebAvg = 0;
  static unsigned long lastTrebMillis;
  //byte specCombo = spectrumAvg;
  byte specCombo = (spectrumByte[trebBand] + spectrumByte[trebBand == 6 ? trebBand-1 : trebBand+1]) / 2;
  //float specCombo = spectrumDecay[trebBand] / 1.0;
  trebAvg = (1.0 - AGCSMOOTH) * trebAvg + AGCSMOOTH * specCombo;

  if (lastTrebVal < trebAvg) lastTrebVal = trebAvg;
  if ((specCombo - trebAvg) > trebLevel && trebTriggered == 0 && currentMillis - lastTrebMillis > trebDelay) {
    trebTriggered = 1;
    lastTrebVal = specCombo;
    lastTrebMillis = currentMillis;
      //Serial.print(bassBand);
      //Serial.print(" ");
      //Serial.println(trebBand);
    return 1;
  } else if ((lastTrebVal - specCombo) > trebDeadzone) {
    trebTriggered = 0;
    return 0;
  } else {
    return 0;
  }
}
