#include "main.h"

Udawa udawa;
Config config;

void setup() {
  #ifdef USE_LOCAL_WEB_INTERFACE
  udawa.addOnWsEvent(_onWsEvent);
  #endif
  #ifdef USE_IOT
  udawa.addOnThingsboardSharedAttributesReceived(_processThingsboardSharedAttributesUpdate);
  #endif
  udawa.begin();
  
  if (!udawa.crashState.fSafeMode) {
    loadConfig();

    // Setup LEDC channels
    ledcSetup(config.ledRChannel, config.frequency, config.resolution); // channel 0, 12 kHz, 8-bit resolution
    ledcSetup(config.ledGChannel, config.frequency, config.resolution); // channel 1, 12 kHz, 8-bit resolution
    ledcSetup(config.ledBChannel, config.frequency, config.resolution); // channel 2, 12 kHz, 8-bit resolution
    ledcSetup(config.growLightChannel, config.frequency, config.resolution); // channel 3, 12 kHz, 8-bit resolution
    ledcSetup(config.pumpChannel, config.frequency, config.resolution); // channel 4, 12 kHz, 8-bit resolution

    // Attach pins to LEDC channels
    ledcAttachPin(config.pinledR, config.ledRChannel); // pin 25, channel 0
    ledcAttachPin(config.pinledG, config.ledGChannel); // pin 26, channel 1
    ledcAttachPin(config.pinledB, config.ledBChannel); // pin 27, channel 2
    ledcAttachPin(config.pinGrowLight, config.growLightChannel); // pin 27, channel 3
    ledcAttachPin(config.pinPump, config.pumpChannel); // pin 27, channel 4

    setColor(5, 0, 0);

    profiles[0] = {1, "Arugula", 1209600000};    // 14 days
    profiles[1] = {2, "Broccoli", 864000000};     // 10 days
    profiles[2] = {3, "Cabbage", 864000000};     // 10 days
    profiles[3] = {4, "Cilantro", 1209600000};    // 14 days
    profiles[4] = {5, "Kale", 1036800000};       // 12 days
    profiles[5] = {6, "Kohlrabi", 604800000};     // 7 days
    profiles[6] = {7, "Mustard", 691200000};     // 8 days
    profiles[7] = {8, "Pea Shoots", 1209600000}; // 14 days
    profiles[8] = {9, "Radish", 604800000};      // 7 days
    profiles[9] = {10, "Sunflower", 864000000};   // 10 days
    profiles[10] = {11, "Amaranth", 691200000};   // 8 days
    profiles[11] = {12, "Beet", 1555200000};     // 18 days
    profiles[12] = {13, "Buckwheat", 604800000};  // 7 days
    profiles[13] = {14, "Chard", 1036800000};    // 12 days
    profiles[14] = {15, "Chia", 691200000};      // 8 days
    profiles[15] = {16, "Fenugreek", 691200000};  // 8 days
    profiles[16] = {17, "Lettuce", 864000000};    // 10 days
    profiles[17] = {18, "Mizuna", 864000000};    // 10 days
    profiles[18] = {19, "Pak Choi", 864000000};   // 10 days
    profiles[19] = {20, "Watercress", 691200000}; // 8 days

    if(config.xSemaphoreGrowControl == NULL){config.xSemaphoreGrowControl = xSemaphoreCreateMutex();}

    if(config.xHandleGrowControl == NULL){
      config.xReturnedGrowControl = xTaskCreatePinnedToCore(_pvTaskCodeGrowControl, PSTR("GrowControl"), GROW_CONTROL_STACKSIZE, NULL, 1, &config.xHandleGrowControl, 1);
      if(config.xReturnedGrowControl == pdPASS){
        udawa.logger->warn(PSTR(__func__), PSTR("Task GrowControl has been created.\n"));
      }
    }

    if(config.growLightBrightness > 0){
      config.growLightBrightnessPrev = 0;
    }
    if(config.pumpPower > 0){
      config.pumpPowerPrev = 0;
    }
  
    saveConfig();
  }
}

unsigned long timer = millis();
void loop() {
  udawa.run();

  if (!udawa.crashState.fSafeMode) {
    if (millis() - timer > 1000) {
      udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), ESP.getFreeHeap());
      timer = millis();
    }
  }
}

