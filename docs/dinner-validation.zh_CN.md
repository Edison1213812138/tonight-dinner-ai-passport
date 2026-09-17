<p align="right"><a href="dinner-validation.md">English</a> · <strong>简体中文</strong></p>

# 今晚吃什么验证报告

验证日期：2026-09-16。ESP-IDF **5.5.3**、LVGL **9.5.0**、应用版本
`1.1.0-dinner`。分支 `feature/tonight-dinner`，官方 main 基线
`25add0044cb3e8ce2319c258ae96551f0ffcc2df`。源码修改尚未提交 Git。

| 验证项 | 结果 |
| --- | --- |
| Build | **PASS / BUILD SUCCESS**：实际执行官方 `./tools/validate.sh` 完整门禁 |
| Host tests | **PASS**：上游测试、12×5 类别、60 个唯一食物、640 组筛选、1,000 次抽取、不立即重复、导航、防抖、时钟回绕、空池及单候选 |
| 持久化测试 | **PASS**：模拟 NVS 的首次启动、重启读取、版本/格式/范围/长度损坏，以及初始化、写入、提交失败 |
| 字体 | **PASS**：376 码点 × 3 字号，真实 LVGL 字形及已知缺字负例 |
| UI 压力测试 | **PASS**：4,500 次状态渲染，23 个固定对象，无持续内存增长，覆盖所有食物名和字号 |
| 镜像与调试包 | **PASS**：分区、偏移、哈希和匹配 ELF 均验证 |
| Device tests | **部分完成**：刷写、哈希、串口启动及用户确认的首页/抽取通过；其他实机检查待测 |

## 实际体积

| 内容 | 字节 |
| --- | ---: |
| 完整可刷入合并 BIN | 752,160 |
| 仅应用 BIN | 686,624 |
| 三档应用字体及码点清单 | 129,154 |
| 全部链接字体，含默认 Montserrat 14 | 142,803 |
| 固件内图片 | 0 |
| 新应用 C/头文件源码，含替换主入口，不含生成字体 | 39,135 |
| MAP 归属的业务代码/数据，不含字体 | 11,477 |
| MAP 归属的应用静态 RAM | 353 |

两个可安装 BIN 都**严格小于 3,000,000 字节**。源码参考 PNG、开发 OTF 子集、
截图、ELF、MAP 都不是安装资源。ELF/MAP 是调试文件，其磁盘体积不计入固件。

链接器统计内部统一内存占用 94,946 字节：数据 6,940、BSS 29,984、IRAM 指令
58,022；该区在运行时分配前剩余 226,350 字节，**不能视为实测运行堆**。
保持 24 KB LVGL 池、9,600 字节官方 DMA，电池任务申请 3,072 字节栈。
电脑端 LVGL 压测结束后空闲 3,440 字节、最大连续空闲 2,816 字节。
电脑和 ESP32-C3 的指针大小、环境不同，真机堆及任务栈余量尚未测量。
LVGL 余量不宽裕，新增大文字、阴影或图片缓冲前必须重新测量。
字体常量占 Flash，绘制缓冲和有界的文字复制才消耗运行内存。

## 精确交付身份

- 合并 BIN SHA-256：`61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb`
- 匹配 ELF SHA-256：`cb86d09e0f1b5d1fe0dcd3cac50d82d260aa65d926c560e3881d6089ba6f62ae`
- 已验证调试包：`build/firmware/61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb/`
- 可刷文件：`build/FoloToy-AI-Passport-full.bin`，地址 **0x0**。
- 完整日志：`build/validation.log`、`build/ui-validation.log`。
- 体积数据：`build/dinner-size-report.json`、`build/size-files.json`、`build/memory.json`。
- 实际 LVGL 预览：`build/preview/four-pages.png` 及各状态 PNG/PPM。

合并刷写会替换固件，且可能重置现有 NVS 设置。兼容分区下需要保留设置时，
使用同一调试包的镜像及 `flash_args` 分段刷写。无需整片擦除，也未授权整片擦除。
已用 `python3 tools/archive_firmware.py verify` 验证此精确固件与调试包的一致性。

## 已授权刷写与启动观察

用户明确同意刷入卡通版后，已向 `/dev/cu.usbmodem1101` 的 **0x0** 地址写入
本报告精确匹配的 752,160 字节合并镜像。Esptool 确认芯片为 ESP32-C3 revision 1.1、
内置 8 MB Flash；只擦除了写入涉及的 `0x00000000..0x000b7fff` 扇区，未整片擦除。
写入后的数据哈希校验通过，Esptool 已自动重启设备。

随后读取 18 秒串口日志，确认版本 `1.1.0-dinner` 与 ELF 前缀 `cb86d09e0`。
日志记录显示初始化 293 ms、LVGL 348 ms、按键 530 ms，CW2017 识别及电池配置
匹配正常。观察窗口内没有 panic、断言、看门狗或错误日志。
这些时间仅证明初始化完成，不等同于实屏首帧时间。
监视已停止、串口已释放。本地日志：`build/flash-1.1.0.log`、
`build/device-startup-1.1.0.log`；结构化记录：`build/device-test-1.1.0.json`。

## 未验证事项

用户已明确反馈“首页和抽取都正常”，卡通首页显示、OK 启动、滚动与结果作为
用户观察验收通过。全部中文/边角、其他按键和长按、首屏计时、至少 100 次抽取、
持续内存稳定性、断电 NVS、电量准确性和背光调暗仍待验证。
其余项目按[实机验收步骤](dinner.zh_CN.md)继续。
