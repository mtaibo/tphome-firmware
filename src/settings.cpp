#include "settings.h"
#include "defaults.h"
#include "leds.h"

#include <Preferences.h> // Lib to save information on flash memory

namespace Settings {
    
    Config config;
    Prefs prefs;
    State state;

    Preferences storage;

    void reboot() {
        Leds::set(Pins::LED_TOP, Leds::BLINK, Leds::SLOW, 3);
        Leds::set(Pins::LED_MID, Leds::BLINK, Leds::SLOW, 3);
        Leds::set(Pins::LED_BTM, Leds::BLINK, Leds::SLOW, 3);
        ESP.restart();
    }

    // Clear every division on the storage to set
    // default settings again
    void reset() {
        storage.begin("storage", false);
        storage.clear();
        storage.end();
        reboot();
    }

    void defaults() {

        /* --------------   Identification  -------------- */
        strlcpy(config.deviceID, Defaults::ID, Sizes::ID);

        /* -----------------   Network   ----------------- */
        strlcpy(config.wifiSSID, WIFI_SSID, Sizes::WIFI);
        strlcpy(config.wifiPass, WIFI_PASS, Sizes::WIFI);
        strlcpy(config.mqttIP, MQTT_IP, Sizes::MQTT);
        strlcpy(config.mqttUser, MQTT_USER, Sizes::MQTT);
        strlcpy(config.mqttPass, MQTT_PASS, Sizes::MQTT);
        config.mqttPort = MQTT_PORT;

        /* --------  Preferences & Initial State  -------- */
        #if defined(DEVICE_TYPE_BLIND)
            prefs.upTime = Defaults::UP_TIME;
            prefs.downTime = Defaults::DOWN_TIME;
            prefs.downPosition = Defaults::DOWN_POSITION;
            prefs.invertedRelays = Defaults::INVERTED_RELAYS;
            state.currentPosition = Defaults::START_POSITION;
        #endif
        
        save();
    }

    // Save only the state settings struct, this function will be called just when
    // the blind was moving and stops
    void saveState() {
        storage.begin("storage", false);
        storage.putBytes("s", &state, sizeof(State));
        storage.end();
    }

    void save() {

        // Temp variables to store current config and prefs on the flash memory
        Config currentStoredConfig;
        Prefs currentStoredPrefs;

        storage.begin("storage", false); // Open storage on write mode (false)

        // Load the more persistent settings divisions from storage to check if they
        // are the same as the intended to save
        storage.getBytes("c", &currentStoredConfig, sizeof(Config));
        storage.getBytes("p", &currentStoredPrefs, sizeof(Prefs));

        // Save every settings division to storage if needed and check if every bit
        // of information on current_stored_config and current config are the same.

        if (memcmp(&config, &currentStoredConfig, sizeof(Config)) != 0)
            storage.putBytes("c", &config, sizeof(Config));

        if (memcmp(&prefs, &currentStoredPrefs, sizeof(Prefs)) != 0)
            storage.putBytes("p", &prefs, sizeof(Prefs));

        storage.putBytes("s", &state, sizeof(State));

        storage.end();
    }

    void load() {

        // Open storage on flash memory on only-read mode (true)
        storage.begin("storage", true);

        // Load every settings division from storage
        size_t r1 = storage.getBytes("c", &config, sizeof(Config));
        size_t r2 = storage.getBytes("p", &prefs, sizeof(Prefs));
        size_t r3 = storage.getBytes("s", &state, sizeof(State));

        storage.end();

        // Check if every block on memory exists and corresponds to its real memory size
        if (r1 != sizeof(Config) || r2 != sizeof(Prefs) || r3 != sizeof(State)) defaults();
    }

    void setup() {defaults();}
}