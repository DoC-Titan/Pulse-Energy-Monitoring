# Verification — 16 October 2026

## Passed

- Retrieved the authenticated, currently deployed Pulse dashboard's `app.js`
  and `index.html` and checked the Web Serial handler, field names, units,
  timestamp fallback, error sanitization and recording behaviour.
- Checked the upstream PZEM004Tv30 header/implementation: ESP32 constructor
  takes Serial2, RX, TX; initializes 9600 8N1; getters expose the six readings.
- Compiled the complete sketch as C++17 with `-Wall -Wextra -Werror` using
  host-side Arduino/Serial/PZEM test doubles. This checks sketch syntax and
  application logic, not the actual ESP32 toolchain or peripheral drivers.
- Executed the sketch against controlled good, unavailable, zero-load,
  invalid-power-factor and recovered readings, plus millis rollover.
- Parsed the resulting NDJSON with the actual deployed site's `clean()`
  function. Confirmed exact field spelling, kWh units, valid JSON, preserved
  genuine zero readings, and six null metrics/quality flags on failure.
- Checked that the deployed site's `stats()` excludes missing-reading gaps
  and integrates uninterrupted valid power samples correctly.
- Checked the 1000 ms schedule, error-counter reset after recovery, sequence
  increments, and early return after an initial failed sensor read.

## Not verified here

- Native ESP32 compilation/upload: Arduino CLI was available, but its board
  index download could not reach the network, so a full board build was not
  completed. The user must compile/upload in Arduino IDE.
- Physical PZEM V4 communication, mains wiring, CT installation, measurement
  accuracy, and real browser-to-COM-port permission/connection.
- Wi-Fi telemetry is not implemented; the existing site only supports USB.

The firmware matches the existing site's software contract. This is not a
claim that physical hardware has been tested or the mains assembly is safe.
