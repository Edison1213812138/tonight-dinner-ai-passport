<p align="right"><a href="dinner-release.zh_CN.md">简体中文</a> · <strong>English</strong></p>

# What's for Dinner? 1.1.0-dinner

## What's new

The first dinner lottery release offers 60 foods in 12 categories, five selection
modes, saved preferences and playful reroll messages. Fries, a smiling burger,
a drink and a crowned snowman decorate the four original screens. All play is
offline. The cover is an AI-generated illustration, not a device screenshot.

## Install and play

Use the release asset `FoloToy-AI-Passport-full.bin`, a complete merged image,
with the official [browser flasher](https://ai-passport.folotoy.cn/tools/web-flasher/).
Connect the AI Passport by USB, select the file and flash it at offset `0x0`.
The image is for the ESP32-C3 AI Passport with 8 MB Flash.

Use Up/Down to select, press OK to confirm, and hold OK to return. Start a draw,
accept the result or roll again. Settings let you select modes, budget and foods
to avoid. See the [play guide](dinner.md) for the full controls.

## Verify the tested firmware

The local tested image has size **752,160 bytes** and SHA-256:

```text
61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb
```

Build: PASS. Host tests: PASS. Device tests: homepage and food drawing confirmed
by the user. Long-press behavior, settings after reboot and 100 consecutive draws
remain unverified on the physical device. See the [validation report](dinner-validation.md).

## Build from source

Activate ESP-IDF 5.5.3, then run:

```sh
./tools/validate.sh
```

The complete gate runs repository checks and host tests, builds the firmware,
verifies the merged image and writes `build/FoloToy-AI-Passport-full.bin`.
Rebuilds can have different hashes because build metadata changes. Use the
checksum above only for the tested binary supplied with this release.

The separate FoloToy community submission was pending review when last checked;
publishing to GitHub does not change that review status.
