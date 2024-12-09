#ifndef PARAMS_H
#define PARAMS_H
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200UL
#define COMPILED __DATE__ " " __TIME__
#define CURRENT_FIRMWARE_TITLE "MICU M1"
#define CURRENT_FIRMWARE_VERSION "0.0.2"

#define USE_WIFI_OTA

#define USE_LOCAL_WEB_INTERFACE
#ifdef USE_LOCAL_WEB_INTERFACE
    #define WS_BLOCKED_DURATION 60000UL
    #define WS_RATE_LIMIT_INTERVAL 1000UL
#endif

//#define USE_IOT
#ifdef USE_IOT
    #define THINGSBOARD_ENABLE_STREAM_UTILS true
    #define USE_IOT_SECURE
    #define USE_IOT_OTA
    const uint8_t IOT_FIRMWARE_FAILURE_RETRIES = 10;
    const uint16_t IOT_FIRMWARE_PACKET_SIZE = 4096;
    #define IOT_MAX_MESSAGE_SIZE 2048UL
    #define IOT_BUFFER_SIZE 2048UL
    #define IOT_STACKSIZE_TB 6000UL
    #define THINGSBOARD_ENABLE_DYNAMIC true
    #define THINGSBOARD_ENABLE_OTA true
    #define THINGSBOARD_ENABLE_DEBUG false
#endif

#define USE_WIFI_LOGGER
#ifdef USE_WIFI_LOGGER
#define WIFI_LOGGER_BUFFER_SIZE 256UL
#endif

#define USE_I2C
#ifdef USE_I2C
//#define USE_HW_RTC
#endif


#define ALARM_STACKSIZE 4096UL

#define GROW_CONTROL_STACKSIZE 4096UL

#define MAX_CRASH_COUNTER 3

static const int tbPort = 8883;
static constexpr char tbAddr[] PROGMEM = "prita.undiknas.ac.id";
static constexpr char spiffsBinUrl[] PROGMEM = "http://prita.undiknas.ac.id/cdn/firmware/micum1.spiffs.bin";
static constexpr char model[] PROGMEM = "M1";
static constexpr char hname[] PROGMEM = "MICUM1";
static constexpr char group[] PROGMEM = "MICU";
static constexpr char logIP[] PROGMEM = "255.255.255.255";
static const uint8_t logLev = 5;
static const bool fIoT = true;
static const bool fWOTA = true;
static const bool fWeb = true;
static const int gmtOff = 28880;
static const uint16_t logPort = 29514;
static const bool fInit = false;
static const bool LEDOn = false;
static const uint8_t pinLEDR = 27;
static const uint8_t pinLEDG = 14;
static const uint8_t pinLEDB = 12;
static const uint8_t pinBuzz = 32;

static const uint8_t s1tx = 26; //Neo 26, V3.1 33, V3 32
static const uint8_t s1rx = 25; //Neo 25, V3.1 32, V3 4
static const unsigned long intvWeb = 1;
static const unsigned long intvAttr = 5;
static const unsigned long intvTele = 900;


#endif
