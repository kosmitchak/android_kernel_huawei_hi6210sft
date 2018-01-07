/* Copyright (c) 2008-2011, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above
 *	   copyright notice, this list of conditions and the following
 *	   disclaimer in the documentation and/or other materials provided
 *	   with the distribution.
 *	 * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *	   contributors may be used to endorse or promote products derived
 *	   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/pwm.h>
#include <linux/gpio.h>
#include <linux/hisi/hi6xxx-lcd_type.h>
#include "balong_fb.h"
#include "balong_fb_def.h"
#include "mipi_dsi.h"
#include "mipi_reg.h"
#include <linux/lcd_tuning.h>
//#include "drv_regulator_user.h"
//#include <linux/equip.h>


static volatile bool g_display_on = false;
static volatile bool backlight_log_once = true;

/*******************************************************************************
** Power ON Sequence(sleep mode to Normal mode)
*/
//static char soft_reset[] = {
//	0x01,
//};

static char bl_level[] = {
	0x51,
	0x00,
};

static char bl_enable[] = {
	0x53,
	0x24,
};

static char te_enable[] = {
	0x35,
	0x00,
};

static char exit_sleep[] = {
	0x11,
};

static char display_on[] = {
	0x29,
};
static char bl_enable_dimming[] = {
	0x53,
	0x2C,
};
//CABC related cmds
static char back_light_control1[] = {
	0xB8,
	0x07,0x90,0x1e,0x00,0x1E,0x32,
};

static char back_light_control2[] = {
	0xB9,
	0x07,0x82,0x3c,0x00,0x3C,0x87,
};

static char back_light_control3[] = {
	0xBA,
	0x07,0x78,0x78,0x0A,0x64,0xB4,
};

static char SRE_control1_cmd[] = {
	0xBB,
	0x01,0x1e,0x14,
};

static char SRE_control2_cmd[] = {
	0xBC,
	0x01,0x50,0x32,
};

static char SRE_control3_cmd[] = {
	0xBD,
	0x01,0xB4,0xA0,
};

static char back_light_control4[] = {
	0xCE,
	0x7d,0x40,0x43,0x49,0x55,0x62,0x71,0x82,
	0x94,0xa8,0xb9,0xcb,0xdb,0xE9,0xf5,0xfc,
	0xff,0x01,0x38,0x04,0x04,0x44,0x20,         //PWM_DIV = 0x01, PWM_CYCLE=0x38  -->45khz
 };

static char write_CABC_UI[] = {
	0x55,
	0x01,
};
//static char write_CABC_still[] = {
//	0x55,
//	0x02,
//};
static char write_CABC_video[] = {
	0x55,
	0x03,
};

static char write_CABC_OFF[] = {
	0x55,
	0x00,
};

//CE related cmd
//static char ce_off_cmd[] = {
//	0xCA,
//	0x00,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
//	0x08,0x20,0x80,0x80,0x0A,0x4A,0x37,0xA0,
//	0x55,0xF8,0x0C,0x0C,0x20,0x10,0x3F,0x3F,
//	0x00,0x00,0x10,0x10,0x3F,0x3F,0x3F,0x3F,
//};
static char basic_ce_cmd[] = {
	0xCA,
	0x01,0x80,0xa0,0xa0,0xff,0x90,0x80,0x28,
	0x30,0x3f,0x10,0x80,0x1A,0x4A,0x37,0x80,
	0x30,0xc8,0x0C,0x0C,0x09,0x07,0x08,0x08,
	0x30,0xa8,0x0e,0x12,0x20,0x18,0x20,0x18,
};


static char china_back_light_control1[] = {
	0xB8,
	0x07,0x91,0x1e,0x30,0x1E,0x32,
};

static char china_ce_cmd[] = {
	0xCA,
	0x01,0x80,0xC0,0xC0,0xC9,0xC0,0xA0,0xC0,
	0x13,0x20,0x10,0x73,0x1A,0x4A,0x37,0x80,
	0x30,0xC8,0x0C,0x0C,0x09,0x07,0x08,0x08,
	0x00,0xA8,0x0E,0x12,0x20,0x18,0x20,0x18,
};

/*******************************************************************************
** Power OFF Sequence(Normal to power off)
*/
static char display_off[] = {
	0x28,
};

static char enter_sleep[] = {
	0x10,
};
/* Delay TE*/
static char Delay_TE[] = {
	0x44,
	0x02, 0x80,
};

static char soft_reset_cmd[] = {
	0X01,
};

static char LTPS_timing_setting_cmd[] = {
	0XC6,
	0x7B, 0X01, 0X45, 0X05, 0X67, 0X67, 0X0A, 0X01,
	0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01, 0X01,
	0X01, 0X0A, 0X19, 0X05
};

static char  MCAP_cmd[] = {
	0xB0,
    0x00,
};

