

#ifndef _DSM_PUB_H
#define _DSM_PUB_H
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/workqueue.h>
#include <linux/wait.h>

#define CLIENT_NAME_LEN						(32) //max client name length
#define DSM_MAX_DEVICE_NAME_LEN				(32) //max device name length
#define DSM_MAX_MODULE_NAME_LEN				(4)  //max module name length
#define DSM_MAX_IC_NAME_LEN					(4)  //max ic name length

/*dsm error no define*/
#define DSM_ERR_NO_ERROR					(0)
#define DSM_ERR_I2C_TIMEOUT					(1)

#define DSM_PMU_IRQ_ERROR_NO				(10000)
#define DSM_PMU_OCP_ERROR_NO_BASE			(10100)
#define DSM_CHARGER_ERROR_NO  				(10200)
#define DSM_PMU_COUL_ERROR_NO				(10300)
#define DSM_BATTERY_ERROR_NO				(10300)
#define DSM_BATTERY_DETECT_ERROR_NO			(10400)
#define DSM_TP_I2C_RW_ERROR_NO 				(20000)
#define DSM_TP_FWUPDATE_ERROR_NO 			(20001)
#define DSM_TP_RAWDATA_ERROR_NO             (20002)
#define DSM_TP_FW_CRC_ERROR_NO              (20003)
#define DSM_TP_DEV_STATUS_ERROR_NO          (20004)
#define DSM_TP_CHANGE_REPORT_ERROR_NO       (20006)
#define DSM_LCD_LDI_UNDERFLOW_NO            (20100)
#define DSM_LCD_TE_TIME_OUT_ERROR_NO        (20101)
#define DSM_TP_NO_IRQ_ERROR_NO				(20002)
#define DSM_LCD_LDI_UNDERFLOW_ERROR_NO     	(20100)
#define DSM_LCD_TE_TIME_OUT_ERROR_NO        (20101)
#define DSM_LCD_STATUS_ERROR_NO				(20102)
#define DSM_LCD_POWER_STATUS_ERROR_NO		(20103)
#define DSM_LCD_PWM_ERROR_NO				(20104)
#define DSM_LCD_BRIGHTNESS_ERROR_NO			(20105)
#define DSM_LCD_ESD_RECOVERY_NO				(20106)
#define DSM_CODEC_ERROR_NO     	 			(20200)
#define DSM_HIFI_ERROR_NO					(20201)
#define ANC_HS_OPEN_BOOST_ERR               (20202)
#define ANC_HS_CANCEL_WORK_BLOCK            (20203)
#define ANC_HS_CLOSE_BOOST_ERR              (20204)
#define ANC_HS_ADCH_READ_ERR                (20205)
#define ANC_HS_ADC_FULL_ERR                 (20206)
#define ANC_HS_MIC_WITH_GPIO_ERR            (20207)
#define ANC_HS_QUEUE_WORK_ERR               (20208)
#define DSM_SDIO_RW_ERROR_NO				(20300)
#define DSM_SHB_ERR_IPC_TIMEOUT				(20400)
#define DSM_SHB_ERR_IOM3_ACC				(20401)
#define DSM_SHB_ERR_IOM3_MAG				(20402)
#define DSM_SHB_ERR_IOM3_GYRO				(20403)
#define DSM_SHB_ERR_IOM3_LIGHT				(20404)
#define DSM_SHB_ERR_IOM3_OTHER				(20405)
#define DSM_SHB_ERR_IOM3_UNKNOW				(20406)
#define DSM_SHB_ERR_MAIN_INIT				(20407)
#define DSM_SHB_ERR_SERVER_INIT				(20408)
#define DSM_SHB_ERR_SERVER_UNINIT			(20409)
#define DSM_SHB_ERR_READ					(20410)
#define DSM_SHB_ERR_WRITE					(20411)
#define DSM_SHB_ERR_CMD						(20412)
#define DSM_SHB_ERR_CFG_DATA				(20413)
#define DSM_SHB_ERR_DYNLOAD					(20414)
#define DSM_SHB_ERR_I2C_DBG					(20415)
#define DSM_SHB_ERR_IOM3_STEP				(20416)
#define DSM_SHB_INVENSENSE_DMP_GET_FIFO_ALL (20417)
#define DSM_SHB_ERR_IOM3_ACC_EXCEED (20418)
#define DSM_SHB_ERR_INVENSENSE_DMP_FIFO_HANDLER_CONTROL_MUTEX (20419)
#define DSM_SHB_ERR_INVENSENSE_DMP_FIFO_HANDLER_DATA_MUTEX (20420)
#define DSM_SHB_ERR_INVENSENSE_DMP_ALL_ZERO_HEAD (20421)
#define DSM_KEY_ERROR_NO					(20500)
#define DSM_POWER_KEY_ERROR_NO				(20501)
#if defined (CONFIG_HUAWEI_DSM)
#define DSM_COMPASS_I2C_RW_ERROR_NO			(21100)
#define DSM_GSENSOR_I2C_RW_ERROR_NO			(21200)
#define DSM_ALS_PS_I2C_RW_ERROR_NO			(21300)
#endif

