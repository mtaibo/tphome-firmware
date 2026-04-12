# Build & Flash

---

## 1. Setup

```bash
python3 -m venv venv
source venv/bin/activate   # Windows: venv\Scripts\activate
pip install -r requirements.txt
```

---

## 2. Hardware

### Wiring

Connect a USB-TTL adapter (3.3V logic — CH340, CP2102 or similar) to the chip:

| USB-TTL | Chip |
|---|---|
| TX | RX |
| RX | TX |
| GND | GND |
| 3.3V | VCC |

> **Do not use 5V** — both the ESP8266 and BK7231N are 3.3V devices.

### Entering bootloader mode

**ESP8266 (TYWE3S):** Pull GPIO 0 to GND before powering up. Connect the USB-TTL while holding GPIO 0 low, then release. No serial output on boot confirms flash mode.

**BK7231N (CB3S / T34):** Pull CEN (chip enable) to GND briefly while the chip is powered to reset into flash mode.

---

## 3. Credentials

```bash
cp include/settings/credentials.example.h include/settings/credentials.h
```

Edit `credentials.h`:

```cpp
#define WIFI_SSID   "your_wifi_ssid"
#define WIFI_PASS   "your_wifi_password"
#define MQTT_IP     "192.168.x.x"
#define MQTT_PORT   1883
#define MQTT_USER   "your_mqtt_user"
#define MQTT_PASS   "your_mqtt_password"
```

> `credentials.h` is in `.gitignore` — it will never be committed.

---

## 4. Build environments

Defined in `platformio.ini`. Each environment combines a chip target and a device type via compiler flags:

| Environment | Chip | Device | Flags |
|---|---|---|---|
| `blind_esp8266` | ESP8266 (TYWE3S) | Blind controller | `DEVICE_HARDWARE_ESP8266` + `DEVICE_TYPE_BLIND` |
| `blind_bk7231n` | BK7231N (CB3S) | Blind controller | `DEVICE_HARDWARE_BK7231N` + `DEVICE_TYPE_BLIND` |
| `light_bk7231` | BK7231N (T34) | Light switch | `DEVICE_HARDWARE_BK7231N` + `DEVICE_TYPE_LIGHT` |

Dependencies pulled automatically by PlatformIO on first build:
- `knolleary/PubSubClient` — MQTT client
- `vshymanskyy/Preferences` — NVS flash storage
- `ESP8266WiFi`, `ESP8266WebServer`, `ESP8266httpUpdate` — ESP8266 network stack

---

## 5. Flash

Replace `/dev/ttyUSB0` with your actual port (`/dev/cu.usbserial-*` on macOS, `COM*` on Windows).

### ESP8266 (TYWE3S)

```bash
# Erase + build + flash
esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 115200 erase_flash && \
pio run -e blind_esp8266 -t upload --upload-port /dev/ttyUSB0
```

### BK7231N (CB3S) — blind

```bash
# Erase + build + flash
ltchiptool flash erase --device /dev/ttyUSB0 && \
pio run -e blind_bk7231n -t upload --upload-port /dev/ttyUSB0
```

### BK7231N (T34) — light

```bash
# Erase + build + flash
ltchiptool flash erase --device /dev/ttyUSB0 && \
pio run -e light_bk7231 -t upload --upload-port /dev/ttyUSB0
```

---

## 6. Flash a pre-built binary

If you downloaded a `.bin` from the [releases page](https://github.com/mtaibo/tphome-firmware/releases) instead of building from source:

### ESP8266

```bash
esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 115200 erase_flash && \
esptool.py --chip esp8266 --port /dev/ttyUSB0 --baud 115200 \
  --flash_mode dout --flash_size 1MB \
  write_flash 0x0 tphome-firmware_v2.0.0_blind_esp8266.bin
```

### BK7231N

```bash
ltchiptool flash erase --device /dev/ttyUSB0 && \
ltchiptool flash write --device /dev/ttyUSB0 tphome-firmware_v2.0.0_blind_bk7231n.bin
```

---

## 7. Subsequent updates via OTA

Once a device is running and connected to the network, all future updates are done wirelessly — no physical access needed. The `tphome-api` backend handles serving the binary and triggering the update via MQTT.
