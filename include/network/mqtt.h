#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>

#include "settings.h"
#include "wifi.h"

#define ATTEMPT_INTERVAL 5000

namespace Mqtt {

    struct State {
        uint32_t lastTime = 0;
        uint8_t attempts = 0;
        bool isConnected = false;
        bool ledOn = false;
    };

    struct Topics {

        char def[Sizes::MQTT]; // Default non-configured device topic

        /* Device topics */
        char cmd[Sizes::MQTT];   // Subscribe to common orders
        char admin[Sizes::MQTT]; // Subscribe to change settings
        char state[Sizes::MQTT]; // Publish device state on each movement

        /* Group topics */
        char room[Sizes::MQTT]; // Subscribe to room orders on same device type
        char global[Sizes::MQTT]; // Subscribe to global orders on same device type
    };

    static Topics topics;

    static State _state;
    static WiFiClient _wifiClient;
    static PubSubClient _client(_wifiClient);

    void setCallback(MQTT_CALLBACK_SIGNATURE) {
        _client.setCallback(callback);
    }

    bool isConnected() {
        return _state.isConnected;
    }

    void setup() {

        /* When the device is not configured, its device ID has 4 hex digits
         * and the device has to subscribe to the def topic to get its new ID */
        if (strlen(Settings::config.deviceID) == 4) {
            snprintf(topics.def, Sizes::MQTT, "def/%.4s", Settings::config.deviceID);
        }

        else { // Topics for configured devices

            /* Create device own topics for commands, administration and reporting state */
            snprintf(topics.cmd,   Sizes::MQTT, "tp/%.5s/c",  Settings::config.deviceID);
            snprintf(topics.admin, Sizes::MQTT, "tp/%.5s/a",  Settings::config.deviceID);
            snprintf(topics.state, Sizes::MQTT, "tp/%.5s/s",  Settings::config.deviceID);

            /* Create group topics for device types and room device types commands */
            snprintf(topics.room,   Sizes::MQTT, "tp/%.3s/c", Settings::config.deviceID);
            snprintf(topics.global, Sizes::MQTT, "tp/%.1s/c", Settings::config.deviceID);
        }

        /* Setup the mqtt server with its credentials, max buffer and callback function */
        _client.setServer(Settings::config.mqttIP, Settings::config.mqttPort);
        _client.setBufferSize(sizeof(Settings::Prefs) + 64); 
        _client.setSocketTimeout(4); 
    }

    void reconnect() {

        /* Restart default values on connection state */
        _state.ledOn = false;
        _state.lastTime = 0;
        _state.attempts = 0;
        _state.isConnected = false;

        /* Start reconnection by setting up again */
        setup();
    }

    void update() {

        uint32_t now = millis();

        /* While device is connected */
        if (_state.isConnected) {

            /* Disconnected after being connected, do not reconnect until user ask for it */
            if (!_client.connected()) {
                _state.isConnected = false;
                _state.lastTime = now;
            } 

            else _client.loop();
            return;
        }

        /* While device is disconnected */
        if (!_client.connected()) {

            /* First connection attempt: wait 3s to stabilize wifi connection */
            if (_state.lastTime == 0) {_state.lastTime = now + 3000; return;}

            if (_state.attempts < 5) { // Not more than 5 attempts per manual reconnection

                /* Check interval between attempts */
                if (now - _state.lastTime > ATTEMPT_INTERVAL) {

                    /* First attempt, turn led on */
                    if (_state.ledOn == false) {
                        Leds::set(Pins::LED_GREEN, Leds::BLINK, Leds::SLOW);
                        _state.ledOn = true;
                    }

                    _client.connect(Settings::config.deviceID, Settings::config.mqttUser, Settings::config.mqttPass);

                    _state.lastTime = now;
                    _state.attempts++;
                }

            } else if (_state.ledOn) {Leds::set(Pins::LED_GREEN, Leds::OFF); _state.ledOn = false;}
        } 
        
        else { // Client is connected but state.isConnected is false === first connection
            if (_state.ledOn) {Leds::set(Pins::LED_GREEN, Leds::OFF); _state.ledOn = false;}
            _state.isConnected = true;
            _state.attempts = 5;
            _state.lastTime = now;

            if (strlen(Settings::config.deviceID) == 4) _client.subscribe(topics.def);
            else {
                _client.subscribe(topics.cmd);
                _client.subscribe(topics.admin);
                _client.subscribe(topics.room);
                _client.subscribe(topics.global);
            }
        }
    }
}

#endif // MQTT_H