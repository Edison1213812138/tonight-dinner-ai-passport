<p align="right">
  <strong>简体中文</strong> · <a href="README.md">English</a>
</p>

# 资源目录（Assets）

本目录集中存放可复用的资源（字库、图片、音乐等），按资源类型分子目录管理。每个资源放在其类型对应的子目录，并记录放置路径、命名方式、集成方式与来源/许可。二进制资源（字体、图片、音频）不属于纯 markdown 文档，请勿与文档混放。涉及版权/授权的资源需注明来源与许可。

## 字库（fonts）

可复用的字库文件与生成的字库源码放在 `fonts/`。

- 命名要能反映字族、字重、字级与格式。
- 记录来源、许可、字符范围、转换命令与目标放置路径。
- 添加字库前评估 Flash 与内部 RAM 影响；ESP32-C3 无 PSRAM。
- 不提交许可不允许分发的字库。

## 图片（images）

可复用的源图与生成的显示资产放在 `images/`。

- 使用描述性命名，并记录尺寸、像素格式、转换步骤与目标路径。
- 优先采用适合 240 × 320 RGB565 显示的格式，并纳入 Flash 与内部 RAM 考量。
- 许可允许时保留可编辑源文件，并记录来源与许可。
- 图片中不得包含设备二维码秘密、凭证或个人数据。

## 音乐与音效（music）

可复用的音乐与音效源码放在 `music/`。

- 记录来源、许可、采样率、位深、声道、转换命令与目标路径。
- 与当前 BSP 音频路径匹配时优先采用 16 kHz、16 位单声道 PCM。
- 嵌入音频前评估 Flash 与内部 RAM 成本；长录音应流式或分块。
- 无再分发许可不提交媒体文件。

## 今晚吃什么的资源

- `images/dinner-wireframe.png`：用户提供的 1224 × 1285 四页设计参考，仅作设计
  对照，保留在本地并排除出公开仓库，绝不编译到固件；用户未提供独立的再分发许可。
- `fonts/dinner_subset.otf`：Noto Sans SC Regular 2.004 的 376 字符开发子集，
  版权所有 2014–2021 Adobe；SIL OFL 1.1 许可保存在 `fonts/OFL.txt`。
  [字体上游](https://github.com/notofonts/noto-cjk/tree/main/Sans)。完整中文字库
  不保存在项目，也不进入固件。
- `fonts/dinner_font_{14,20,30}.c`：2-bpp、未压缩的 LVGL 9.5 位图字体，
  在 `main/CMakeLists.txt` 编译，由 `dinner_ui.c` 显式指定给所有文字控件。
  `fonts/dinner_font_inventory.c` 提供设备启动时的字符覆盖检查。
- `fonts/inventory.json`：全部码点及字体子集 SHA-256。应用只展示固定文案，
  不支持任意外部文字；未使用图标字体。

使用 Pillow 12.3.0、fonttools 4.60.1 运行
`python3 tools/generate_dinner_fonts.py` 可重建；默认输入是项目内 OTF 子集。
新增字符时用 `--source /path/to/NotoSansSC-Regular.otf` 指定完整开发字体。
随后运行 `python3 tools/check_dinner_fonts.py` 和 `./tools/test_dinner_ui.sh`。
后者检查全部码点在三种字号的真实 LVGL 字形，以及已知缺失字的负例。
OTF、JSON、PNG、预览文件都不进入固件。详见[应用说明](../docs/dinner.zh_CN.md)。

1.1.0 的卡通贴纸由 `main/dinner_decoration.c` 原生绘制，按用户要求加入快餐、
饮料元素，不使用图片或新字体资源；原 PNG 仍只作设计参考。

## 社区封面

`images/tonight-dinner-cover.png` 是内置 image_gen 生成的 1086 × 1448 竖版 3:4
玩法插画，用于社区详情。图片明确标注“AI 生成 · 玩法示意”，不是实机截图。
完整生成提示词保存在 `images/tonight-dinner-cover.prompt.txt`。
封面不编译入固件，不含凭据或设备资料。
