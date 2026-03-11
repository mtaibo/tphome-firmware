#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <stdint.h>
#include <string.h>

#include "credentials.h"

namespace Sizes {
    static constexpr uint8_t ID    =  8;
    static constexpr uint8_t MQTT  = 16;
    static constexpr uint8_t WIFI  = 32;
}

namespace Defaults {

    /* --- Identification --- */
    static constexpr char ID []   = "";

    /* --- Timings & Preferences --- */
    static constexpr uint16_t UP_TIME          = 1000; // 10.00s
    static constexpr uint16_t DOWN_TIME        = 1000; // 10.00s
    static constexpr uint16_t DOWN_POSITION    = 2000; // 20.00%
    static constexpr bool INVERTED_RELAYS      = false;

    static constexpr uint16_t MOTOR_SAFE_TIME  = 50;   // 0.50s
    static constexpr uint16_t LONG_PULSE       = 500;  // 5.00s
    static constexpr uint16_t SHORT_PULSE      = 200;  // 2.00s

    /* --- Initial State --- */
    static constexpr uint16_t START_POSITION   = 10000; // 100.00%
    static constexpr bool IS_MOVING            = false;
    static constexpr bool IS_WAITING           = false;
}

#endif