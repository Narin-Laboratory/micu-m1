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

  if(config.growLightBrightness != config.growLightBrightnessPrev){
    udawa.logger->debug(PSTR(__func__), PSTR("before: %d, after: %d\n"), config.growLightBrightnessPrev, config.growLightBrightness);
    updateGrowLight();
  }
  if(config.pumpPower != config.pumpPowerPrev){
    udawa.logger->debug(PSTR(__func__), PSTR("before: %d, after: %d\n"), config.pumpPowerPrev, config.pumpPower);
    updatePump();
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
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), config.pumpPower);
}