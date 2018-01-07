/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/*
    2014/09/22  created by zhoujie (00174260)
*/
#include "hw_flash.h"

/* LM3646 Registers define */
#define REG_CHIPID                 0x00
#define REG_ENABLE			0x01
#define REG_FLAGS1			0x08
#define REG_FLAGS2			0x09
#define REG_FLASH_FEATURES		0x08
#define REG_LED1_FLASH_CURRENT_CONTROL		0x06
#define REG_LED1_TORCH_CURRENT_CONTROL		0x07
#define REG_IVFM			0x01
#define REG_TORCH_RAMP_TIME		0x06
#define REG_MAX_CURRENT         0x05

#define CHIP_ID                              0x10
#define MODE_STANDBY			0x00
#define MODE_INDICATOR			0x01
#define MODE_TORCH			0x02
#define MODE_FLASH			0x03
#define STROBE_PIN			0x20
#define TORCH_PIN			0x10
#define TX_PIN				0x40

#define FLASH_LED_MAX			0x7f
#define TORCH_LED_MAX			0x7f
#define MAX_BRIGHTNESS_FORMMI   0x09

/* Internal data struct define */
typedef enum {
	RESET=0,
	STROBE,
	TORCH,
	MAX_PIN,
}lm3646_pin_type;

/* Internal data struct define */
struct hw_lm3646_private_data_t {
	unsigned char flash_led[FLASH_LED_MAX];
	unsigned char torch_led[TORCH_LED_MAX];
	unsigned int flash_led_num;
	unsigned int torch_led_num;

	/* flash control pin */
	//unsigned int strobe;
        /* flash control pin */
        unsigned int pin[MAX_PIN];
        
	unsigned int chipid;
};

/* Internal varible define */
static struct hw_lm3646_private_data_t hw_lm3646_pdata;
static struct hw_flash_ctrl_t hw_lm3646_ctrl;
static struct i2c_driver hw_lm3646_i2c_driver;

DEFINE_HISI_FLASH_MUTEX(lm3646);

/* Function define */
#if 0
static int lm3646_set_strobe(struct hw_flash_ctrl_t *flash_ctrl,
	unsigned int state)
{
	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	cam_debug("%s strobe0=%d, state=%d.", __func__,
		flash_ctrl->flash_info.strobe0, state);
	gpio_direction_output(flash_ctrl->flash_info.strobe0, state);
	return 0;
}
#endif

static int hw_lm3646_init(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_lm3646_private_data_t *pdata;
	int rc = 0;

	cam_debug("%s ernter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;

      flash_ctrl->pctrl = devm_pinctrl_get_select(flash_ctrl->dev,
						PINCTRL_STATE_DEFAULT);
	if (NULL == flash_ctrl->pctrl) {
		cam_err("%s failed to set pin.", __func__);
		return -EIO;
	}

	rc = gpio_request(pdata->pin[RESET], "flash-reset");
	if (rc < 0) {
		cam_err("%s failed to request reset pin.", __func__);
		return -EIO;
	}

	rc = gpio_request(pdata->pin[STROBE], "flash-strobe");
	if (rc < 0) {
		cam_err("%s failed to request strobe pin.", __func__);
		goto err1;
	}

	rc = gpio_request(pdata->pin[TORCH], "flash-torch");
	if (rc < 0) {
		cam_err("%s failed to request torch pin.", __func__);
		goto err2;
	}
        gpio_direction_output(pdata->pin[RESET],1);
	return rc;
err2:
	gpio_free(pdata->pin[STROBE]);
err1:
	gpio_free(pdata->pin[RESET]);
       return rc;
}

static int hw_lm3646_exit(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_lm3646_private_data_t *pdata;

	cam_debug("%s ernter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	flash_ctrl->func_tbl->flash_off(flash_ctrl);

	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;

	gpio_free(pdata->pin[TORCH]);
	gpio_free(pdata->pin[STROBE]);
	gpio_free(pdata->pin[RESET]);
	flash_ctrl->pctrl = devm_pinctrl_get_select(flash_ctrl->dev,
						PINCTRL_STATE_IDLE);

	return 0;
}

static int hw_lm3646_flash_mode(struct hw_flash_ctrl_t *flash_ctrl,
	int data)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	struct hw_lm3646_private_data_t *pdata;
	unsigned char val;

	cam_debug("%s data=%d.\n", __func__, data);
	cam_info("%s 220250 data=%d.\n", __func__, data);
	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
	pdata = flash_ctrl->pdata;

	/* clear error flag,resume chip */
	i2c_func->i2c_read(i2c_client, REG_FLAGS1, &val);
       i2c_func->i2c_read(i2c_client, REG_FLAGS2, &val);
	i2c_func->i2c_read(i2c_client, REG_LED1_FLASH_CURRENT_CONTROL, &val);

	/* set LED Flash current value */
	if (data < FLASH_LED_MAX) {
		cam_info("%s flash_led=0x%x.", __func__,
			pdata->flash_led[data]);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (data & 0x7f));

	} else {
		cam_warn("%s data(%d) > flash_led_num(%d).", __func__,
			data, pdata->flash_led_num);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (0x7f));
	}
       i2c_func->i2c_write(i2c_client, REG_MAX_CURRENT, 0x7c);
	i2c_func->i2c_write(i2c_client, REG_LED1_FLASH_CURRENT_CONTROL, 0x22);
	if (flash_ctrl->flash_mask_enable) {              
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd3);
	} else {
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd3);
	}

	return 0;
}

