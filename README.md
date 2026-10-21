# Pulse • Appliance Monitor

Live electrical monitoring, visible load changes, and saved demonstration results.

This repository contains the existing Pulse website in **HTML, CSS, and vanilla JavaScript**, together with the existing **Arduino C++ ESP32 USB firmware**. The website files and firmware sketch are preserved unchanged from the working project. No build system, npm install, API key, or server is needed for the current website.

## Repository contents

| Path | Purpose |
| --- | --- |
| `docs/index.html` | Existing website layout |
| `docs/style.css` | Existing website styling |
| `docs/app.js` | Live charts, Web Serial, recordings, and exports |
| `firmware/Pulse_ESP32_USB/Pulse_ESP32_USB.ino` | Working ESP32 firmware |
| `firmware/Pulse_ESP32_USB/README.md` | Original firmware setup guide |
| `firmware/Pulse_ESP32_USB/VALIDATION.md` | Historical firmware validation notes |
| `guides/DATA_FORMAT.md` | Data contract and saved-result behavior |
| `guides/ROADMAP.md` | Extension points for future analytics and Wi-Fi |
| `VALIDATION.md` | Checks performed on this export |

## Publish with GitHub Pages

1. Extract this ZIP. Create a GitHub repository, for example `pulse-appliance-monitor`. A public repository is the simplest option for GitHub Free Pages hosting; its source and published website will be publicly visible.
2. Upload the **contents inside** `Pulse_GitHub_Repo` to the repository root and commit them to `main`. Do not upload only the ZIP or add an extra outer folder. At the repository root you should see `README.md`, `docs`, `firmware`, and `guides`.
3. Open **Settings → Pages**. Under **Build and deployment**, choose **Deploy from a branch**, select **main** and **/docs**, then **Save**.
4. Wait for GitHub's Pages deployment to finish. Open the published HTTPS address shown in Settings → Pages, normally `https://YOUR-USERNAME.github.io/pulse-appliance-monitor/`.
5. Future changes committed to `docs/` on `main` will be published through Pages. Keep filenames and relative links intact.

Official instructions: https://docs.github.com/en/pages/getting-started-with-github-pages/configuring-a-publishing-source-for-your-github-pages-site

No custom GitHub Actions workflow is required for this branch-based setup. If uploading through the browser omits `.nojekyll`, this plain HTML/CSS/JS site still works; retain it when using Git.

## Run your demonstration

If your current firmware is already working, **keep it installed**. The sketch is included for backup and reproducibility, not because a reflash is required.

1. Plug the ESP32 into the laptop with a USB data cable. Use your already verified sensor wiring.
2. Close Arduino Serial Monitor/Plotter and other software using that serial port.
3. Open the GitHub Pages website directly in desktop Chrome or Edge.
4. Go to **Connection & setup**, click the USB connect button, and select the ESP32 port. The website opens it at **115200 baud** and switches to hardware mode.
5. Wait for live readings, name the appliance, and start recording. Change the appliance load to demonstrate the chart response; add observations if useful.
6. Stop and save. Open Saved sessions to review results and download CSV, JSON, or a graph PNG for your PPT.

The simulation mode remains available for rehearsals and is labelled separately from measured hardware data. Web Serial requires a supported browser and a user-approved port connection. See https://developer.chrome.com/docs/capabilities/serial.

## What “24/7 hosting” means here

GitHub Pages serves the website continuously, subject to its service availability and limits. It does **not** run the ESP32, store measurements in a cloud database, or collect USB readings when the laptop is off.

Current data path: PZEM → ESP32 firmware → USB → browser → local saved sessions / downloaded exports.

For live collection, keep the sensor and ESP32 powered, USB connected, the laptop awake, and the browser page running. A session automatically saves at 14,400 readings (about four hours at one reading per second); another session must then be started. This release is intended for demonstrations, not unattended continuous logging.

For future collection independent of a laptop, add ESP32 Wi-Fi transmission, an authenticated ingestion service, and persistent database storage. GitHub Pages can continue to host the frontend.

## Preserve existing recordings before changing websites

Saved sessions live in this browser's localStorage, scoped to the website origin. Your previous site's saved sessions will **not** automatically appear on the new GitHub Pages address. Export important sessions as JSON/CSV from the old website before moving. This version has no file-import UI. Clearing browser data can remove saved sessions; downloaded exports are your backup. Stop and save before closing the page.

## Local preview

With Python 3 installed, from the repository root run:

```sh
python3 -m http.server 8000 --directory docs
```

Open `http://localhost:8000` in desktop Chrome or Edge. Use localhost for serial testing; an ordinary HTTP address on another machine may not meet the browser's secure-context requirement. Google Fonts is an optional external font dependency; system font fallbacks are included.

## Firmware and hardware

Firmware setup is documented in `firmware/Pulse_ESP32_USB/README.md`. Open the `.ino` file in Arduino IDE; use the Espressif ESP32 board package and the PZEM004Tv30 library by Jakub Mandula. The sketch uses GPIO16 as ESP32 RX and GPIO17 as ESP32 TX and emits newline-delimited JSON once per second.

Keep the confirmed working wiring. The sensor includes hazardous mains connections: disconnect power before handling it and have mains wiring secured and checked by a qualified person. Do not infer mains terminal positions from these software files.

## Future development

Baseline learning, health scores, and failure prediction are **not implemented** in this release. See `guides/ROADMAP.md` for the planned extension points. Keep raw measured records and simulation records distinguishable when developing algorithms.
