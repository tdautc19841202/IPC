#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/statvfs.h>

#include <sys/time.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <errno.h>

#include <pthread.h>
#include <signal.h>

#include "ismp_c.h"
#include "is_fw_evt.h"

#define OTA_VERSION_STR "ISOTA_FW_VERSION"
#define VERSION_STORAGE_PATH ("/tmp/"OTA_VERSION_STR)
FILE *FdMMC = NULL;

static bool OtaShouldExit;

int sys_ota(void)
{
	/* 系统OTA升级 */
    printf("sys_ota sys OTA Begin\n");
    printf("Ota file : %s\n", FwMgrGetOTAFilePath());

    return -1;
}

int sys_reboot(void)
{
	/* 系统重启 */
	unsetenv(OTA_VERSION_STR);
	return 0;
}

int sys_ota_write_version(const char *version)
{
	/*
	将当前版本号写入 flash/eeprom 等存储设备中
	*/
	char readback[20] = {0};
	rewind(FdMMC);
	fwrite(version, strlen(version)+1, 1, FdMMC);
	rewind(FdMMC);
	fread(readback, strlen(version)+1, 1,  FdMMC);
	printf("%s  ota version :%s to flash/eeprom done! readback:%s err:%s \n", __FUNCTION__, version, readback, strerror(errno));
	return 0;
}

char *sys_ota_read_version(void)
{
	/*
	从flash/eeprom等存储设备中读取当前设备版本�?
	*/
	char *readBuf = NULL;
	struct stat st;
	if(!stat(VERSION_STORAGE_PATH, &st)&& st.st_size){
		readBuf = (char*)malloc(st.st_size + 1);
		rewind(FdMMC);
		fread(readBuf, st.st_size+1, 1,  FdMMC);
		printf("%s read size:%ld ota version :%s from flash/eeprom done!\n", __FUNCTION__, st.st_size, readBuf);
	}
	return readBuf;
}

int sys_ota_version_setenv(char *version)
{
	/*启动IServer 服务进程前，需要将当前的版本号设置到环境变量里面，首次升级无版本号可不设置 */
	if(version){
    	setenv(OTA_VERSION_STR, version, true);
    	free(version);
    }
    return 0;
}

