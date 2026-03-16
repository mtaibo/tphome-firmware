#ifndef NETWORK_H
#define NETWORK_H

#include "settings.h"
#include "commands.h"
#include "wifi.h"
#include "mqtt.h"

namespace Network {

    void inline setup() {
        Wifi::setup();
        Mqtt::setup();
        Mqtt::setCallback(Commands::callback);
   }

    void inline update() {
        Wifi::update();
        if (Wifi::isConnected()) Mqtt::update();

        if (Wifi::isConnected()) Leds::set(Pins::LED_MID, Leds::ON);
        else Leds::set(Pins::LED_MID, Leds::OFF);

        if (Mqtt::isConnected()) Leds::set(Pins::LED_BTM, Leds::ON);
        else Leds::set(Pins::LED_BTM, Leds::OFF);
    }

    void inline reconnect() {
        if (!Wifi::isConnected()) Wifi::reconnect();
        else Mqtt::reconnect();
    }
}

#endif // NETWORK_H