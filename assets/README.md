<p align="right">
  <a href="README.zh_CN.md">简体中文</a> · <strong>English</strong>
</p>

# Assets

This directory stores reusable fonts, images, music, and sound effects, organized by asset type.

Keep each asset in the matching subdirectory and document its destination, naming, integration method, and source/license. Do not mix binary assets with Markdown documentation.

## Fonts

Store reusable font files and generated font sources in `fonts/`.

- Use descriptive names that include the family, weight, size, and format when relevant.
- Document the source, license, character range, conversion command, and expected destination.
- Check Flash and internal-RAM impact before adding a font; the ESP32-C3 has no PSRAM.
- Do not commit fonts whose license does not permit redistribution.

## Images

Store reusable source images and generated display assets in `images/`.

- Use descriptive names and document dimensions, pixel format, conversion steps, and destination.
- Prefer formats suitable for the 240 × 320 RGB565 display and account for Flash and internal RAM.
- Preserve editable sources where licensing permits, and record the source and license.
- Never commit device QR secrets, credentials, or personal data in images.

## Music and sound effects

Store reusable music and sound-effect sources in `music/`.

- Document the source, license, sample rate, bit depth, channels, conversion command, and destination.
- Prefer 16 kHz, 16-bit mono PCM when it matches the current BSP audio path.
- Check Flash and internal-RAM cost before embedding audio; stream or chunk long recordings.
- Do not commit media without redistribution permission.

## Tonight's Dinner assets

- `images/dinner-wireframe.png`: user-supplied four-screen reference, 1224 × 1285;
  retained locally for design review, excluded from the public repository and
  never linked into firmware. No independent redistribution license was supplied.
- `fonts/dinner_subset.otf`: 376-codepoint development subset of Noto Sans SC
  Regular 2.004, copyright 2014–2021 Adobe; SIL OFL 1.1 in `fonts/OFL.txt`.
  [Upstream font family](https://github.com/notofonts/noto-cjk/tree/main/Sans).
  The complete CJK font is neither stored here nor included in the firmware.
- `fonts/dinner_font_{14,20,30}.c`: generated 2-bpp uncompressed LVGL 9.5 bitmap
  fonts; `main/CMakeLists.txt` compiles them, and `dinner_ui.c` assigns every label
  explicitly. `fonts/dinner_font_inventory.c` enables on-device coverage checking.
- `fonts/inventory.json`: exact codepoints and subset SHA-256. All application
  strings are fixed; arbitrary external text is unsupported. No icon font is used.

Regenerate with `python3 tools/generate_dinner_fonts.py` using Pillow 12.3.0 and
fonttools 4.60.1. The tracked OTF subset is the default input; when adding new
characters, pass `--source /path/to/NotoSansSC-Regular.otf`. Run
`python3 tools/check_dinner_fonts.py` and `./tools/test_dinner_ui.sh` afterward.
The latter checks actual LVGL descriptors for every codepoint, all three sizes,
and a negative missing-codepoint case. Firmware generation does not compile OTF,
JSON, PNG or preview files. See the [application guide](../docs/dinner.md).

Cartoon stickers introduced in 1.1.0 are native drawing code in
`main/dinner_decoration.c`, inspired by the user's fast-food/drink request.
They use no image/font assets; the original PNG remains reference material only.

## Community cover

`images/tonight-dinner-cover.png` is a 1086 × 1448 portrait 3:4 illustration,
generated with the built-in image_gen tool for the community listing. The image
explicitly says it is AI-generated gameplay illustration, not a real screenshot.
The full generation prompt is saved in `images/tonight-dinner-cover.prompt.txt`.
The cover is not linked into firmware and contains no credentials or device data.
