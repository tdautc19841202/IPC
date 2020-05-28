#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

typedef char s8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

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

static u32 _CheckSum(u32 u32Sum, u8 *pu8Data, u16 u16ByteCnt)
{
    while(u16ByteCnt--)
        u32Sum += *pu8Data++;

    return u32Sum;
}
static u32 _EasyAtoi(const char *pStr)
{
    u32 intStrLen = strlen(pStr);
    u16 bUseHex = 0;
    u32 intRetNumber = 0;

    if (pStr == NULL)
    {
        return 0xFFFFFFFF;
    }

    if (intStrLen > 2)
    {
        if (pStr[0] == '0' &&(pStr[1] == 'X' || pStr[1] == 'x'))
        {
            bUseHex = 1;
            pStr += 2;
        }
    }
    if (bUseHex == 1)
    {
        for (int i = 0; i < intStrLen - 2; i++)
        {
            if ((pStr[i] > '9' || pStr[i] < '0')    \
                && (pStr[i] > 'f' || pStr[i] < 'a') \
                && (pStr[i] > 'F' || pStr[i] < 'A'))
            {
                return 0xFFFFFFFF;
            }
        }
        sscanf(pStr, "%x", &intRetNumber);
    }
    else
    {
        for (int i = 0; i < intStrLen; i++)
        {
            if (pStr[i] > '9' || pStr[i] < '0')
            {
                return 0xFFFFFFFF;
            }
        }
        intRetNumber =  atoi(pStr);
    }
    return intRetNumber;
}
static s32 _FileSize(const s8* ps8Name)
{
    struct stat stStatBuf;
    if(stat(ps8Name, &stStatBuf)==0)
        return stStatBuf.st_size;
    return -1;
}
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
int main(int argc, char **argv)
{
    s32 s32Opt = 0;
    u8 bCreateHeader = 0;
    u8 bAddStartScript = 0;
    u8 bAddEndScript = 0;
    u8 bInputHeader = 0;
    s32 s32Fd = 0;
    s32 s32SrcFd = 0;
    s8 as8SrcFilePath[256];
    s8 as8DstFilePath[64];
    u32 u32DstFileSize = 0;
    u32 u32FileOperation = 0;
    SS_OTA_BinHeader stHeader;
    s8 as8OtaBinPath[256];
    s8 as8StartScriptPath[256];
    s8 as8EndScriptPath[256];
    s32 s32Ret = 0;
    u8 pu8Buf[128];
    s32 s32ReadCnt = 0;
    u32 u32CheckSum = 0;

    /*
        s: source file path
        d: dst file path
        t: dst file size
        p: file operation
        c: creat header
        a: add header
    */
    memset(as8SrcFilePath, 0, 256);
    memset(as8DstFilePath, 0, 64);
    memset(as8OtaBinPath, 0, 256);
    memset(as8StartScriptPath, 0, 256);
    memset(as8EndScriptPath, 0, 256);
    memset(&stHeader, 0, sizeof(SS_OTA_BinHeader));
    while ((s32Opt = getopt(argc, argv, "s:d:t:p:a:c:b:e:")) != -1)
    {
        switch (s32Opt)
        {
            case 's':
            {
                if (strlen(optarg) != 0)
                {
                    strcpy(as8SrcFilePath, optarg);
                }
            }
            break;
            case 'd':
            {
                if (strlen(optarg) != 0)
                {
                    strcpy(as8DstFilePath, optarg);   
                }
            }
            break;
            case 't':
            {
                u32DstFileSize = _EasyAtoi(optarg);
            }
            break;
            case 'p':
            {
                u32FileOperation = _EasyAtoi(optarg);
            }
            break;
            case 'a':
            {
                bInputHeader = 1;
                if (strlen(optarg) != 0)
                {
                    strcpy(as8OtaBinPath, optarg);   
                }
            }
            break;
            case 'c':
            {
                bCreateHeader = 1;
                if (strlen(optarg) != 0)
                {
                    strcpy(as8OtaBinPath, optarg);   
                }
            }
            break;
            case 'b':
            {
                bAddStartScript = 1;
                if (strlen(optarg) != 0)
                {
                    strcpy(as8StartScriptPath, optarg);   
                }
            }
            break;
            case 'e':
            {
                bAddEndScript = 1;
                if (strlen(optarg) != 0)
                {
                    strcpy(as8EndScriptPath, optarg);   
                }
            }
            break;
            default:
                fprintf(stdout, "Error operation\n");

                return -1;
        }
    }
    if (!bCreateHeader && (bAddStartScript | bAddEndScript))
    {
        fprintf(stdout, "Error operation\n");
        
        return -1;
    }
    if (!(bInputHeader ^ bCreateHeader))
    {
        fprintf(stdout, "Error operation\n");
        
        return -1;
    }
    if (bCreateHeader)
    {
        s32Fd = open(as8OtaBinPath, O_CREAT | O_RDWR | O_TRUNC, 0777);
        if (s32Fd == -1)
        {
            fprintf(stdout, "Open %s error\n", as8OtaBinPath);
            return -1;
        }
        if (bAddStartScript)
        {
            s32SrcFd = open(as8StartScriptPath, O_RDONLY);
            if (s32SrcFd < 0)
            {
                fprintf(stdout, "open src file %s error\n", as8StartScriptPath);
                close(s32Fd);
            
                return -1;
            }
            memset(pu8Buf, 0, 128);
            do
            {    
                s32ReadCnt = _FileRead(s32SrcFd, (void *)pu8Buf, 128);
                if (s32ReadCnt)
                    u32CheckSum = _CheckSum(u32CheckSum, pu8Buf, s32ReadCnt);
            }while(s32ReadCnt);
            close(s32SrcFd);
            stHeader.stShellStart.u32DataCheckSum = u32CheckSum;
            stHeader.stShellStart.u32ShFileSize = _FileSize(as8StartScriptPath);
            printf("Start scripts Checksum 0x%x\nFile size 0x%x\n", stHeader.stShellStart.u32DataCheckSum, stHeader.stShellStart.u32ShFileSize);
        }
        s32ReadCnt = 0;
        u32CheckSum = 0;
        if (bAddEndScript)
        {
            s32SrcFd = open(as8EndScriptPath, O_RDONLY);
            if (s32SrcFd < 0)
            {
                fprintf(stdout, "open src file %s error\n", as8EndScriptPath);
                close(s32Fd);
            
                return -1;
            }
            memset(pu8Buf, 0, 128);
            do
            {    
                s32ReadCnt = _FileRead(s32SrcFd, (void *)pu8Buf, 128);
                if (s32ReadCnt)
                    u32CheckSum = _CheckSum(u32CheckSum, pu8Buf, s32ReadCnt);
            }while(s32ReadCnt);
            close(s32SrcFd);
            stHeader.stShellEnd.u32DataCheckSum = u32CheckSum;
            stHeader.stShellEnd.u32ShFileSize = _FileSize(as8EndScriptPath);
            printf("End scripts Checksum 0x%x\nFile size 0x%x\n", stHeader.stShellEnd.u32DataCheckSum, stHeader.stShellEnd.u32ShFileSize);
        }
        s32Ret = _FileWrite(s32Fd, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
        if (s32Ret < 0)
        {
            fprintf(stdout, "Write %s error\n", as8OtaBinPath);
            close(s32Fd);

            return -1;
        }


        return 0;
    }
    else if (bInputHeader)
    {
        s32Fd = open(as8OtaBinPath, O_RDWR);
        if (s32Fd == -1)
        {
            fprintf(stdout, "Open %s error\n", as8OtaBinPath);
            return -1;
        }
        s32Ret = _FileRead(s32Fd, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
        if (s32Ret < 0)
        {
            fprintf(stdout, "Read %s error\n", as8OtaBinPath);
            return -1;
        }
        lseek(s32Fd, 0, SEEK_SET);
        
    }
    if (stHeader.u32BlkCount < OTA_BLK_CNT)
    {
        strcpy(stHeader.stOtaBlkConfig[stHeader.u32BlkCount].as8DstFilePath, as8DstFilePath);
        stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32DstFileSize = u32DstFileSize;
        stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32FileOpt = u32FileOperation;

        s32SrcFd = open(as8SrcFilePath, O_RDONLY);
        if (s32SrcFd < 0)
        {
            fprintf(stdout, "open src file %s error\n", as8SrcFilePath);
            close(s32Fd);
        
            return -1;
        }
        memset(pu8Buf, 0, 128);
        do
        {    
            s32ReadCnt = _FileRead(s32SrcFd, (void *)pu8Buf, 128);
            if (s32ReadCnt)
                u32CheckSum= _CheckSum(u32CheckSum, pu8Buf, s32ReadCnt);
        }while(s32ReadCnt);
        stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32FileCheckSum = u32CheckSum;
        stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32FileSize = _FileSize(as8SrcFilePath);
        printf("File size: 0x%x\n", stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32FileSize);
        printf("Dst file : %s\nDst size :  0x%x\n", stHeader.stOtaBlkConfig[stHeader.u32BlkCount].as8DstFilePath,
                stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32DstFileSize);
        printf("Src file : %s\nChecksum : 0x%x\n", as8SrcFilePath, u32CheckSum);
        printf("Opt      : %d\n", stHeader.stOtaBlkConfig[stHeader.u32BlkCount].u32FileOpt);
        stHeader.u32HeaderCheckSum = _CheckSum(0, (void *)&stHeader + 8, sizeof(SS_OTA_BinHeader) - 8);
        stHeader.u32BlkCount++;
        printf("Header checksum: 0x%x\n", stHeader.u32HeaderCheckSum);
        printf("Header BlkCnt: %d\n", stHeader.u32BlkCount);
        close(s32SrcFd);
        s32Ret = _FileWrite(s32Fd, (void *)&stHeader, sizeof(SS_OTA_BinHeader));
        if (s32Ret < 0)
        {
            fprintf(stdout, "Write %s error\n", as8OtaBinPath);
            close(s32Fd);

            return -1;
        }
    }
    else
    {
        fprintf(stdout, "Block is full\n");
        close(s32Fd);
        
        return -1;
    }
    close(s32Fd);

    return 0;
}
