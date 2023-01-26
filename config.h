#if !defined(ESP8266_FASTLED_WEBSERVER_CONFIG_H)
#define ESP8266_FASTLED_WEBSERVER_CONFIG_H

#if !defined(BUILDING_USING_PLATFORMIO)
     #define PRODUCT_MASTER
    // #define PRODUCT_TV55
    // #define PRODUCT_DENWIN01
    // #define PRODUCT_FIBO256
    // #define PRODUCT_DENWIN02
    // #define PRODUCT_BIGDENWIN
    // #define PRODUCT_AUDIO
    // #define PRODUCT_MOMSTAIRS
    // #define PRODUCT_ANDOCAR
    // #define PRODUCT_DEFAULT
#endif /// !defined(BUILDING_USING_PLATFORMIO)


// ////////////////////////////////////////////////////////////////////////////////////////////////////
// Additional configuration options ... defaults shown
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// #define UTC_OFFSET_IN_SECONDS (-6L * 60L * 60L) // UTC-6 (East-coast US ... no DST support)
// #define NTP_UPDATE_THROTTLE_MILLLISECONDS (5UL * 60UL * 60UL * 1000UL) // Ping NTP server no more than every 5 minutes
//
// TODO: add option to disable NTP altogether

// ////////////////////////////////////////////////////////////////////////////////////////////////////
// Include the configuration files for this build
// ////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1 // just for collapsing
    // Product-specific configuration
    #if defined(PRODUCT_DEFAULT) || defined(PRODUCT_MASTER)
        #include "./include/configs/product/default.h"
    #elif defined(PRODUCT_ANDOCAR)
        #include "./include/configs/product/andoCar.h"
    #elif defined(PRODUCT_MOMSTAIRS)
        #include "./include/configs/product/momstairs.h"
    #elif defined(PRODUCT_AUDIO)
        #include "./include/configs/product/audio.h"
    #elif defined(PRODUCT_TV55)
        #include "./include/configs/product/tv55.h"
    #elif defined(PRODUCT_DENWIN01) || defined(PRODUCT_DENWIN02)
        #include "./include/configs/product/smallwindow.h"
    #elif defined(PRODUCT_BIGDENWIN)
        #include "./include/configs/product/bigwindow.h"
    #elif defined(PRODUCT_1628_RINGS)
        #include "./include/configs/product/1628rings.h"
    #elif defined(PRODUCT_KRAKEN64)
        #include "./include/configs/product/kraken64.h"
    #elif defined(PRODUCT_CHAMAELEON64)
        #include "./include/configs/product/chamaeleon64.h"
    #elif defined(PRODUCT_FIBONACCI32)
        #include "./include/configs/product/fibonacci32.h"
    #elif defined(PRODUCT_FIBONACCI64_FULL) || defined(PRODUCT_FIBONACCI64_MINI) || defined(PRODUCT_FIBONACCI64_MICRO) || defined(PRODUCT_FIBONACCI64_NANO)
        #include "./include/configs/product/fibonacci64.h"
    #elif defined(PRODUCT_FIBONACCI128)
        #include "./include/configs/product/fibonacci128.h"
    #elif defined(PRODUCT_FIBO256)
        #include "./include/configs/product/fibonacci256.h"
    #elif defined(PRODUCT_FIBONACCI512)
        #include "./include/configs/product/fibonacci512.h"
    #elif defined(PRODUCT_FIBONACCI1024)
        #include "./include/configs/product/fibonacci1024.h"
    #elif defined(PRODUCT_ESP8266_THING) // aka parallel
        #include "./include/configs/product/esp8266_thing.h"
    #else
        #error "Must define product to build against"
    #endif

    // Board-specific configuration
    #if defined(ARDUINO_ARCH_ESP32)
        #include "./include/configs/controller/controller_esp32.h"
    #elif defined(ARDUINO_ARCH_ESP8266)
        #include "./include/configs/controller/controller_esp8266.h"
    #else
        #error "Unknown board type ... currently only support ESP8266 and ESP32"
    #endif
#endif


// ////////////////////////////////////////////////////////////////////////////////////////////////////
// Set defaults for optional values
// ////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1 // just for collapsing
    #if !defined(NTP_UPDATE_THROTTLE_MILLLISECONDS)
        #define NTP_UPDATE_THROTTLE_MILLLISECONDS (5UL * 60UL * 60UL * 1000UL) // Ping NTP server no more than every 5 minutes
    #endif
#endif

// ////////////////////////////////////////////////////////////////////////////////////////////////////
// Validate configuration options
// ////////////////////////////////////////////////////////////////////////////////////////////////////