static char  interface_setting[] = {
	0xB3,
    0x04,0x08,0x00,0x22,0x00,
};

static char  interface_ID_set_cmd[] = {
	0xB4,
    0x0C,
};
static char  DSI_CTRL_cmd[] = {
	0xB6,
    0x3A,0xD3,
};
static char  set_pixsel_cmd[] = {
	0x3A,
    0x77,
};
static char  set_col_addr_cmd[] = {
	0x2A,
    0x00, 0x00, 0x04, 0xAF,
};

static char  set_page_addr_cmd[] = {
	0x2B,
    0x00,0x00,0x07,0x7F,
};

static char  send_image_cmd[] = {
	0x2C,
};
static char sequencer_test_control_cmd[] = {
    0xD6,
    0x01,
};

static struct dsi_cmd_desc jdi_display_on_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 5, WAIT_TYPE_MS,
		sizeof(soft_reset_cmd), soft_reset_cmd},
	{DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
		sizeof(MCAP_cmd), MCAP_cmd},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(interface_setting), interface_setting},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(interface_ID_set_cmd), interface_ID_set_cmd},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(DSI_CTRL_cmd), DSI_CTRL_cmd},
	{DTYPE_GEN_WRITE2, 0, 200, WAIT_TYPE_US,
		sizeof(sequencer_test_control_cmd), sequencer_test_control_cmd},
	{DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(basic_ce_cmd), basic_ce_cmd},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(LTPS_timing_setting_cmd), LTPS_timing_setting_cmd},
	{DTYPE_DCS_WRITE1, 0, 200, WAIT_TYPE_US,
		sizeof(set_pixsel_cmd), set_pixsel_cmd},
	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(set_col_addr_cmd), set_col_addr_cmd},
	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(set_page_addr_cmd), set_page_addr_cmd},
	{DTYPE_DCS_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(send_image_cmd), send_image_cmd},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(bl_level), bl_level},
	{DTYPE_DCS_WRITE1, 0, 10, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(te_enable), te_enable},
	{DTYPE_GEN_LWRITE, 0, 200, WAIT_TYPE_US,
		sizeof(Delay_TE), Delay_TE},
	{DTYPE_DCS_WRITE, 0, 120, WAIT_TYPE_MS,
		sizeof(exit_sleep), exit_sleep},
	{DTYPE_DCS_WRITE, 0, 10, WAIT_TYPE_MS,
		sizeof(display_on), display_on},
};

static struct dsi_cmd_desc jdi_display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 0, 20, WAIT_TYPE_MS,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 0, 80, WAIT_TYPE_MS,
		sizeof(enter_sleep), enter_sleep},
};
static struct dsi_cmd_desc jdi_cabc_cmds[] = {
	{DTYPE_GEN_WRITE2, 0, 100, WAIT_TYPE_US,
		sizeof(MCAP_cmd), MCAP_cmd},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(back_light_control1), back_light_control1},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(back_light_control2), back_light_control2},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(back_light_control3), back_light_control3},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(SRE_control1_cmd), SRE_control1_cmd},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(SRE_control2_cmd), SRE_control2_cmd},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(SRE_control3_cmd), SRE_control3_cmd},
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(back_light_control4), back_light_control4},
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(bl_enable), bl_enable},
};

static struct dsi_cmd_desc jdi_cabc_ui_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(write_CABC_UI), write_CABC_UI},
};

//static struct dsi_cmd_desc jdi_cabc_still_on_cmds[] = {
//	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
//		sizeof(write_CABC_still), write_CABC_still},
//};

static struct dsi_cmd_desc jdi_cabc_vid_on_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(write_CABC_video), write_CABC_video},
};
static struct dsi_cmd_desc jdi_cabc_off_cmds[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(write_CABC_OFF), write_CABC_OFF},
};
static struct dsi_cmd_desc jdi_china_ce_cmds[] = {
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(china_back_light_control1), china_back_light_control1},
	{DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(china_ce_cmd), china_ce_cmd},
};
static struct dsi_cmd_desc jdi_basic_ce_cmds[] = {
	{DTYPE_GEN_LWRITE, 0, 100, WAIT_TYPE_US,
		sizeof(back_light_control1), back_light_control1},
	{DTYPE_GEN_LWRITE, 0, 10, WAIT_TYPE_US,
		sizeof(basic_ce_cmd), basic_ce_cmd},
};
/*******************************************************************************
** LCD VCC
*/


#if 0
static struct regulator *g_lcd_vddio = NULL;
//lcd vddio
static int LCD_VDDIO_GET(struct platform_device *pdev)
{
    printk("%s enter\n",__func__);

    BUG_ON(pdev==NULL);

    g_lcd_vddio = regulator_get(&pdev->dev,SYNAPTICS_VBUS_LCD);
    if (IS_ERR(g_lcd_vddio)) {
        printk( "%s: failed to get regulator lcd vddio\n", __func__);
        return -EINVAL;
    }
    return 0;
}

