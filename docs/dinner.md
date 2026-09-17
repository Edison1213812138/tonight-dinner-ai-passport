<p align="right"><a href="dinner.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# Tonight's Dinner

A completely offline three-button lottery for FoloToy AI Passport (ESP32-C3,
240 × 320, no PSRAM). Boot directly into the application's redesigned home.
Dark background, warm white text, muted gray and yellow focus follow the supplied
wireframe. No photo, GIF, network service, audio or complete Chinese font is linked.

## Baseline and implementation

Started from official main `25add0044cb3e8ce2319c258ae96551f0ffcc2df` on
`feature/tonight-dinner`. The originally supplied `ai-passport-main` archive is
untouched. Read AGENTS, the AI guide and hardware guide; inspected the official
`demo/*` inventory and `demo/rock-paper-scissors` input pattern, plus the
`docs/reference/shinku-chen/eat-what` and offline Pokédex records. The current
repository puts reference records in `docs/reference/`; root `reference/` and
`plays/` are absent. Older demo callback code is not copied because current BSP
callbacks must only enqueue work. No BSP or partition-table changes were needed.

The four main pages are home, two-phase draw, result and settings. Submenus,
empty-pool recovery, reset confirmation and the lightweight app entry are states
inside the same persistent screen. The baseline is a standalone firmware template,
not a plugin loader: home long-OK returns to this app's minimal entry, not a second
installed stock firmware. It never enters the hardware-test demo shell.

## Cartoon decoration update (1.1.0)

Home now has a small cartoon food band above Start: twin-arch fries, a smiling
burger, a drink and a crowned snowman inspired by the supplied fast-food/ice-drink
references. Draw has a snowman and fries; result has tiny celebratory stickers;
settings has a snowman and cup beside the heading. Dense submenus stay clear.
The stickers retain the existing navy/cream/gray/yellow palette and are procedural
LVGL rectangles and strokes in the screen's background draw event. They add no
bitmap resource, canvas buffer, LVGL object, or timer. Changing state invalidates
the background so old stickers are cleared. The screen still has 23 objects.
The actual 24 KB-pool host stress test passes 4,500 renders; final pool free space
is 3,440 bytes with a 2,816-byte largest block. Device rendering remains unverified.

## Buttons

| Context | UP / DOWN | Short OK | Long OK |
| --- | --- | --- | --- |
| Home | Switch Start / Settings | Execute selected item | Exit to app entry |
| Draw / Easter egg | Ignored | Ignored | Cancel to home |
| Result | Confirm / Retry / Settings | Execute selected item | Home |
| Settings | Move selection, wrapping | Enter submenu | Previous page (home or result) |
| Mode / Budget | Move selection | Select and save | Settings |
| Diet | Move tag | Toggle excluded tag and save | Settings |
| Dislike categories | Move category; six visible rows scroll | Open category | Settings |
| Dislike foods | Move whole-category switch / five foods | Toggle exclusion and save | Category list |
| Empty pool | Restore defaults / Settings | Execute selection | Home |
| Reset confirmation | Cancel / Confirm (Cancel is default) | Execute selection | Settings |
| Confirmed | Ignored | Ignored | Home; otherwise automatic after 1.5 s |
| App entry | Single entry | Re-enter home | Stay at entry |

`[x]` means selected/excluded; `[ ]` means unselected/allowed. A whole-category
exclusion takes priority without destroying individual food choices. BSP ADC
debounce is retained; an extra 90 ms application guard rejects duplicate events.
A BSP double-click event performs one ordinary selection, never a special action.
After 60 seconds idle the backlight dims to 8%; any accepted key restores 75%.

## Food and selection

All 12 categories and all 60 names from the requirement are compiled in
`main/dinner_food_database.c` as one constant table (five foods per category).
Budget means an entertainment spending **ceiling**: 0 unrestricted, 1 cheap,
2 normal, 3 all levels. The five exclusion bits are spicy, seafood, beef/lamb,
raw and fried. Tags describe common recipes only, not guaranteed ingredients.

Cheap, strong-flavor and party modes give matching foods 4× weight; light mode
excludes spicy, oily and fried items. Hard exclusions always win. A fixed
60-byte candidate list is built, the previous result is removed when there is
another choice, a weighted category is selected, and then a weighted food inside
it. Category weights equal the eligible food weights, avoiding bias from varying
category sizes. A local xorshift generator is seeded by ESP random data and uptime;
bounded rejection sampling is used for selection. No radio starts for entropy.

The LVGL timer updates text only when due: category 0–800 ms, final category hold
800–1150 ms, food 1150–1950 ms, final food hold until 2200 ms. The result briefly
flashes yellow. Retry counts 3–5 occasionally show a teasing line, 8 shows a fixed
line and 10 adds two 500 ms captions before the ordinary draw. Confirmation or a
new home draw resets the retry streak. Empty pools offer explicit recovery.

## Ownership, persistence and memory

Application state, food data and candidate storage are fixed-size. The screen and
its controls are created once; page changes update/hide them. One persistent LVGL
timer handles animation; no per-draw tasks or timer allocation occurs. LVGL has a
24 KB pool and the BSP retains one 9,600-byte DMA buffer. Fonts are 2-bpp constants
in Flash. LVGL label string allocations remain bounded by fixed copy lengths.

BSP button callbacks enqueue into a static eight-event queue. The app task takes
the LVGL lock briefly for navigation/rendering, copies dirty settings, and writes
NVS outside the lock. A separate optional 3,072-byte-stack battery worker starts
after the first screen; slow gauge initialization cannot block navigation.
All LVGL access from workers is locked. The screen persists at app entry, so there
are no deleted objects for producers to access. Do not later add teardown without
first stopping the timer and workers.

