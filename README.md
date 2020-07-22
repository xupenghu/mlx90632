# MLX90632

## 简介

MLX90632 是一款微型 SMD 温度计 IC，可实现高精度非接触式温度测量，在热特性动态变化的环境以及空间受限的情况下作用尤为显著。

## 特性和优势

- 高温环境下精准稳定的工作
- 采用 3mm x 3mm x 1mm DFN 封装，无需采用体积庞大的 TO 罐封装
- 消费级：测量物体温度为 -20 °C 至 200 °C，精度±1 ℃；医疗级：测量物体温度为-20 °C 至 100 °C，人体温度测量精度高达±0.2 °C
- 使用 I2C 数字接口进行出厂校准，50° 的视场角
- 工作温度范围： -20 °C 至 85 °C
  
## 使用说明

### 依赖

- RT-Thread 4.0.0+
- Sensor 组件
- IIC 驱动：MLX90632使用 IIC 进行数据通讯，需要系统 IIC 驱动支持；

### 获取软件包

使用 MLX90632 软件包需要在 RT-Thread 的包管理中选中它，具体路径如下：

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        sensors drivers --->
            [*] mlx90632: a mlx90632 package for rt-thread.package
                   [*] Enable mlx90632 example 
                    Version (latest)  --->
```

**Version**：软件包版本选择

### 使用软件包

刚才的menuconfig默认创建了一个使用例子，编译下载后，使用msh命令行可以打印出当前的被测温度。具体使用方式清参考 **example_mlx90632.c**

```
[I/I2C] I2C bus [i2c1] registered

 \ | /
- RT -     Thread Operating System
 / | \     4.0.3 build Jul 22 2020
 2006 - 2020 Copyright by rt-thread team
[I/sensor] rt_sensor init success
temp:297 
msh >temp:297 
temp:297 
temp:297 
temp:321
temp:323 

```
## 注意事项
- 如果读取的温度值是0，则需要调低IIC的通讯速率


## 联系人信息
- [xph](https://github.com/xupenghu)
- [邮箱](xupenghu@outlook.com)
- [项目地址](https://github.com/xupenghu/mlx90632)




