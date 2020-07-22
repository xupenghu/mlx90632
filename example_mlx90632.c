/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-22     xph    first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include "sensor.h"
#include "sensor_melexis_mlx90632.h"

#define MLX90632_ADDR (0x3A) // gnd ; 0x3B vcc

static void mlx90632_temp_entry(void *parameter)
{
    rt_device_t dev = RT_NULL;
    struct rt_sensor_data sensor_data;
    rt_size_t res;
    dev = rt_device_find("temp_mlx");
    if (dev == RT_NULL)
    {
        rt_kprintf("can not find device : mlx90632 \n");
        return;
    }

    if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
    {
        rt_kprintf("open device failed!\n");
        return;
    }
    while (1)
    {
        res = rt_device_read(dev, 0, &sensor_data, 1);
        if (res != 1)
        {
            rt_kprintf("read data failed! result is %d\n", res);
            rt_device_close(dev);
            return;
        }
        else
        {
            rt_kprintf("temp:%ld \n", sensor_data.data.temp);
        }
        rt_thread_mdelay(10);
    }
}

static int mlx90632_example(void)
{
    rt_thread_t mlx90632_thread;
    mlx90632_thread = rt_thread_create(
        "mlx_tem",
        mlx90632_temp_entry,
        RT_NULL,
        1024,
        RT_THREAD_PRIORITY_MAX / 2,
        10);
    if (mlx90632_thread != RT_NULL)
    {
        rt_thread_startup(mlx90632_thread);
    }
    else
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}

INIT_APP_EXPORT(mlx90632_example);

static int rt_hw_mlx90632_port(void)
{
    struct rt_sensor_config cfg;
    cfg.intf.user_data = (void *)MLX90632_ADDR;
    cfg.intf.dev_name = "i2c1";
    cfg.mode = RT_SENSOR_MODE_POLLING;
    rt_hw_mlx90632_init("mlx90632", &cfg);
    return RT_EOK;
}

INIT_COMPONENT_EXPORT(rt_hw_mlx90632_port);
