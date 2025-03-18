#include <Arduino.h>
#include "UdawaLogger.h"
#include "UdawaSerialLogger.h"
#include "Udawa.h"
#include "UdawaConfig.h"
#include <time.h>

struct Config {
    SemaphoreHandle_t xSemaphoreGrowControl = NULL;
    TaskHandle_t xHandleGrowControl;
    BaseType_t xReturnedGrowControl;

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
    bool growLightState = false;
    bool pumpState = false;
    unsigned long pumpLastOn = 0;

    uint8_t pumpPWMCutOff = 180;

    String sowingDatetime = "2024-07-09 08:00:00";
    uint8_t selectedProfile = 0;
    uint8_t mode = 0;
    long remainingTimeToHarvest = 0;
};

struct Profile {
    uint8_t id;
    String name;
    unsigned long incubationTS;
};
Profile profiles[21];

GenericConfig configHelper("/micu.json");


#ifdef USE_LOCAL_WEB_INTERFACE
void _onWsEventMain(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
#endif
#ifdef USE_IOT
void _processThingsboardSharedAttributesUpdate(const JsonObjectConst &data);
#endif

void setColor(uint8_t r, uint8_t g, uint8_t b);
void updateGrowLight();
void updatePump();
void _pvTaskCodeGrowControl(void*);
void loadConfig();
void saveConfig();
void syncClientAttr(uint8_t direction);
int calculateDaylightIntensity(struct tm current_tm);