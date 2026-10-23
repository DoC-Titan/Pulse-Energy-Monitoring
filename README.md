# Pulse Studio · 2.0

A minimal appliance-monitoring website in HTML, CSS, and vanilla JavaScript. Includes the original working ESP32 USB firmware, unchanged.

## What is new

- Graphite monochrome/light default.
- Six palettes: Graphite, Forest, Ocean, Violet, Rose, Amber. Each supports light and dark; System mode follows the device preference.
- Custom graph accent, line thickness, and optional area shading. Appearance persists in the browser.
- Compact readings strip, connection menu, settings drawer, and expandable observations.
- Fit fluctuations, vertical +/− zoom, zero-based reset, 30s/1m/2m/5m windows, hover readings, frozen view, and focus view.
- USB monitoring at 115200 baud; clearly labelled simulation for rehearsals.
- Local saved sessions, CSV/JSON exports, graph PNG, and import of existing Pulse schemaVersion 1 JSON sessions.

## Run locally

From this folder, with Python 3:

```sh
python3 -m http.server 8000 --directory docs
```

Open http://localhost:8000 in desktop Chrome or Edge. No npm packages or build step are required. All website assets are included; no CDN fonts or chart libraries are needed.

## GitHub Pages

The downloadable repository uses a `docs/` website folder. Upload its contents to your GitHub repository, keeping the directory structure. In Settings → Pages, choose Deploy from a branch, main, and /docs. GitHub will show the published HTTPS URL. The deployed GPT Site uses the identical website files.

Official guide: https://docs.github.com/en/pages/getting-started-with-github-pages/configuring-a-publishing-source-for-your-github-pages-site

## Connect the working ESP32

Keep your installed firmware if it already works. Open the website directly in desktop Chrome or Edge, close Arduino Serial Monitor, and select Connect device. Choose the ESP32 serial port. The browser will show HARDWARE after connection. The site starts in clearly labelled Demo mode until you connect.

Do not change the already verified sensor wiring for this UI update. The included firmware README is the original hardware/setup reference. Hardware and mains-side assembly should be handled with power disconnected and qualified supervision.

## Zoom explained

**Fit fluctuations** fits the Y axis around the measurements in the current time window. **+** narrows the range; **−** widens it. **Reset** restores a zero-based axis. A magnified chart can make small changes appear large, so its bounds are always shown. Readings outside the zoomed range are counted in the caption. Zoom cannot reveal variation smaller than the sensor's measurement resolution.

Time-window selection changes how much history is visible. **Freeze view** preserves that chart window while monitoring and recording continue. **Resume live** follows new samples again. Missing measurements and time gaps over five seconds are not connected. The single-second data stream cannot capture fast inrush transients.

## Record and move results

Name the appliance, start recording, optionally add notes, then Stop & save. Sessions → Review provides summaries and a presentation-ready PNG. Export JSON for the complete session or CSV for readings. Use Import JSON to move a saved session from the old Pulse site to this new address.

Browser storage is specific to the website origin and browser profile. A new Site or GitHub address starts with separate saved sessions. Export important data before clearing browser storage or switching computers. Sessions stop at 14,400 samples (about four hours at one sample/second). A storage-full message means you should export before closing.

GitHub/GPT hosting serves the website; it does not collect USB data when your browser is closed. Keep the laptop awake and connected while collecting. Wi-Fi ingestion, cloud history, baseline learning, health scoring, and failure prediction are future features.

## Data contract

Firmware emits one JSON object per line, ending in a newline:

```json
{"voltage":230.1,"current":0.080,"power":12.4,"energy":0.012,"frequency":50.0,"powerFactor":0.67,"sensorOk":true,"error":null,"consecutiveErrors":0,"sequence":0,"uptimeMs":1000}
```

This is an illustrative example, not a measured result. Units: V, A, W, kWh, Hz, and dimensionless power factor. Zero is valid; missing/invalid values remain null. Sensor errors are shown separately from an active serial link. The browser timestamps incoming samples. Session energy integrates power in Wh across valid adjacent samples up to five seconds apart. Meter energy is the independent cumulative kWh counter.

Saved sessions retain `schemaVersion:1` with appliance/source metadata, readings, notes, and an `analysis` array for future versioned algorithm results. `core.js` isolates validation, summaries, and chart scaling from UI code. Simulation data must not be mixed into real training data.

## Checks

```sh
node --check docs/app.js
node tests/core.test.cjs
```

Tests cover measurement validity, zero values, sensor errors, integration gaps, fitted chart bounds, zoom clipping, and session imports. Browser verification covered all 12 palette/mode combinations, recording/save/review, graph zoom, and PNG download. USB hardware was not accessible during redesign verification; the firmware and serial contract are unchanged.

## Files

- `docs/index.html`: layout and menus
- `docs/style.css`: responsive light/dark styling
- `docs/core.js`: measurement utilities
- `docs/app.js`: themes, charts, USB, recordings, exports/imports
- `docs/icon.svg`: local Pulse icon
- `firmware/Pulse_ESP32_USB/`: original firmware and historical documentation
- `tests/core.test.cjs`: dependency-free core checks