static int LCD_VDDIO_PUT()
{
    printk("%s enter\n",__func__);
    if (IS_ERR(g_lcd_vddio)) {
        printk( "%s: failed to put regulator lcd vddio\n", __func__);
        return -EINVAL;
    }
    regulator_put(g_lcd_vddio);
    return 0;
}
static int LCD_VDDIO_ENABLE()
{
    int error;
    printk("%s enter\n",__func__);
    if (IS_ERR(g_lcd_vddio)) {
    	printk( "%s: failed to enable regulator lcd vddio 1\n", __func__);
    	return -EINVAL;
    }
    error = regulator_enable(g_lcd_vddio);
    if (error < 0) {
    	printk( "%s: failed to enable regulator lcd vddio 2\n", __func__);
    	return -EINVAL;
    }
    return 0;
}

static int LCD_VDDIO_DISABLE()
{
    int error;
    printk("%s enter\n",__func__);
    if (IS_ERR(g_lcd_vddio)) {
    	printk( "%s: failed to disable regulator lcd vddio 1\n", __func__);
    	return -EINVAL;
    }

    error = regulator_disable(g_lcd_vddio);
    if (error < 0) {
    	printk( "%s: failed to disable regulator lcd vddio 2\n", __func__);
    	return -EINVAL;
    }
    return 0;
}
#endif

/*******************************************************************************
** LCD IOMUX
*/

/*******************************************************************************
** LCD GPIO
*/


static void jdi_power_on(struct balong_panel_info *pinfo)
{
	pr_info("%s suc!\n", __func__);


	//LCD_VDDIO_ENABLE();

	/*LCD GPIO*/
    set_pinctrl_normal(pinfo);
	LCD_GPIO_REQUEST(pinfo);

	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 1);
	mdelay(5);

	LCD_VCC_ENABLE(pinfo->lcd_bias_vcc);  //vsn vsp enable
	LCD_VCC_ENABLE(pinfo->lcd_vsp_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_vsn_vcc);

	mdelay(5);
	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 0);
	mdelay(10);
	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_reset, 1);
	mdelay(10);
    LCD_VCC_ENABLE(pinfo->lcd_bl_vcc); //backlight
	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_drv_en, 1);	//backlight

}

static void jdi_power_off(struct balong_panel_info *pinfo)
{
	LCD_GPIO_SET_DIR_OUTPUT(pinfo->gpio_drv_en, 0);	//backlight
	mdelay(5);

	gpio_direction_output(pinfo->gpio_reset, 0);
	mdelay(10);
	LCD_VCC_DISABLE(pinfo->lcd_bias_vcc); //vsn vsp
	LCD_VCC_DISABLE(pinfo->lcd_vsn_vcc);
	LCD_VCC_DISABLE(pinfo->lcd_vsp_vcc);
	//gpio_direction_output(pinfo->gpio_power, 0); TBD
	mdelay(1);

	LCD_GPIO_FREE(pinfo);
	set_pinctrl_lowpower(pinfo);
	LCD_VCC_DISABLE(pinfo->lcd_bl_vcc);

	pr_info("%s suc!\n", __func__);
}

static struct balong_fb_panel_data jdi_panel_data;

/******************************************************************************/
static struct lcd_tuning_dev *p_tuning_dev = NULL;
static int cabc_mode = 1; /* allow application to set cabc mode to ui mode */
static int cabc_wanted = CABC_OFF;
static int ce_mode = 0;
#if 0
static int jdi_set_gamma(struct lcd_tuning_dev *ltd, enum lcd_gamma gamma)
{
	int ret = 0;
	struct platform_device *pdev = NULL;
	struct balong_fb_data_type *balongfd = NULL;
	//u32 ade_base = 0;

	BUG_ON(ltd == NULL);
	pdev = (struct platform_device *)(ltd->data);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	//ade_base = balongfd->ade_base;

	/* Fix me: Implement it */

	return ret;
}
#endif
static int jdi_set_cabc(struct lcd_tuning_dev *ltd, enum  tft_cabc cabc)
{
	int ret = 0;
	struct platform_device *pdev = NULL;
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(ltd == NULL);
	pdev = (struct platform_device *)(ltd->data);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	//now check backlight
	if ((balongfd->bl_level <= 15)&&(CABC_OFF != cabc))
	{
	    cabc_wanted = cabc;
	    balongfb_loge("could not set cabc to %d, because backlight %d <= 15!\n",cabc,balongfd->bl_level );
	    ret = -1;
	    return ret;
	}

	/* Fix me: Implement it */
	switch (cabc)
	{
		case CABC_UI:
			mipi_dsi_cmds_tx(jdi_cabc_ui_on_cmds, \
							ARRAY_SIZE(jdi_cabc_ui_on_cmds));
			cabc_mode = 1;
			cabc_wanted = CABC_OFF;
			balongfb_loge("set to CABC_UI mode wxh!\n");
			break;
		case CABC_VID:
			mipi_dsi_cmds_tx(jdi_cabc_vid_on_cmds, \
							ARRAY_SIZE(jdi_cabc_vid_on_cmds));
			cabc_mode = 2;
			cabc_wanted = CABC_OFF;
			balongfb_loge("set to CABC_VID mode wxh!\n");
			break;
		case CABC_OFF:
			mipi_dsi_cmds_tx(jdi_cabc_off_cmds, \
							ARRAY_SIZE(jdi_cabc_off_cmds));
			cabc_mode = 0;
			cabc_wanted = CABC_OFF;
			balongfb_loge("set to CABC_OFF mode wxh!\n");
			break;
		default:
			ret = -1;
			break;
	}

	return ret;
}


