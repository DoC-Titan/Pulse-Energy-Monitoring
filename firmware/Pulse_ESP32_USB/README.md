# Pulse ESP32 USB firmware

This firmware is for Vedant's confirmed 3.3 V-compatible PZEM-004T-100A V4 and
a classic ESP32 development board (ESP-WROOM-32 / ESP32 Dev Module).

Dashboard: https://pulse-appliance-monitor.citrusgoose128.chatgpt.site

## What connects today

PZEM -> ESP32 UART -> USB cable -> desktop Chrome/Edge -> existing Pulse dashboard.

The existing deployed dashboard source was inspected on 16 October 2026. It
opens Web Serial at 115200 baud and consumes newline-delimited JSON with the
six fields below. This firmware matches that implementation; no website edits
are needed. The website can be online while sensor data arrives locally by USB.

This release does not transmit by Wi-Fi. The current website has no Wi-Fi
ingestion endpoint or database. Firmware alone cannot add those. No Wi-Fi
password, cloud token or invented server URL is embedded in this sketch.

## Wiring (all power disconnected)

| PZEM communication pin | ESP32 development board |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| TX | GPIO16 / RX2 |
| RX | GPIO17 / TX2 |

Use this direct connection only for the actual board confirmed to support
3.3 V interface power and UART. Remove the earlier level-shifter connections;
GPIO25/OE and its resistor are not used by this firmware. Never feed 5 V into
an ESP32 GPIO. Electronics GND is not mains neutral or protective earth.

The PZEM measurement circuitry still needs its correct AC supply. The CT must
surround only the insulated live conductor actually feeding the tested load,
not the complete extension cable or only the added PZEM voltage-sensing wire.
Keep the CT leads connected to the correct CT terminals before energizing.
Have the modified extension, wire ratings, terminations, strain relief and
enclosure checked by an electrician. Do not operate exposed mains beside a PC.

## Upload in Arduino IDE

1. Install Arduino IDE and the `esp32` board package by Espressif Systems.
   If needed, add this Additional Boards Manager URL in Preferences:
   https://espressif.github.io/arduino-esp32/package_esp32_index.json
2. In Library Manager install **PZEM004Tv30** by **Jakub Mandula**. Do not use
   the older library named just PZEM004T. The v3 Modbus protocol is also used
   by the compatible v4 variant; the library name does not have to say v4.
3. Extract this ZIP. Keep `Pulse_ESP32_USB.ino` inside `Pulse_ESP32_USB/` and
   open it in Arduino IDE.
4. Select **ESP32 Dev Module** (or the exact matching classic ESP32 DevKit)
   and its COM port. This sketch is not configured for an ESP32-C3/S3 board.
5. Keep the extension unplugged while uploading. Connect the ESP32 using a
   USB DATA cable, then click Upload. If upload stalls at Connecting, hold
   BOOT until writing begins, then release it.
6. Once the mains assembly has been checked and enclosed, power the sensor.
   Open Serial Monitor at **115200 baud** to see one JSON line per second.
7. Close Serial Monitor AND Serial Plotter before connecting the website.
   Only one program/tab should hold the COM port at a time.

## Connect the existing website

1. Open the dashboard URL above directly in desktop **Chrome or Edge** and
   sign in if prompted. Use a normal top-level tab, not an embedded preview.
2. Open **Connection & setup** -> **Connect ESP32**.
3. Choose the ESP32's USB serial/COM device in the browser permission dialog.
4. Return to **Live monitor**. The source should become **ESP32 · USB serial**.
5. Set the appliance name, e.g. `Phone charger test`, and wait for numeric
   readings. Click **Start recording**. Later click **Stop & save**.
6. Find the run under **Saved sessions** and export CSV or JSON.

Sessions are stored in this browser, not in the ESP32 or a cloud database.
Keep the tab and USB connection open during recording. Background tab
throttling or a sleeping laptop can interrupt collection. Export important runs.
The current website caps each session at 14,400 readings (about four hours at
one reading per second). Clearing browser storage can delete saved sessions.

## Exact data contract

| JSON key | Unit |
| --- | --- |
| voltage | V |
| current | A |
| power | W (active power) |
| energy | kWh, cumulative PZEM counter |
| frequency | Hz |
| powerFactor | Ratio 0 to 1 |

Illustrative healthy line (not measured data):

```json
{"voltage":230.1,"current":0.080,"power":12.4,"energy":0.012,"frequency":50.0,"powerFactor":0.67,"sensorOk":true,"error":null,"consecutiveErrors":0,"sequence":0,"uptimeMs":1000}
```

The browser provides the real timestamp when it receives the line. `uptimeMs`
is only a diagnostic counter, not a date. The firmware does not reset meter
energy on boot. Session energy in the site is calculated separately from power.

On a failed/nonfinite/out-of-range-PF read, all six measurements are `null`,
`sensorOk` is false and `error` is `pzem_read_failed`. It retries on the next
sampling interval and automatically recovers after a good read. It never sends
NaN, fake zero readings, simulated values or an old sample as a fresh one.

The current website ignores extra diagnostic keys. It displays null metrics as
dashes and saves missing-value quality flags. Its green "Receiving hardware
data" badge means USB records are arriving, not that the sensor is healthy.
Check for numeric metrics, or inspect `sensorOk` in Serial Monitor. The website
does not currently export these extra firmware diagnostic keys.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| No COM port | Data-capable USB cable, correct board/port, board's USB driver |
| Port busy / browser cannot open | Close Serial Monitor, Plotter, and other tabs using the port |
| A few discarded lines immediately after reset | ESP32 ROM boot text is not JSON; subsequent sketch output is JSON only |
| Every metric is a dash | Sensor AC supply, communication VCC/GND, crossed TX/RX, correct UART pins; disconnect power before rewiring |
| Voltage/frequency work but power/current stay near zero | Actual load running and correct single conductor through CT; a missing CT cannot be diagnosed from UART success alone |
| Tiny charger load is poorly resolved | 100 A CT has a low-current measurement limit; phone near full charge may draw very little |
| Meter kWh changes slowly | Meter energy has 1 Wh resolution; site session energy can update sooner by integrating power |
| Sensor reads recover later | No reset needed; firmware keeps retrying |

Do not alter calibration or reset energy as a first troubleshooting step.

## References

- PZEM library and ESP32 example: https://github.com/mandulaj/PZEM-004T-v30
- v3/v4 protocol compatibility: https://github.com/mathieucarbou/MycilaPZEM
- ESP32 Arduino installation: https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html

## Verification scope

The actual deployed dashboard's parser and field names were inspected.
See VALIDATION.md for checks performed. No physical ESP32/PZEM was available
here: a hardware reading and the browser's COM-port permission must be tested
on your computer. This package does not certify the mains wiring.
