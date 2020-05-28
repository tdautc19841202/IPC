#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>

#ifndef __packed
#define __packed __attribute((packed))
#endif
#include <ubi-user.h>

typedef char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef int s32;

#define NOTIFY_SZ (2048)

#define OTA_BLK_CNT 32

typedef struct
{
    u32 u32FileSize;
    s8 as8DstFilePath[64];
    u32 u32DstFileSize;
    u32 u32FileOpt;
    u32 u32FileCheckSum;
}SS_OTA_BlockConfig;
typedef struct
{
    u32 u32ShFileSize;
    u32 u32DataCheckSum;
}SS_OTA_ShellData;

typedef struct
{
    u32 u32HeaderCheckSum;
    u32 u32BlkCount;
    SS_OTA_ShellData stShellStart;
    SS_OTA_ShellData stShellEnd;
    SS_OTA_BlockConfig stOtaBlkConfig[OTA_BLK_CNT];
}SS_OTA_BinHeader;
typedef s32 (*_SS_OTA_FuncOtaFileOpen)(const s8 *pData);
typedef s32 (*_SS_OTA_FuncOtaFileClose)(s32 s32Fd);

typedef void (*_SS_OTA_FuncOtaProcess)(const s8 *pInfo, u32 intPrecent);
static s32 _FileRead(s32 s32Fd, void *pBuf, s32 s32Size)
{
    s32 s32ReadCnt = 0;
    s32 s32Ret = 0;
    do
    {
        s32Ret = read(s32Fd, pBuf, s32Size - s32ReadCnt);
        if (s32Ret < 0)
        {
            return -1;
        }
        if (s32Ret == 0)
        {
            break;
        }
        s32ReadCnt += s32Ret;
        pBuf += s32Ret;
    }while(s32ReadCnt < s32Size);

    return s32ReadCnt;
}
static s32 _FileWrite(s32 s32Fd, void *pBuf, s32 s32Size)
{
    s32 s32WriteCnt = 0;
    s32 s32Ret = 0;
    do
    {
        s32Ret = write(s32Fd, pBuf, s32Size - s32WriteCnt);
        if (s32Ret < 0)
        {
            perror("write");
            return -1;
        }
        if (s32Ret == 0)
        {
            break;
        }
        s32WriteCnt += s32Ret;
        pBuf += s32Ret;
    }while(s32WriteCnt < s32Size);

    return s32WriteCnt;
}
static void _NotifyPrecentInfo(const s8 *pu8Info, u32 u32Precent)
{
    fprintf(stderr, "%s(%d%%)Completed\n", pu8Info, u32Precent);
}

