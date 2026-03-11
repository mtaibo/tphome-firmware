#ifndef ACTIONS_H
#define ACTIONS_H

#include "buttons.h"

#if defined(DEVICE_TYPE_BLIND)
    #include "blinds.h"
#elif defined(DEVICE_TYPE_LIGHT)
    #include "lights.h"
#endif

namespace Actions {
    inline void check() {

        #if defined(DEVICE_TYPE_BLIND)

            if (auto action = Buttons::getAction(Pins::BTN_TOP)) {
                if (action == Buttons::SHORT) Blinds::Position::set(10000);
                else if (action == Buttons::MEDIUM) Blinds::Position::set(Settings::prefs.downPosition);
                else if (action == Buttons::LONG) Blinds::Position::set(Settings::state.currentPosition);
            }

            else if (auto action = Buttons::getAction(Pins::BTN_MID)) {
                if (action == Buttons::SHORT) Blinds::Relays::stop();
                else if (action == Buttons::MEDIUM) Blinds::Position::set(Settings::state.currentPosition);
                else if (action == Buttons::LONG) Blinds::Position::set(Settings::state.currentPosition);
            }

            else if (auto action = Buttons::getAction(Pins::BTN_BTM)) {

                if (action == Buttons::SHORT) {
                    if (Settings::state.currentPosition > Settings::prefs.downPosition) {
                        Blinds::Position::set(Settings::prefs.downPosition);
                    } else Blinds::Position::set(0);
                }

                else if (action == Buttons::MEDIUM) Blinds::Position::set(0);
                else if (action == Buttons::LONG) Blinds::Position::set(Settings::state.currentPosition);
            }

        #endif
    }
}

#endif