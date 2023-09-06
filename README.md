## 硬件部分

**核心板**

使用STM32F103RCT6最小系统板。实际上使用C8T6完全可以，这里只是手里这块板闲置，而且板载了SPI的屏幕接口。

<img src="https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659001.webp" style="zoom:67%;" />

LED使用板载

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061724841.webp)

**屏幕**

经典0.96OLED，使用的是7针SPI驱动

<img src="https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659305.webp" style="zoom:50%;" />

这里直接使用板载的SPI7针屏幕接口

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061712887.webp)

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061725030.webp)

**WIFI模块**

ESP8266-01S

<img src="https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659527.webp" style="zoom: 50%;" />

使用STM32的串口1与8266通信，TX，RX交叉连接。

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061714773.webp)

**温湿度传感器**

DHT11

<img src="https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659666.webp" style="zoom: 33%;" />

任意GPIO均可，这里使用PA8

**光照度传感器**

BH1750

<img src="https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659784.webp" style="zoom:50%;" />

使用I2C通信，SDA：PB10，SCL：PB11

**下载**

板载了ISP一键下载，但是有STLINK，下载更方便。

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659058.webp)

**串口**

向电脑输出调试信息，这里使用CH340

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061659277.webp)

与STM32的串口2相连

![](https://picbed-1312285733.cos.ap-beijing.myqcloud.com/img/202309061718081.webp)

## 软件部分

### EMQX服务器

在云服务器上安装Docker，然后部署EMQX。详细可以参考EMQX官方文档。

如果不修改默认配置，只需两条命令。

```shell
docker pull emqx/emqx:5.0.26
docker run -d --name emqx -p 1883:1883 -p 8083:8083 -p 8084:8084 -p 8883:8883 -p 18083:18083 emqx/emqx:5.0.26
```

当然也可以使用公用服务器，或者各家的物联网平台。



### 驱动

LED，蜂鸣器的部分比较简单，初始化IO口，然后按照要求输出电平即可。

#### DHT11

使用单数据线传输数据。

#### BH1750

使用I2C传输数据。为灵活修改使用软件模拟方式，I2C驱动代码集成在BH1750模块的代码中。

关于I2C的详细内容看[这里](https://www.eisbre.xyz/archives/iic-spi)

#### esp8266

使用串口1发送AT指令，控制8266。

#### MQTT

这一部分还在探索中。

### 微信小程序

使用JS编写界面，微信小程序开发工具提供了多种方式，选择合适的即可。