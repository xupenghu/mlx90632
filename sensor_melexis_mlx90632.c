/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-22     xph    first version
 */
#include "sensor_melexis_mlx90632.h"

#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG
#define DBG_SECTION_NAME "sensor.mlx90632"
#define DBG_COLOR
#include <rtdbg.h>

#define MLX90614_DEFALUT_ADDR (0x3A)
#define SENSOR_TEMP_RANGE_MAX (100)
#define SENSOR_TEMP_RANGE_MIN (0)

static struct mlx90632_device mlx90632_dev;

static int8_t rt_i2c_read_reg(mlx90632_device_t dev, int16_t reg, uint8_t *data, uint16_t len)
{
    rt_uint8_t tmp[2];
    struct rt_i2c_msg msgs[2];
    tmp[0] = (reg >> 8) & 0xff;
    tmp[1] = reg & 0xff;
    msgs[0].addr = dev->addr;  /* Slave address */
    msgs[0].flags = RT_I2C_WR; /* Write flag */
    msgs[0].buf = tmp;         /* Slave register address */
    msgs[0].len = 2;           /* Number of bytes sent */

    msgs[1].addr = dev->addr;  /* Slave address */
    msgs[1].flags = RT_I2C_RD; /* Read flag */
    msgs[1].buf = data;        /* Read data pointer */
    msgs[1].len = len;         /* Number of bytes read */

    if (rt_i2c_transfer(dev->bus, msgs, 2) != 2)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
static int8_t rt_i2c_write_reg(mlx90632_device_t dev, int16_t reg, uint8_t *reg_data, uint16_t length)
{

    /* Implement the I2C write routine according to the target machine. */
    rt_uint8_t tmp[2] = {0};
    struct rt_i2c_msg msgs[2];
    tmp[0] = (reg >> 8) & 0xff;
    tmp[1] = reg & 0xff;

    msgs[0].addr = dev->addr;  /* Slave address */
    msgs[0].flags = RT_I2C_WR; /* Write flag */
    msgs[0].buf = tmp;         /* Slave register address */
    msgs[0].len = 2;           /* Number of bytes sent */

    msgs[1].addr = dev->addr;                    /* Slave address */
    msgs[1].flags = RT_I2C_WR | RT_I2C_NO_START; /* Read flag */
    msgs[1].buf = reg_data;                      /* Read data pointer */
    msgs[1].len = length;                        /* Number of bytes read */

    if (rt_i2c_transfer(dev->bus, msgs, 2) != 2)
    {
        return -RT_ERROR;
    }

    return RT_EOK;
}
/* Implementation of I2C read for 16-bit values */
int8_t mlx90632_i2c_read(int16_t register_address, uint16_t *value)
{
    uint8_t data[2];
    int8_t ret = RT_EOK;
    ret = rt_i2c_read_reg(&mlx90632_dev, register_address, data, 2);
    //Endianness
    *value = data[1] | (data[0] << 8);
    return ret;
}

/* Implementation of I2C read for 32-bit values */
int8_t mlx90632_i2c_read32(int16_t register_address, uint32_t *value)
{
    uint8_t data[4];
    int8_t ret = RT_EOK;
    ret = rt_i2c_read_reg(&mlx90632_dev, register_address, data, 4);
    //Endianness
    *value = data[2] << 24 | data[3] << 16 | data[0] << 8 | data[1];
    return ret;
}

/* Implementation of I2C write for 16-bit values */
int32_t mlx90632_i2c_write(int16_t register_address, uint16_t value)
{
    uint8_t data[2];
    data[0] = value >> 8;
    data[1] = value;
    return rt_i2c_write_reg(&mlx90632_dev, register_address, data, 2);
}

/* Implementation of reading all calibration parameters for calucation of Ta and To */
static int mlx90632_read_eeprom(mlx90632_device_t dev)
{
    int32_t ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_P_R, (uint32_t *)&(dev->PR));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_P_G, (uint32_t *)&(dev->PG));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_P_O, (uint32_t *)&(dev->PO));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_P_T, (uint32_t *)&(dev->PT));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_Ea, (uint32_t *)&(dev->Ea));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_Eb, (uint32_t *)&(dev->Eb));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_Fa, (uint32_t *)&(dev->Fa));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_Fb, (uint32_t *)&(dev->Fb));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read32(MLX90632_EE_Ga, (uint32_t *)&(dev->Ga));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read(MLX90632_EE_Gb, (uint16_t *)&(dev->Gb));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read(MLX90632_EE_Ha, (uint16_t *)&(dev->Ha));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read(MLX90632_EE_Hb, (uint16_t *)&(dev->Hb));
    if (ret < 0)
        return ret;
    ret = mlx90632_i2c_read(MLX90632_EE_Ka, (uint16_t *)&(dev->Ka));
    if (ret < 0)
        return ret;
    return 0;
}

void usleep(int min_range, int max_range)
{
    while (--min_range)
        ;
}

