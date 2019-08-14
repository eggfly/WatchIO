# WatchIO

WatchIO 是源自我多年前的一个DIY智能手表的梦想，就是给爱好者们提供一款体积小巧的可联网可编程的彩屏手表。

淘宝链接: https://item.taobao.com/item.htm?id=599937397362

它基于乐鑫ESP32模组，支持Wi-Fi和蓝牙通信，仅有1.5个5角硬币大小，兼容Arduino和MicroPython，资源丰富。USB串口、锂电池和充电电路，可以让很多可穿戴的创意实现。内置的BMP280气压温度传感器、6轴运动传感器MPU6050给手表提供了更多的应用场景。

上图!

<div align="center"><img src="images/intro.jpg" width="75%" height="75%" /></div>
<div align="center"><img src="images/1.jpg" width="75%" height="75%" /></div>

# 介绍

- 超迷你尺寸：宽度3.10cm高度2.26cm
- ESP32主控（双核240 MHz 4MB flash）
- 板载Wi-Fi&蓝牙（Wi-Fi支持Station/SoftAP）
- 0.96寸IPS彩屏&PWM背光控制（ST7735驱动/160*80像素）
- Micro USB接口（CP2104串口芯片免驱动）
- 六轴运动传感器（MPU6050 I<sup>2</sup>C接口）
- 支持Arduino和MicroPython
- RTC实时时钟芯片（PCF8563 I<sup>2</sup>C接口）
- 气压&温度传感器（BMP280 I<sup>2</sup>C接口）
- 侧边按钮&多功能按钮（上中下多功能按钮）
- 120mAh Li-Po电池（内置电压ADC检测&充放电电路）

# 文档

- 文档和Arduino固件编译教程: [点击查看 /docs.md](docs.md)

- Note: [Arduino源码在src目录](src/)
- 固件演示视频: https://www.bilibili.com/video/av63202850/
- ESP32 Arduino编译环境搭建视频教程: https://www.bilibili.com/video/av63204960/

# 注意事项

- 开关机在侧面的拨动开关，**拨到下面是开机，拨到上面是关机**（锂电池的正极和PCB之间成为断开）
- 如上，如果插入USB给锂电池充电，**需要把拨动开关拨到下面**，才能正常充进去电，否则仅为USB给ESP32供电
- 锂电池充电管理IC是TP4057，充电电流配置为400mA，充电时间大概10-20分钟左右，充满后自动停止充电


# 更多图片

<div align="center"><img src="images/2.jpg" width="75%" height="75%" /></div>
<div align="center"><img src="images/3.jpg" width="75%" height="75%" /></div>
<div align="center"><img src="images/pcb.png" width="75%" height="75%" /></div>
<div align="center"><img src="images/front.png" width="75%" height="75%" /></div>
<div align="center"><img src="images/back.png" width="75%" height="75%" /></div>
