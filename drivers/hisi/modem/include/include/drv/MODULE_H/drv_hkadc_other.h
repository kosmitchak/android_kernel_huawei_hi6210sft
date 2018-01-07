

#ifndef __DRV_HKADC_OTHER_H__
#define __DRV_HKADC_OTHER_H__

#ifndef _DRV_LLT_
typedef enum _KEY_EVENT
{
	KEY_WAKEUP = 0,
	KEY_WLAN,
	KEY_WPS,
	KEY_FACTORY_RESTORE,
	KEY_POWER_OFF,
	KEY_UPDATE,
	KEY_SSID,
    KEY_WPS_PROMPT = 7,
	KEY_WIFI_OFFLOAD = 15,
	KEY_EVENT_MAX
} KEY_EVENT;
#else

#ifndef KEY_WAKEUP
#define KEY_WAKEUP 0
#endif

#ifndef KEY_WLAN
#define KEY_WLAN 1
#endif

#ifndef KEY_FACTORY_RESTORE
#define KEY_FACTORY_RESTORE 3
#endif

#ifndef KEY_POWER_OFF
#define KEY_POWER_OFF 4
#endif

#ifndef KEY_UPDATE
#define KEY_UPDATE       5
#endif

#ifndef KEY_SSID
#define KEY_SSID         6
#endif

#ifndef KEY_WPS_PROMPT
#define KEY_WPS_PROMPT   7
#endif

#ifndef KEY_WIFI_OFFLOAD
#define KEY_WIFI_OFFLOAD 15
#endif

#ifndef KEY_EVENT_MAX
#define KEY_EVENT_MAX    16
#endif

#endif

#endif