static int hw_lm3646_torch_mode(struct hw_flash_ctrl_t *flash_ctrl,
	int data)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	struct hw_lm3646_private_data_t *pdata;
	unsigned char val;

	cam_debug("%s data=%d.\n", __func__, data);
	cam_info("%s 220250 data=%d.\n", __func__, data);
	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;

	/* clear error flag,resume chip */
	i2c_func->i2c_read(i2c_client, REG_FLAGS1, &val);
       i2c_func->i2c_read(i2c_client, REG_FLAGS2, &val);
	i2c_func->i2c_read(i2c_client, REG_LED1_TORCH_CURRENT_CONTROL, &val);

	/* set LED Flash current value */
	if (data < FLASH_LED_MAX) {
		cam_info("%s flash_led=0x%x.", __func__,
			pdata->flash_led[data]);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (data & 0x7f));

	} else {
		cam_warn("%s data(%d) > flash_led_num(%d).", __func__,
			data, pdata->flash_led_num);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (0x7f));
	}
       i2c_func->i2c_write(i2c_client, REG_MAX_CURRENT, 0x7c);
	i2c_func->i2c_write(i2c_client, REG_LED1_TORCH_CURRENT_CONTROL, 0x22);
	if (flash_ctrl->flash_mask_enable) {              
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd2);
	} else {
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd2);
	};

	return 0;
}


static int hw_lm3646_torch_mode_1(struct hw_flash_ctrl_t *flash_ctrl,
	int data)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	struct hw_lm3646_private_data_t *pdata;
	unsigned char val;

	cam_debug("%s data=%d.\n", __func__, data);
	cam_info("%s 220250 data=%d.\n", __func__, data);
	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;

	/* clear error flag,resume chip */
	i2c_func->i2c_read(i2c_client, REG_FLAGS1, &val);
       i2c_func->i2c_read(i2c_client, REG_FLAGS2, &val);
	i2c_func->i2c_read(i2c_client, REG_LED1_TORCH_CURRENT_CONTROL, &val);
       i2c_func->i2c_write(i2c_client, REG_MAX_CURRENT, 0x7c);
	/* set LED Flash current value */
	if (data == TORCH_LEFT_MODE) {
		cam_info("%s flash_led=TORCH_LEFT_MODE", __func__);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (0x7f));

	} else if (data == TORCH_RIGHT_MODE){
		cam_info("%s flash_led=TORCH_RIGHT_MODE", __func__);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (0x0));
	}else {
            cam_info("%s default flash_led=TORCH_LEFT_MODE  mode = %d", __func__,data);
		/* REG_CURRENT_CONTROL[3:0] control flash current */
		val = ((val & 0x80) | (0x7f));
        }
     
	i2c_func->i2c_write(i2c_client, REG_LED1_TORCH_CURRENT_CONTROL, val);
	if (flash_ctrl->flash_mask_enable) {              
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd2);
	} else {
		i2c_func->i2c_write(i2c_client, REG_ENABLE, 0xd2);
	};

	return 0;
}



static int hw_lm3646_on(struct hw_flash_ctrl_t *flash_ctrl, void *data)
{
	struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;
	int rc=-1;

	if ((NULL == flash_ctrl) || (NULL == cdata)) {
		cam_err("%s flash_ctrl or cdata is NULL.", __func__);
		return -1;
	}

	cam_debug("%s mode=%d, level=%d.\n", __func__, cdata->mode, cdata->data);
	cam_info("%s 220250 ernter.\n", __func__);
	mutex_lock(flash_ctrl->hw_flash_mutex);
	if (FLASH_MODE == cdata->mode) {
		rc = hw_lm3646_flash_mode(flash_ctrl, cdata->data);
	} else {
		rc = hw_lm3646_torch_mode(flash_ctrl, cdata->data);
	}
	flash_ctrl->state.mode = cdata->mode;
	flash_ctrl->state.data = cdata->data;
	mutex_unlock(flash_ctrl->hw_flash_mutex);

	return rc;
}


