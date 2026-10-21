# Export validation

The owner reported successful live hardware readings before this export. This packaging pass did not repeat a physical hardware test or compile/reflash the ESP32.

- All three website files match the existing deployed-source files byte for byte.
- Every file from the supplied working firmware ZIP is preserved byte for byte under `firmware/`.
- JavaScript syntax checked with `node --check`.
- Website measurement normalization checked with a firmware-shaped sample, valid zero readings, sensor-failure null values, and an out-of-range power factor.
- Relative stylesheet/script references and ZIP contents checked.
- No credentials, deployment metadata, or personal measurement logs included.

The repository has not been uploaded to GitHub or deployed to GitHub Pages by this export. Browser/USB behavior on the new URL should be confirmed after publishing. Historical firmware checks remain documented in `firmware/Pulse_ESP32_USB/VALIDATION.md`.

## Website checksums

- `index.html`: byte-identical; SHA-256 `5e0183454c6fd0cf5d4e8894db2ed132b2351275821f58fb863a2e8a44508cfd`.
- `style.css`: byte-identical; SHA-256 `ea5226e4a315aa8547378e1b64cca90ffd6ce4bc8a153795c4d64a5779ce7895`.
- `app.js`: byte-identical; SHA-256 `ed05fc60ab00acb90b1dd2f9e1aabb3bb53fcc7c7c442550d3c17d095fbcb76c`.
