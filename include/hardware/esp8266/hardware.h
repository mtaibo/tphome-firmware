#ifndef HARDWARE_H
#define HARDWARE_H

#include "defaults.h"
#include "settings.h"
#include "hardware/esp8266/pinout.h"
#include "hardware/esp8266/driver_input.h"
#include "hardware/esp8266/driver_output.h"

namespace Hardware { // Hardware for ESP8266

    // --- BUTTONS ---
    using BtnBtm = InputPin<Pins::BTN_BTM, Defaults::LONG_PULSE>;
    using BtnMid = InputPin<Pins::BTN_MID, Defaults::LONG_PULSE>;
    using BtnTop = InputPin<Pins::BTN_TOP, Defaults::LONG_PULSE>;

    // --- LEDS ---
    using LedConfig = OutputPin<Pins::LED_CONFIG>;
    using LedGreen  = OutputPin<Pins::LED_GREEN>;
    using LedBtm    = OutputPin<Pins::LED_BTM>;
    using LedMid    = OutputPin<Pins::LED_MID>;
    using LedTop    = OutputPin<Pins::LED_TOP>;

    // --- RELAYS ---
    using RelayUp   = OutputPin<Pins::RELAY_UP>;
    using RelayDown = OutputPin<Pins::RELAY_DOWN>;

    // --- LIGHT CONTROL ---
    static inline void enableLight() {LedConfig::setup(); LedConfig::on();}
    static inline void disableLight() {LedConfig::off();}

    // --- LED CONTROL ---
    static void setState(uint8_t pin, bool state) {
        switch (pin) {
            case Pins::LED_GREEN: state ? LedGreen::on() : LedGreen::off(); break;
            case Pins::LED_BTM: state ? LedBtm::on() : LedBtm::off(); break;
            case Pins::LED_MID: state ? LedMid::on() : LedMid::off(); break;
            case Pins::LED_TOP: state ? LedTop::on() : LedTop::off(); break;
        }
    }

    // --- GLOBAL SETUP ---
    static void setup() {

        LedGreen::setup();
        LedBtm::setup();
        LedMid::setup();
        LedTop::setup();

        BtnBtm::setup();
        BtnMid::setup();
        BtnTop::setup();

        RelayUp::setup();
        RelayDown::setup();

        enableLight();
    }
}

#endif // HARDWARE_H