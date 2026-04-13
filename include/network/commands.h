#ifndef COMMANDS_H
#define COMMANDS_H

#include "blinds.h"
#include "mqtt.h"
#include "ota.h"

namespace Commands {

    #if defined(DEVICE_TYPE_BLIND)

        enum class Cmd : uint8_t {
            UP    = 0xC0,
            DOWN  = 0xC1,
            STOP  = 0xC2,
            PING  = 0xC3,
            STATE = 0xC4
        };

    #elif defined(DEVICE_TYPE_LIGHT)
    #endif

    #if defined(DEVICE_TYPE_BLIND)

        enum class AdminCmd : uint8_t {
            OTA       = 0xA0, // No payload
            REBOOT    = 0xA1, // No payload
            RESET_MEM = 0xA2, // No payload
            SET_POS   = 0xA3, // uint16_t (0-10000)
            SET_PREFS = 0xA4, // sizeof(Settings::Prefs) bytes
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

        /* Defining the State struct that is sent to mqtt client */

        struct __attribute__((packed)) State {
            uint8_t position;
            uint8_t state;
        } payload {
            (uint8_t) (Settings::state.currentPosition / 100),
            (uint8_t) Blinds::_motor.state
        };

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

    static void handleAdmin(byte* payload, unsigned int length) {
        if (length < 1) return;

        const AdminCmd cmd = static_cast<AdminCmd>(payload[0]);
        const byte*    data = payload + 1;
        const uint8_t dataLen = length - 1;

        switch (cmd) {
            case AdminCmd::OTA: if (dataLen == 0) OTA::start(); break;
            case AdminCmd::REBOOT: if (dataLen == 0) Settings::reboot(); break;
            case AdminCmd::RESET_MEM: if (dataLen == 0) Settings::reset(); break;

            case AdminCmd::SET_POS:
                if (dataLen == sizeof(uint16_t)) {
                    memcpy(&Settings::state.currentPosition, data, sizeof(uint16_t));
                    Settings::save();
                } break;

            case AdminCmd::SET_PREFS:
                if (dataLen == sizeof(Settings::Prefs)) {
                    memcpy(&Settings::prefs, data, sizeof(Settings::Prefs));
                    Settings::save();
                } break;

            default: break;
        }
    }

    void callback(char* topic, byte* payload, unsigned int length) {

        if (length == 0) return;

        /* Command for global topic */
        if (strcmp(topic, Mqtt::topics.global) == 0) {
            delay(random(50, 1000)); // Random delay to prevent all devices publishing at the same time
            if (payload[0] == 0x01) ping();
        }

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

        /* Common topic commands */
        if (strcmp(topic, Mqtt::topics.cmd) == 0) {
            handleCmd(payload[0]);
        }

        /* Admin topic commands */
        else if (strcmp(topic, Mqtt::topics.admin) == 0) {
            handleAdmin(payload, length);
        }
    }
}

#endif // COMMANDS_H