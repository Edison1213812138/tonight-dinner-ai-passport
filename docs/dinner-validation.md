<p align="right"><a href="dinner-validation.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Dinner validation report

Validated on 2026-09-16 with ESP-IDF 5.5.3, LVGL 9.5.0, application version
`1.1.0-dinner`, branch `feature/tonight-dinner`, based on main
`25add0044cb3e8ce2319c258ae96551f0ffcc2df`. Source changes are not committed.

| Check | Result |
| --- | --- |
| Build | **PASS / BUILD SUCCESS** — actual `./tools/validate.sh` complete gate |
| Host tests | **PASS** — upstream tests, 60 unique foods / 12 × 5 categories, 640 filter combinations, 1,000 draws, no immediate repeats, navigation, debounce, time rollover, empty/single pool |
| Storage tests | **PASS** — mocked NVS first boot, restart, schema/version/range/length errors and initialization/write/commit failures |
| Fonts | **PASS** — 376 codepoints × 3 sizes, actual LVGL descriptors and known-missing negative case |
| UI stress | **PASS** — 4,500 state renders, 23 fixed objects, bounded allocation, all names/fonts exercised |
| Firmware layout/archive | **PASS** — image offsets, partition checks, bundle hashes and matching ELF verified |
| Device tests | **PARTIAL** — flash/hash, serial startup and user-confirmed home/draw PASS; remaining device checks pending |

## Measured sizes

| Resource | Bytes |
| --- | ---: |
| Verified merged installable BIN | 752,160 |
| Application-only BIN | 686,624 |
| Three app fonts plus codepoint inventory | 129,154 |
| All linked font resources including default Montserrat 14 | 142,803 |
| Images linked into firmware | 0 |
| Application C/header source including replaced main.c, excluding generated fonts | 39,135 |
| Linked application code/data excluding fonts, as attributed by linker MAP | 11,477 |
| Application-specific static RAM attributed by MAP | 353 |

Both installable BIN files satisfy **strictly less than 3,000,000 bytes**. Source
reference PNG, OTF development subset, screenshots, ELF and MAP are not installed
resources. ELF/MAP are debug files; their larger disk sizes do not count as firmware.

Linker static unified internal-memory usage is 94,946 bytes (6,940 data + 29,984
BSS + 58,022 IRAM text); 226,350 bytes remain in that linker region before runtime
allocations. This is **not measured runtime free heap**. The LVGL pool stays 24 KB,
BSP DMA is 9,600 bytes, and the optional battery task requests a 3,072-byte stack.
The host LVGL stress test ended with 3,440 pool bytes free and 2,816 bytes in the
largest free block. Host pointer sizes/configuration differ from ESP32-C3; the
real-device heap, largest block and task stack margins remain unverified. The
remaining pool margin is modest; do not add large labels, shadows or image buffers
without repeating the memory checks. Constant font resources use Flash; only
rendering buffers and bounded label copies consume runtime memory.

## Exact deliverable identity

- Merged BIN SHA-256: `61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb`
- Matching ELF SHA-256: `cb86d09e0f1b5d1fe0dcd3cac50d82d260aa65d926c560e3881d6089ba6f62ae`
- Verified archive: `build/firmware/61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb/`
- Flashable image: `build/FoloToy-AI-Passport-full.bin`, offset **0x0**.
- Logs: `build/validation.log`, `build/ui-validation.log`.
- Numeric details: `build/dinner-size-report.json`, `build/size-files.json`, `build/memory.json`.
- Actual LVGL previews: `build/preview/four-pages.png` and per-state PNG/PPM files.

The merged image can reset existing NVS settings. A compatible segmented update
uses the same archive's image files/`flash_args` to preserve unaddressed NVS.
No full-chip erase is needed or authorized. The retained debug bundle was checked
with `python3 tools/archive_firmware.py verify` against this exact image.

## Authorized device flash and startup observation

After the user explicitly approved flashing the cartoon build, wrote the exact
verified 752,160-byte merged image at `0x0` on `/dev/cu.usbmodem1101`.
Esptool identified ESP32-C3 revision 1.1 with 8 MB embedded Flash. Write-region
sectors `0x00000000..0x000b7fff` were erased, not the whole chip; written data hash
verification passed. Esptool reset the device after writing.

An 18-second serial observation confirmed `1.1.0-dinner` and ELF prefix
`cb86d09e0`, display initialization at 293 ms, LVGL at 348 ms, buttons at 530 ms,
and CW2017 detection/profile matching. No panic, assertion, watchdog or error log
was observed in that window. These timestamps establish initialization only,
not the actual visible first-frame time. The monitor was closed and the port
released. Raw local logs: `build/flash-1.1.0.log`,
`build/device-startup-1.1.0.log`; structured results: `build/device-test-1.1.0.json`.

## Unverified

The user explicitly confirmed that both the cartoon home and the draw work
normally. Home display, OK-triggered rolling and the final result therefore pass
as user-observed checks. Complete glyph/corner inspection, other keys/long presses,
first-frame timing, 100 draws, sustained heap stability, NVS power-cycle retention,
battery accuracy and dimming remain unverified. Continue with the remaining
[device acceptance steps](dinner.md).
