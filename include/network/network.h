#ifndef NETWORK_H
#define NETWORK_H

#include "settings.h"
#include "wifi.h"

namespace Network {

    void inline setup() {
        Wifi:setup();
    }

    void inline update() {
        Wifi::update();
    }
}

#endif // NETWORK_H