static u32 _CheckSum(u32 u32Sum, u8 *pu8Data, u16 u16ByteCnt)
{
    while(u16ByteCnt--)
        u32Sum += *pu8Data++;

    return u32Sum;
}
static s32 _OpenFile(const s8 *pOpenData)
{
    s32 s32Fd = open(pOpenData, O_RDONLY);
    if (s32Fd < 0)
    {
        perror("open");
    
        return -1;
    }

    return s32Fd;
}
static s32 _CloseFile(s32 s32Fd)
{
    return close(s32Fd);
}
static s32 gs32FdTmp = 0;
static pid_t fPid;
static s32 _OpenCompressedFile(const s8 *pOpenData)
{
    s32 s32Fd[2];

    if (pipe(s32Fd) < 0)
    {
        perror("pipe");
    
        return -1;
    }
    gs32FdTmp = dup(STDOUT_FILENO);
    if (gs32FdTmp < 0)
    {
        perror("dup");
    
        return -1;
    }
    if (dup2(s32Fd[1], STDOUT_FILENO) < 0)
    {
        perror("dup2");
        close(s32Fd[0]);
        close(s32Fd[1]);
        close(gs32FdTmp);
    
        return -1;
    }
    fPid = fork();
    switch (fPid)
    {
        case -1:
        {
            perror("error in fork!");
            close(s32Fd[0]);
            close(s32Fd[1]);
            close(gs32FdTmp);
            
            return -1;
        }
        break;
        case 0:
        {
            close(s32Fd[0]);
            if(execlp("gunzip", "gunzip", "-c", optarg, NULL) < 0)  
            {
                perror("execlp error!");  
                close(s32Fd[1]);
                close(gs32FdTmp);
            
                exit(-1) ;  
            }
            close(s32Fd[1]);
            close(gs32FdTmp);
            exit(0);
        }
        break;
        default:
            break;
    }
    close(s32Fd[1]);

    return s32Fd[0];
}
static s32 _CloseCompressedFile(s32 s32Fd)
{
    
    kill(fPid, SIGKILL);
    waitpid(fPid, NULL, 0);
    if (dup2(gs32FdTmp, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        close(s32Fd);
        close(gs32FdTmp);
    
        return -1;
    }
    close(s32Fd);
    close(gs32FdTmp);
    return 0;
}
int SS_OTA_VerifyBlock(_SS_OTA_FuncOtaFileOpen fpFileOpen, const s8 *pOpenData, _SS_OTA_FuncOtaFileClose fpFileClose, _SS_OTA_FuncOtaProcess fpProcess)
{
    s32 s32FdFrom = 0;
    s32 s32Ret = 0;
    u8 pu8Buf[128];
    u32 u32SizeGap = 0;
    u32 u32ReadBytes = 0;
    s8 ps8NotifyInfo[50];
    u32 u32NotifyCnt = 0;
    u32 u32FileCheckSum = 0;
    SS_OTA_BinHeader stHeader;
    u8 i = 0;

    if (!fpFileOpen)
        return -1;
    if (!fpFileClose)
        return -1;
    if (!fpProcess)
        return -1;
    s32FdFrom = fpFileOpen(pOpenData);
    if (s32FdFrom < 0)
        return -1;
    memset(&stHeader, 0, sizeof(SS_OTA_BinHeader));
    s32Ret =_FileRead(s32FdFrom, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
    if (s32Ret !=  sizeof(SS_OTA_BinHeader))
    {
        fprintf(stderr, "File header read error!\n");
        s32Ret = -1;

        goto ERR;
    }
    if (_CheckSum(0, (u8 *)&stHeader + 8, sizeof(SS_OTA_BinHeader) - 8) != stHeader.u32HeaderCheckSum)
    {
        fprintf(stderr, "Header checksum error!\n");
        s32Ret = -1;

        goto ERR;
    }
    fprintf(stderr, "Checksum 0x%x blkcount %d\n", stHeader.u32HeaderCheckSum, stHeader.u32BlkCount);
    u32SizeGap = stHeader.stShellStart.u32ShFileSize % 128;
    if (u32SizeGap)
    {
        u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
        u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
    }
    while (u32ReadBytes < stHeader.stShellStart.u32ShFileSize)
    {
        s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
        u32ReadBytes += s32Ret;
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
    }
    if (u32FileCheckSum != stHeader.stShellStart.u32DataCheckSum)
    {
        fprintf(stderr, "Shell start data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellStart.u32DataCheckSum, u32FileCheckSum);
        s32Ret = -1;

        goto ERR;
    }
    u32FileCheckSum = 0;
    u32ReadBytes = 0;
    u32SizeGap = stHeader.stShellEnd.u32ShFileSize % 128;
    if (u32SizeGap)
    {
        u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
        u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
    }
    while (u32ReadBytes < stHeader.stShellEnd.u32ShFileSize)
    {
        s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
        u32ReadBytes += s32Ret;
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
    };
    if (u32FileCheckSum != stHeader.stShellEnd.u32DataCheckSum)
    {
        fprintf(stderr, "Shell start data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellEnd.u32DataCheckSum, u32FileCheckSum);
        s32Ret = -1;

        goto ERR;
    }
    for (i = 0; i < stHeader.u32BlkCount; i++)
    {
        /********Verifying*********/
        u32FileCheckSum = 0;
        u32ReadBytes = 0;
        u32NotifyCnt = 0;
        sprintf(ps8NotifyInfo, "Verifying Block %d", i);
        fpProcess(ps8NotifyInfo, 0);
        u32SizeGap = stHeader.stOtaBlkConfig[i].u32FileSize % 128;
        if (u32SizeGap)
        {
            u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
            u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
        }
        while (u32ReadBytes < stHeader.stOtaBlkConfig[i].u32FileSize)
        {
            s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
            u32ReadBytes += s32Ret;
            u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
            if ((u32NotifyCnt % NOTIFY_SZ) == 0)
            {
                fpProcess(ps8NotifyInfo, u32ReadBytes * 100 / stHeader.stOtaBlkConfig[i].u32FileSize);
            }
            u32NotifyCnt++;
        };
        fpProcess(ps8NotifyInfo, 100);
        if (u32FileCheckSum != stHeader.stOtaBlkConfig[i].u32FileCheckSum)
        {
            fprintf(stderr, "Block %d checksum error! head 0x%x cal 0x%x\n", i, stHeader.stOtaBlkConfig[i].u32FileCheckSum, u32FileCheckSum);
            s32Ret = -1;
            
            goto ERR;
        }
        else
            fprintf(stderr, "blk %d head 0x%x cal 0x%x\n", i, stHeader.stOtaBlkConfig[i].u32FileCheckSum, u32FileCheckSum);
    }

ERR:
    fpFileClose(s32FdFrom);

    return s32Ret;

}
int SS_OTA_UpdateBlock(_SS_OTA_FuncOtaFileOpen fpFileOpen, const s8 *pOpenData, _SS_OTA_FuncOtaFileClose fpFileClose, _SS_OTA_FuncOtaProcess fpProcess)
{
    s32 s32FdFrom = 0;
    s32 s32FdTo = 0;
    s32 s32Ret = 0;
    u8 pu8Buf[128];
    u32 u32SizeGap = 0;
    u32 u32WriteBytes = 0;
    u32 u32ReadBytes = 0;
    s8 ps8NotifyInfo[50];
    u32 u32NotifyCnt = 0;
    SS_OTA_BinHeader stHeader;
    u32 u32FileCheckSum = 0;
    u64 u64UbiVolSize = 0;
    u8 i = 0;

    if (!fpFileOpen)
        return -1;
    if (!fpFileClose)
        return -1;
    if (!fpProcess)
        return -1;
    s32FdFrom = fpFileOpen(pOpenData);
    if (s32FdFrom < 0)
        return -1;
    memset(&stHeader, 0, sizeof(SS_OTA_BinHeader));
    s32Ret =_FileRead(s32FdFrom, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
    if (s32Ret !=  sizeof(SS_OTA_BinHeader))
    {
        fprintf(stderr, "File header read error!\n");
        fpFileClose(s32FdFrom);

        return -1;
    }
    if (_CheckSum(0, (u8 *)&stHeader + 8, sizeof(SS_OTA_BinHeader) - 8) != stHeader.u32HeaderCheckSum)
    {
        fprintf(stderr, "Header checksum error!\n");
        fpFileClose(s32FdFrom);

        return -1;
    }
    fprintf(stderr, "Checksum %d blkcount %d\n", stHeader.u32HeaderCheckSum, stHeader.u32BlkCount);
    u32SizeGap = stHeader.stShellStart.u32ShFileSize % 128;
    if (u32SizeGap)
    {
        u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
        u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
    }
    while (u32ReadBytes < stHeader.stShellStart.u32ShFileSize)
    {
        s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
        u32ReadBytes += s32Ret;
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
    };
    if (u32FileCheckSum != stHeader.stShellStart.u32DataCheckSum)
    {
        fprintf(stderr, "Shell start data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellStart.u32DataCheckSum, u32FileCheckSum);
        fpFileClose(s32FdFrom);

        return -1;
    }
    u32FileCheckSum = 0;
    u32ReadBytes = 0;
    u32SizeGap = stHeader.stShellEnd.u32ShFileSize % 128;
    if (u32SizeGap)
    {
        u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
        u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
    }
    while (u32ReadBytes < stHeader.stShellEnd.u32ShFileSize)
    {
        s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
        u32ReadBytes += s32Ret;
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
    };
    if (u32FileCheckSum != stHeader.stShellEnd.u32DataCheckSum)
    {
        fprintf(stderr, "Shell end data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellEnd.u32DataCheckSum, u32FileCheckSum);
        fpFileClose(s32FdFrom);

        return -1;
    }
    for (i = 0; i < stHeader.u32BlkCount; i++)
    {
        if (stHeader.stOtaBlkConfig[i].u32FileOpt == 2)
        {
            s32FdTo = open(stHeader.stOtaBlkConfig[i].as8DstFilePath, O_WRONLY | O_TRUNC);
            if (s32FdTo < 0)
            {
                perror("open");
                fpFileClose(s32FdFrom);

                return -1;
            }
        }
        else
        {
            s32FdTo = open(stHeader.stOtaBlkConfig[i].as8DstFilePath, O_WRONLY);
            if (s32FdTo < 0)
            {
                perror("open");
                fpFileClose(s32FdFrom);

                return -1;
            }
            if (stHeader.stOtaBlkConfig[i].u32FileOpt == 1)
            {
                u64UbiVolSize = stHeader.stOtaBlkConfig[i].u32FileSize;
                ioctl(s32FdTo, UBI_IOCVOLUP, &u64UbiVolSize);
            }
            else
            {
                /********Erasing*********/
                u32WriteBytes = 0;
                u32ReadBytes = 0;
                u32NotifyCnt = 0;
                sprintf(ps8NotifyInfo, "Erasing Block %d", i);
                fpProcess(ps8NotifyInfo, 0);
                memset(pu8Buf, 0xFF, 128);
                u32SizeGap = stHeader.stOtaBlkConfig[i].u32DstFileSize % 128;
                if (u32SizeGap)
                {
                    u32WriteBytes += _FileWrite(s32FdTo, pu8Buf, u32SizeGap);
                }
                while (u32WriteBytes < stHeader.stOtaBlkConfig[i].u32DstFileSize)
                {
                    u32WriteBytes += _FileWrite(s32FdTo, pu8Buf, 128);
                    if ((u32NotifyCnt % NOTIFY_SZ) == 0)
                    {
                        fpProcess(ps8NotifyInfo, u32WriteBytes * 100 / stHeader.stOtaBlkConfig[i].u32DstFileSize);
                    }
                    u32NotifyCnt++;
                };
                fpProcess(ps8NotifyInfo, 100);
                fprintf(stderr, "######Erase 0x%x bytes#######\n", u32WriteBytes);
                lseek(s32FdTo, 0, SEEK_SET);
            }
        }
        /********Updating*********/
        u32WriteBytes = 0;
        u32ReadBytes = 0;
        u32NotifyCnt = 0;
        sprintf(ps8NotifyInfo, "Updating block %d", i);
        fpProcess(ps8NotifyInfo, 0);
        u32SizeGap = stHeader.stOtaBlkConfig[i].u32FileSize % 128;
        if (u32SizeGap)
        {
            u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
            u32WriteBytes += _FileWrite(s32FdTo, pu8Buf, u32SizeGap);
        }
        while (u32WriteBytes < stHeader.stOtaBlkConfig[i].u32FileSize)
        {
            u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, 128);
            u32WriteBytes += _FileWrite(s32FdTo, pu8Buf, 128);
            if ((u32NotifyCnt % NOTIFY_SZ) == 0)
            {
                fpProcess(ps8NotifyInfo, u32WriteBytes * 100 / stHeader.stOtaBlkConfig[i].u32FileSize);
            }
            u32NotifyCnt++;
        }
        close(s32FdTo);
        fpProcess(ps8NotifyInfo, 100);
        fprintf(stderr, "######From blk%d dump 0x%x bytes to %s #######\n", i, u32ReadBytes, stHeader.stOtaBlkConfig[i].as8DstFilePath);
    }
    fpFileClose(s32FdFrom);

    return 0;
}
int SS_OTA_StartScripts(_SS_OTA_FuncOtaFileOpen fpFileOpen, const s8 *pOpenData, _SS_OTA_FuncOtaFileClose fpFileClose)
{
    s32 s32Ret = 0;
    s32 s32FdFrom = 0;
    u8 *pScripts = NULL;
    u8 *pDataTmp = NULL;

    SS_OTA_BinHeader stHeader;
    u32 u32FileCheckSum = 0;
    FILE *pFile = NULL;
    u32 u32WriteBytes = 0;

    if (!fpFileOpen)
        return -1;
    if (!fpFileClose)
        return -1;
    s32FdFrom = fpFileOpen(pOpenData);
    if (s32FdFrom < 0)
        return -1;
    memset(&stHeader, 0, sizeof(SS_OTA_BinHeader));
    s32Ret =_FileRead(s32FdFrom, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
    if (s32Ret !=  sizeof(SS_OTA_BinHeader))
    {
        fprintf(stderr, "File header read error!\n");
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    if (_CheckSum(0, (u8 *)&stHeader + 8, sizeof(SS_OTA_BinHeader) - 8) != stHeader.u32HeaderCheckSum)
    {
        fprintf(stderr, "Header checksum error!\n");
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    if (stHeader.stShellStart.u32ShFileSize == 0)
    {
        fprintf(stderr, "No start scripts!\n");
        s32Ret = 0;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    pScripts = (u8 *)malloc(stHeader.stShellStart.u32ShFileSize);
    if (pScripts == NULL)
    {
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    if (_FileRead(s32FdFrom, pScripts, stHeader.stShellStart.u32ShFileSize) < 0)
    {
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR1;
    }
    u32FileCheckSum = _CheckSum(u32FileCheckSum, pScripts, stHeader.stShellStart.u32ShFileSize);
    if (u32FileCheckSum != stHeader.stShellStart.u32DataCheckSum)
    {

        fprintf(stderr, "Shell start data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellStart.u32DataCheckSum, u32FileCheckSum);
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR1;
    }
    fpFileClose(s32FdFrom);
    pFile = popen("sh", "w");
    if (!pFile)
    {
        s32Ret = -1;

        goto ERR1;
    }
    pDataTmp = pScripts;
    do
    {
        s32Ret = fwrite(pDataTmp, 1, stHeader.stShellStart.u32ShFileSize, pFile);
        if (s32Ret < 0)
        {
            perror("fwrite");
            s32Ret = -1;
            
            goto ERR2;
        }
        if (s32Ret == 0)
        {
            break;
        }
        u32WriteBytes += s32Ret;
        pDataTmp += s32Ret;
    }while(u32WriteBytes < stHeader.stShellStart.u32ShFileSize);

ERR2:
    pclose(pFile);
ERR1:
    free(pScripts);
ERR0:

    return s32Ret;
}
int SS_OTA_EndScripts(_SS_OTA_FuncOtaFileOpen fpFileOpen, const s8 *pOpenData, _SS_OTA_FuncOtaFileClose fpFileClose, u32 u32Status)
{
    s32 s32Ret = 0;
    s32 s32FdFrom = 0;
    u32 u32SizeGap = 0;
    u8 *pScripts = NULL;
    u8 *pDataTmp = NULL;
    SS_OTA_BinHeader stHeader;
    u32 u32FileCheckSum = 0;
    FILE *pFile = NULL;
    u32 u32WriteBytes = 0;
    u32 u32ReadBytes = 0;
    u8 pu8Buf[128];

    if (!fpFileOpen)
        return -1;
    if (!fpFileClose)
        return -1;
    s32FdFrom = fpFileOpen(pOpenData);
    if (s32FdFrom < 0)
        return -1;
    memset(&stHeader, 0, sizeof(SS_OTA_BinHeader));
    s32Ret =_FileRead(s32FdFrom, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
    if (s32Ret !=  sizeof(SS_OTA_BinHeader))
    {
        fprintf(stderr, "File header read error!\n");
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    if (_CheckSum(0, (u8 *)&stHeader + 8, sizeof(SS_OTA_BinHeader) - 8) != stHeader.u32HeaderCheckSum)
    {
        fprintf(stderr, "Header checksum error!\n");
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    u32SizeGap = stHeader.stShellStart.u32ShFileSize % 128;
    if (u32SizeGap)
    {
        u32ReadBytes += _FileRead(s32FdFrom, pu8Buf, u32SizeGap);
        u32FileCheckSum = _CheckSum(u32FileCheckSum, (u8 *)pu8Buf, u32SizeGap);
    }
    while (u32ReadBytes < stHeader.stShellStart.u32ShFileSize)
    {
        s32Ret = _FileRead(s32FdFrom, pu8Buf, 128);
        u32ReadBytes += s32Ret;
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pu8Buf, s32Ret);
    }
    if (u32FileCheckSum != stHeader.stShellStart.u32DataCheckSum)
    {
        fprintf(stderr, "Shell start data checksum error! head 0x%x cal 0x%x\n", stHeader.stShellStart.u32DataCheckSum, u32FileCheckSum);
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    memset(pu8Buf, 0, 128);
    sprintf((s8 *)pu8Buf, "OTA_STATUS=%d\necho OTA UPDATA STATUS: ${OTA_STATUS}\n", u32Status);
    pScripts = (u8 *)malloc(stHeader.stShellEnd.u32ShFileSize + strlen((s8 *)pu8Buf) + 1);
    if (pScripts == NULL)
    {
        s32Ret = -1;
        fpFileClose(s32FdFrom);

        goto ERR0;
    }
    strcpy((s8 *)pScripts, (s8 *)pu8Buf);
    pDataTmp = pScripts + strlen((s8 *)pu8Buf) + 1;
    if (stHeader.stShellEnd.u32ShFileSize)
    {
        u32FileCheckSum = 0;
        if (_FileRead(s32FdFrom, pDataTmp, stHeader.stShellEnd.u32ShFileSize) < 0)
        {
            s32Ret = -1;
            fpFileClose(s32FdFrom);
        
            goto ERR1;
        }
        u32FileCheckSum = _CheckSum(u32FileCheckSum, pDataTmp, stHeader.stShellEnd.u32ShFileSize);
        if (u32FileCheckSum != stHeader.stShellEnd.u32DataCheckSum)
        {
        
            fprintf(stderr, "Shell start data checksum error!\n");
            s32Ret = -1;
            fpFileClose(s32FdFrom);
        
            goto ERR1;
        }
    }
    fpFileClose(s32FdFrom);
    pFile = popen("sh", "w");
    if (!pFile)
    {
        s32Ret = -1;

        goto ERR1;
    }
    pDataTmp = pScripts;
    do
    {
        s32Ret = fwrite(pDataTmp, 1, stHeader.stShellEnd.u32ShFileSize + strlen((s8 *)pu8Buf) + 1, pFile);
        if (s32Ret < 0)
        {
            perror("fwrite");
            s32Ret = -1;
            
            goto ERR2;
        }
        if (s32Ret == 0)
        {
            break;
        }
        u32WriteBytes += s32Ret;
        pDataTmp += s32Ret;
    }while(u32WriteBytes < stHeader.stShellEnd.u32ShFileSize);

ERR2:
    pclose(pFile);
ERR1:
    free(pScripts);
ERR0:
    return s32Ret;
}
int main(int argc, char **argv)
{
    s32 s32Ret = 0;

    if (argc != 3)
    {
        fprintf(stderr, "usage: ./prog_ota -x xxx.gz, ./prog_ota -r xxx\n");
        return -1;
    }
    while ((s32Ret = getopt(argc, argv, "x:r:")) != -1)
    {        
        switch (s32Ret)        
        {            
            case 'x':
            {
                if (SS_OTA_VerifyBlock(_OpenCompressedFile, optarg, _CloseCompressedFile, _NotifyPrecentInfo) < 0)
                {
                    perror("SS_OTA_VerifyBlock");
                    return -1;
                }
                if (SS_OTA_StartScripts(_OpenCompressedFile, optarg, _CloseCompressedFile) < 0)
                {
                    perror("SS_OTA_StartScripts");
                    return -1;
                }
                s32Ret = SS_OTA_UpdateBlock(_OpenCompressedFile, optarg, _CloseCompressedFile, _NotifyPrecentInfo);
                s32Ret = SS_OTA_EndScripts(_OpenCompressedFile, optarg, _CloseCompressedFile, s32Ret);
            }
            break;
            case 'r':
            {
                if (SS_OTA_VerifyBlock(_OpenFile, optarg, _CloseFile, _NotifyPrecentInfo) < 0)
                {
                    perror("SS_OTA_VerifyBlock");
                    return -1;
                }
                if (SS_OTA_StartScripts(_OpenFile, optarg, _CloseFile) < 0)
                {
                    perror("SS_OTA_StartScripts");
                    return -1;
                }
                s32Ret = SS_OTA_UpdateBlock(_OpenFile, optarg, _CloseFile, _NotifyPrecentInfo);
                s32Ret = SS_OTA_EndScripts(_OpenFile, optarg, _CloseFile, s32Ret);
                if (s32Ret < 0)
                {
                    return -1;
                }
            }
            break;
            default:
                return -1;
        }
    }
 
    return s32Ret;
}