static int hw_lm3646_on_1(struct hw_flash_ctrl_t *flash_ctrl, void *data)
{
	struct hw_flash_cfg_data *cdata = (struct hw_flash_cfg_data *)data;
	int rc=-1;

	if ((NULL == flash_ctrl) || (NULL == cdata)) {
		cam_err("%s flash_ctrl or cdata is NULL.", __func__);
		return -1;
	}

	cam_debug("%s mode=%d, level=%d.\n", __func__, cdata->mode, cdata->data);
	cam_info("%s 220250 ernter.\n", __func__);
	mutex_lock(flash_ctrl->hw_flash_mutex);
	if (FLASH_MODE == cdata->mode) {
		rc = hw_lm3646_flash_mode(flash_ctrl, cdata->data);
	} else {
		rc = hw_lm3646_torch_mode_1(flash_ctrl, cdata->mode);
	}
	flash_ctrl->state.mode = cdata->mode;
	flash_ctrl->state.data = cdata->data;
	mutex_unlock(flash_ctrl->hw_flash_mutex);

	return rc;
}


static int hw_lm3646_off(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	unsigned char val;

	cam_debug("%s ernter.\n", __func__);
	cam_info("%s 220250 ernter.\n", __func__);
	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	mutex_lock(flash_ctrl->hw_flash_mutex);
	flash_ctrl->state.mode = STANDBY_MODE;
	flash_ctrl->state.data = 0;
	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;

       i2c_func->i2c_read(i2c_client, REG_FLAGS1, &val);
       i2c_func->i2c_read(i2c_client, REG_FLAGS2, &val);
	i2c_func->i2c_write(i2c_client, REG_ENABLE, MODE_STANDBY);
	mutex_unlock(flash_ctrl->hw_flash_mutex);

	return 0;
}

static int hw_lm3646_match(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_flash_i2c_client *i2c_client;
	struct hw_flash_i2c_fn_t *i2c_func;
	struct hw_lm3646_private_data_t *pdata;
	unsigned char id;

	cam_debug("%s ernter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return -1;
	}

	i2c_client = flash_ctrl->flash_i2c_client;
	i2c_func = flash_ctrl->flash_i2c_client->i2c_func_tbl;
	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;

	i2c_func->i2c_read(i2c_client, REG_CHIPID, &id);      
	cam_info("%s id=0x%x.\n", __func__, id);
       id = id & CHIP_ID;
	if (id != CHIP_ID) {
		cam_err("%s match error, id(0x%x) != 0x%x.",
			__func__, id, CHIP_ID);
		return -1;
	}
	//i2c_func->i2c_write(i2c_client, REG_IVFM, 0x00);

	return 0;
}

static int hw_lm3646_get_dt_data(struct hw_flash_ctrl_t *flash_ctrl)
{
	struct hw_lm3646_private_data_t *pdata;
	struct device_node *of_node;
	int i;
	int rc = -1;

	cam_debug("%s enter.\n", __func__);

	if (NULL == flash_ctrl) {
		cam_err("%s flash_ctrl is NULL.", __func__);
		return rc;
	}

	pdata = (struct hw_lm3646_private_data_t *)flash_ctrl->pdata;
	of_node = flash_ctrl->dev->of_node;


    rc = of_property_read_u32_array(of_node, "huawei,flash-pin",
		pdata->pin, MAX_PIN);
	if (rc < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return rc;
	} else {
		for (i=0; i<MAX_PIN; i++) {
			cam_debug("%s pin[%d]=%d.\n", __func__, i,
				pdata->pin[i]);
		}
	}

	rc = of_property_read_u32(of_node, "huawei,flash-chipid",
		&pdata->chipid);
	cam_info("%s hisi,chipid 0x%x, rc %d\n", __func__,
		pdata->chipid, rc);
	if (rc < 0) {
		cam_err("%s failed %d\n", __func__, __LINE__);
		return rc;
	}

	rc = of_property_read_u32(of_node, "huawei,flash_led_num",
		&pdata->flash_led_num);
	cam_info("%s hisi,flash_led_num %d, rc %d\n", __func__,
		pdata->flash_led_num, rc);
	if (rc < 0) {
		cam_err("%s failed %d\n", __func__, __LINE__);
		return rc;
	}

	rc = of_property_read_u32(of_node, "huawei,torch_led_num",
		&pdata->torch_led_num);
	cam_info("%s hisi,torch_led_num %d, rc %d\n", __func__,
		pdata->torch_led_num, rc);
	if (rc < 0) {
		cam_err("%s failed %d\n", __func__, __LINE__);
		return rc;
	}

	rc = of_property_read_u8_array(of_node, "huawei,flash_led",
		pdata->flash_led, pdata->flash_led_num);
	if (rc < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return rc;
	} else {
		for (i=0; i< pdata->flash_led_num; i++) {
			cam_debug("%s flash_led[%d]=%d.\n", __func__, i,
				pdata->flash_led[i]);
		}
	}

	rc = of_property_read_u8_array(of_node, "huawei,torch_led",
		pdata->torch_led, pdata->torch_led_num);
	if (rc < 0) {
		cam_err("%s failed line %d\n", __func__, __LINE__);
		return rc;
	} else {
		for (i=0; i< pdata->torch_led_num; i++) {
			cam_debug("%s torch_led[%d]=%d.\n", __func__, i,
				pdata->torch_led[i]);
		}
	}

	return rc;
}

