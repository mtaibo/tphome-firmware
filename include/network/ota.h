#include <ESP8266httpUpdate.h>
#include "settings.h"
#include "wifi.h"
#include "leds.h"

namespace OTA {

    char url[100];

    void start() {

        Leds::off();
        
        /* Building firmware direction */
        snprintf(url, sizeof(url), "http://%s/firmware", Settings::config.mqttIP);

        WiFiClient client;
        ESPhttpUpdate.update(client, url);
    }
}