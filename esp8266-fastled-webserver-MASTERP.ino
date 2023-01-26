#include "common.h"

#define MASTERPACKETSIZE 8

IPAddress multicast(224, 0, 1, 3);

IPAddress gateIP(192, 168, 137, 1);
IPAddress  subIP(255, 255, 255, 0);

IPAddress      masterP(192, 168, 137, 223);
IPAddress    blueWinIP(192, 168, 137, 231);
IPAddress       tv55IP(192, 168, 137, 232);
IPAddress   denWin01IP(192, 168, 137, 233);
IPAddress   denWin02IP(192, 168, 137, 234);
IPAddress    fibo256IP(192, 168, 137, 235);
IPAddress  bigDenWinIP(192, 168, 137, 236);
IPAddress laundryWinIP(192, 168, 137, 237);
IPAddress  deckWin01IP(192, 168, 137, 238);
IPAddress  deckWin02IP(192, 168, 137, 239);
IPAddress   deckWinsIP(192, 168, 137, 240); // parallel output

IPAddress   STA_IP = masterP;
IPAddress STA_GATE = IPAddress(192,168,137,1);
IPAddress  STA_SUB = IPAddress(255,255,255,0);
IPAddress  STA_DNS = IPAddress(192,168,137,1);

WiFiManager wifiManager;
ESP8266WebServer webServer(80);
ESP8266HTTPUpdateServer httpUpdateServer;

unsigned int udpPort = 8888;
WiFiUDP Udp;
byte buff[MASTERPACKETSIZE];

#if defined(KEEP_TIME)
  int utcOffsetInSeconds = -6 * 60 * 60;
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds, NTP_UPDATE_THROTTLE_MILLLISECONDS);
#endif

String nameString;

CRGB leds[NUM_PIXELS];

const uint8_t brightnessCount = 5;
const uint8_t brightnessMap[brightnessCount] = { 16, 32, 64, 128, 255 };
uint8_t brightnessIndex = DEFAULT_BRIGHTNESS_INDEX;

uint8_t secondsPerPalette = 10;

uint8_t cooling = 49;
uint8_t sparking = 60;

uint8_t speed = 1;

///////////////////////////////////////////////////////////////////////


uint8_t gCurrentPaletteNumber = 0;

CRGB              solidColor = CRGB::Black;
CRGBPalette16    IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
CRGBPalette16 RedWhiteBlue_p = CRGBPalette16(CRGB::Red, CRGB::Red, CRGB::White, CRGB::Blue);
CRGBPalette16      RedBlue_p = CRGBPalette16(CRGB::Red, CRGB::Red, CRGB::Blue, CRGB::Blue);

CRGBPalette16 gCurrentPalette(solidColor);
CRGBPalette16 gTargetPalette(gGradientPalettes[0]);

uint8_t currentPatternIndex = DEFAULT_PATTERN_INDEX; // Index number of which pattern is current
uint8_t autoplay = 0;

uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0,
              currentMillis;

uint8_t autoPaletteMode = 0;
uint8_t currentPaletteIndex = 0;

uint8_t    gHue =  0;
uint8_t  oldHue =  0;
uint8_t  newHue =  0;
uint8_t    huey =  0;
uint8_t rgbRate =  0;

uint8_t x[NUM_PIXELS];
uint8_t y[NUM_PIXELS];

void dimAll(byte value)
{
  for (auto led : leds) {
    led.nscale8(value);
  }
}

uint8_t hueDirect = true;

uint16_t edges[26];
uint8_t  modus[26];

#include "Audio.h"
#include "AudioPatterns.h"
#include "Palettes.h"

const PatternAndName patterns[] = {
  //{ off,            "Off" },          //  0
  { rainFlow,       "Rainflow" },       //  1
  { plasma,         "Plasma" },         //  2
  { mappedPlasma,   "Mapped Plasma" },  //  3
  { paletteG,       "Palette G" },      //  4
  { mappedPaletteG, "Mapped Palette G"},// 5
  { vuPalette,      "Palette Meter" },  //  6
  { sevenBands,     "7 Bands" },        //  7
  { xPalette,       "Palette X" },      //  8
  { yPalette,       "Palette Y" },      //  9
  { xyPalette,      "Palette XY" },     //  10
  { pride,          "Pride"     },      //  11
  { colorWaves,     "Color Waves" },    //  12
  { giantRuler,     "Giant Ruler" },    //  13 
  { showSolidColor, "Solid Color" }     //  14 This *must* be the last pattern
};