#define DSM_EXTISP_LOAD_FW_ERROR_NO			(20603)
#define DSM_FLASH_OPEN_SHOTR_ERROR_NO		(20607)
#define DSM_FLASH_HOT_DIE_ERROR_NO          (20608)
#define DSM_EXTISP_PQ_ERROR_NO              (20609)
#define DSM_EXTISP_I2C_ERROR_NO             (20610)
#define DSM_EXTISP_USPI_ERROR_NO            (20611)
#define DSM_EXTISP_VCM_ERROR_NO             (20612)
#define DSM_EXTISP_INTERRUPT_ERROR_NO       (20613)
#define DSM_EXTISP_COMMONFUNC_ERROR_NO      (20614)
#define DSM_EXTISP_A3_ERROR_NO              (20615)
#define DSM_EXTISP_PIPE_ERROR_NO            (20616)
#define DSM_EXTISP_RDN_ERROR_NO             (20617)
#define DSM_EXTISP_TXLM_ERROR_NO            (20618)
#define DSM_EXTISP_MTX_ERROR_NO             (20619)
#define DSM_EXTISP_MRX_ERROR_NO             (20620)
#define DSM_EXTISP_FEC0_ERROR_NO            (20621)
#define DSM_EXTISP_SENSOR_ERROR_NO          (20622)
#define DSM_EXTISP_FEC1_ERROR_NO            (20623)
#define DSM_EXTISP_CAP_ERROR_NO             (20624)
#define DSM_EXTISP_OTHER_ERROR_NO           (20625)
#define DSM_CSI_MIPI0_ERROR_NO              (20626)
#define DSM_CSI_MIPI1_ERROR_NO              (20627)
/************DMD NUMBER FOR WIFI BEGIN************/
/*************** 20700 ~ 20799 *******************/
#define DSM_WIFI_MODULE_INIT_ERROR          (20700)
#define DSM_WIFI_RESERVED                   (20701)
#define DSM_WIFI_CMD52_ERROR                (20702)
#define DSM_WIFI_CMD53_ERROR                (20703)
/*wifi open*/
#define DSM_WIFI_SDIO_RESET_COMM_ERROR      (20704)
#define DSM_WIFI_SDIO_PROBE_ATTACH_ERROR    (20705)
#define DSM_WIFI_SDIO_FIRMWARE_DL_ERROR     (20706)
#define DSM_WIFI_DHD_DEV_INIT_IOCTL_ERROR   (20707)
/*wifi scan*/
#define DSM_WIFI_WLC_SET_PASSIVE_SCAN_ERROR (20708)
#define DSM_WIFI_WLC_SCAN_ERROR             (20709)
#define DSM_WIFI_WLC_SET_SCANSUPPRESS_ERROR (20710)
#define DSM_WIFI_WLC_GET_CHANNEL_ERROR      (20711)
#define DSM_WIFI_WLC_SCAN_RESULTS_ERROR     (20712)
/*wifi connect*/
#define DSM_WIFI_WLC_DISASSOC_ERROR         (20713)
#define DSM_WIFI_WLC_SET_SSID_ERROR         (20714)
#define DSM_WIFI_SET_CIPHER_ERROR           (20715)
#define DSM_WIFI_SET_KEY_MGMT_ERROR         (20716)
#define DSM_WIFI_SET_SHAREDKEY_ERROR        (20717)
#define DSM_WIFI_OPEN_ERROR                 (20718)
/*************** 20700 ~ 20799 *******************/
/************DMD NUMBER FOR WIFI END**************/

#define DSM_USB_ERROR_NO					(20800)

#define DSM_USB_ERROR_NO					(20800)
#ifdef CONFIG_HUAWEI_FS_DSM
#define DSM_FS_EXT4_ERROR					(22000)
#define DSM_FS_EXT4_ERROR_INODE				(22001)
#define DSM_FS_EXT4_ERROR_FILE				(22002)
#define DSM_FS_EXT4_ERROR_READ_SUPER		(22003)
#define DSM_FS_EXT4_ERROR_READ_SUPER_SECOND (22004)
#define DSM_FS_EXT4_ERROR_WRITE_SUPER		(22005)
#endif
struct dsm_client_ops{
	int (*poll_state) (void);
	int (*dump_func) (int type, void *buff, int size);
};

struct dsm_dev{
	const char *name;
	const char *device_name;
	const char *ic_name;
	const char *module_name;
	struct dsm_client_ops *fops;
	size_t buff_size;
};

struct dsm_client{
	char *client_name;
	char *device_name;
	char *ic_name;
	char *module_name;
	int client_id;
	int error_no;
	unsigned long buff_flag;
	struct dsm_client_ops *cops;
	wait_queue_head_t waitq;
	size_t read_size;
	size_t used_size;
	size_t buff_size;
	u8 dump_buff[];
};

#ifdef CONFIG_HUAWEI_DSM
struct dsm_client *dsm_register_client (struct dsm_dev *dev);
int dsm_client_ocuppy(struct dsm_client *client);
int dsm_client_unocuppy(struct dsm_client *client);
int dsm_client_record(struct dsm_client *client, const char *fmt, ...);
int dsm_client_copy(struct dsm_client *client, void *src, int sz);
void dsm_client_notify(struct dsm_client *client, int error_no);
#else
static inline struct dsm_client *dsm_register_client (struct dsm_dev *dev)
{
	return NULL;
}
static inline int dsm_client_ocuppy(struct dsm_client *client)
{
	return 1;
}
static inline int dsm_client_unocuppy(struct dsm_client *client)
{
	return 0;
}

static inline int dsm_client_record(struct dsm_client *client, const char *fmt, ...)
{
	return 0;
}
static inline int dsm_client_copy(struct dsm_client *client, void *src, int sz)
{
	return 0;
}
static inline void dsm_client_notify(struct dsm_client *client, int error_no)
{
	return;
}
#endif

#endif
