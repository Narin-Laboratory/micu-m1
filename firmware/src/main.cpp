#include "main.h"

Udawa udawa;
Config config;

void setup() {
  #ifdef USE_LOCAL_WEB_INTERFACE
  udawa.addOnWsEvent(_onWsEventMain);
  #endif
  #ifdef USE_IOT
  udawa.addOnThingsboardSharedAttributesReceived(_processThingsboardSharedAttributesUpdate);
  #endif
  udawa.addOnSyncClientAttributesCallback(syncClientAttr);
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

    profiles[0] = {1, "Arugula", 14};    // 14 days
    profiles[1] = {2, "Broccoli", 10};     // 10 days
    profiles[2] = {3, "Cabbage", 10};     // 10 days
    profiles[3] = {4, "Cilantro", 14};    // 14 days
    profiles[4] = {5, "Kale", 12};       // 12 days
    profiles[5] = {6, "Kohlrabi", 7};     // 7 days
    profiles[6] = {7, "Mustard", 8};     // 8 days
    profiles[7] = {8, "Pea Shoots", 14}; // 14 days
    profiles[8] = {9, "Radish", 7};      // 7 days
    profiles[9] = {10, "Sunflower", 10};   // 10 days
    profiles[10] = {11, "Amaranth", 8};   // 8 days
    profiles[11] = {12, "Beet", 18};     // 18 days
    profiles[12] = {13, "Buckwheat", 7};  // 7 days
    profiles[13] = {14, "Chard", 12};    // 12 days
    profiles[14] = {15, "Chia", 8};      // 8 days
    profiles[15] = {16, "Fenugreek", 8};  // 8 days
    profiles[16] = {17, "Lettuce", 10};    // 10 days
    profiles[17] = {18, "Mizuna", 10};    // 10 days
    profiles[18] = {19, "Pak Choi", 10};   // 10 days
    profiles[19] = {20, "Watercress", 8}; // 8 days
    profiles[20] = {21, "Strawberry", 365}; // 365 days

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

  unsigned long now = millis();
  if (!udawa.crashState.fSafeMode) {
    if (now - timer > 1000) {
      udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), ESP.getFreeHeap());

      #ifdef USE_LOCAL_WEB_INTERFACE
      if(udawa.ws.count() > 0){
        JsonDocument doc;
        JsonObject sysInfo = doc[PSTR("sysInfo")].to<JsonObject>();

        sysInfo[PSTR("heap")] = ESP.getFreeHeap();
        sysInfo[PSTR("uptime")] = now;
        sysInfo[PSTR("datetime")] = udawa.RTC.getDateTime();
        sysInfo[PSTR("rssi")] = udawa.wiFiHelper.rssiToPercent(WiFi.RSSI());

        udawa.wsBroadcast(doc);
      }
      #endif

      if(config.mode == 0){
        setColor(5, 0, 0);
      }
      else if(config.mode == 1){
        setColor(0, 0, 5);
      }

      timer = now;
    }
  }
}