const uint8_t patternCount = ARRAY_SIZE2(patterns);


void setup() {
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  EEPROM.begin(512); // TODO: move settings (currently EEPROM) to fields.hpp/.cpp
  readSettings();

  initializeAudio();

  if (!MYFS.begin()) {
    Serial.println(F("An error occurred when attempting to mount the flash file system"));
  } else {
    Serial.println("FS contents:");

    Dir dir = MYFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    }
    Serial.printf("\n");
  }

  // Do a little work to get a unique-ish name. Get the
  // last two bytes of the MAC (HEX'd)":

  // copy the mac address to a byte array
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);

  // format the last two digits to hex character array, like 0A0B
  char macID[5];
  sprintf(macID, "%02X%02X", mac[WL_MAC_ADDR_LENGTH - 2], mac[WL_MAC_ADDR_LENGTH - 1]);

  String macIdString = macID;
  macIdString.toUpperCase();

//  nameString = WIFI_NAME;
  nameString = NAME_PREFIX + macIdString;

  const size_t nameCharCount = static_eval<size_t, constexpr_strlen(NAME_PREFIX) + 4>::value;
  const size_t nameBufferSize = static_eval<size_t, nameCharCount+1>::value;
  char nameChar[nameBufferSize];
  memset(nameChar, 0, nameBufferSize);
  size_t loopUntil = (nameCharCount <= nameString.length() ? nameCharCount : nameString.length());
  for (size_t i = 0; i < loopUntil; i++) {
    nameChar[i] = nameString.charAt(i);
  }

  Serial.printf("Name: %s\n", nameChar );

  wifiManager.setConfigPortalBlocking(false);
  
  wifiManager.setSTAStaticIPConfig(STA_IP,STA_GATE,STA_SUB,STA_DNS);
  //wifiManager.setSTAStaticIPConfig(STA_IP,STA_GATE,STA_SUB);
  
  if(wifiManager.autoConnect(nameChar)){
    Serial.println("Wi-Fi connected");
  }
  else {
    Serial.println("Wi-Fi manager portal running");
  }

  Udp.begin(udpPort);
  // Multi
  
  httpUpdateServer.setup(&webServer);

  webServer.on("/all", HTTP_GET, []() {
    String json = getFieldsJson();
    webServer.send(200, "application/json", json);
  });
  
  webServer.on("/product", HTTP_GET, []() {
    String json = "{\"productName\":\"" PRODUCT_FRIENDLY_NAME "\"}";
    webServer.send(200, "application/json", json);
  });

  webServer.on("/info", HTTP_GET, []() {
    String json = getInfoJson();
    webServer.send(200, "application/json", json);
  });

  webServer.on("/fieldValue", HTTP_GET, []() {
    String name = webServer.arg("name");
    String value = getFieldValue(name);
    webServer.send(200, "text/json", value);
  });

  webServer.on("/fieldValue", HTTP_POST, []() {
    String name = webServer.arg("name");
    String value = webServer.arg("value");
    String newValue = setFieldValue(name, value);
    webServer.send(200, "text/json", newValue);
  });

  webServer.on("/power", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPower(value.toInt());
    sendInt(power);
  });

  webServer.on("/cooling", HTTP_POST, []() {
    String value = webServer.arg("value");
    cooling = value.toInt();
    broadcastInt("cooling", cooling);
    sendInt(cooling);
  });

  webServer.on("/sparking", HTTP_POST, []() {
    String value = webServer.arg("value");
    sparking = value.toInt();
    broadcastInt("sparking", sparking);
    sendInt(sparking);
  });

  webServer.on("/speed", HTTP_POST, []() {
    String value = webServer.arg("value");
    speed = value.toInt();
    broadcastInt("speed", speed);
    sendInt(speed);
  });

  webServer.on("/twinkleSpeed", HTTP_POST, []() {
    String value = webServer.arg("value");
    long tmp = value.toInt();
    if (tmp < 0) {
      tmp = 0;
    } else if (tmp > 8) {
      tmp = 8;
    }
    twinkleSpeed = (uint8_t)tmp;
    writeAndCommitSettings();
    broadcastInt("twinkleSpeed", twinkleSpeed);
    sendInt(twinkleSpeed);
  });

  webServer.on("/twinkleDensity", HTTP_POST, []() {
    String value = webServer.arg("value");
    long tmp = value.toInt();
    if (tmp < 0) {
      tmp = 0;
    } else if (tmp > 8) {
      tmp = 8;
    }
    twinkleDensity = tmp;
    writeAndCommitSettings();
    broadcastInt("twinkleDensity", twinkleDensity);
    sendInt(twinkleDensity);
  });

  webServer.on("/coolLikeIncandescent", HTTP_POST, []() {
    String value = webServer.arg("value");
    long tmp = value.toInt();
    if (tmp < 0) {
      tmp = 0;
    } else if (tmp > 1) {
      tmp = 1;
    }
    coolLikeIncandescent = tmp;
    writeAndCommitSettings();
    broadcastInt("coolLikeIncandescent", coolLikeIncandescent);
    sendInt(coolLikeIncandescent);
  });

  webServer.on("/solidColor", HTTP_POST, []() {
    String r = webServer.arg("r");
    String g = webServer.arg("g");
    String b = webServer.arg("b");
    setSolidColor(r.toInt(), g.toInt(), b.toInt());
    sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
  });

  webServer.on("/pattern", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPattern(value.toInt());
    sendInt(currentPatternIndex);
  });

  webServer.on("/patternName", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPatternName(value);
    sendInt(currentPatternIndex);
  });

  webServer.on("/autoPaletteMode", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPaletteMode(value.toInt());
    sendInt(autoPaletteMode);
  });

  webServer.on("/palette", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPalette(value.toInt());
    sendInt(currentPaletteIndex);
  });

  webServer.on("/paletteName", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPaletteName(value);
    sendInt(currentPaletteIndex);
  });

  webServer.on("/brightness", HTTP_POST, []() {
    String value = webServer.arg("value");
    setBrightness(value.toInt());
    sendInt(brightness);
  });

  webServer.on("/autoplay", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoplay(value.toInt());
    sendInt(autoplay);
  });

  webServer.on("/autoplayDuration", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoplayDuration(value.toInt());
    sendInt(autoplayDuration);
  });

  webServer.on("/list", HTTP_GET, handleFileList);
  webServer.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) webServer.send(404, "text/plain", "FileNotFound");
  });
  
  webServer.on("/edit", HTTP_PUT, handleFileCreate);
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  webServer.on("/edit", HTTP_POST, []() {
        webServer.send(200, "text/plain", "");
      }, handleFileUpload);

  webServer.enableCORS(true);
  webServer.serveStatic("/", LittleFS, "/", "max-age=86400");

  MDNS.begin(nameChar);
  MDNS.setHostname(nameChar);

  webServer.begin();
  Serial.println("HTTP web server started");

  autoPlayTimeout = millis() + (autoplayDuration * 1000);

  for(uint16_t i=0; i<NUM_PIXELS; i++){
    uint8_t angle = (i*256)/NUM_PIXELS;
    x[i] = cos8(angle);
    y[i] = sin8(angle);
  }
}

