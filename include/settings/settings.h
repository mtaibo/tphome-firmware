#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h> // Module for uint8_t and uint16_t
#include "defaults.h"



namespace Settings {

    struct __attribute__((__packed__)) Config {

        // Identity
        char deviceID[IDENTITY_SIZE];
        char room[IDENTITY_SIZE];
        char name[IDENTITY_SIZE];

        // WiFi
        char wifiSSID[WIFI_SIZE];
        char wifiPass[WIFI_SIZE];

        // MQTT
        char mqttIP[MQTT_SIZE];
        char mqttUser[MQTT_SIZE];
        char mqttPass[MQTT_SIZE];
        uint16_t mqttPort;
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