#if 0
static void jdi_store_ct_cscValue(unsigned int csc_value[])
{
    down(&ct_sem);
    g_csc_value [0] = csc_value[0];
    g_csc_value [1] = csc_value[1];
    g_csc_value [2] = csc_value[2];
    g_csc_value [3] = csc_value[3];
    g_csc_value [4] = csc_value[4];
    g_csc_value [5] = csc_value[5];
    g_csc_value [6] = csc_value[6];
    g_csc_value [7] = csc_value[7];
    g_csc_value [8] = csc_value[8];
    g_is_csc_set = 1;
    up(&ct_sem);

    return;
}

static int jdi_set_ct_cscValue(struct balong_fb_data_type *k3fd)
{
    u32 edc_base = 0;
    edc_base = k3fd->edc_base;
    down(&ct_sem);
    if (1 == g_is_csc_set) {
        set_reg(edc_base + 0x400, 0x1, 1, 27);
        set_reg(edc_base + 0x408, g_csc_value[0], 13, 0);
        set_reg(edc_base + 0x408, g_csc_value[1], 13, 16);
        set_reg(edc_base + 0x40C, g_csc_value[2], 13, 0);
        set_reg(edc_base + 0x40C, g_csc_value[3], 13, 16);
        set_reg(edc_base + 0x410, g_csc_value[4], 13, 0);
        set_reg(edc_base + 0x410, g_csc_value[5], 13, 16);
        set_reg(edc_base + 0x414, g_csc_value[6], 13, 0);
        set_reg(edc_base + 0x414, g_csc_value[7], 13, 16);
        set_reg(edc_base + 0x418, g_csc_value[8], 13, 0);
    }
    up(&ct_sem);
    return 0;
}
#endif

#if 0
static int jdi_set_color_temperature(struct lcd_tuning_dev *ltd, unsigned int csc_value[])
{
    int flag = 0;
    struct platform_device *pdev;
    struct balong_fb_data_type *k3fd;

    if (ltd == NULL) {
        return -1;
    }
    pdev = (struct platform_device *)(ltd->data);
    k3fd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);

    if (k3fd == NULL) {
        return -1;
    }

    jdi_store_ct_cscValue(csc_value);
    flag = jdi_set_ct_cscValue(k3fd);
    return flag;

}
#endif

//static struct lcd_tuning_ops sp_tuning_ops = {
//	.set_gamma = jdi_set_gamma,
//	.set_cabc = jdi_set_cabc,
//	.set_color_temperature = jdi_set_color_temperature,
//};

static ssize_t jdi_lcd_info_show(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	int ret = 0;
	struct balong_panel_info *pinfo = NULL;

	pinfo = jdi_panel_data.panel_info;
	sprintf(buf, "jdi_LPM070W425C 7.02' CMD TFT %d x %d\n",
		pinfo->xres, pinfo->yres);
	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t show_cabc_mode(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", cabc_mode);
}

static ssize_t store_cabc_mode(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	unsigned long val = 0;

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
		return ret;
	if(val == 1) {
		/* allow application to set cabc mode to ui mode */
		//cabc_mode =1;
		jdi_set_cabc(p_tuning_dev, CABC_UI);
	} else if (val == 2) {
		/* force cabc mode to video mode */
		//cabc_mode =2;
		jdi_set_cabc(p_tuning_dev, CABC_VID);
	}else if(val == 0){
		/* force cabc mode to OFF */
		//cabc_mode = 0;
		jdi_set_cabc(p_tuning_dev, CABC_OFF);
	}
	return sprintf((char *)buf, "%d\n", cabc_mode);
}
static void mipi_jdi_panel_backlight_set(bool status)
{     //we say status:true backlight on.
#if 0
	struct balong_fb_data_type *k3fd = NULL;
	u32 level = 0;
	char bl_level_adjust[2] = {
		0x51,
		0x00,
	};

	struct dsi_cmd_desc  jdi_bl_level_adjust[] = {
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(bl_level_adjust), bl_level_adjust},
	};

	k3fd = k3fd_backlight;
	BUG_ON(k3fd == NULL);


       if(false == status)
       {
            level = 0;
            gpio_direction_output((k3fd->panel_info).gpio_lcd_pwm_en, 0);
       }
       else
       {
            level = k3fd->bl_level;
            gpio_direction_output((k3fd->panel_info).gpio_lcd_pwm_en, 1);
       }

	bl_level_adjust[1] = level;

	mipi_dsi_cmds_tx(jdi_bl_level_adjust, \
		ARRAY_SIZE(jdi_bl_level_adjust));
#endif
}

