#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include "settings.h"
#include "wifi.h"
#include "blinds.h"

namespace Mqtt {

    struct Topics {
        
        /* Device topics */
        char cmd[Sizes::MQTT];   // Subscribe to common orders
        char admin[Sizes::MQTT]; // Subscribe to change settings
        char state[Sizes::MQTT]; // Publish device state on each movement

        /* Group topics */
        char room[Sizes::MQTT]; // Subscribe to room orders on same device type
        char global[Sizes::MQTT]; // Subscribe to global orders on same device type
    };

    static Topics _topics;
    static WiFiClient _wifiClient;
    static PubSubClient _client(_wifiClient);

    void callback(char* topic, byte* payload, unsigned int length) {
        if (length == 0) return;

        if (length == 1) {
            uint8_t cmd = payload[0];
            if (cmd <= 100) Blinds::Position::set(cmd*100);
            else switch (cmd) {
                case 0xD0: Blinds::Relays::stop(); break;
                case 0xD1: Blinds::Position::set(10000); break;
                case 0xD2: Blinds::Position::set(0); break;
            }
        } 
        
        else if (length == sizeof(Settings::Prefs)) {
            memcpy(&Settings::prefs, payload, sizeof(Settings::Prefs));
            Settings::save();
        }
    }

    void setup() {
        
        /* Create device own topics for commands, administration and reporting state */
        snprintf(_topics.cmd,   Sizes::MQTT, "tp/%.5s/c",  Settings::config.deviceID);
        snprintf(_topics.admin, Sizes::MQTT, "tp/%.5s/a",  Settings::config.deviceID);
        snprintf(_topics.state, Sizes::MQTT, "tp/%.5s/s",  Settings::config.deviceID);

        /* Create group topics for device type,  */
        snprintf(_topics.room,   Sizes::MQTT, "tp/%.3s/c", Settings::config.deviceID);
        snprintf(_topics.global, Sizes::MQTT, "tp/%.1s/c", Settings::config.deviceID);

        /* Setup the mqtt server */
        _client.setServer(Settings::config.mqttIP, Settings::config.mqttPort);
        _client.setBufferSize(sizeof(Settings::Prefs) + 64); 
        _client.setCallback(callback);
    }

    void update() {

        /* Only update mqtt server if wifi is connected */
        if (!Wifi::isConnected()) return;

        if (!_client.connected()) {

            if (_client.connect(Settings::config.deviceID, Settings::config.mqttUser, Settings::config.mqttPass)) {
                _client.subscribe(_topics.cmd);
                _client.subscribe(_topics.room);
            }
        } else {
            _client.loop();
        }
    }

    void sendState() {
        if (!_client.connected()) return;
        _client.publish(_topics.state, (uint8_t*)&Settings::state, sizeof(Settings::State), true);
    }
}

#endif // MQTT_H