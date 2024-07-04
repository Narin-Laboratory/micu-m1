#ifndef PARAMS_H
#define PARAMS_H
#include <Arduino.h>

#define SERIAL_BAUD_RATE 115200UL
#define COMPILED __DATE__ " " __TIME__
#define CURRENT_FIRMWARE_TITLE "MICUM1"
#define CURRENT_FIRMWARE_VERSION "0.0.0"

#define USE_WIFI_OTA

#define USE_LOCAL_WEB_INTERFACE
#ifdef USE_LOCAL_WEB_INTERFACE
#define WS_BLOCKED_DURATION 60000UL
#define WS_RATE_LIMIT_INTERVAL 1000UL
#endif

#define USE_IOT
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
#define USE_HW_RTC
#endif

#define GROW_CONTROL_STACKSIZE 4096UL

const int tbPort = 8883;
constexpr char tbAddr[] PROGMEM = "prita.undiknas.ac.id";
constexpr char spiffsBinUrl[] PROGMEM = "http://prita.undiknas.ac.id/cdn/firmware/micum1.spiffs.bin";
constexpr char model[] PROGMEM = "MICUM1";
constexpr char group[] PROGMEM = "MICU";
constexpr char logIP[] PROGMEM = "255.255.255.255";
const uint8_t logLev = 5;
const bool fIoT = true;
const bool fWOTA = true;
const bool fWeb = true;
const int gmtOff = 28880;
const uint16_t logPort = 29514;


#endif