static  ssize_t set_backlight_status(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	static int backlight_status = -1;
	int status = 0;

	status = simple_strtoul(buf, NULL, 0);

	if ((status != 0) && (status != 1)) {
		printk(KERN_ERR "[%s %d] Must be 1 or 0\n", __func__, __LINE__);
	} else if (status != backlight_status) {
		printk(KERN_ERR "[%s %d] status %d, backlight_status %d\n", __func__, __LINE__, status, backlight_status);
		if (1 == status) {
              mipi_jdi_panel_backlight_set(true);
		} else if (0 == status) {
              mipi_jdi_panel_backlight_set(false);
		}

		backlight_status = status;
	}
	return count;
}
static ssize_t get_ce_mode(struct device *dev,
	struct device_attribute *attr, char *buf)
{
	return snprintf(buf, 16,"%d\n", ce_mode);
}

static ssize_t set_ce_mode(struct device *dev,
	struct device_attribute *attr, const char *buf, size_t count)
{
	int ret = 0;
	unsigned long val = 0;

	ret = strict_strtoul(buf, 0, &val);
	if (ret)
		return ret;

	ce_mode = val;

	if(1 == ce_mode) {
	    mipi_dsi_cmds_tx(jdi_china_ce_cmds, ARRAY_SIZE(jdi_china_ce_cmds));
	    mdelay(5);
	}
	else
	{
	    mipi_dsi_cmds_tx(jdi_basic_ce_cmds, ARRAY_SIZE(jdi_basic_ce_cmds));
	    mdelay(5);
	}

	return snprintf((char *)buf, 16,"%d\n", ce_mode);
}

static DEVICE_ATTR(lcd_info, S_IRUGO, jdi_lcd_info_show, NULL);
static DEVICE_ATTR(cabc_mode, 0644, show_cabc_mode, store_cabc_mode);
static DEVICE_ATTR(set_backlight_status, 0664, NULL, set_backlight_status);
static DEVICE_ATTR(color_enhance_mode, 0664, get_ce_mode, set_ce_mode);

static struct attribute *jdi_attrs[] = {
	&dev_attr_lcd_info.attr,
	&dev_attr_cabc_mode.attr,
	&dev_attr_set_backlight_status.attr,
	&dev_attr_color_enhance_mode.attr,
	NULL,
};
static struct attribute_group jdi_attr_group = {
	.attrs = jdi_attrs,
};

static int jdi_sysfs_init(struct platform_device *pdev)
{
	int ret = 0;
	ret = sysfs_create_group(&pdev->dev.kobj, &jdi_attr_group);
	if (ret) {
		balongfb_loge("create sysfs file failed!\n");
		return ret;
	}
	return 0;
}

static void jdi_sysfs_deinit(struct platform_device *pdev)
{
	sysfs_remove_group(&pdev->dev.kobj, &jdi_attr_group);
}

static int mipi_jdi_panel_on(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct balong_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);

	pr_info("%s enter succ!\n",__func__);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	pinfo = &(balongfd->panel_info);
	pr_info("%s,pinfo->lcd_init_step = %d!\n",__func__,pinfo->lcd_init_step);
	if (pinfo->lcd_init_step == LCD_INIT_POWER_ON) {
		if (!g_display_on) {
			LCD_VCC_SET_VOLTAGE(pinfo->lcd_bias_vcc, 5500000, 5500000); /* 5.5V */
			LCD_VCC_SET_VOLTAGE(pinfo->lcd_vsn_vcc, 5500000, 5500000); /* 5.5V */
			LCD_VCC_SET_VOLTAGE(pinfo->lcd_vsp_vcc, 5500000, 5500000); /* 5.5V */

			jdi_power_on(pinfo);
		}
		pinfo->lcd_init_step = LCD_INIT_SEND_SEQUENCE;

	} else if (pinfo->lcd_init_step == LCD_INIT_SEND_SEQUENCE) {
		//jdi_set_ct_cscValue(k3fd);
		/* lcd display on sequence */
		if (!g_display_on) {
		mipi_dsi_cmds_tx(jdi_display_on_cmds, ARRAY_SIZE(jdi_display_on_cmds));
		mipi_dsi_cmds_tx(jdi_cabc_cmds, ARRAY_SIZE(jdi_cabc_cmds));
		if (1 == ce_mode)
		{
			mipi_dsi_cmds_tx(jdi_china_ce_cmds, ARRAY_SIZE(jdi_china_ce_cmds));
			mdelay(5);
		}
		mipi_dsi_cmds_tx(jdi_cabc_ui_on_cmds, ARRAY_SIZE(jdi_cabc_ui_on_cmds));
		mdelay(5);
		g_display_on = true;
		backlight_log_once = true;
		cabc_wanted = CABC_OFF;
		cabc_mode = 1;

		}
	} else {
		balongfb_loge("failed to init lcd!\n");
	}

	return 0;
}

