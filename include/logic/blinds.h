#ifndef BLINDS_H
#define BLINDS_H

#include "settings.h"
#include "leds.h"

#if defined(DEVICE_HARDWARE_ESP8266)
    #include "hardware/esp8266/hardware.h"
#elif defined(DEVICE_HARDWARE_BK7231N)
    #include "hardware/bk7231n/hardware.h"
#endif

#define TOL 50 // Macro to compare different positions or times
#define STOPPING_TIME 2000 // 2.00s - Macro for time running after reaching max position

namespace Blinds {

    enum State { IDLE, WAITING, MOVING, STOPPING };
    enum Direction { NONE, UP, DOWN };
        
    struct Motor {
        State state = IDLE;
        Direction direction = NONE;
        uint32_t startTime = 0;
        uint32_t lastTime = 0;
        uint16_t nextPosition = Settings::state.currentPosition;
    };
        
    static Motor _motor;

    namespace Relays {

        inline void stop() {

            /* Stop relays to prevent both relays 
             * being active simultaneously */
            Hardware::RelayUp::off();
            Hardware::RelayDown::off();

            /* Turn off every movement led to prevent
             * both leds being active simultaneously */
            Leds::set(Pins::LED_TOP, Leds::OFF);
            Leds::set(Pins::LED_BTM, Leds::OFF);

            /* Save settings if there is not more movement */
            if (_motor.state == IDLE) Settings::saveState();
        }

        inline void up() {
            Relays::stop(); // Stop movement before starting movement again

            /* Start next movement */
            Settings::prefs.invertedRelays ? Hardware::RelayDown::on() : Hardware::RelayUp::on();
            Leds::set(Pins::LED_TOP, Leds::ON);
        }

        inline void down() {
            Relays::stop(); // Stop movement before starting movement again

            /* Start next movement */
            Settings::prefs.invertedRelays ? Hardware::RelayUp::on() : Hardware::RelayDown::on();
            Leds::set(Pins::LED_BTM, Leds::ON);
        }
    }

    inline void stop() { // Blinds global action
        if (_motor.state != IDLE) {
            _motor.state = IDLE;
            _motor.direction = NONE;
            _motor.startTime = 0;
            Settings::state.currentPosition = _motor.nextPosition;
            
            Relays::stop();
            Leds::set(Pins::LED_MID, Leds::ON, Leds::MEDIUM, 0, 50);
        }
    }

    namespace Position {

        inline void update() {
            
            uint32_t now = (uint32_t) millis();

            switch (_motor.state) {

                case IDLE: break; // If blind is not moving, return

                case WAITING: { // Changing between directions while moving needs a motor safe time

                    Relays::stop();

                    /* Start the timers if it is first waiting period */
                    if (_motor.startTime == 0) {
                        _motor.startTime = now;
                        _motor.lastTime = now;
                        break;
                    }

                    /* Check if safe time was enough and start movement */
                    if ((now - _motor.startTime) >= Defaults::MOTOR_SAFE_TIME) {

                        _motor.state = MOVING;
                        _motor.startTime = now;
                        _motor.lastTime = now;

                        if (_motor.direction == UP) Relays::up();
                        else if (_motor.direction == DOWN) Relays::down();
                    } break;
                }

                case MOVING: {

                    /* Auxiliar variable */
                    uint16_t currentPosition = Settings::state.currentPosition;

                    /* Init all variables and state on first movement */
                    if (_motor.startTime == 0) {
                        _motor.startTime = now;
                        _motor.lastTime = now;
                        if (_motor.direction == UP) Relays::up();
                        else if (_motor.direction == DOWN) Relays::down();
                        break;
                    }

                    /* Calculate delta time and delta position to modify current position */
                    uint32_t dt = now - _motor.lastTime; _motor.lastTime = now;
                    uint32_t totalTime = ((_motor.direction == DOWN) ? (uint32_t) Settings::prefs.downTime : (uint32_t) Settings::prefs.upTime) * 10;
                    uint16_t movedPosition = (dt * 10000) / totalTime;

                    /* Move current position (with excess checks) */
                    if (_motor.direction == DOWN) (currentPosition > movedPosition) ? currentPosition -= movedPosition : currentPosition = 0;
                    else if (_motor.direction == UP) (currentPosition < 10000) ? currentPosition += movedPosition : currentPosition = 10000;

                    /* Update currentPosition on global variable */
                    Settings::state.currentPosition = currentPosition;


                    /* ~~~ STOP MOVEMENT CRITERIA ~~~ */

                    // Time excess
                    if ((now - _motor.startTime) >= totalTime) stop();

                    // Position excess
                    else if ((_motor.direction == UP && currentPosition >= _motor.nextPosition) || 
                             (_motor.direction == DOWN && currentPosition <= _motor.nextPosition)) {

                        _motor.startTime = 0;
                        _motor.state = STOPPING;
                    }

                    break;
                }
                    
                case STOPPING: {

                    if (_motor.startTime == 0) {_motor.startTime = now; break;}

                    if ((now - _motor.startTime) >= STOPPING_TIME || 
                        (_motor.nextPosition != 0 && _motor.nextPosition != 10000)) stop();

                    break;
                }
            }
        }

        inline void set(uint16_t targetPosition) {

            /* Ignore this function call if the device is at asked position */
            if (targetPosition > Settings::state.currentPosition) {
                if ((targetPosition - Settings::state.currentPosition) < TOL) return;
            } else if ((Settings::state.currentPosition - targetPosition) < TOL) return;

            /* Determine new motor direction and position from current and target position */
            targetPosition = ((targetPosition + 50) / 100) * 100;  // Round target_position
            _motor.direction = (targetPosition > Settings::state.currentPosition) ? UP : DOWN;
            _motor.nextPosition = targetPosition;  // Save new next position on global state

            /* Set the new state for the next update function cycle */
            if (_motor.state == IDLE) _motor.state = MOVING;
            else if (_motor.state == WAITING) _motor.state = WAITING;
            else if (_motor.state == MOVING) _motor.state = WAITING;
        }
    }

    /* Public blinds update */
    inline void update() {
        Position::update();
    }
}

#endif