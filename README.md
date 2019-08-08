# WatchIO

[原理图Schematic_esp8285-watch.pdf](Schematic_esp8285-watch.pdf)

不多说，上图
![img](images/1.jpg)
![img](images/2.jpg)
![img](images/3.jpg)
![img](images/intro.jpg)
![img](images/pcb.png)
![img](images/front.png)
![img](images/back.png)


# 介绍


- 超迷你尺寸：宽度3.10cm高度2.26cm
- ESP32主控（双核240 MHz 4MB flash）
- 板载Wi-Fi&蓝牙（Wi-Fi支持Station/SoftAP）
- 0.96寸IPS彩屏（ST7735驱动/160*80像素）
- Micro USB接口（CP2104串口芯片免驱动）
- 六轴运动传感器（MPU6050 I<sup>2</sup>C接口）
- 支持Arduino和MicroPython
- RTC实时时钟芯片（PCF8563 I<sup>2</sup>C接口）
- 气压&温度传感器（BMP280 I<sup>2</sup>C接口）
- 侧边按钮&多功能按钮（上中下多功能按钮）
- 120mAh Li-Po电池（内置电压ADC检测&充放电电路）
 
 # 备注

- 出厂默认设置忽略了MTDI的stapping pin, 强制烧了efuse配置flash SDIO_VCC是3.3V 命令: espefuse.py set_flash_voltage 3.3V
 