#ifdef USE_LOCAL_WEB_INTERFACE
void _onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    // Handle WebSocket connection event
  } else if (type == WS_EVT_DATA) {
    // Handle WebSocket data event
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data);

    String cmd = "";
    if(doc["cmd"] != nullptr){
      cmd = doc["cmd"].as<String>();
    }


    if(cmd == "setGrowLight"){
      if(doc["brightness"] != nullptr){
        config.growLightBrightness = doc["brightness"].as<uint8_t>();
        udawa.logger->debug(PSTR(__func__), PSTR("brightness: %d\n"), doc["brightness"].as<uint8_t>());
      }
    }
    else if(cmd == "setPump"){
      if(doc["power"] != nullptr){
        config.pumpPower = doc["power"].as<uint8_t>();
        udawa.logger->debug(PSTR(__func__), PSTR("power: %d\n"), doc["power"].as<uint8_t>());
      }
    }
    else if(cmd == "setSowingTS"){
      if(doc["ts"] != nullptr){
        config.sowingTS = doc["ts"].as<unsigned long>();
        udawa.logger->debug(PSTR(__func__), PSTR("SowingTS: %d\n"), doc["ts"].as<unsigned long>());
      }
    }
    else if(cmd == "setProfile"){
      if(doc["id"] != nullptr){
        config.selectedProfile = doc["id"].as<uint8_t>();
        udawa.logger->debug(PSTR(__func__), PSTR("selected profile: %d\n"), doc["id"].as<uint8_t>());
      }
    }
    else if(cmd == "setMode"){
      if(doc["mode"] != nullptr){
        config.mode = doc["mode"].as<uint8_t>();
        udawa.logger->debug(PSTR(__func__), PSTR("Operation mode: %d\n"), doc["mode"].as<uint8_t>());
      }
    }
    else if(cmd == "setDateTime"){
      if(doc["ts"] != nullptr){
        config.selectedProfile = doc["ts"].as<uint8_t>();
        udawa.logger->debug(PSTR(__func__), PSTR("setDateTime: %d\n"), doc["ts"].as<unsigned long>());
      }
    }
    else if(cmd == "saveConfig"){
      saveConfig();
    }
    else if(cmd == "syncConfig"){
      //syncConfig();
    }
    else if(cmd == "reboot"){
      udawa.reboot(3);
    }

  }
}
#endif

#ifdef USE_IOT
void _processThingsboardSharedAttributesUpdate(const JsonObjectConst &data) {
  String _data;
  serializeJson(data, _data);
  udawa.logger->debug(PSTR(__func__), PSTR("From main.cpp %s\n"), _data.c_str());
}
#endif

void setColor(uint8_t r, uint8_t g, uint8_t b){
  if(config.invert){
    ledcWrite(config.ledRChannel, r);
    ledcWrite(config.ledGChannel, g);
    ledcWrite(config.ledBChannel, b);
    udawa.logger->debug(PSTR(__func__), PSTR("R: %d, G: %d, B: %d\n"), r, g, b);
  }else{
    ledcWrite(config.ledRChannel, 255 - r);
    ledcWrite(config.ledGChannel, 255 - g);
    ledcWrite(config.ledBChannel, 255 - b);
    udawa.logger->debug(PSTR(__func__), PSTR("R: %d, G: %d, B: %d\n"), 255-r, 255-g, 255-b);
  }
}

void updateGrowLight(){
  uint8_t valuePrev = map(config.growLightBrightnessPrev, 0, 100, 0, 255);
  uint8_t valueCurr = map(config.growLightBrightness, 0, 100, 0, 255);
  if(valueCurr >= valuePrev){
    for(int i = valuePrev; i <= valueCurr; i++){
      ledcWrite(config.growLightChannel, i);
      udawa.logger->debug(PSTR(__func__), PSTR("New value is greater: %d\n"), i);
      vTaskDelay((const TickType_t) 5 / portTICK_PERIOD_MS);
    }
  }else{
    for(int i = valuePrev; i >= valueCurr; i--){
      ledcWrite(config.growLightChannel, i);
      udawa.logger->debug(PSTR(__func__), PSTR("New value is lesser: %d\n"), i);
      vTaskDelay((const TickType_t) 5 / portTICK_PERIOD_MS);
    }
  }
  config.growLightBrightnessPrev = config.growLightBrightness;
  saveConfig();
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), config.growLightBrightness);
}