#ifdef USE_LOCAL_WEB_INTERFACE
void _onWsEventMain(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    // Handle WebSocket connection event
  } else if (type == WS_EVT_DATA) {
    // Handle WebSocket data event
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, data);

    if(doc["brightness"] != nullptr){
      config.growLightBrightness = doc["brightness"].as<uint8_t>();
      udawa.logger->debug(PSTR(__func__), PSTR("brightness: %d\n"), doc["brightness"].as<uint8_t>());
    }
    else if(doc["power"] != nullptr){
      config.pumpPower = doc["power"].as<uint8_t>();
      udawa.logger->debug(PSTR(__func__), PSTR("power: %d\n"), doc["power"].as<uint8_t>());
    }
    else if(doc["sowingDatetime"] != nullptr){
      config.sowingDatetime = doc["sowingDatetime"].as<String>();
      udawa.logger->debug(PSTR(__func__), PSTR("sowingDatetime: %s\n"), config.sowingDatetime.c_str());
      saveConfig();
      syncClientAttr(2);
    }
    else if(doc["profile"] != nullptr){
      config.selectedProfile = doc["profile"].as<uint8_t>();
      udawa.logger->debug(PSTR(__func__), PSTR("selected profile: %i\n"), doc["profile"].as<uint8_t>());
      saveConfig();
      syncClientAttr(2);
    }
    else  if(doc["mode"] != nullptr){
      config.mode = doc["mode"].as<uint8_t>();
      udawa.logger->debug(PSTR(__func__), PSTR("Operation mode: %d\n"), doc["mode"].as<uint8_t>());
      saveConfig();
      syncClientAttr(2);
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
    //udawa.logger->debug(PSTR(__func__), PSTR("R: %d, G: %d, B: %d\n"), r, g, b);
  }else{
    ledcWrite(config.ledRChannel, 255 - r);
    ledcWrite(config.ledGChannel, 255 - g);
    ledcWrite(config.ledBChannel, 255 - b);
    //udawa.logger->debug(PSTR(__func__), PSTR("R: %d, G: %d, B: %d\n"), 255-r, 255-g, 255-b);
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
  config.growLightState = config.growLightBrightness > 0 ? true : false;
  saveConfig();
  syncClientAttr(2);
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
  config.pumpState = config.pumpPower > 0 ? true : false;
  saveConfig();
  syncClientAttr(2);
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
      if(!config.pumpState){
        config.pumpLastOn = 0;
      }else{
        config.pumpLastOn = millis();
      }
    }

    // Manual mode
    if(config.mode == 0){
      
    }
    // Auto mode
    else if(config.mode == 1){
      
      // Get current time from the ESP32Time object
      time_t currentTime = udawa.RTC.getEpoch(); // Get time in seconds

      // Parse the sowing datetime string into a tm struct
      struct tm timeinfo;
      // Get the current hour
      struct tm current_tm;
      localtime_r(&currentTime, &current_tm);
      int currentHour = current_tm.tm_hour;

      if (strptime(config.sowingDatetime.c_str(), "%Y-%m-%d %H:%M:%S", &timeinfo) != NULL) {

        // Convert the tm struct to seconds since epoch
        time_t sowingTime = mktime(&timeinfo);

        // Calculate remaining time until harvest
        if (config.selectedProfile >= 1 && config.selectedProfile <= 21) {
          config.remainingTimeToHarvest = (profiles[config.selectedProfile - 1].incubationTS * 24 * 3600) - (currentTime - sowingTime);
        } else {
          // Handle invalid selectedProfile value
        }
        

        // Update "Siap panen dalam" based on the remaining time
        // ... your code to update the UI with the remaining time ...
        
        // Example:
        if (config.remainingTimeToHarvest > 0) {
          // Still growing
          // You can display the remaining time in days, hours, minutes, etc.
          JsonDocument doc;
          char buffer[512];
          doc[PSTR("micuM1Stream")][PSTR("remainingTimeToHarvest")] = config.remainingTimeToHarvest;
          serializeJson(doc, buffer);
          udawa.wsBroadcast(buffer);

        } else {
          // Ready to harvest
          setColor(0, 255, 0); // Turn LED green for harvest
          udawa.setAlarm(111, 0, 0, 0);
          continue;
        }

        // Check if it's daytime (6 AM - 6 PM)
        if (currentHour >= 6 && currentHour < 18) {
          uint8_t intensity = calculateDaylightIntensity(current_tm);
          config.growLightBrightness = intensity;
          udawa.logger->debug(PSTR(__func__), PSTR("Daylight intensity: %d\n"), intensity);
          if (!config.growLightState) {
            udawa.logger->debug(PSTR(__func__), PSTR("before: %d, after: %d\n"), config.growLightBrightnessPrev, config.growLightBrightness);
          }
        } 
        else {
          // Turn off the grow light
          if (config.growLightState) {
            config.growLightBrightness = 0;
          }
        }

        unsigned long currentMillis = millis();
        // Water the plants every 12 hours for 5 seconds
        if (currentHour % 12 == 0 && !config.fPumpActivated) { 
          config.fPumpActivated = true;
          config.pumpPower = 100;
          config.pumpLastOn = millis();
          udawa.logger->debug(PSTR(__func__), PSTR("currentHour: %d, fPumpActivated: %d\n"), currentHour, config.fPumpActivated);
        } 
        else{ 
          if(currentHour > 12){
            config.fPumpActivated = false;
          }
          config.pumpPower = 0;
          udawa.logger->debug(PSTR(__func__), PSTR("currentHour: %d, fPumpActivated: %d\n"), currentHour, config.fPumpActivated);
        }
      } else {
        // Handle error: invalid sowing datetime
        udawa.logger->error(PSTR(__func__), PSTR("Error parsing datetime string: %s\n"), config.sowingDatetime.c_str());
      }
    }

    //Pump failsafe
    if(millis() - config.pumpLastOn > 5000){
      config.pumpPower = 0;
    }


    vTaskDelay((const TickType_t) 1000 / portTICK_PERIOD_MS);
  }
}

