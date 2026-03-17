#ifndef WIFI_H
#define WIFI_H

#include "settings.h"
#include "leds.h"

#if defined(DEVICE_HARDWARE_ESP8266)
    #include "hardware/esp8266/driver_wifi.h"
#elif defined(DEVICE_HARDWARE_BK7231N)
    #include "hardware/bk7231n/driver_wifi.h"
#endif

#define ATTEMPT_INTERVAL 5000
#define MAX_ATTEMPTS 5

namespace Wifi {

    struct State {
        uint32_t lastTime = 0;
        uint8_t attempts = 0;
        bool isConnected = false;
        bool ledOn = false;
    };

    inline static State _state;

    /* Public function for other modules to check if wifi is connected */
    inline bool isConnected() {
        return _state.isConnected;
    }
    
    inline bool isOnTimeout() {
        return (!_state.isConnected) && (_state.attempts >= MAX_ATTEMPTS);
    }

    inline void setup() {

        /* Configure the initial settings of the wifi connection */
        Hardware::Wifi::setup(Settings::config.deviceID);

        /* First connection attempt */
        Hardware::Wifi::begin(Settings::config.wifiSSID, Settings::config.wifiPass);
        Leds::set(Pins::LED_GREEN, Leds::BLINK, Leds::FAST);
        _state.lastTime = millis();
        _state.ledOn = true;
        _state.attempts = 1;
    }

    inline void reconnect() {

        /* Restart default values on connection state */
        _state.lastTime = 0;
        _state.attempts = 0;
        _state.isConnected = false;

        /* Disconnect and clear last connection attempts */
        Hardware::Wifi::disconnect();

        /* Start reconnection by setting up again */
        setup();
    }

    inline void update() {

        uint32_t now = (uint32_t) millis();

        /* While device is connected */
        if (_state.isConnected) {

            /* After first connection, wait 60 secconds until next status check */
            if (now - _state.lastTime < 60000) return;
            
            /* Disconnected after being connected, do not reconnect until user ask for it */
            if (!Hardware::Wifi::isConnected()) {
                _state.isConnected = false;
                _state.lastTime = now;
                _state.attempts = MAX_ATTEMPTS;
            } 

            return;
        }

        /* While device is disconnected */
        if (!Hardware::Wifi::isConnected()) {

            if (_state.attempts < MAX_ATTEMPTS) { // Not more than 3 attempts allowed on each reconnection

                /* Check interval between attempts */
                if (now - _state.lastTime > ATTEMPT_INTERVAL) {
                    _state.lastTime = now;
                    _state.attempts++;
                    Hardware::Wifi::begin(Settings::config.wifiSSID, Settings::config.wifiPass);
                }
            } else if (_state.ledOn) {Leds::set(Pins::LED_GREEN, Leds::OFF); _state.ledOn = false;}
        } 
        
        else { // WiFi is connected but state.isConnected is false
            if (_state.ledOn) {Leds::set(Pins::LED_GREEN, Leds::OFF); _state.ledOn = false;}
            _state.isConnected = true;
            _state.attempts = 0;
            _state.lastTime = now;
        }
    }
}

#endif