void sendInt(uint8_t value)
{
  sendString(String(value));
}

void sendString(String value)
{
  webServer.send(200, "text/plain", value);
}

void broadcastInt(String name, uint8_t value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
  //  webSocketsServer.broadcastTXT(json);
}

void broadcastString(String name, String value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
  //  webSocketsServer.broadcastTXT(json);
}


void loop() {
  random16_add_entropy(analogRead(AUDIO_PIN));

  wifiManager.process();
  webServer.handleClient();
  MDNS.update();

  static bool hasConnected = false;

  EVERY_N_SECONDS(1) {
    if (WiFi.status() != WL_CONNECTED) {
      //      Serial.printf("Connecting to %s\n", ssid);
      hasConnected = false;
    }
    else if (!hasConnected) {
      hasConnected = true;
      MDNS.begin(nameString);
      MDNS.setHostname(nameString);
      webServer.begin();
      Serial.println("HTTP web server started");
      Serial.print("Connected! Open http://");
      Serial.print(WiFi.localIP());
      Serial.print(" or http://");
      Serial.print(nameString);
      Serial.println(".local in your browser");
    }
  }

  checkPingTimer();
//  handleIrInput();  // empty function when ENABLE_IR is not defined
/*
  if (power == 0) {
    fill_solid(leds, NUM_PIXELS, CRGB::Black);
    FastLED.delay(1000 / FRAMES_PER_SECOND); // this function calls FastLED.show() at least once
    return;
  }
  */
  
  if (power == 0) {
     buff[0] = 0;
     sendPackets();
     FastLED.delay(1000 / FRAMES_PER_SECOND);
     return;
  }
  
  currentMillis = millis();
    if(lastFrameMillis - currentMillis > AUDIODELAY)
      readAudio();
    lastFrameMillis = currentMillis;
  
  EVERY_N_SECONDS( secondsPerPalette ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 8);
    gHue++;
  }

  if (autoplay && (currentMillis > autoPlayTimeout)) {
    adjustPattern(true);
    autoPlayTimeout = currentMillis + (autoplayDuration * 1000);
  }

  patterns[currentPatternIndex].pattern();
  if(patterns[currentPatternIndex].name != "sevenBands")
    sendPackets();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
}