static int mipi_jdi_panel_off(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	struct balong_panel_info *pinfo = NULL;

	BUG_ON(pdev == NULL);
	pr_info("%s enter succ!\n",__func__);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);
	pinfo = &(balongfd->panel_info);

	if (g_display_on) {
		g_display_on = false;
		mipi_dsi_cmds_tx(jdi_display_off_cmds, ARRAY_SIZE(jdi_display_off_cmds));

		jdi_power_off(pinfo);
	}
	return 0;
}

static int mipi_jdi_panel_remove(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;

	BUG_ON(pdev == NULL);

	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	if (balongfd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		PWM_CLK_PUT(&(balongfd->panel_info));
	}

	LCD_VCC_PUT(&(balongfd->panel_info));
    set_pinctrl_lowpower(&(balongfd->panel_info));
	jdi_sysfs_deinit(pdev);

	return 0;
}

static int mipi_jdi_panel_set_backlight(struct platform_device *pdev)
{
	struct balong_fb_data_type *balongfd = NULL;
	u32 level = 0;

	char bl_level_adjust[2] = {
		0x51,
		0x00,
	};

	struct dsi_cmd_desc  jdi_bl_level_adjust[] = {
		{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
			sizeof(bl_level_adjust), bl_level_adjust},
	};

	struct dsi_cmd_desc  jdi_bl_enable_dimming[] = {
	{DTYPE_DCS_WRITE1, 0, 100, WAIT_TYPE_US,
		sizeof(bl_enable_dimming), bl_enable_dimming},
	};

	BUG_ON(pdev == NULL);
	balongfd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(balongfd == NULL);

	/*Our eyes are more sensitive to small brightness.
	So we adjust the brightness of lcd following iphone4 */
	level = balongfd->bl_level;

	if (level > 255) {
		level = 255;
    }
    //backlight may turn off when bl_level is below 6.
    /*Our eyes are more sensitive to small brightness.
	So we adjust the brightness of lcd following iphone4
	---the original description for >248*/
	if (level < 6 && level != 0)
	{
		level = 6;
	} /*else if (level >= 29 && level <= 34) {
		level = 35;
	} else if (level >= 59 && level <= 69) {
		level = 70;
	} else if (level >= 123 && level <= 133) {
		level = 134;
	} else if (level > 248) {
		level = 248;
	}*/

	bl_level_adjust[1] = level;

	mipi_dsi_cmds_tx(jdi_bl_level_adjust, \
		ARRAY_SIZE(jdi_bl_level_adjust));

	/*if((level <= 15)&&(level != 0))
	{    //set cabc off because of fliker
	    if(0 != cabc_mode)
	    {
	        int tmp = 0;
	        balongfb_loge("cabc_mode=%d cabc_wanted=%d\n",cabc_mode,cabc_wanted);
	        tmp = cabc_mode;
	        jdi_set_cabc(p_tuning_dev, CABC_OFF);
	        if (1 == tmp)
	        {
	            cabc_wanted = CABC_UI;
	        }
	        else if(2 == tmp)
	        {
	            cabc_wanted = CABC_VID;
	        }
	    }
	}
	else
	{    //resume to the latest mode
	    if(CABC_UI == cabc_wanted)
	    {
	        balongfb_loge("send jdi_cabc_ui_on_cmds wxh!\n");
	        jdi_set_cabc(p_tuning_dev, CABC_UI);
	    }
	    else if(CABC_VID == cabc_wanted)
	    {
	        balongfb_loge("send jdi_cabc_vid_on_cmds wxh!\n");
	        jdi_set_cabc(p_tuning_dev, CABC_VID);
	    }
	}*/
	if (backlight_log_once) {
		backlight_log_once = false;
		balongfb_loge("----balongfd->bl_level=%d,set backlight to level = %d\n",balongfd->bl_level, level);

		mdelay(20);
		mipi_dsi_cmds_tx(jdi_bl_enable_dimming, \
			ARRAY_SIZE(jdi_bl_enable_dimming));
	}

	return 0;
}