void HandleMessage(void *dst, unsigned int nEvt, unsigned int wParam, unsigned int lParam)
{
	if(nEvt == EV_NETWORK_UPDATE_STATUS){
		switch(wParam){
			case ISFWMGR_EVT_CHECK_NEWFIRMWARE_RESULT:{ /* 反馈检测云端OTA更新信息 */
				switch(lParam){
					case  ISFWMGR_RET_OK: {
						/* 此设备检测到有可更新版本 */
						/*前设备版本号      FwMgrGetCurOTAFileVersion()*/
						/*云端最新的版本号为: FwMgrGetOTAFileVersion()*/
						printf("Device Current Version:%s\n", FwMgrGetCurOTAFileVersion());
						printf("Available Version on Servers :%s\n", FwMgrGetOTAFileVersion());
            printf("Available Version Banner:%s\n", FwMgrGetOTAFileBanner());
            printf("Available Version Description :%s\n", FwMgrGetOTAFileDescription());
						/*IServer OTA 进程空闲状态，等待上层选择开始下载 (bStartDownload = true) 还是忽略此版 (bStartDownload = false) */
            //默认为下载
						bool bStartDownload = true;

            //可以根据Description设置下载文件名字、路径、临时文件名
            //第一个参数为临时文件夹，下载时会先把加密文件放到对应的地方，建议固定，第二个参数为文件最终放置路径
            //FwMgrSetExtStoragePath("./OTA","IS_TEST.bin");

						if(bStartDownload)
							FwMgrStartOTADownload();
						else{
							FwMgrSkipOTAVersion();
              /* 忽略此版本后 IServer OTA进程将会进入休眠*/
              //退出OTA
              OtaShouldExit = true;
            }
						break;
					}

					case ISFWMGR_RET_NO_CONNECT:{
						printf("NetWork is down! Can't connect to OTA Servers\n");
						/* 无法连接IServer OTA 服务器，请检测网络设*/
						/*  IServer OTA进程将会进入休眠状*/
						break;
					}

					case ISFWMGR_RET_NO_NEW_FIRMWARE:{
						/* 此设备在云端没有更新的版本，无升级需*/
						/*  IServer OTA进程将会进入休眠状*/
						printf("No Update!\n");

            //退出OTA
            OtaShouldExit = true;
						break;
					}

					case ISFWMGR_RET_DISK_FULL:{
						/* 此设备空间不足，无法开始OTA下载 */
						/*	IServer OTA进程将会进入休眠状*/
						printf("Disk Space Not Enough!\n");

            //退出OTA
            OtaShouldExit = true;
						break;
					}
				}
				break;
			}

			case ISFWMGR_EVT_DOWNLOAD_BIN_START:{ /* 开始下*/
        printf("Start Download!\n");
				switch(lParam){
					case ISFWMGR_RET_OPEN_FAIL:{
						printf("OTA File Create Failed, Because Permissions Error!\n");
						/*下载开始，创建文件失败，一般因为权限不足导*/
						/* IServer OTA 进程自动进入休眠状*/
            //退出OTA
            OtaShouldExit = true;
						break;
					}
				}
				break;
			}

			case ISFWMGR_EVT_DOWNLOAD_PROGRESS: { /* 下载状态*/
				/* lParam为下载的百分0 ~ 100，下载完成后还需要MD5校验 */
				printf("========>Downloading : %u%%\n", lParam);
				break;
			}

			case ISFWMGR_EVT_DOWNLOAD_COMPLETE: { /* 下载完成 */
				switch(lParam){
					case ISFWMGR_RET_SLICE_INFO_ERR:{
						/* 服务器上的OTA文件丢失或出错无法下*/
					  /* IServer OTA 进程会自动进入休        */
						break;
					}
					case ISFWMGR_RET_SLICE_DOWNLOAD_FAIL:{
						/* 分块下载完成，但是合并出*/
						/* IServer OTA 进程会检测出错分块，重新自动下载丢失或者出错分*/
						break;
					}
				}
				break;
			}

			case ISFWMGR_EVT_VERIFY_COMPLETE: { /* 下载后后自动MD5校验 */
				switch(lParam){
					case ISFWMGR_RET_OK: {
						printf(" MD5 CheckSum OK, Go OTA Upgrade!\n");
						/* 本地MD5校验值与服务器上的值相同，触发升级                                  */
						/* IServer OTA进程进入休眠状                                      */
						/***********************************************************/
						/*                      注意 ！！                              */
						/***********************************************************/
						/* 客户在校验成功后才可触发跟平台相关的升级动作                                  */
						/*	sys_ota: 系统的OTA升级流程								                     */
						/*	sys_reboot: 系统重启									                     */
						/*	sys_ota_write_version: 升级成功后，必须要更新版本号                  */
						/*													                               */
						/***********************************************************/
						sys_ota();
						sys_reboot();
            sys_ota_write_version(FwMgrGetOTAFileVersion());
            //退出OTA
            OtaShouldExit = true;
						break;

					}
					case ISFWMGR_RET_MD5SUM_ERR:{
						printf("MD5 CheckSum Error, Need Download Again!\n");
						/* 校验完成，但是校验失败，说明下载的文件和服务器上的不同，需要重新下*/
						break;
					}
				}
				break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	printf("ismp SIM run start ...\n");
	if(!access(VERSION_STORAGE_PATH, R_OK|W_OK)){
		FdMMC = fopen(VERSION_STORAGE_PATH, "rb+");
	}else{
		FdMMC = fopen(VERSION_STORAGE_PATH, "w+");
	}
	if(!FdMMC){
		printf("fopen %s failed!\n",VERSION_STORAGE_PATH);
		exit(0);
	}

  //step 1: 设置当前设备软件版本至OTA进程
  char *CurSysVer = sys_ota_read_version();
  FwMgrSetSysVer(CurSysVer);
  if(CurSysVer){
    free(CurSysVer);
    CurSysVer = NULL;
  }

  //step2: Init ManagerProvider and FwManager
	MgrPdrInit();
	FwMgrInit();
  //设置debug level,debug的时候使用
  FwMgrSetLogLevel(IS_DBGLV_VERBOSE);

  //step3: Register callback
  MgrPdrInstallSignalCb();
	MgrPdrRegistMsgHandle(HandleMessage);

  //step4: OTA Thread
  FwMgrThreadStart();
  MgrPdrThreadStart();
  OtaShouldExit = false;

  //此时后台跑OTA，对应的事件处理再callback函数里

	while(1){
		usleep(5*1000*1000);

    //FIXME checkalive 合并
		bool FwMgrAlive = FwMgrIsMgrAlive();
		bool EvtMgrAlive = MgrPdrEvtMgIsAlive();
		printf("FwMgrAlive:%u MgrPdrAlive:%u\n",FwMgrAlive, EvtMgrAlive);

		if(!FwMgrAlive){
			if(!EvtMgrAlive)
				break;
		}else if(OtaShouldExit){
        //正常退出OTA
        FwMgrExitOTA(true);
        MgrPdrExit();
    }else {
      //获取当前版本
			const char *ver = FwMgrGetCurOTAFileVersion();
			if(ver)
				printf("FwMgr alive 5s! VERSION:%s\n",ver);
		}
	}

  //step5 : Deinit ManagerProvider and FwManager
  MgrPdrDeinit();
	FwMgrDeinit();
	printf("Exit Main!\n");

	fclose(FdMMC);
	return 0;
}
