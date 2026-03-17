#ifndef ACTIONS_H
#define ACTIONS_H

#include "buttons.h"

#if defined(DEVICE_TYPE_BLIND)
    #include "blinds.h"
#elif defined(DEVICE_TYPE_LIGHT)
    #include "lights.h"
#endif

#include "network.h"
#include "mqtt.h"
#include "wifi.h"
#include "mode.h"

namespace Actions {
    inline void check() {

        #if defined(DEVICE_TYPE_BLIND)

            if (Mode::is(Mode::Value::CONNECTION)) {

                if (auto action = Buttons::getAction(Pins::BTN_TOP)) {
                    if (action == Buttons::SHORT) {Network::reconnect();}
                    else if (action == Buttons::MEDIUM) {} // OTA new firmware
                    else if (action == Buttons::LONG) {
                        Mode::set(Mode::Value::NORMAL);
                        Leds::set(Pins::BTN_MID, Leds::OFF);
                        Leds::set(Pins::BTN_BTM, Leds::OFF);
                    }
                }

                else if (auto action = Buttons::getAction(Pins::BTN_MID)) {
                    if (action == Buttons::SHORT) {Wifi::reconnect();}
                    else if (action == Buttons::MEDIUM) {}
                    else if (action == Buttons::LONG) {} // Access point
                }

                else if (auto action = Buttons::getAction(Pins::BTN_BTM)) {
                    if (action == Buttons::SHORT) {Mqtt::reconnect();}
                    else if (action == Buttons::MEDIUM) {}
                    else if (action == Buttons::LONG) {}
                }

            } else { // NORMAL BLIND MODE

                if (auto action = Buttons::getAction(Pins::BTN_TOP)) {
                    if (action == Buttons::SHORT) Blinds::Position::set(10000);
                    else if (action == Buttons::MEDIUM) Blinds::Position::set(Settings::prefs.downPosition);
                    else if (action == Buttons::LONG) Mode::set(Mode::Value::CONNECTION);
                }

                else if (auto action = Buttons::getAction(Pins::BTN_MID)) {
                    if (action == Buttons::SHORT) Blinds::Relays::stop();
                    else if (action == Buttons::MEDIUM) Blinds::Position::set(Settings::state.currentPosition);
                    else if (action == Buttons::LONG) {}
                }

                else if (auto action = Buttons::getAction(Pins::BTN_BTM)) {

                    if (action == Buttons::SHORT) {
                        if (Settings::state.currentPosition > Settings::prefs.downPosition) {
                            Blinds::Position::set(Settings::prefs.downPosition);
                        } else Blinds::Position::set(0);
                    }

                    else if (action == Buttons::MEDIUM) Blinds::Position::set(0);
                    else if (action == Buttons::LONG) {}
                }
            }

        #endif
    }
}

#endif