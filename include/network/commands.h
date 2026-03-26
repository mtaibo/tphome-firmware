#ifndef COMMANDS_H
#define COMMANDS_H

#include "blinds.h"
#include "mqtt.h"

namespace Commands {

    #if defined(DEVICE_TYPE_BLIND)

        enum class Cmd : uint8_t {
            UP   = 0xC0,
            DOWN = 0xC1,
            STOP = 0xC2,
            PING = 0xC3,
            STATE = 0xC4,
        };

    #elif defined(DEVICE_TYPE_LIGHT)
    #endif

    /* Assign a new deviceID to an unconfigured device */
    struct __attribute__((packed)) DeviceID {
        char    type;
        uint8_t zone;
        uint8_t device;
    };

    /* Get, if the device is connected, a 1 */
    void ping() {
        uint8_t payload = 1;
        Mqtt::_client.publish(Mqtt::topics.state,
                            reinterpret_cast<const uint8_t*>(&payload),
                            sizeof(payload));
    }

    /* Report blind state while moving */
    void publishState() {

        uint8_t position = (uint8_t) (Settings::state.currentPosition / 100);
        uint8_t state = (uint8_t) Blinds::_motor.state;

        struct __attribute__((packed)) State {
            uint8_t position;
            uint8_t state;
        } payload {position, state};

        Mqtt::_client.publish(Mqtt::topics.state,
                            reinterpret_cast<const uint8_t*>(&payload),
                            sizeof(State));
    }

    static void handleCmd(uint8_t cmd) {

        #if defined(DEVICE_TYPE_BLIND)

            if (cmd <= 100) {Blinds::Position::set(cmd * 100); return;}

            switch (static_cast<Cmd>(cmd)) {
                case Cmd::UP: Blinds::Position::set(10000); break;
                case Cmd::DOWN:
                    if (Settings::state.currentPosition > Settings::prefs.downPosition) {
                        Blinds::Position::set(Settings::prefs.downPosition);
                    } else Blinds::Position::set(0);
                    break;
                case Cmd::STOP: Blinds::stop(); break;
                case Cmd::PING: ping(); break;
                case Cmd::STATE: publishState(); break;
                default: break;
            }

        #elif defined(DEVICE_TYPE_LIGHT)
        #endif
    }

    void callback(char* topic, byte* payload, unsigned int length) {

        if (length == 0) return;

        /* Command for non-configured deviceID */
        if (strlen(Settings::config.deviceID) == 4) {

            if (strcmp(topic, Mqtt::topics.def) != 0) return; // Check topic
            if (length != sizeof(DeviceID))           return; // Check length

            /* Build new deviceID */
            const auto* id = reinterpret_cast<const DeviceID*>(payload);
            snprintf(Settings::config.deviceID, 6, "%c%02d%02d",
                     id->type, id->zone % 100, id->device % 100);

            /* Reboot with new deviceID to build new topics */
            Settings::save();
            Settings::reboot();
            return;
        }

        /* Common commands on cmd, room and global topics */
        if (strcmp(topic, Mqtt::topics.cmd)    == 0 ||
            strcmp(topic, Mqtt::topics.room)   == 0 ||
            strcmp(topic, Mqtt::topics.global) == 0) {

            if (length != 1) return;
            handleCmd(payload[0]);
        }

        /* Admin commands on admin topic to change Prefs variables */
        else if (strcmp(topic, Mqtt::topics.admin) == 0) {
            if (length != sizeof(Settings::Prefs)) return;
            memcpy(&Settings::prefs, payload, sizeof(Settings::Prefs));
            Settings::save();
        }
    }
}

#endif // COMMANDS_H