#if 0
static int mipi_jdi_panel_set_fastboot(struct platform_device *pdev)
{
	struct balong_fb_data_type *k3fd = NULL;
	int retval;

	BUG_ON(pdev == NULL);

	k3fd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);

	LCD_VCC_ENABLE(pinfo->lcd_bl_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_bias_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_vsn_vcc);
	LCD_VCC_ENABLE(pinfo->lcd_vsp_vcc);
    set_pinctrl_normal(pinfo);
	LCD_GPIO_REQUEST(pinfo);


	if (k3fd->panel_info.bl_set_type & BL_SET_BY_PWM) {
		PWM_IOMUX_SET(&(k3fd->panel_info), NORMAL);
		PWM_GPIO_REQUEST(&(k3fd->panel_info));
	}

	g_display_on = true;

	k3fd_backlight = k3fd;

	return 0;
}
#endif

static int mipi_jdi_panel_set_cabc(struct platform_device *pdev, int value)
{
#if 0
	u32 edc_base = 0;
	struct balong_fb_data_type *k3fd = NULL;

	BUG_ON(pdev == NULL);
	k3fd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);
	edc_base = k3fd->edc_base;
#endif
	return 0;
}
#if 0
static int mipi_jdi_panel_check_esd(struct platform_device *pdev)
{
	u32 dsi_base = 0;
	u32 tmp = 0;
	u32 delay_count =0;
	bool is_timeout;
	u32 lcd_status = 0;
	struct balong_fb_data_type *k3fd = NULL;

	BUG_ON(pdev == NULL);
	k3fd = (struct balong_fb_data_type *)platform_get_drvdata(pdev);
	BUG_ON(k3fd == NULL);
	dsi_base = k3fd->dsi_base;

	/* read pwm */
	outp32(dsi_base + MIPIDSI_GEN_HDR_ADDR, 0x1 << 8 |0x37);
	outp32(dsi_base + MIPIDSI_GEN_HDR_ADDR, 0xAC << 8 |0x14);

	/* check command fifo empty */
	while (1) {
		tmp = inp32(k3fd->dsi_base + MIPIDSI_CMD_PKT_STATUS_ADDR);
		if (((tmp & 0x00000001) == 0x00000001) || delay_count > 100) {
			is_timeout = (delay_count > 100) ? true : false;
			delay_count = 0;
			break;
		} else {
			udelay(1);
			++delay_count;
		}
	}

	/* check read command finish */
	while (1) {
		tmp = inp32(k3fd->dsi_base + MIPIDSI_CMD_PKT_STATUS_ADDR);
		if (((tmp & 0x00000040) != 0x00000040) || delay_count > 100) {
			is_timeout = (delay_count > 100) ? true : false;
			delay_count = 0;
			break;
		} else {
			udelay(1);
			++delay_count;
		}
	}

	if (is_timeout == false) {
		lcd_status = inp32(k3fd->dsi_base + MIPIDSI_GEN_PLD_DATA_ADDR);
	}

	return lcd_status;
}
#endif
#if 0
void equip_set_backlight_status(struct EQUIP_PARAM* arg)
{
	set_backlight_status(NULL, NULL, arg->str_in, sizeof(arg->str_in));
}
extern int get_tps65132_info();
static int equip_get_LCD_status(struct EQUIP_PARAM* arg)
{
	int ret = 0;

	int tps65132_status = get_tps65132_info();

	if(tps65132_status > 0)
	{
		 ret = snprintf(arg->str_out,16, "%s", "1");
	}
	else
	{
		ret = snprintf(arg->str_out, 16,"%s", "0");
	}

       return ret;
}
#endif

static struct balong_panel_info jdi_panel_info = {0};
static struct balong_fb_panel_data jdi_panel_data = {
	.panel_info = &jdi_panel_info,
	.on = mipi_jdi_panel_on,
	.off = mipi_jdi_panel_off,
	.remove = mipi_jdi_panel_remove,
	.set_backlight = mipi_jdi_panel_set_backlight,
	.set_fastboot = NULL,//mipi_jdi_panel_set_fastboot,
	.set_cabc = mipi_jdi_panel_set_cabc,
    //.check_esd = mipi_jdi_panel_check_esd,
};

