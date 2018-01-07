#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/pinctrl/pinctrl.h>
#include <linux/pinctrl/consumer.h>
#include <linux/printk.h>
#include <linux/huawei/gps/huawei_gps.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/stat.h>
#include <linux/rfkill.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/version.h>
#include <linux/workqueue.h>
#include <linux/unistd.h>
#include <linux/bug.h>
#include <linux/mutex.h>
#include <linux/wakelock.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>


#ifdef CONFIG_HWCONNECTIVITY
#include <linux/huawei/hw_connectivity.h>
#endif

#define DTS_COMP_GPS_POWER_NAME "huawei,gps_power"
#define GPS_REF_CLK_FREQ    49152000
#define GPS_STANDBY         0
#define GPS_ACTIVE          1

typedef struct gps_bcm_info {
    struct gpio gpioid_en;
    struct clk *clk;
    struct clk *refclk_sel;
    struct clk *refclk;
    struct pinctrl *pctrl;
    struct pinctrl_state *pins_normal;
    struct pinctrl_state *pins_idle;
    struct gpio       gpioid_hostwake;
	bool is_hostwake;
} GPS_BCM_INFO;

static struct clk *gps_ref_clk = NULL;
static GPS_BCM_INFO *g_gps_bcm = NULL;

#define HOST_WAKE_MODULE_NAME "gps_geofence_wake"
struct gps_geofence_wake{
    /// irq from gpio_to_irq()
    int irq;
    /// HOST_WAKE_GPIO
    int host_req_pin;
    /// misc driver structure
    struct miscdevice misc;
    /// wake_lock
    struct wake_lock wake_lock;
};
static struct gps_geofence_wake g_geofence_wake;
static int gps_geofence_wake_open(struct inode *inode, struct file *filp)
{
    printk("%s\n", __func__);
    return 0;
}

static int gps_geofence_wake_release(struct inode *inode, struct file *filp)
{
    printk("%s\n", __func__);
    return 0;
}

static int gps_geofence_wake_ioctl( struct file *filp,
        unsigned int cmd, unsigned int arg)
{
    printk("%s\n", __func__);
    return 0;
}

static const struct file_operations gps_geofence_wake_fops = {
    .owner = THIS_MODULE,
    .open = gps_geofence_wake_open,
    .release = gps_geofence_wake_release,
    .unlocked_ioctl = gps_geofence_wake_ioctl
};

/// set/reset wake lock by HOST_WAKE level
/// \param gpio the value of HOST_WAKE_GPIO
static void gps_geofence_wake_lock(int gpio)
{
    struct gps_geofence_wake *ac_data = &g_geofence_wake;
    ///we need to use wake_lock_timeout instead of wake_unlock
    wake_lock_timeout(&ac_data->wake_lock, 5*HZ);
}

static irqreturn_t gps_host_wake_isr(int irq, void *dev)
{
    struct gps_geofence_wake *ac_data = &g_geofence_wake;
    int gps_host_wake = ac_data->host_req_pin;
    char gpio_value = 0x00;

    printk("%s\n", __func__);

    gpio_value = gpio_get_value(gps_host_wake);

    // wake_lock
    gps_geofence_wake_lock(gpio_value);

    return IRQ_HANDLED;
}

/// initialize GPIO and IRQ
/// \param gpio the GPIO of HOST_WAKE
/// \return if SUCCESS, return the id of IRQ, if FAIL, return -EIO
static int gps_gpio_irq_init(int gpio)
{
    int ret = 0;
    int irq = 0;

    printk("[gps]%s\n", __func__);
    // 1. Set GPIO
    if ((gpio_request(gpio, "gps_host_wake")))
    {
        printk("[gps]Can't request HOST_REQ GPIO %d.It may be already registered in init.xyz.3rdparty.rc/init.xyz.rc\n", gpio);
        return -EIO;
    }
    gpio_export(gpio, false);
    gpio_direction_input(gpio);

    // 2. Set IRQ
    irq = gpio_to_irq(gpio);
    if (irq < 0)
    {
        printk("[gps]Could not get HOST_WAKE_GPIO = %d!, err = %d\n", gpio, irq);
        gpio_free(gpio);
        return -EIO;
    }

    ret = request_irq(irq, gps_host_wake_isr, IRQF_TRIGGER_RISING | IRQF_NO_SUSPEND, "gps_host_wake", NULL);
    if (ret)
    {
        printk("[gps]Request_host wake irq failed.\n");
        gpio_free(gpio);
        return -EIO;
    }

    ret = irq_set_irq_wake(irq, 1);

    if (ret)
    {
        printk("[gps]Set_irq_wake failed.\n");
        gpio_free(gpio);
        free_irq(irq, NULL);
        return -EIO;
    }

    return irq;
}