#if 1 // just for collapsing
    #if !defined(LED_TYPE)
        #error "LED_TYPE must be defined by product"
    #endif
    #if !defined(COLOR_ORDER)
        #error "COLOR_ORDER must be defined by product"
    #endif
    #if !defined(NUM_PIXELS)
        #error "NUM_PIXELS must be defined by product"
    #endif
    #if !defined(AVAILABLE_MILLI_AMPS)
        #error "AVAILABLE_MILLI_AMPS must be defined by product"
    #endif
    #if !defined(MAX_MILLI_AMPS_PER_PIXEL)
        #error "MAX_MILLI_AMPS_PER_PIXEL must be defined by product"
    #endif
    #if !defined(FRAMES_PER_SECOND)
        #error "FRAMES_PER_SECOND must be defined by product"
    #endif
    #if !defined(DEFAULT_PATTERN_INDEX)
        #error "DEFAULT_PATTERN_INDEX must be defined by product"
    #endif
    #if !defined(DEFAULT_COLOR_CORRECTION)
        #error "DEFAULT_COLOR_CORRECTION must be defined by product"
    #endif
    #if defined(ENABLE_IR) && !defined(IR_RECV_PIN)
        #error "IR_RECV_PIN must be defined by product when ENABLE_IR is defined"
    #endif
    #if !defined(NAME_PREFIX)
        #error "NAME_PREFIX must be defined by product"
    #endif
    #if !defined(PRODUCT_FRIENDLY_NAME)
        #error "PRODUCT_FRIENDLY_NAME must be defined by product"
    #endif
    // IS_FIBONACCI: There does not appear to be a way to check, at compile-time here, if requirements are met
    #if !defined(IS_FIBONACCI) || ((IS_FIBONACCI != 0) && (IS_FIBONACCI != 1))
        #error "IS_FIBONACCI must be defined to zero or one"
    #endif
    #if !defined(HAS_COORDINATE_MAP) || ((HAS_COORDINATE_MAP != 0) && (HAS_COORDINATE_MAP != 1))
        #error "HAS_COORDINATE_MAP must be defined to zero or one"
    #endif
    #if IS_FIBONACCI && (!HAS_COORDINATE_MAP)
        #error "IS_FIBONACCI is true, so HAS_COORDINATE_MAP must also be true (but is not)"
    #endif
    #if !defined(PARALLEL_OUTPUT_CHANNELS)
        #error "PARALLEL_OUTPUT_CHANNELS must be defined"
    #elif (PARALLEL_OUTPUT_CHANNELS == 1)
        // nothing to test here
    #elif (PARALLEL_OUTPUT_CHANNELS == 4)
        static_assert(NUM_PIXELS == (PIXELS_ON_DATA_PIN_1 + PIXELS_ON_DATA_PIN_2 + PIXELS_ON_DATA_PIN_3 + PIXELS_ON_DATA_PIN_4), "");
    #elif (PARALLEL_OUTPUT_CHANNELS == 5)
        static_assert(NUM_PIXELS == (PIXELS_ON_DATA_PIN_1 + PIXELS_ON_DATA_PIN_2 + PIXELS_ON_DATA_PIN_3 + PIXELS_ON_DATA_PIN_4 + PIXELS_ON_DATA_PIN_5), "");
    #elif (PARALLEL_OUTPUT_CHANNELS == 6)
        static_assert(NUM_PIXELS == (PIXELS_ON_DATA_PIN_1 + PIXELS_ON_DATA_PIN_2 + PIXELS_ON_DATA_PIN_3 + PIXELS_ON_DATA_PIN_4 + PIXELS_ON_DATA_PIN_5 + PIXELS_ON_DATA_PIN_6), "");
    #else
        #error "PARALLEL_OUTPUT_CHANNELS currently tested only with values 1, 4, or 6."
    #endif
    #if (UTC_OFFSET_IN_SECONDS < (-14L * 60L * 60L))
        #error "UTC_OFFSET_IN_SECONDS offset does not appear correct (< -14H) ... Note it is defined in seconds."
    #elif (UTC_OFFSET_IN_SECONDS > (14L * 60L * 60L))
        #error "UTC_OFFSET_IN_SECONDS offset does not appear correct (> +14H) ... Note it is defined in seconds."
    #endif
    #if (NTP_UPDATE_THROTTLE_MILLLISECONDS < (15UL * 1000UL))
        #error "NTP_UPDATE_THROTTLE_MILLLISECONDS less than 15 seconds ... may exceed rate limits"
    #endif
#endif



#if defined(MILLI_AMPS)
    #error "Legacy symbol MILLI_AMPS should be changed to AVAILABLE_MILLI_AMPS"
#endif
#define MAX_MILLI_AMPS (MAX_MILLI_AMPS_PER_PIXEL * NUM_PIXELS)


#endif // ESP8266_FASTLED_WEBSERVER_CONFIG_H
