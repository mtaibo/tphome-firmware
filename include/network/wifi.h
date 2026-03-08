#ifndef WIFI_H
#define WIFI_H

#include <ESP8266WiFi.h>

#include "settings.h"
#include "leds.h"

#define ATTEMPT_INTERVAL 5000

namespace Wifi {

    auto &c = Settings::config;

    struct State {
        uint32_t lastTime = 0;
        uint8_t attempts = 0;
        bool isConnected = false;
        bool ledOn = false;
    };

    static State _state;

    void setup() {

        /* Prevent the chip from reconnecting on its own */
        WiFi.setAutoConnect(false);
        WiFi.setAutoReconnect(false);

        WiFi.setSleepMode(WIFI_NONE_SLEEP); // Prevent the chip from random sleeping
        WiFi.setOutputPower(20.5); // Range: 0.0 - 20.5

        WiFi.persistent(false); // Prevent the chip from saving wifi credentials on flash memory

        WiFi.hostname(c.deviceID); // Show the deviceID as name on router

        WiFi.mode(WIFI_STA); // WiFi Station mode, just connects to the router
        WiFi.begin(c.wifiSSID, c.wifiPass);

        /* First connection attempt */
        Leds::set(LED_GREEN, Leds::BLINK, Leds::FAST);
        _state.lastTime = millis();
        _state.ledOn = true;
        _state.attempts = 1;
    }

    void reconnect() {

        /* Restart default valures on state connection */
        _state.lastTime = 0;
        _state.attempts = 0;
        _state.isConnected = false;

        /* Disconnect and clear last connection attempts */
        WiFi.disconnect(true);

        /* Start reconnection by setting up again */
        setup();
    }

    void update() {

        uint32_t now = (uint32_t) millis();

        /* While device is connected */
        if (_state.isConnected) {

            /* After first connection, wait 60 secconds until next status check */
            if (now - _state.lastTime < 60000) return;
            
            /* Disconnected after being connected */
            if (WiFi.status() != WL_CONNECTED) {
                _state.isConnected = false;
                _state.lastTime = now;
                _state.attempts = 3;
            } return; 
        }


        /* While device is disconnected */
        if (WiFi.status() != WL_CONNECTED) {

            if (_state.attempts < 3) { // Not more than three attempts allowed on each reconnection

                /* Check interval between attempts */
                if (now - _state.lastTime > ATTEMPT_INTERVAL) {
                    _state.lastTime = now;
                    _state.attempts++;
                    WiFi.begin(c.wifiSSID, c.wifiPass);
                }
            } else if (_state.ledOn) {Leds::set(LED_GREEN, Leds::OFF); _state.ledOn = false;}
        } 
        
        else { // WiFi is connected but _state.isConnected is false
            if (_state.ledOn) {Leds::set(LED_GREEN, Leds::OFF); _state.ledOn = false;}
            _state.isConnected = true;
            _state.attempts = 0;
            _state.lastTime = now;
        }
    }
}

#endif