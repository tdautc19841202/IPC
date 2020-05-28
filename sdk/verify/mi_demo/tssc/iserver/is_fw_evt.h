#ifndef ISFWMGR_EVT_DEF_H
#define ISFWMGR_EVT_DEF_H

typedef enum {
	IS_DBGLV_UNKNOWN = 0,
	IS_DBGLV_DEFAULT,
	IS_DBGLV_VERBOSE,
	IS_DBGLV_DEBUG,
	IS_DBGLV_INFO,
	IS_DBGLV_WARN,
	IS_DBGLV_ERROR,
	IS_DBGLV_FATAL,
	IS_DBGLV_SILENT,
} IS_DBGLV_LEVEL;

typedef enum {
	///Network upgrade
	EV_NETWORK_UPDATE_STATUS,
	EV_NETWORK_STATUS,
	EV_NETWORK_UPDATE_DOWNLOAD_CONFIRM_DIALOG,
} EN_ISFWNET_RET;

typedef enum {
	ISFWMGR_RET_OK,
	ISFWMGR_RET_NO_UPGRADE_NEEDED,
	ISFWMGR_RET_NOT_ALIVE,
	ISFWMGR_RET_INVALID,
	ISFWMGR_RET_PTHREAD_FAIL,
	ISFWMGR_RET_DOWNLOAD_FAIL,
	ISFWMGR_RET_CURL_FAIL,
	ISFWMGR_RET_OPEN_FAIL,
	ISFWMGR_RET_READ_FAIL,
	ISFWMGR_RET_WRITE_FAIL,
	ISFWMGR_RET_NOT_FOUND,
	ISFWMGR_RET_NO_CONNECT,
	ISFWMGR_RET_NO_OPERATION,
	ISFWMGR_RET_NO_NEW_FIRMWARE,
	ISFWMGR_RET_ERROR_MODEL,
	ISFWMGR_RET_FATAL,
	ISFWMGR_RET_DOWNLOADING,
	ISFWMGR_RET_DISK_FULL,
	ISFWMGR_RET_MD5SUM_ERR,
	ISFWMGR_RET_SLICE_DOWNLOAD_FAIL,
	ISFWMGR_RET_SLICE_INFO_ERR,
	ISFWMGR_RET_EXT_DISK_FULL,
	ISFWMGR_RET_EXT_DISK_ERROR,
	ISFWMGR_RET_DISK_ERROR,
} EN_ISFWMGR_RET;

typedef enum {
	ISFWMGR_EVT_DOWNLOAD_PROGRESS = 0x1000,
	ISFWMGR_EVT_DOWNLOAD_COMPLETE,
	ISFWMGR_EVT_DOWNLOAD_BIN_START,
	ISFWMGR_EVT_VERIFY_PROGRESS,
	ISFWMGR_EVT_VERIFY_COMPLETE,
	ISFWMGR_EVT_VERIFY_BIN_START,
	ISFWMGR_EVT_CHECK_NEWFIRMWARE_RESULT,
	ISFWMGR_EVT_DOWNLOAD_DECRYPT_PROGRESS,
	ISFWMGR_EVT_DOWNLOAD_ENCRYPT_PROGRESS,
	ISFWMGR_EVT_DOWNLOAD_ERROR_INFO,
	ISFWMGR_EVT_INSTALL_EXT_STORAGE,
} EN_ISFWMGR_EVT;

#endif

