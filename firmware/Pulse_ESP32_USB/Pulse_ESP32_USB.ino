/*
 * Pulse ESP32 USB firmware v1.0.0
 * Target: classic ESP32 DevKit / ESP-WROOM-32, PZEM-004T-100A V4.
 * Library: PZEM004Tv30 by Jakub Mandula (Library Manager).
 *
 * Direct 3.3 V wiring, ONLY for the confirmed compatible PZEM revision:
 * PZEM VCC -> ESP32 3V3; GND -> GND; TX -> GPIO16; RX -> GPIO17.
 * No level shifter, OE pin, GPIO25 connection or OE resistor is used.
 *
 * USB Serial: 115200 baud, one JSON object followed by newline per sample.
 * Meter UART: Serial2, 9600 8N1, initialized by the PZEM library.
 * Matches the existing Pulse dashboard's six field names and units.
 * No Wi-Fi backend, fabricated readings, automatic energy reset, or relay control.
 *
 * Disconnect mains and USB before changing wiring. Mains terminals must be
 * professionally checked and enclosed before operating beside a USB-connected PC.
 */

#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <math.h>

#if !defined(ARDUINO_ARCH_ESP32)
#error "Select an ESP32 board, such as ESP32 Dev Module."
#endif

constexpr uint8_t PZEM_RX_PIN = 16; // ESP32 receives from PZEM TX
constexpr uint8_t PZEM_TX_PIN = 17; // ESP32 transmits to PZEM RX
constexpr uint32_t USB_BAUD = 115200;
constexpr uint32_t SAMPLE_INTERVAL_MS = 1000;

// Default general address 0xF8: use exactly one PZEM on this UART.
PZEM004Tv30 pzem(Serial2, PZEM_RX_PIN, PZEM_TX_PIN);

struct Reading {
  float voltage;
  float current;
  float power;
  float energy;
  float frequency;
  float powerFactor;
};

// Explicit declarations keep Arduino's automatic prototype generation from
// placing functions that use Reading before the struct definition.
bool readMeter(Reading &r);
void emitReading(const Reading &r, bool ok);

uint32_t lastAttemptMs = 0;
uint32_t sequenceNumber = 0;
uint32_t consecutiveErrors = 0;

bool validNonnegative(float value) {
  return isfinite(value) && value >= 0.0f;
}

bool readMeter(Reading &r) {
  // The library reads/caches the complete Modbus register frame.
  // Stop immediately if the first request fails, avoiding six timeouts.
  r.voltage = pzem.voltage();
  if (!validNonnegative(r.voltage)) return false;

  r.current = pzem.current();
  r.power = pzem.power();
  r.energy = pzem.energy(); // Already kWh: DO NOT divide by 1000 again.
  r.frequency = pzem.frequency();
  r.powerFactor = pzem.pf();

  return validNonnegative(r.current) && validNonnegative(r.power) &&
         validNonnegative(r.energy) && validNonnegative(r.frequency) &&
         validNonnegative(r.powerFactor) && r.powerFactor <= 1.0f;
}

void printMeasurement(const char *name, float value, uint8_t decimals, bool ok) {
  Serial.print('"');
  Serial.print(name);
  Serial.print("\":");
  if (ok) Serial.print(value, decimals);
  else Serial.print("null");
  Serial.print(',');
}

void emitReading(const Reading &r, bool ok) {
  Serial.print('{');
  printMeasurement("voltage", r.voltage, 1, ok);
  printMeasurement("current", r.current, 3, ok);
  printMeasurement("power", r.power, 1, ok);
  printMeasurement("energy", r.energy, 3, ok);
  printMeasurement("frequency", r.frequency, 1, ok);
  printMeasurement("powerFactor", r.powerFactor, 2, ok);
  Serial.print("\"sensorOk\":");
  Serial.print(ok ? "true" : "false");
  Serial.print(",\"error\":");
  Serial.print(ok ? "null" : "\"pzem_read_failed\"");
  Serial.print(",\"consecutiveErrors\":");
  Serial.print(consecutiveErrors);
  Serial.print(",\"sequence\":");
  Serial.print(sequenceNumber++);
  Serial.print(",\"uptimeMs\":");
  Serial.print(millis());
  Serial.println('}');
  // No timestamp: the dashboard assigns real browser arrival time.
  // Never send uptime as an ISO timestamp or invent a calendar date.
}

void setup() {
  Serial.begin(USB_BAUD);
  // The PZEM constructor initializes Serial2 at the meter's baud rate.
  // Do not reopen Serial2 at 115200; that rate is only for USB Serial.
  delay(1000);
  lastAttemptMs = millis() - SAMPLE_INTERVAL_MS;
  // Deliberately no resetEnergy(), setAddress(), or calibration writes.
}

void loop() {
  const uint32_t now = millis();
  // Unsigned subtraction keeps the schedule working across millis() rollover.
  if (static_cast<uint32_t>(now - lastAttemptMs) < SAMPLE_INTERVAL_MS) {
    delay(1);
    return;
  }
  lastAttemptMs = now;

  Reading r = {};
  const bool ok = readMeter(r);
  if (ok) consecutiveErrors = 0;
  else if (consecutiveErrors < UINT32_MAX) ++consecutiveErrors;
  emitReading(r, ok);
}