static rt_size_t mlx90632_polling_get_data(rt_sensor_t sensor, struct rt_sensor_data *data)
{

    rt_int16_t ambient_new_raw;
    rt_int16_t ambient_old_raw;
    rt_int16_t object_new_raw;
    rt_int16_t object_old_raw;
    /* Get raw data from MLX90632 */
    mlx90632_read_temp_raw(&ambient_new_raw, &ambient_old_raw, &object_new_raw, &object_old_raw);
    /* Pre-calculations for ambient and object temperature calculation */
    mlx90632_dev.pre_ambient = mlx90632_preprocess_temp_ambient(ambient_new_raw, ambient_old_raw, mlx90632_dev.Gb);
    mlx90632_dev.pre_object = mlx90632_preprocess_temp_object(object_new_raw, object_old_raw, ambient_new_raw, ambient_old_raw, mlx90632_dev.Ka);
    /* Set emissivity = 1 */
    mlx90632_set_emissivity(1.0);
    /* Calculate ambient and object temperature */
    mlx90632_dev.ambient = mlx90632_calc_temp_ambient(ambient_new_raw, ambient_old_raw, mlx90632_dev.PT, mlx90632_dev.PR, mlx90632_dev.PG, mlx90632_dev.PO, mlx90632_dev.Gb);
    mlx90632_dev.object = mlx90632_calc_temp_object(mlx90632_dev.pre_object, mlx90632_dev.pre_ambient, mlx90632_dev.Ea, mlx90632_dev.Eb, mlx90632_dev.Ga, mlx90632_dev.Fa, mlx90632_dev.Fb, mlx90632_dev.Ha, mlx90632_dev.Hb);
    data->data.temp = (rt_int32_t)(mlx90632_dev.object * 10);
    data->timestamp = rt_sensor_get_ts();
    data->type = RT_SENSOR_CLASS_TEMP;
    return 1;
}

static rt_size_t mlx90632_fetch_data(struct rt_sensor_device *sensor, void *buf, rt_size_t len)
{
    RT_ASSERT(buf);

    if (sensor->config.mode == RT_SENSOR_MODE_POLLING)
    {
        return mlx90632_polling_get_data(sensor, buf);
    }

    return 0;
}

static rt_err_t mlx90632_control(struct rt_sensor_device *sensor, int cmd, void *args)
{
    return RT_EOK;
}

static struct rt_sensor_ops sensor_ops =
    {
        mlx90632_fetch_data,
        mlx90632_control};

static int _mlx90632_init(const char *bus_name, mlx90632_device_t dev)
{
    rt_err_t ret = RT_EOK;
    dev->bus = (struct rt_i2c_bus_device *)rt_device_find(bus_name);
    if (dev->bus == RT_NULL)
    {
        LOG_E("can not find %s bus.", bus_name);
        return -RT_ERROR;
    }
    ret = mlx90632_init();
    if (ret != RT_EOK)
    {
        LOG_E("mlx90632 init error. code : %d", ret);
        return -RT_ERROR;
    }
    dev->PR = 0x00587f5b;
    dev->PG = 0x04a10289;
    dev->PT = 0xfff966f8;
    dev->PO = 0x00001e0f;
    dev->Ea = 4859535;
    dev->Eb = 5686508;
    dev->Fa = 53855361;
    dev->Fb = 42874149;
    dev->Ga = -14556410;
    dev->Ha = 16384;
    dev->Hb = 0;
    dev->Gb = 9728;
    dev->Ka = 10752;
    /* Read EEPROM calibration parameters */
    ret = mlx90632_read_eeprom(dev);
    if (ret != RT_EOK)
    {
        LOG_W("Read EEPROM calibration parameters. Use default parameters.");
    }
    return RT_EOK;
}

int rt_hw_mlx90632_init(const char *name, struct rt_sensor_config *cfg)
{
    rt_err_t ret = RT_EOK;
    rt_memset(&mlx90632_dev, 0, sizeof(struct mlx90632_device));
    if (cfg->intf.user_data)
    {
        mlx90632_dev.addr = (rt_uint8_t)(cfg->intf.user_data);
    }
    else
    {
        mlx90632_dev.addr = MLX90614_DEFALUT_ADDR;
    }

    if (_mlx90632_init(cfg->intf.dev_name, &mlx90632_dev) != RT_EOK)
    {
        return -RT_ERROR;
    }

    rt_sensor_t sensor = RT_NULL;
    sensor = rt_calloc(1, sizeof(struct rt_sensor_device));
    if (RT_NULL == sensor)
    {
        LOG_E("calloc failed");
        return -RT_ERROR;
    }
    sensor->info.type = RT_SENSOR_CLASS_TEMP;
    sensor->info.vendor = RT_SENSOR_VENDOR_MELEXIS;
    sensor->info.model = "mlx90632";
    sensor->info.unit = RT_SENSOR_UNIT_DCELSIUS;
    sensor->info.intf_type = RT_SENSOR_INTF_I2C;
    sensor->info.range_max = SENSOR_TEMP_RANGE_MAX;
    sensor->info.range_min = SENSOR_TEMP_RANGE_MIN;
    sensor->info.period_min = 100;

    sensor->config = *cfg;
    sensor->ops = &sensor_ops;

    ret = rt_hw_sensor_register(sensor, name, RT_DEVICE_FLAG_RDONLY, &mlx90632_dev);
    if (ret != RT_EOK)
    {
        LOG_E("device register err. code: %d", ret);
        rt_free(sensor);
    }

    return RT_EOK;
}