static int __init jdi_probe(struct platform_device *pdev)
{
	struct balong_panel_info *pinfo = NULL;
	//struct resource *res = NULL;
	struct platform_device *reg_pdev = NULL;
	struct lcd_properities lcd_props;

//	enum EQUIP_DEVICE equip_dev;
//	enum EQUIP_OPS ops;

	pr_info("%s enter succ!\n",__func__);

	g_display_on = false;
	pinfo = jdi_panel_data.panel_info;

	/* init lcd panel info */
	pinfo->xres = 1200;
	pinfo->yres = 1920;
	pinfo->width  = 94;  //mm
	pinfo->height = 151; //mm
	pinfo->type = PANEL_MIPI_CMD;
	pinfo->orientation = LCD_PORTRAIT;
	pinfo->bpp = ADE_OUT_RGB_888;
	pinfo->s3d_frm = ADE_FRM_FMT_2D;
	pinfo->bgr_fmt = ADE_RGB;
	pinfo->bl_set_type = BL_SET_BY_MIPI;
	pinfo->bl_max = 100;
	pinfo->bl_min = 1;
	pinfo->frc_enable = 1;
	if(NULL != strstr(saved_command_line, "androidboot.swtype=factory"))
		pinfo->esd_enable = 0;
	else
		pinfo->esd_enable = 1; //B160 bugfix 1->0
	pr_info("esd_enable =%d\n", pinfo->esd_enable);
	pinfo->sbl_enable = 1;

	pinfo->sbl.bl_max = 0xff;
	pinfo->sbl.cal_a = 0x0f;
	pinfo->sbl.cal_b = 0xd8;
	pinfo->sbl.str_limit = 0x40;

	pinfo->ldi.h_back_porch = 20;
	pinfo->ldi.h_front_porch = 107;
	pinfo->ldi.h_pulse_width = 10;
	pinfo->ldi.v_back_porch = 6;
	pinfo->ldi.v_front_porch = 8;
	pinfo->ldi.v_pulse_width = 2;

	pinfo->ldi.hsync_plr = 1;
	pinfo->ldi.vsync_plr = 0;
	pinfo->ldi.pixelclk_plr = 1;
	pinfo->ldi.data_en_plr = 0;

	pinfo->ldi.disp_mode = LDI_DISP_MODE_NOT_3D_FBF;

	/* Note: must init here */
	//pinfo->clk_rate = 160000000;

	pinfo->mipi.lane_nums = DSI_4_LANES;
	pinfo->mipi.color_mode = DSI_24BITS_1;
	pinfo->mipi.vc = 0;

	//pinfo->mipi.dsi_bit_clk = 500;
	pinfo->frame_rate = 60;


	//LCD_VDDIO_GET(pdev);
	//LCD_IOMUX_GET(pinfo);
	LCD_VCC_GET(pdev, pinfo);

	set_pinctrl_init(pdev, pinfo);

	/* lcd resource */
	LCD_RESOURCE(pdev, pinfo);

	pdev->id = 1;

	/* alloc panel device data */
	if (platform_device_add_data(pdev, &jdi_panel_data,
		sizeof(struct balong_fb_panel_data))) {
		balongfb_loge("platform_device_add_data failed!\n");
		platform_device_put(pdev);
		//LCD_VDDIO_PUT(pdev);
		return -ENOMEM;
	}

	reg_pdev = balong_fb_add_device(pdev);
	if(NULL == reg_pdev)
	{
	#if 0
		balongfb_loge("k3_fb_add_device failed!\n");
		platform_device_put(pdev);
		LCD_VDDIO_PUT(pdev);
	#endif
		return -ENOMEM;
	}


	/* for cabc */
	lcd_props.type = TFT;
	lcd_props.default_gamma = GAMMA22;

#if 0
	p_tuning_dev = lcd_tuning_dev_register(&lcd_props, &sp_tuning_ops, (void *)reg_pdev);
	if (IS_ERR(p_tuning_dev)) {
		balongfb_loge("lcd_tuning_dev_register failed!\n");
		return -1;
	}
#endif

	jdi_sysfs_init(pdev);

	/*
	equip_dev = LCD;
	ops  = EP_READ;
	register_equip_func(equip_dev, ops, equip_get_LCD_status);
	ops  = EP_WRITE;
	register_equip_func(equip_dev, ops, equip_set_backlight_status);
	k3fd_backlight = (struct balong_fb_data_type *)platform_get_drvdata(reg_pdev);
	*/
	pr_info("%s exit succ!\n",__func__);
	return 0;
}

static const struct of_device_id mipi_jdi_lpm070w425c_match_table[] = {
    {
        .compatible ="hisilicon,mipi-jdi-lpm070w425c",
        .data = NULL,
    },
    {},
};

static struct platform_driver this_driver = {
	.probe = jdi_probe,
	.remove = NULL,
	.suspend = NULL,
	.resume = NULL,
	.shutdown = NULL,
	.driver = {
		.name = "mipi_jdi_LPM070W425C",
		.of_match_table = mipi_jdi_lpm070w425c_match_table,
	},
};

static int __init mipi_jdi_panel_init(void)
{
	int ret = 0;
    if( get_lcd_type() != JDI_LPM070W425C_LCD)
    {
        return ret;
    }
	ret = platform_driver_register(&this_driver);
	if (ret) {
		balongfb_loge("not able to register the driver\n");
	}

	return ret;
}

module_init(mipi_jdi_panel_init);
