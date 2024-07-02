#include <Arduino.h>
#include "UdawaLogger.h"
#include "UdawaSerialLogger.h"
#include "Udawa.h"

struct Config {
    uint8_t pinledR = 25;
    uint8_t pinledG = 26;
    uint8_t pinledB = 27;
    uint8_t pinGrowLight = 32;
    uint8_t pinPump = 33;

    uint8_t ledRChannel = 0;
    uint8_t ledGChannel = 1;
    uint8_t ledBChannel = 2;
    uint8_t growLightChannel = 3;
    uint8_t pumpChannel = 4;

    u_int16_t frequency = 12000;
    uint8_t resolution = 8;

    const boolean invert = false;

    uint8_t growLightBrightnessPrev = 0;
    uint8_t growLightBrightness = 0;
    uint8_t pumpPowerPrev = 0;
    uint8_t pumpPower = 0;

    uint8_t pumpPWMCutOff = 180;

    unsigned long timer = millis();
    unsigned long previousMillisGrowLight = 0;
    unsigned long previousMillisPump = 0;
    const unsigned long interval = 50; // 50ms interval for updates
    bool growLightInProgress = false;
    bool pumpInProgress = false;
    int growLightStep = 0;
    int pumpStep = 0;
};

#ifdef USE_LOCAL_WEB_INTERFACE
void _onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
#endif
#ifdef USE_IOT
void _processThingsboardSharedAttributesUpdate(const JsonObjectConst &data);
#endif

void setColor(uint8_t r, uint8_t g, uint8_t b);
void updateGrowLight();
void updatePump();