void updatePump(){
  uint8_t valuePrev = map(config.pumpPowerPrev, 0, 100, config.pumpPWMCutOff, 255);
  uint8_t valueCurr = map(config.pumpPower, 0, 100, config.pumpPWMCutOff, 255);
  if(valueCurr >= valuePrev){
    for(int i = valuePrev; i <= valueCurr; i++){
      ledcWrite(config.pumpChannel, i <= config.pumpPWMCutOff ? 0 : i);
      udawa.logger->debug(PSTR(__func__), PSTR("New value is greater: %d\n"), i);
      vTaskDelay((const TickType_t) 10 / portTICK_PERIOD_MS);
    }
  }else{
    for(int i = valuePrev; i >= valueCurr; i--){
      ledcWrite(config.pumpChannel, i <= config.pumpPWMCutOff ? 0 : i);
      udawa.logger->debug(PSTR(__func__), PSTR("New value is lesser: %d\n"), i);
      vTaskDelay((const TickType_t) 10 / portTICK_PERIOD_MS);
    }
  }
  config.pumpPowerPrev = config.pumpPower;
  saveConfig();
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), config.pumpPower);
}

void _pvTaskCodeGrowControl(void*){
  while(true){
    if(config.growLightBrightness != config.growLightBrightnessPrev){
      udawa.logger->debug(PSTR(__func__), PSTR("before: %d, after: %d\n"), config.growLightBrightnessPrev, config.growLightBrightness);
      updateGrowLight();
    }
    if(config.pumpPower != config.pumpPowerPrev){
      udawa.logger->debug(PSTR(__func__), PSTR("before: %d, after: %d\n"), config.pumpPowerPrev, config.pumpPower);
      updatePump();
    }

    // Manual mode
    if(config.mode == 0){
      
    }
    // Auto mode
    else if(config.mode == 1){
      unsigned long currentTS = udawa.RTC.getEpoch() * 1000;
      if(currentTS - config.sowingTS <= profiles[config.selectedProfile - 1].incubationTS){

      }else{
        
      }
    }


    vTaskDelay((const TickType_t) 1 / portTICK_PERIOD_MS);
  }
}

void loadConfig(){
  JsonDocument doc;
  bool status = configHelper.load(doc);
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), (int)status);
  if(status){
    if(doc[PSTR("sowingTS")] != nullptr){config.sowingTS = doc[PSTR("sowingTS")].as<unsigned long>();} else{config.sowingTS = udawa.RTC.getEpoch() * 1000;}
    if(doc[PSTR("selectedProfile")] != nullptr){config.selectedProfile = doc[PSTR("selectedProfile")].as<uint8_t>();} else{config.selectedProfile = 1;}
    if(doc[PSTR("mode")] != nullptr){config.mode = doc[PSTR("mode")].as<uint8_t>();} else{config.mode = 0;}
    if(doc[PSTR("growLightBrightnessPrev")] != nullptr){config.growLightBrightnessPrev = doc[PSTR("growLightBrightnessPrev")].as<uint8_t>();}
    if(doc[PSTR("growLightBrightness")] != nullptr){config.growLightBrightness = doc[PSTR("growLightBrightness")].as<uint8_t>();}
    if(doc[PSTR("pumpPowerPrev")] != nullptr){config.pumpPowerPrev = doc[PSTR("pumpPowerPrev")].as<uint8_t>();}
    if(doc[PSTR("pumpPower")] != nullptr){config.pumpPower = doc[PSTR("pumpPower")].as<uint8_t>();}
  }
}

void saveConfig(){
  JsonDocument doc;
  doc[PSTR("sowingTS")] = config.sowingTS;
  doc[PSTR("selectedProfile")] = config.selectedProfile;
  doc[PSTR("mode")] = config.mode;
  doc[PSTR("growLightBrightnessPrev")] = config.growLightBrightnessPrev;
  doc[PSTR("growLightBrightness")] = config.growLightBrightness;
  doc[PSTR("pumpPowerPrev")] = config.pumpPowerPrev;
  doc[PSTR("pumpPower")] = config.pumpPower;
  bool status = configHelper.save(doc);
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), (int)status);
}