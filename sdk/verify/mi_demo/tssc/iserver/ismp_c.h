#ifndef __ISMP_C_H__
#define __ISMP_C_H__
#ifdef __ISMP_CI_C__
#define ISMP_C_EXTERN
#else
#define ISMP_C_EXTERN extern
#endif

#include <stdbool.h>

typedef void (*MsgHandle)(void *dst, unsigned int nEvt, unsigned int wParam, unsigned int lParam);

ISMP_C_EXTERN void MgrPdrInit(void);

ISMP_C_EXTERN void MgrPdrDeinit(void);

ISMP_C_EXTERN void MgrPdrThreadStart(void);

ISMP_C_EXTERN int MgrPdrInstallSignalCb(void);

ISMP_C_EXTERN bool MgrPdrExit(void);

ISMP_C_EXTERN bool MgrPdrSetEnv(const char *name, const char *val);

ISMP_C_EXTERN char *MgrPdrGetEnv(const char *name);

ISMP_C_EXTERN bool MgrPdrRegistMsgHandle(MsgHandle handle);

ISMP_C_EXTERN bool MgrPdrUnregistMsgHandle();

ISMP_C_EXTERN bool MgrPdrEvtMgIsAlive(void);

ISMP_C_EXTERN bool FwMgrInit(void);

ISMP_C_EXTERN bool FwMgrDeinit(void);

ISMP_C_EXTERN bool FwMgrThreadStart(void);

ISMP_C_EXTERN bool FwMgrSetSysVer(const char *val);

ISMP_C_EXTERN bool FwMgrStartOTADownload(void);

ISMP_C_EXTERN bool FwMgrExitOTA(int reboot);

ISMP_C_EXTERN bool FwMgrRestartOTA(void);

ISMP_C_EXTERN bool FwMgrSkipOTAVersion(void);

ISMP_C_EXTERN bool FwMgrSetOTASilentMode(int silent);

ISMP_C_EXTERN bool FwMgrIsThreadStart(void);

ISMP_C_EXTERN bool FwMgrIsMgrAlive(void);

ISMP_C_EXTERN const char *FwMgrGetOTAModelName(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFileSize(void);

ISMP_C_EXTERN bool FwMgrGetOTAForceUpgrade(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFileBanner(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFileDate(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFileVersion(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFileDescription(void);

ISMP_C_EXTERN const char *FwMgrGetCurOTAFileVersion(void);

ISMP_C_EXTERN const char *FwMgrGetOTAFilePath(void);

ISMP_C_EXTERN int FwMgrSetExtStoragePath(const char *mnt_path, const char *bin_path);

ISMP_C_EXTERN int FwMgrCheckOTAFileUpdate(void);

ISMP_C_EXTERN long FwMgrGetOTACurSpeed(void);

ISMP_C_EXTERN bool FwMgrSetOTASpeedLimit(long speed);

ISMP_C_EXTERN int FwMgrSetLogLevel(int lv);


#endif