const uint8_t SETTINGS_MAGIC_BYTE = 0x96;
void readSettings(){
  if (EEPROM.read(511) != SETTINGS_MAGIC_BYTE) { return; }

  brightness = EEPROM.read(0);

  currentPatternIndex = EEPROM.read(1);
  if (currentPatternIndex >= patternCount) {
    currentPatternIndex = patternCount - 1;
  }

  byte r = EEPROM.read(2);
  byte g = EEPROM.read(3);
  byte b = EEPROM.read(4);

  if (r == 0 && g == 0 && b == 0)
  {} else { solidColor = CRGB(r, g, b); }

  power = EEPROM.read(5);

  autoplay = EEPROM.read(6);
  autoplayDuration = EEPROM.read(7);

  currentPaletteIndex = EEPROM.read(8);
  if (currentPaletteIndex >= paletteCount) {
    currentPaletteIndex = paletteCount - 1;
  }

  twinkleSpeed = EEPROM.read(9);
  twinkleDensity = EEPROM.read(10);

  cooling = EEPROM.read(11);
  sparking = EEPROM.read(12);

  coolLikeIncandescent = EEPROM.read(13);
}

void writeAndCommitSettings() {
  EEPROM.write(0, brightness);
  EEPROM.write(1, currentPatternIndex);
  EEPROM.write(2, solidColor.r);
  EEPROM.write(3, solidColor.g);
  EEPROM.write(4, solidColor.b);
  EEPROM.write(5, power);
  EEPROM.write(6, autoplay);
  EEPROM.write(7, autoplayDuration);
  EEPROM.write(8, currentPaletteIndex);
  EEPROM.write(9, twinkleSpeed);
  EEPROM.write(10, twinkleDensity);
  EEPROM.write(11, cooling);
  EEPROM.write(12, sparking);
  EEPROM.write(13, coolLikeIncandescent);
  EEPROM.write(17, autoPaletteMode);
  EEPROM.write(511, SETTINGS_MAGIC_BYTE);
  EEPROM.commit();
}

void setPower(uint8_t value){
  power = value == 0 ? 0 : 1;
  writeAndCommitSettings();
  broadcastInt("power", value);
}
void setPaletteMode(uint8_t value){
  autoPaletteMode = value == 0 ? 0 : 1;
  writeAndCommitSettings();
  broadcastInt("autoPaletteMode", autoPaletteMode);
}
void setAutoplay(uint8_t value){
  autoplay = value == 0 ? 0 : 1;
  writeAndCommitSettings();
  broadcastInt("autoplay", autoplay);
}

void setAutoplayDuration(uint8_t value){
  autoplayDuration = value;
  writeAndCommitSettings();
  autoPlayTimeout = millis() + (autoplayDuration * 1000);

  broadcastInt("autoplayDuration", autoplayDuration);
}

void setSolidColor(CRGB color){ setSolidColor(color.r, color.g, color.b); }