/// cleanup GPIO and IRQ
static void gps_gpio_irq_cleanup(int gpio, int irq)
{
    pr_debug("[gps]%s\n", __func__);
    gpio_free(gpio);
    free_irq(irq, NULL);
}




static ssize_t gps_write_proc_nstandby(struct file *filp, const char __user *buffer, size_t len, loff_t *off)
{
    char gps_nstandby = '0';
    printk(KERN_INFO "[GPS] gps_write_proc_nstandby \n");

    if ((len < 1) || (NULL == g_gps_bcm)) {
        printk(KERN_ERR "[GPS] gps_write_proc_nstandby g_gps_bcm is NULL or read length = 0.\n");
        return -EINVAL;
    }

    if (copy_from_user(&gps_nstandby, buffer, sizeof(gps_nstandby))) {
        printk(KERN_ERR "[GPS] gps_write_proc_nstandby copy_from_user failed!\n");
        return -EFAULT;
    }

    if (gps_nstandby == '0') {
        printk(KERN_INFO "[GPS] gps nstandby.\n");
        gpio_set_value(g_gps_bcm->gpioid_en.gpio, GPS_STANDBY);
    }
    else if (gps_nstandby == '1') {
        printk(KERN_INFO "[GPS] gps active.\n");
        gpio_set_value(g_gps_bcm->gpioid_en.gpio, GPS_ACTIVE);
    }
    else {
        printk(KERN_ERR "[GPS] gps nstandby write error code[%d].\n",gps_nstandby);
    }

    return len;
}

static ssize_t gps_read_proc_nstandby(struct file *filp, const char __user *buffer, size_t len, loff_t *off)
{
    int gps_nstandby = 0;
    char tmp[2];
    memset(tmp,0,sizeof(tmp));
    printk(KERN_INFO "[GPS] gps_read_proc_nstandby \n");
    if (len < 1 || NULL == g_gps_bcm) {
        printk(KERN_ERR "[GPS] gps_read_proc_nstandby g_gps_bcm is NULL or read length = 0.\n");
        return -EINVAL;
    }
    len = 1;
    printk(KERN_INFO "[GPS] gps_read_proc_nstandby off = %d\n", (unsigned int)*off);
    if ((unsigned int)*off > 0) {
        return 0;
    }
    gps_nstandby = gpio_get_value(g_gps_bcm->gpioid_en.gpio);
    sprintf(tmp,"%d",gps_nstandby);
    printk(KERN_INFO "[GPS] gps nstandby status[%s] \n",tmp);

    if (copy_to_user(buffer, tmp, len)) {
        printk(KERN_ERR "[GPS] gps_read_proc_nstandby copy_to_user failed!\n");
        return -EFAULT;
    }
    *off += len;
    return len;
}

static const struct file_operations gps_proc_fops_nstandby= {
   .owner = THIS_MODULE,
   .write = gps_write_proc_nstandby,
   .read = gps_read_proc_nstandby,
};