static ssize_t hw_lm3646_lightness_show(struct device *dev,
	struct device_attribute *attr,char *buf)
{
        int rc=0;

        snprintf(buf, MAX_ATTRIBUTE_BUFFER_SIZE, "mode=%d, data=%d.\n",
		hw_lm3646_ctrl.state.mode, hw_lm3646_ctrl.state.mode);
        rc = strlen(buf)+1;
        return rc;
}

static int hw_lm3646_param_check(char *buf, unsigned long *param,
	int num_of_par)
{
	char *token;
	int base, cnt;

	token = strsep(&buf, " ");

	for (cnt = 0; cnt < num_of_par; cnt++)
	{
		if (token != NULL)
		{
			if ((token[1] == 'x') || (token[1] == 'X')) {
				base = 16;
			} else {
				base = 10;
			}

			if (strict_strtoul(token, base, &param[cnt]) != 0) {
				return -EINVAL;
			}

			token = strsep(&buf, " ");
		}
		else
		{
			return -EINVAL;
		}
	}
	return 0;
}

static ssize_t hw_lm3646_lightness_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	struct hw_flash_cfg_data cdata = {0};
	unsigned long param[2]={0};
	int rc=0;

	rc = hw_lm3646_param_check((char *)buf, param, 2);
	if (rc < 0) {
		cam_err("%s failed to check param.", __func__);
		return rc;
	}

	cdata.mode = (int)param[0];
	cdata.data = (int)param[1];

	if (cdata.mode == STANDBY_MODE) {
		rc = hw_lm3646_off(&hw_lm3646_ctrl);
		if (rc < 0) {
			cam_err("%s lm3646 flash off error.", __func__);
			return rc;
		}
	} else {
		rc = hw_lm3646_on(&hw_lm3646_ctrl, &cdata);
		if (rc < 0) {
			cam_err("%s lm3646 flash on error.", __func__);
			return rc;
		}
	}

	return count;
}

static ssize_t hw_lm3646_flash_mask_show(struct device *dev,
	struct device_attribute *attr,char *buf)
{
        int rc=0;

        snprintf(buf, MAX_ATTRIBUTE_BUFFER_SIZE, "flash_mask_disabled=%d.\n",
		hw_lm3646_ctrl.flash_mask_enable);
        rc = strlen(buf)+1;
        return rc;
}

static ssize_t hw_lm3646_flash_mask_store(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	if ('0' == buf[0]) {
		hw_lm3646_ctrl.flash_mask_enable = false;
	} else {
		hw_lm3646_ctrl.flash_mask_enable = true;
	}
	cam_debug("%s flash_mask_enable=%d.", __func__,
			hw_lm3646_ctrl.flash_mask_enable);
	return count;
}

static void hw_lm3646_torch_brightness_set(struct led_classdev *cdev,
	enum led_brightness brightness)
{
	struct hw_flash_cfg_data cdata;
	int rc;
	unsigned int led_bright = brightness;
        cam_info("%s brightness= %d",__func__,brightness);
	if (STANDBY_MODE == led_bright) {
		rc = hw_lm3646_off(&hw_lm3646_ctrl);
		if (rc < 0) {
			cam_err("%s pmu_led off error.", __func__);
			return;
		}
	} else {
            int max_level;
            max_level =  3;
            cdata.mode = ((brightness-1) / max_level) + TORCH_MODE;
            cdata.data =((brightness-1) % max_level);

            cam_info("%s brightness=0x%x, mode=%d, data=%d.", __func__, brightness, cdata.mode, cdata.data);

            rc = hw_lm3646_on_1(&hw_lm3646_ctrl, &cdata);
        	if (rc < 0) {
        		cam_err("%s pmu_led on error.", __func__);
        		return;
        	}
	}
}