void loadConfig(){
  JsonDocument doc;
  bool status = configHelper.load(doc);
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), (int)status);
  if(status){
    if(doc[PSTR("sowingDatetime")] != nullptr){config.sowingDatetime = doc[PSTR("sowingDatetime")].as<String>();} else{config.sowingDatetime = "2024-07-09 08:00:00";}
    if(doc[PSTR("selectedProfile")] != nullptr){config.selectedProfile = doc[PSTR("selectedProfile")].as<uint8_t>();} else{config.selectedProfile = 1;}
    if(doc[PSTR("mode")] != nullptr){config.mode = doc[PSTR("mode")].as<uint8_t>();} else{config.mode = 0;}
    if(doc[PSTR("growLightBrightnessPrev")] != nullptr){config.growLightBrightnessPrev = doc[PSTR("growLightBrightnessPrev")].as<uint8_t>();}
    if(doc[PSTR("growLightBrightness")] != nullptr){config.growLightBrightness = doc[PSTR("growLightBrightness")].as<uint8_t>();}
    if(doc[PSTR("pumpPowerPrev")] != nullptr){config.pumpPowerPrev = doc[PSTR("pumpPowerPrev")].as<uint8_t>();}
    if(doc[PSTR("pumpPower")] != nullptr){config.pumpPower = doc[PSTR("pumpPower")].as<uint8_t>();}
    if(doc[PSTR("growLightState")] != nullptr){config.growLightState = doc[PSTR("growLightState")].as<bool>();}
    if(doc[PSTR("pumpState")] != nullptr){config.pumpState = doc[PSTR("pumpState")].as<bool>();}
  }
}

void saveConfig(){
  JsonDocument doc;
  doc[PSTR("sowingDatetime")] = config.sowingDatetime;
  doc[PSTR("selectedProfile")] = config.selectedProfile;
  doc[PSTR("mode")] = config.mode;
  doc[PSTR("growLightBrightnessPrev")] = config.growLightBrightnessPrev;
  doc[PSTR("growLightBrightness")] = config.growLightBrightness;
  doc[PSTR("pumpPowerPrev")] = config.pumpPowerPrev;
  doc[PSTR("pumpPower")] = config.pumpPower;
  doc[PSTR("growLightState")] = config.growLightState;
  doc[PSTR("pumpState")] = config.pumpState;
  bool status = configHelper.save(doc);
  udawa.logger->debug(PSTR(__func__), PSTR("%d\n"), (int)status);
}

void syncClientAttr(uint8_t direction){
  String ip = WiFi.localIP().toString();
  
  JsonDocument doc;

  #ifdef USE_IOT
  if(tb.connected() && (direction == 0 || direction == 1) ){
    /..
    serializeJson(doc, buffer);
    udawa.iotSendAttributes(buffer);
    doc.clear();
  }
  #endif

  #ifdef USE_LOCAL_WEB_INTERFACE
  if((direction == 0 || direction == 2)){
    doc.clear();
    JsonObject micuM1State = doc[PSTR("micuM1State")].to<JsonObject>();
    micuM1State[PSTR("growLightState")] = config.growLightState;
    udawa.logger->debug(PSTR(__func__), PSTR("growLightState: %d brightness: %d\n"), config.growLightState, config.growLightBrightness);
    micuM1State[PSTR("pumpState")] = config.pumpState;
    micuM1State[PSTR("mode")] = config.mode;
    micuM1State[PSTR("sowingDatetime")] = config.sowingDatetime;
    micuM1State[PSTR("selectedProfile")] = config.selectedProfile;
    udawa.wsBroadcast(doc);
    udawa.wsBroadcast(doc);

    doc.clear();
    JsonArray profilesArray = doc[PSTR("profiles")].to<JsonArray>();
    for (int i = 0; i < countof(profiles); i++) {
      profilesArray[i][PSTR("id")] = profiles[i].id;
      profilesArray[i][PSTR("name")] = profiles[i].name;
      profilesArray[i][PSTR("incubationTS")] = profiles[i].incubationTS;
    }
    udawa.wsBroadcast(doc);
  }
  #endif
}

// Function to calculate daylight intensity based on real date and time
int calculateDaylightIntensity(struct tm current_tm) {
  // Define the start and end of daylight hours
  int startHour = 6; // 6 AM
  int endHour = 18; // 6 PM

  // Calculate the total duration of daylight in hours
  int daylightDuration = endHour - startHour;

  // Calculate the midpoint of the daylight hours
  int midDay = startHour + daylightDuration / 2; // Midday is at 12 PM

  // Calculate the current hour
  int currentHour = current_tm.tm_hour;

  // Calculate the hour difference from midday
  int hourFromMidDay = abs(currentHour - midDay);

  // Define the maximum intensity at midday
  int maxIntensity = 100;

  // Calculate the intensity based on the time of day
  int intensity = maxIntensity - (hourFromMidDay * (maxIntensity / (daylightDuration / 2)));

  // Ensure the intensity is within the valid range
  if (intensity < 0) {
    intensity = 0;
  } else if (intensity > maxIntensity) {
    intensity = maxIntensity;
  }

  return intensity;
}