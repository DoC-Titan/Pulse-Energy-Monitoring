# Data contract

## USB transport

115200 baud, newline-delimited JSON (one complete object followed by a newline). Meter communication is handled separately by the firmware. The current website does not connect over Wi-Fi or MQTT.

Illustrative valid record, not an actual measurement:

```json
{"voltage":230.1,"current":0.080,"power":12.4,"energy":0.012,"frequency":50.0,"powerFactor":0.67,"sensorOk":true,"error":null,"consecutiveErrors":0,"sequence":0,"uptimeMs":1000}
```

| Key | Unit | Accepted website value |
| --- | --- | --- |
| voltage | V | Finite number ≥ 0 |
| current | A | Finite number ≥ 0 |
| power | W | Finite number ≥ 0 |
| energy | kWh | Finite number ≥ 0; cumulative meter energy |
| frequency | Hz | Finite number ≥ 0 |
| powerFactor | dimensionless | Number from 0 to 1 |

Zero is a valid measurement. Missing, nonnumeric, negative, or out-of-range values become `null`, with quality flags. A failed sensor read makes the firmware emit all six measurements as `null`, `sensorOk:false`, and `error:"pzem_read_failed"`. Never replace missing measurements with zeros.

Firmware diagnostic keys (`sensorOk`, `error`, `consecutiveErrors`, `sequence`, `uptimeMs`) are currently ignored by the website and are not retained in exported readings. The receiving indicator means records are arriving, not that the sensor is healthy. Inspect measurements and quality flags too.

The browser adds ISO `timestamp` and `receivedAt` values. Optional incoming timestamps are used only if valid and within 24 hours of the browser clock. This firmware does not supply wall-clock timestamps.

## Saved sessions

localStorage key: `pulse.sessions.v1`.

JSON exports contain `schemaVersion:1`, `id`, `appliance`, `source`, `startedAt`, `endedAt`, `readings`, `notes`, and `analysis`. `analysis` is currently an empty array reserved for later additions. Source is `serial` for USB hardware or `demo` for simulation.

CSV exports contain timestamp, receivedAt, source, voltage, current, power, energy, frequency, powerFactor, and quality. Notes and the complete session structure are in JSON.

Live charts show approximately the last 120 seconds, capped at 300 points. Readings become stale after five seconds. Recordings stop and save at 14,400 readings.

Session energy is computed in Wh by trapezoidal integration of valid adjacent power samples separated by no more than five seconds. Missing intervals are excluded. This is separate from the cumulative meter energy field in kWh. Average power is the arithmetic mean of valid samples. Peak power is the highest received sample; it is not a high-speed inrush-current measurement.
