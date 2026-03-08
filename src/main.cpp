#include "settings.h"
#include "network.h"
#include "actions.h"

#if defined(DEVICE_TYPE_BLIND)
    #include "blinds.h"
#elif defined(DEVICE_TYPE_LIGHT)
    #include "lights.h"
#endif

void setup() {
    Settings::setup();
    Hardware::setup();
    Network::setup();
}

void loop() {

    Network::update();

    /* Check inputs */
    Actions::check();

    /* Update interface */
    Leds::update();
    Buttons::update();

    /* Update device functions */
    #if defined(DEVICE_TYPE_BLIND)
        Blinds::update();
    #elif defined(DEVICE_TYPE_LIGH)
        Lights::update();
    #endif
}