static int create_gps_proc_file(void)
{
    int ret = 0;
    struct proc_dir_entry *gps_dir = NULL;
    struct proc_dir_entry *gps_nstandby_file = NULL;
    gps_dir = proc_mkdir("gps", NULL);
    if (!gps_dir) {
        printk(KERN_ERR "[GPS] proc dir create failed\n");
        ret = -ENOMEM;
        return ret;
    }

    gps_nstandby_file = proc_create("nstandby", S_IRUGO| S_IWUSR | S_IWGRP | S_IFREG, gps_dir, &gps_proc_fops_nstandby);
    if (!gps_nstandby_file) {
        printk(KERN_ERR "[GPS] proc nstandby file create failed\n");
        ret = -ENOMEM;
        return ret;
    }
    printk(KERN_INFO "[GPS] gps create proc file ok. \n");

    return ret;
}

static int k3_gps_bcm_probe(struct platform_device *pdev)
{
    GPS_BCM_INFO *gps_bcm = NULL;
    struct device *gps_power_dev = &pdev->dev;
    struct device_node *np = gps_power_dev->of_node;
    enum of_gpio_flags gpio_flags;
    int ret = 0;
    int irq = 0;
    int ref_clk = 1;
    struct gps_geofence_wake *ac_data = &g_geofence_wake;
    
    const char *GPS_feature_type = "GPS"; 
    int feature_type_len = 0; 

    printk(KERN_INFO "[GPS] start find gps_power\n");
    gps_bcm = kzalloc(sizeof(GPS_BCM_INFO), GFP_KERNEL);
    if (!gps_bcm) {
        printk(KERN_ERR "[GPS] Alloc memory failed\n");
        return -ENOMEM;
    }
    
    GPS_feature_type = of_get_property(np,"broadcom_config,product", &feature_type_len); 

    if ((NULL != GPS_feature_type) && (0 != strstr(GPS_feature_type,"hi3635")) )
      {
       gps_bcm->gpioid_en.gpio = of_get_named_gpio(np, "huawei,gps_en", 0);
       ref_clk = 0;
       printk(KERN_INFO "[GPS] product is hi3635\n");
       
      }
    else
        {
          gps_bcm->gpioid_en.gpio = of_get_gpio_by_prop(np , "huawei,gps_en" , 0 , 0 , &gpio_flags);
        }
    
    if (!gpio_is_valid(gps_bcm->gpioid_en.gpio))
    {
        printk(KERN_ERR "[GPS] get huawei,gps_en gpio failed.\n");
        ret = -1;
        goto err_free;
    }

    ret = gpio_request(gps_bcm->gpioid_en.gpio, "gps_enbale");
    if (ret) {
        printk(KERN_ERR "[GPS] gpio_direction_output %d failed, ret:%d\n", gps_bcm->gpioid_en.gpio, ret);
        goto err_free;
    }
    gpio_export(gps_bcm->gpioid_en.gpio, false);
    ret = gpio_direction_output( gps_bcm->gpioid_en.gpio, 0);
    if (ret) {
        printk(KERN_ERR "[GPS] gpio_direction_output %d failed, ret:%d\n", gps_bcm->gpioid_en.gpio, ret);
        goto err_free;
    }
    printk(KERN_INFO "[GPS] finish gpio_direction_output gps_power\n");
    
    GPS_feature_type = of_get_property(np,"broadcom_config,ic_type", &feature_type_len);
    if (NULL != GPS_feature_type)
    {
        if (0 != strstr(GPS_feature_type,"47531") )
          {
           printk(KERN_INFO "[GPS] ic_type is 47531\n");
           gps_bcm->is_hostwake = 1;        
          }
    }
    
    if(gps_bcm->is_hostwake){
        gps_bcm->gpioid_hostwake.gpio = of_get_named_gpio(np, "huawei,gps_hostwake", 0);
        if (gps_bcm->gpioid_hostwake.gpio<0)
        {
            ret = -1;
            goto err_free;
         }
        // 1. Init GPIO and IRQ for HOST_WAKE
        printk("[gps]%s,gps_bcm->gpioid_hostwake.gpio=%d\n", __func__,gps_bcm->gpioid_hostwake.gpio);

        irq = gps_gpio_irq_init(gps_bcm->gpioid_hostwake.gpio);
        if (irq < 0)
        {
            return -EIO;
        }

        // 2. Register Driver
        memset(ac_data, 0, sizeof(struct gps_geofence_wake));

        // 2.1 Misc device setup
        ac_data->misc.minor = MISC_DYNAMIC_MINOR;
        ac_data->misc.name = HOST_WAKE_MODULE_NAME;
        ac_data->misc.fops = &gps_geofence_wake_fops;

        // 2.2 Information that be used later
        ac_data->irq = irq;
        ac_data->host_req_pin = gps_bcm->gpioid_hostwake.gpio;


        printk("[gps]misc register, name %s, irq %d, host req pin num %d\n", ac_data->misc.name, irq, ac_data->host_req_pin);
        // 2.3 Register misc driver
        if (0 != (ret = misc_register(&ac_data->misc)))
        {
            printk("[gps]cannot register gps geofence wake miscdev on minor=%d (%d)\n", MISC_DYNAMIC_MINOR, ret);
            return ret;
        }

        // 3. Init wake_lock
        wake_lock_init(&ac_data->wake_lock, WAKE_LOCK_SUSPEND, "gps_geofence_wakelock");
        }

        ret = create_gps_proc_file();
        if (ret) {
            printk(KERN_ERR "[GPS] gps create proc file failed.\n");
            goto err_free;
        }


    /* Set 32KC clock */
    gps_bcm->clk = of_clk_get_by_name(np, "gps_32k");
    if (IS_ERR(gps_bcm->clk)) {
        printk(KERN_ERR "[GPS] clk_32k get failed!\n");
        ret = -1;
        goto err_clk_get;
    }
    ret = clk_prepare_enable(gps_bcm->clk);
    if(ret) {
        printk(KERN_ERR "[GPS] clk_32k enable is failed\n");
        goto err_free_clk;
    }
    printk(KERN_INFO "[GPS] clk_32k is finished\n");


    gps_bcm->pctrl = devm_pinctrl_get(gps_power_dev);
    if (IS_ERR(gps_bcm->pctrl)) {
        printk(KERN_ERR "[GPS] pinctrl get error!\n");
        ret = -1;
        goto err_pinctrl_get;
    }

    gps_bcm->pins_normal = pinctrl_lookup_state(gps_bcm->pctrl, "default");
    if (IS_ERR(gps_bcm->pins_normal)) {
        printk(KERN_ERR "[GPS] gps_bcm->pins_normal lookup error!\n");
        ret = -1;
        goto err_free_pinctrl;
    }

    gps_bcm->pins_idle = pinctrl_lookup_state(gps_bcm->pctrl, "idle");
    if (IS_ERR(gps_bcm->pins_normal)) {
        printk(KERN_ERR "[GPS] gps_bcm->pins_idle lookup error!\n");
        ret = -1;
        goto err_free_pinctrl;
    }

    ret = pinctrl_select_state(gps_bcm->pctrl, gps_bcm->pins_normal);
    if (ret) {
        printk(KERN_ERR "[GPS] pinctrl_select_state error!\n");
        goto err_free_pinctrl;
    }
    printk(KERN_INFO "[GPS] pinctrl is finish\n");
    if(ref_clk){
        gps_bcm->refclk_sel = of_clk_get_by_name(np, "ref_clk_sel");
        if (IS_ERR(gps_bcm->refclk_sel)) {
            printk(KERN_ERR "[GPS] ref_clk_sel get failed!\n");
            ret = -1;
            goto err_refclk_sel_get;
        }
        ret = clk_set_rate(gps_bcm->refclk_sel, GPS_REF_CLK_FREQ);
        if (ret < 0) {
            printk(KERN_ERR "[GPS] ref_clk_sel set rate failed\n");
            ret = -1;
            goto err_free_refclk_sel;
        }

        gps_bcm->refclk = of_clk_get_by_name(np, "ref_clk");
        if (IS_ERR(gps_bcm->refclk)) {
            printk(KERN_ERR "[GPS] ref_clk get failed!\n");
            ret = -1;
            goto err_refclk_get;
        }
    }
    else
    {
        if(of_property_read_u32(np, "huawei,gps_refclk", &gps_bcm->refclk)<0)
            {
            printk(KERN_ERR "[GPS] gps_refclk get failed!\n");
            ret = -1;
            goto err_refclk_get;
            }

    }
        gps_ref_clk = gps_bcm->refclk;
        printk(KERN_INFO "[GPS] ref clk is finished!\n");
    platform_set_drvdata(pdev, gps_bcm);
    g_gps_bcm = gps_bcm;
    return 0;

err_refclk_get:
err_free_refclk_sel:
    if (IS_ERR(gps_bcm->refclk_sel)) {
        clk_put(gps_bcm->refclk_sel);
    }
err_refclk_sel_get:
err_free_pinctrl:
    if (IS_ERR(gps_bcm->pctrl)) {
        pinctrl_put(gps_bcm->pctrl);
    }
err_pinctrl_get:
err_free_clk:
    if(IS_ERR(gps_bcm->clk)) {
        clk_put(gps_bcm->clk);
    }
err_clk_get:

err_free:
    kfree(gps_bcm);
    gps_bcm = NULL;
    g_gps_bcm = NULL;
    return ret;
}

