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
#include "ap.h"

namespace Actions {
    inline void check() {

        #if defined(DEVICE_TYPE_BLIND)

            if (Mode::is(Mode::Value::CONNECTION)) {

                if (auto action = Buttons::getAction(Pins::BTN_TOP)) {
                    if (action == Buttons::SHORT) {
                        if(!Network::isConnected()) Network::reconnect();
                        else Mode::set(Mode::Value::NORMAL);
                    }
                    else if (action == Buttons::MEDIUM) {}
                    else if (action == Buttons::LONG) {}
                }

                else if (auto action = Buttons::getAction(Pins::BTN_MID)) {
                    if (action == Buttons::SHORT) AP::start();
                    else if (action == Buttons::MEDIUM) {}
                    else if (action == Buttons::LONG) {}
                }

                else if (auto action = Buttons::getAction(Pins::BTN_BTM)) {
                    if (action == Buttons::SHORT) Settings::reboot();
                    else if (action == Buttons::MEDIUM) {}
                    else if (action == Buttons::LONG) {}
                }

            } else { // NORMAL BLIND MODE

                if (auto action = Buttons::getAction(Pins::BTN_TOP)) {
                    if (action == Buttons::SHORT) Blinds::Position::set(10000);
                    else if (action == Buttons::MEDIUM) {
                        if (Settings::state.currentPosition < Settings::prefs.downPosition) 
                            Blinds::Position::set(Settings::prefs.downPosition);
                    }
                    else if (action == Buttons::LONG) Mode::set(Mode::Value::CONNECTION);
                }

                else if (auto action = Buttons::getAction(Pins::BTN_MID)) {
                    if (action == Buttons::SHORT) Blinds::stop();
                    else if (action == Buttons::MEDIUM) {}
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