void setSolidColor(uint8_t r, uint8_t g, uint8_t b){
  solidColor = CRGB(r, g, b);
  writeAndCommitSettings();
  setPattern(patternCount - 1);

  broadcastString("color", String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
}

// increase or decrease the current pattern number, and wrap around at the ends
void adjustPattern(bool up){
  if (up)
    currentPatternIndex++;
  else
    currentPatternIndex--;

  // wrap around at the end
  if (currentPatternIndex >= patternCount) {
    currentPatternIndex = 0;
  }

  if (autoplay == 0) {
    writeAndCommitSettings();
  }

  broadcastInt("pattern", currentPatternIndex);
}

void setPattern(uint8_t value){
  if (value >= patternCount)
    value = patternCount - 1;

  currentPatternIndex = value;

  if (autoplay == 0) {
    writeAndCommitSettings();
  }

  broadcastInt("pattern", currentPatternIndex);
}

void setPatternName(String name){
  for (uint8_t i = 0; i < patternCount; i++) {
    if (patterns[i].name == name) {
      setPattern(i);
      break;
    }
  }
}

void setPalette(uint8_t value){
  if (value >= paletteCount)
    value = paletteCount - 1;

  currentPaletteIndex = value;
  writeAndCommitSettings();
  broadcastInt("palette", currentPaletteIndex);
}

void setPaletteName(String name){
  for (uint8_t i = 0; i < paletteCount; i++) {
    if (paletteNames[i] == name) {
      setPalette(i);
      break;
    }
  }
}

void adjustBrightness(bool up){
  if (up && brightnessIndex < brightnessCount - 1)
    brightnessIndex++;
  else if (!up && brightnessIndex > 0)
    brightnessIndex--;

  brightness = brightnessMap[brightnessIndex];

  FastLED.setBrightness(brightness);
  writeAndCommitSettings();
  broadcastInt("brightness", brightness);
}

void setBrightness(uint8_t value){
  brightness = value;

  FastLED.setBrightness(brightness);
  writeAndCommitSettings();
  broadcastInt("brightness", brightness);
}

void blueWinPacket(){
  Udp.beginPacket(blueWinIP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void tv55Packet(){  
  Udp.beginPacket(tv55IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void denWin01Packet(){
  Udp.beginPacket(denWin01IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void fibo256Packet(){
  Udp.beginPacket(fibo256IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void denWin02Packet(){
  Udp.beginPacket(denWin02IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void bigDenWinPacket(){  
  Udp.beginPacket(bigDenWinIP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void laundryWinPacket(){
  Udp.beginPacket(laundryWinIP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();  
}
void deckWinsPacket(){
  Udp.beginPacket(deckWinsIP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void deckWin01Packet(){
  Udp.beginPacket(deckWin01IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}
void deckWin02Packet(){
  Udp.beginPacket(deckWin02IP, udpPort);
  Udp.write(buff, MASTERPACKETSIZE);
  Udp.endPacket();
}

void sendPackets(){
     blueWinPacket();
        tv55Packet();
    denWin01Packet();
  //fibo256Packet();
    denWin02Packet();
   bigDenWinPacket();
  laundryWinPacket();
    deckWinsPacket();
  //deckWin01Packet();
  //deckWin02Packet();
}

//void sendPackets(){
//  Udp.beginPacketMulticast(multicast, udpPort, masterP);
//  Udp.write(buff, MASTERPACKETSIZE);
//  Udp.endPacket();
//}
void xPalette(){
  buff[0] = 8;
  buff[1] = speed;
  buff[2] = brightness;
  buff[3] = gHue;
  buff[4] = currentPaletteIndex;
}
void yPalette(){
  buff[0] = 9;
  buff[1] = speed;
  buff[2] = brightness;
  buff[3] = gHue;
  buff[4] = currentPaletteIndex;
}
void xyPalette(){
  buff[0] = 10;
  buff[1] = speed;
  buff[2] = brightness;
  buff[3] = gHue;
  buff[4] = currentPaletteIndex;
}

void pride(){
  uint8_t msmultiplier = beatsin88(74, 23, 60);
  buff[0] = 11;
  buff[1] = brightness;
  buff[2] = msmultiplier;
}

void colorWaves(){
  uint8_t msmultiplier = beatsin88(74, 23, 60);
  buff[0] = 12;
  buff[1] = brightness;
  buff[2] = msmultiplier;
}

void giantRuler(){
  buff[0] = 13;
}

void showSolidColor(){
  buff[0] = 14;
  buff[1] = brightness;
  buff[2] = solidColor.r;
  buff[3] = solidColor.g;
  buff[4] = solidColor.b;
}