NVS namespace `dinner`, key `settings` stores a versioned 16-byte little-endian
record. Missing records create defaults, malformed records use defaults, and
initialization/save/commit errors do not erase unrelated NVS. Save failure restores
defaults in RAM with a visible message; persistence requires a successful later
write. Real power-cycle behavior still needs device testing. Every 100 completed
draws serial logging reports heap, minimum heap and largest free block.

## Reproduce validation

Activate ESP-IDF **5.5.3**, then run:

```bash
./tools/validate.sh --static
./tools/validate.sh --firmware
./tools/validate.sh
./tools/test_dinner_ui.sh
```

The official gate includes dinner database/filter/navigation tests, mocked NVS
restart/error tests, font inventory drift checks, and a strict **3,000,000-byte**
limit for both application and merged firmware. The UI test uses the pinned LVGL
9.5 library, the actual application UI/fonts, a 24 KB pool and a 240 × 20 RGB565
partial draw buffer. It writes PPM previews in `build/preview`, verifies actual
font coverage plus a negative case, and exercises 4,500 state renders with fixed
object count and bounded heap. This does not emulate the ESP32 or physical panel.

The official gate retains a hash-addressed bundle in `build/firmware/`, including
matching application/merged BIN, ELF, MAP, partition table and bootloader. Verify
it with `python3 tools/archive_firmware.py verify <bundle-directory>`. The merged
image is `build/FoloToy-AI-Passport-full.bin`, flashed at `0x0`; it can overwrite
NVS padding gaps. For compatible settings-preserving updates, use the bundle's
segmented `flash_args`. Never write the application-only image at `0x0`.
The cartoon build was subsequently flashed with explicit user approval; write/hash and
serial startup passed. No full-chip erase was performed. See the validation report.
See [the validation report](dinner-validation.md) for the exact delivered sizes.

## Changed files

- `CMakeLists.txt`: application version.
- `main/CMakeLists.txt`: only the dinner application and subset fonts are linked.
- `main/main.c`: BSP initialization, bounded button queue, LVGL timer, background
  battery sampling, idle dimming, storage worker loop and memory diagnostics.
- `main/dinner.h`: pure data, state and input contract.
- `main/dinner_food_database.c`: 12 categories, 60 foods and labels.
- `main/dinner_settings.c`: defaults and record validity.
- `main/dinner_random_selector.c`: filters, weights and two-stage random selection.
- `main/dinner_navigation.c`: navigation, draw timing, confirmation and Easter eggs.
- `main/dinner_storage.h`, `main/dinner_storage.c`: versioned NVS persistence.
- `main/dinner_decoration.h`, `main/dinner_decoration.c`: native cartoon drawing and state-aware background decoration.
- `main/dinner_ui.h`, `main/dinner_ui.c`: fixed LVGL widgets, four-page rendering,
  focus, font coverage and battery status.
- `sdkconfig.defaults`: Bluetooth disabled, size optimization, explicit UTF-8.
- `assets/fonts/dinner_font_14.c`, `dinner_font_20.c`, `dinner_font_30.c`,
  `dinner_font_inventory.c`, `dinner_subset.otf`, `inventory.json`, `OFL.txt`:
  generated font subsets, source inventory and license.
- `assets/images/dinner-wireframe.png`: local-only supplied design reference, excluded from the public repository and firmware.
- `assets/README.md`, `assets/README.zh_CN.md`: provenance and reproducibility.
- `tests/test_dinner.c`, `tests/test_dinner_storage.c`,
  `tests/dinner_storage_stubs/nvs.h`, `nvs_flash.h`: pure-logic/NVS tests.
- `tests/dinner_ui/CMakeLists.txt`, `lv_conf.h`, `test_ui.c`: real LVGL host checks.
- `tools/generate_dinner_fonts.py`, `check_dinner_fonts.py`, `check_dinner_size.py`,
  `test_dinner_ui.sh`: repeatable resource and UI checks.
- `tools/validate.sh`: dinner checks added to the official gate.
- `docs/dinner.md`, `docs/dinner.zh_CN.md`, `docs/dinner-validation.md`,
  `docs/dinner-validation.zh_CN.md`: usage, implementation and measured results.

## Device acceptance (flash and serial startup completed; physical checks pending)

1. With explicit flash approval, select the ESP32-C3 serial port and the verified
   exact merged image. Confirm whether replacing NVS is acceptable or use compatible
   segmented flashing. No existing-firmware backup or erase is a prerequisite.
2. Power on; check home appears within 1–2 seconds, text is complete, corners are
   unobstructed, battery is a valid percentage or `--%`, and no radio UI appears.
3. Check UP/DOWN focus, short OK, long OK on every submenu, home exit/re-entry,
   long-press cancellation during each animation phase and rapid repeated presses.
4. Draw and confirm; verify both rolling stages, category/food consistency and
   the 2.2-second timing. Retry at least 100 times, inspect 3–5/8/10 Easter eggs,
   no immediate duplicate with multiple candidates, and serial heap diagnostics.
5. Change every mode, budget and diet option. Exclude a category and a food,
   power-cycle and confirm retention. Exclude all categories; verify empty-pool
   recovery and cancellation of the reset confirmation. Test a one-food pool.
6. Inspect every long food label (especially the hotpots), status/error text and
   all font sizes on the physical panel. Let the screen idle 60 seconds and wake
   with each key. Check free heap/largest block stability and responsiveness while
   saving and polling the battery. Observe startup logs for errors/assertions.

Build success, host screenshots and a successful flash are separate from device
acceptance. Physical display, ADC timing, startup latency, actual NVS power loss,
battery communication and 100-draw real-device stability remain unverified.