static void K3_gps_bcm_shutdown(struct platform_device *pdev)
{
    GPS_BCM_INFO *gps_bcm = platform_get_drvdata(pdev);
    printk(KERN_INFO "[GPS] K3_gps_bcm_shutdown!\n");

    if (!gps_bcm) {
        printk(KERN_ERR "[GPS] gps_bcm is NULL,just return.\n");
        return;
    }

    platform_set_drvdata(pdev, NULL);
    kfree(gps_bcm);
    gps_bcm = NULL;
    g_gps_bcm = NULL;
    return;
}

static const struct of_device_id gps_power_match_table[] = {
    {
        .compatible = DTS_COMP_GPS_POWER_NAME,   // compatible must match with which defined in dts
        .data = NULL,
    },
    {
    },
};

MODULE_DEVICE_TABLE(of, gps_power_match_table);

static struct platform_driver k3_gps_bcm_driver = {
    .probe          = k3_gps_bcm_probe,
    .suspend        = NULL,
    .remove         = NULL,
    .shutdown       = K3_gps_bcm_shutdown,
    .driver = {
        .name = "huawei,gps_power",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(gps_power_match_table), // dts required code
    },
};

static int __init k3_gps_bcm_init(void)
{
#ifdef CONFIG_HWCONNECTIVITY
    //For OneTrack, we need check it's the right chip type or not.
    //If it's not the right chip type, don't init the driver
    if (!isMyConnectivityChip(CHIP_TYPE_BCM)) {
        printk(KERN_ERR "gps chip type is not match, skip driver init");
        return -EINVAL;
    } else {
        printk(KERN_INFO "gps chip type is matched with Broadcom, continue");
    }
#endif

    return platform_driver_register(&k3_gps_bcm_driver);
}

static void __exit k3_gps_bcm_exit(void)
{
    platform_driver_unregister(&k3_gps_bcm_driver);
}

int set_gps_ref_clk_enable_bcm433x(bool enable)
{
    int ret = 0;

    printk(KERN_INFO "[GPS] set_gps_ref_clk_enable(%d) \n",enable);
    if (IS_ERR(gps_ref_clk)) {
        printk(KERN_ERR "[GPS] ERROR: refclk is invalid! \n");
        return -1;
    }

    if (enable) {
        ret = clk_prepare_enable(gps_ref_clk);
        if (ret < 0) {
            printk(KERN_ERR "[GPS] ERROR: refclk enable failed! \n");
            return -1;
        }
    } else {
        clk_disable_unprepare(gps_ref_clk);
    }
    printk(KERN_INFO "[GPS] set_gps_ref_clk_enable finish \n");

    return 0;
}

module_init(k3_gps_bcm_init);
module_exit(k3_gps_bcm_exit);

MODULE_AUTHOR("DRIVER_AUTHOR");
MODULE_DESCRIPTION("GPS Boardcom 47511 driver");
MODULE_LICENSE("GPL");
