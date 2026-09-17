<p align="right"><a href="dinner-release.md">English</a> · <strong>简体中文</strong></p>

# 今晚吃什么？1.1.0-dinner

## 新增玩法

首个晚饭抽签版本提供 12 类、60 种美食，支持五种选择模式、偏好保存和有趣的重抽
彩蛋。薯条、笑脸汉堡、饮料杯与皇冠雪人点缀四个独立设计的页面。全部玩法离线
运行。封面是 AI 生成的示意插画，并非实机截图。

## 刷入与游玩

下载发布附件 `FoloToy-AI-Passport-full.bin`，这是完整合并固件。打开官方
[浏览器刷机工具](https://ai-passport.folotoy.cn/tools/web-flasher/)，通过 USB
连接 AI Passport，选择文件，从 `0x0` 地址刷入。固件适用于配备 8 MB Flash 的
ESP32-C3 AI Passport。

上/下键选择，OK 短按确认、长按返回。开始抽取后，可以接受结果或再抽一次。
设置支持选择模式、预算与忌口。完整操作见[玩法说明](dinner.zh_CN.md)。

## 校验已测试固件

本地已测试镜像大小为 **752,160 字节**，SHA-256 为：

```text
61c4f7e678d7357f624369a0e9d227168b114a8a6d308b6a2c81ac037dfd88bb
```

Build：PASS。Host tests：PASS。Device tests：用户已确认首页与食物抽取正常。
实机长按、重启后设置恢复及连续 100 次抽取仍未验证。详见[验收报告](dinner-validation.zh_CN.md)。

## 从源码构建

启用 ESP-IDF 5.5.3 环境，然后运行：

```sh
./tools/validate.sh
```

完整检查会运行仓库检查与主机测试，构建并校验合并固件，输出
`build/FoloToy-AI-Passport-full.bin`。构建元数据变化可能导致重新构建后的哈希不同；
上面的校验值仅对应随本次发布提供的已测试固件。

独立的 FoloToy 社区投稿在最近查询时为待审核；发布到 GitHub 不会改变社区审核状态。
