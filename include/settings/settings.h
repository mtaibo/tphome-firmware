#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h> // Module for uint8_t and uint16_t
#include "defaults.h"

namespace Settings {

    struct __attribute__((__packed__)) Config {

        // Identity
        char deviceID[Sizes::ID];

        // WiFi
        char wifiSSID[Sizes::WIFI];
        char wifiPass[Sizes::WIFI];

        // MQTT
        char mqttIP[Sizes::MQTT];
        char mqttUser[Sizes::MQTT];
        char mqttPass[Sizes::MQTT];
        uint16_t mqttPort;

        // Firmware version
        uint8_t version[Sizes::VERSION];
    };

    #if defined(DEVICE_TYPE_BLIND)

        struct __attribute__((__packed__)) Prefs {
            uint16_t upTime;
            uint16_t downTime;
            uint16_t downPosition;
            bool invertedRelays;
        };

        struct __attribute__((__packed__)) State {
            uint16_t currentPosition;
        };

    #elif defined(DEVICE_TYPE_LIGHT)
        struct __attribute__((__packed__)) Prefs {};
        struct __attribute__((__packed__)) State {};
    #endif

    extern Config config;
    extern Prefs prefs;
    extern State state;

    void reboot();
    void reset();
    void defaults();

    void saveState();

    void save();
    void load();
    void setup();
}

#endif // SETTINGS_H