static struct device_attribute hw_lm3646_lightness =
    __ATTR(lightness, 0664, hw_lm3646_lightness_show, hw_lm3646_lightness_store);

static struct device_attribute hw_lm3646_flash_mask =
    __ATTR(flash_mask, 0664, hw_lm3646_flash_mask_show, hw_lm3646_flash_mask_store);

static int hw_lm3646_register_attribute(struct hw_flash_ctrl_t *flash_ctrl, struct device *dev)
{
	int rc = 0;

	if ((NULL == flash_ctrl) || (NULL == dev)) {
		cam_err("%s flash_ctrl or dev is NULL.", __func__);
		return -1;
	}

	flash_ctrl->cdev_torch.name = "torch";
	flash_ctrl->cdev_torch.max_brightness
		= MAX_BRIGHTNESS_FORMMI;
	flash_ctrl->cdev_torch.brightness_set = hw_lm3646_torch_brightness_set;
	rc = led_classdev_register((struct device *)dev, &flash_ctrl->cdev_torch);
	if (rc < 0) {
		cam_err("%s failed to register torch classdev.", __func__);
		goto err_out;
	}

	rc = device_create_file(dev, &hw_lm3646_lightness);
	if (rc < 0) {
		cam_err("%s failed to creat lightness attribute.", __func__);
		goto err_create_lightness_file;
	}

	rc = device_create_file(dev, &hw_lm3646_flash_mask);
	if (rc < 0) {
		cam_err("%s failed to creat flash_mask attribute.", __func__);
		goto err_create_flash_mask_file;
	}
	return 0;
err_create_flash_mask_file:
	device_remove_file(dev, &hw_lm3646_lightness);
err_create_lightness_file:
	led_classdev_unregister(&flash_ctrl->cdev_torch);
err_out:
	return rc;
}

static int hw_lm3646_remove(struct i2c_client *client)
{
	cam_debug("%s enter.", __func__);

	hw_lm3646_ctrl.func_tbl->flash_exit(&hw_lm3646_ctrl);

	client->adapter = NULL;
	return 0;
}

static const struct i2c_device_id hw_lm3646_id[] = {
	{"lm3646", (unsigned long)&hw_lm3646_ctrl},
	{}
};

static const struct of_device_id hw_lm3646_dt_match[] = {
	{.compatible = "huawei,lm3646"},
	{}
};
MODULE_DEVICE_TABLE(of, lm3646_dt_match);

static struct i2c_driver hw_lm3646_i2c_driver = {
	.probe	= hw_flash_i2c_probe,
	.remove = hw_lm3646_remove,
	.id_table	= hw_lm3646_id,
	.driver = {
		.name = "hw_lm3646",
		.of_match_table = hw_lm3646_dt_match,
	},
};

static int __init hw_lm3646_module_init(void)
{
	cam_info("%s erter.\n", __func__);
	return i2c_add_driver(&hw_lm3646_i2c_driver);
}

static void __exit hw_lm3646_module_exit(void)
{
	cam_info("%s enter.", __func__);
	i2c_del_driver(&hw_lm3646_i2c_driver);
	return;
}

static struct hw_flash_i2c_client hw_lm3646_i2c_client;

static struct hw_flash_fn_t hw_lm3646_func_tbl = {
	.flash_config = hw_flash_config,
	.flash_init = hw_lm3646_init,
	.flash_exit = hw_lm3646_exit,
	.flash_on = hw_lm3646_on,
	.flash_off = hw_lm3646_off,
	.flash_match = hw_lm3646_match,
	.flash_get_dt_data = hw_lm3646_get_dt_data,
	.flash_register_attribute = hw_lm3646_register_attribute,
};

static struct hw_flash_ctrl_t hw_lm3646_ctrl = {
	.flash_i2c_client = &hw_lm3646_i2c_client,
	.func_tbl = &hw_lm3646_func_tbl,
	.hw_flash_mutex = &flash_mut_lm3646,
	.pdata = (void*)&hw_lm3646_pdata,
	.flash_mask_enable = true,
	.state = {
		.mode = STANDBY_MODE,
	},
};

module_init(hw_lm3646_module_init);
module_exit(hw_lm3646_module_exit);
MODULE_DESCRIPTION("LM3646 FLASH");
MODULE_LICENSE("GPL v2");

