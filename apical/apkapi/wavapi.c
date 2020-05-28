#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "mi_ao.h"
#include "mi_ai.h"
#include "mi_sys.h"
#include "mi_aio_datatype.h"
#include "apkapi.h"
#include "wavapi.h" 

MI_AUDIO_AnrConfig_t stAnrCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_MUSIC,
    .u32NrIntensity = 15,
    .u32NrSmoothLevel = 10,
    .eNrSpeed = E_MI_AUDIO_NR_SPEED_MID,
};

MI_AI_AecConfig_t stAecCfg = {
    .bComfortNoiseEnable = FALSE,
    .s16DelaySample = 0,
    .u32AecSupfreq = {4, 6, 36, 49, 50, 51},
    .u32AecSupIntensity = {5, 4, 4, 5, 10, 10, 10},
};

MI_AUDIO_AgcConfig_t stAgcCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .s32NoiseGateDb = -60,
    .s32TargetLevelDb = -3,
    .stAgcGainInfo = {
        .s32GainInit = 0,
        .s32GainMax = 20,
        .s32GainMin = 0,
    },
    .u32AttackTime = 1,
    .s16Compression_ratio_input = {-80, -60, -40, -25, 0},
    .s16Compression_ratio_output = {-80, -30, -15, -10, -3},
    .u32DropGainMax = 12,
    .u32NoiseGateAttenuationDb = 0,
    .u32ReleaseTime = 3,
};

MI_AUDIO_EqConfig_t stEqCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_DEFAULT,
    .s16EqGainDb = {[0 ... 128] = 3},
};

MI_AUDIO_HpfConfig_t stHpfCfg = {
    .eMode = E_MI_AUDIO_ALGORITHM_MODE_USER,
    .eHpfFreq = E_MI_AUDIO_HPF_FREQ_150,
};

int wavein_init(int ftest)
{
    MI_BOOL  bAiEnableVqe = 1;  //语音质量增强
    MI_BOOL  bAiEnableAenc = 1;  //音频编码功能
    MI_BOOL  bAiEnableHpf = FALSE;  //高通滤波功能
    MI_BOOL  bAiEnableAgc = FALSE;  //自动增益控制
    MI_BOOL  bAiEnableNr = FALSE;  //语音降噪功能
    MI_BOOL  bAiEnableAec = 1;  //回声抵消功能
    MI_BOOL  bAiEnableEq = FALSE;  //均衡器功能
    MI_AUDIO_SampleRate_e eAiOutputResampleRate = 8000;
    MI_U32   u32AiChnCnt = 1;  //音频通道数量
    MI_BOOL  bAiEnableHwAec = FALSE;
    MI_S32   s32AiVolume = 14;  //音频增益电压
    MI_AUDIO_DEV AiDevId = 0;  //音频设备号
    MI_AUDIO_AencType_e eAiAencType = E_MI_AUDIO_AENC_TYPE_G711A;
    MI_AUDIO_SampleRate_e eAiWavSampleRate = 8000;
    MI_AUDIO_SampleRate_e eAiSampleRate = 8000;  //音频采样率
    AiSoundMode_e eAiSoundMode = E_AI_SOUND_MODE_MONO;  //单声道
    MI_U32  u32VqeWorkingSampleRate = 8000;
    MI_AUDIO_Attr_t     AiSetAttr;
    MI_AI_AencConfig_t  stAiSetAencConfig;
    MI_AI_VqeConfig_t   AiSetVqeConfig;
    MI_U32              u32ChnIdx;
    MI_SYS_ChnPort_t    stAiChnOutputPort0[MI_AUDIO_MAX_CHN_NUM];

    if(ftest){
        bAiEnableAec = FALSE; //工厂测试模式下关闭回声抵消功能
    }
    memset(&AiSetAttr, 0x0, sizeof(MI_AUDIO_Attr_t));
    AiSetAttr.eBitwidth = E_MI_AUDIO_BIT_WIDTH_16;
    AiSetAttr.eSamplerate = eAiSampleRate;
    AiSetAttr.eSoundmode = eAiSoundMode;
    AiSetAttr.eWorkmode = E_MI_AUDIO_MODE_I2S_MASTER;
    AiSetAttr.u32ChnCnt = u32AiChnCnt;
    AiSetAttr.u32CodecChnCnt = 0; // useless
    AiSetAttr.u32FrmNum = 6;  // useless
    AiSetAttr.u32PtNumPerFrm = AiSetAttr.eSamplerate / 16; // for aec
    AiSetAttr.WorkModeSetting.stI2sConfig.bSyncClock = FALSE; // useless
    AiSetAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB;
    AiSetAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0;

    memset(&AiSetVqeConfig, 0x0, sizeof(MI_AI_VqeConfig_t));
    if (bAiEnableVqe)
    {
        AiSetVqeConfig.bAecOpen = bAiEnableAec;
        AiSetVqeConfig.bAgcOpen = bAiEnableAgc;
        AiSetVqeConfig.bAnrOpen = bAiEnableNr;
        AiSetVqeConfig.bEqOpen = bAiEnableEq;
        AiSetVqeConfig.bHpfOpen = bAiEnableHpf;
        AiSetVqeConfig.s32FrameSample = 128;
        if ((bAiEnableAec) && (E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
        {
            AiSetVqeConfig.s32WorkSampleRate = u32VqeWorkingSampleRate;
        }
        else
        {
            AiSetVqeConfig.s32WorkSampleRate = eAiSampleRate;
        }
        // AEC
        memcpy(&AiSetVqeConfig.stAecCfg, &stAecCfg, sizeof(MI_AI_AecConfig_t));

        // AGC
        memcpy(&AiSetVqeConfig.stAgcCfg, &stAgcCfg, sizeof(MI_AUDIO_AgcConfig_t));

        // ANR
        memcpy(&AiSetVqeConfig.stAnrCfg, &stAnrCfg, sizeof(MI_AUDIO_AnrConfig_t));

        // EQ
        memcpy(&AiSetVqeConfig.stEqCfg, &stEqCfg, sizeof(MI_AUDIO_EqConfig_t));

        // HPF
        memcpy(&AiSetVqeConfig.stHpfCfg, &stHpfCfg, sizeof(MI_AUDIO_HpfConfig_t));
    }
    ExecFunc(MI_AI_SetPubAttr(AiDevId, &AiSetAttr), MI_SUCCESS);
    ExecFunc(MI_AI_Enable(AiDevId), MI_SUCCESS);

    for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
    {
        ExecFunc(MI_AI_SetVqeVolume(AiDevId, u32ChnIdx, s32AiVolume), MI_SUCCESS);
    }

    if (bAiEnableHwAec)
    {
        ExecFunc(MI_AI_SetVqeVolume(AiDevId, 1, 1), MI_SUCCESS);
    }

    memset(&stAiSetAencConfig, 0x0, sizeof(MI_AI_AencConfig_t));
    if (bAiEnableAenc)
    {
        stAiSetAencConfig.eAencType = eAiAencType;
        if (E_MI_AUDIO_AENC_TYPE_G726 == eAiAencType)
        {
            stAiSetAencConfig.stAencG726Cfg.eG726Mode = eAiSoundMode;
        }
        else if(E_MI_AUDIO_AENC_TYPE_G711A == eAiAencType){
            stAiSetAencConfig.stAencG711Cfg.eSamplerate = eAiSampleRate;
            stAiSetAencConfig.stAencG711Cfg.eSoundmode = eAiSoundMode;
        }
    }
    
    memset(&stAiChnOutputPort0, 0x0, sizeof(stAiChnOutputPort0));

    for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
    {
        stAiChnOutputPort0[u32ChnIdx].eModId = E_MI_MODULE_ID_AI;
        stAiChnOutputPort0[u32ChnIdx].u32DevId = AiDevId;
        stAiChnOutputPort0[u32ChnIdx].u32ChnId = u32ChnIdx;
        stAiChnOutputPort0[u32ChnIdx].u32PortId = 0;
        ExecFunc(MI_SYS_SetChnOutputPortDepth(&stAiChnOutputPort0[u32ChnIdx], 1, TOTAL_BUF_DEPTH), MI_SUCCESS);
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
    {
        if (bAiEnableHwAec)
        {
            ExecFunc(MI_AI_SetExtAecChn(AiDevId, u32ChnIdx, 1), MI_SUCCESS);
        }
        ExecFunc(MI_AI_EnableChn(AiDevId, u32ChnIdx), MI_SUCCESS);
    }

    for (u32ChnIdx = 0; u32ChnIdx < u32AiChnCnt; u32ChnIdx++)
    {
        if(bAiEnableVqe)
        {
            ExecFunc(MI_AI_SetVqeAttr(AiDevId, u32ChnIdx, 0, 0, &AiSetVqeConfig), MI_SUCCESS);
            ExecFunc(MI_AI_EnableVqe(AiDevId, u32ChnIdx), MI_SUCCESS);
            if ((TRUE == AiSetVqeConfig.bAecOpen)&& (E_MI_AUDIO_SAMPLE_RATE_INVALID != u32VqeWorkingSampleRate))
            {
                eAiWavSampleRate = u32VqeWorkingSampleRate;
            }
        }
        if (bAiEnableAenc)
        {
            ExecFunc(MI_AI_SetAencAttr(AiDevId, u32ChnIdx, &stAiSetAencConfig), MI_SUCCESS);
            ExecFunc(MI_AI_EnableAenc(AiDevId, u32ChnIdx), MI_SUCCESS);
        }
    }
    return 0;
}

void wavein_exit(void)
{
    MI_AI_DisableAenc(AI_DEV_ID,AI_CHN_ID0);
    MI_AI_DisableChn(AI_DEV_ID, AI_CHN_ID0);
    //MI_AI_DisableChn(AI_DEV_ID, AI_CHN_ID1);
    MI_AI_Disable(AI_DEV_ID);
}
int waveout_init(void)
{
    // AioDevAttr_t tAudioDevAttr = {};
    // AioChnAttr_t tAudioChnAttr = {};
    MI_AUDIO_Attr_t tAudioDevAttr;

    // tAudioDevAttr.bitWidth   = 16;
    // tAudioDevAttr.soundMode  = AUDIO_SOUND_MODE_MONO;
    // tAudioDevAttr.Samplerate = AO_SAMPRATE;
    // tAudioDevAttr.bufferLen  = bufsize;
    tAudioDevAttr.eBitwidth   = E_MI_AUDIO_BIT_WIDTH_16;  //采样精度
    tAudioDevAttr.eWorkmode   = E_MI_AUDIO_MODE_I2S_SLAVE; //I2S主模式
    tAudioDevAttr.WorkModeSetting.stI2sConfig.bSyncClock = TRUE;   //AI AO异步，TRUE则同步（同步必须工作于Slave Mode）
    tAudioDevAttr.WorkModeSetting.stI2sConfig.eFmt = E_MI_AUDIO_I2S_FMT_I2S_MSB; //I2S格式设置
    tAudioDevAttr.WorkModeSetting.stI2sConfig.eMclk = E_MI_AUDIO_I2S_MCLK_0; //MCLK时钟设置（固定）
    tAudioDevAttr.eSoundmode  = E_MI_AUDIO_SOUND_MODE_MONO; //单声道
    // tAudioDevAttr.u32FrmNum   = 20;  //缓存帧数目（未确定）
    tAudioDevAttr.u32PtNumPerFrm = MI_AUDIO_SAMPLE_PER_FRAME; //每帧的采样点个数（未确定）
    tAudioDevAttr.u32ChnCnt = 1; //支持的通道数目(单声道为1)
    tAudioDevAttr.eSamplerate = E_MI_AUDIO_SAMPLE_RATE_8000;  //采样率
    
    ExecFunc(MI_AO_SetPubAttr(AO_DEV_ID, &tAudioDevAttr),MI_SUCCESS);

    // tAudioChnAttr.bEnable      = TRUE;
    // tAudioChnAttr.mode         = USER_MODE;
    // tAudioChnAttr.chn.chnID    = AO_CHN_ID;
    // tAudioChnAttr.chn.chnType  = AO_CHANNEL;
    // tAudioChnAttr.chn.deviceID = AO_DEV_ID;
    // MI_AO_SetChnAttr(AO_CHN_ID, &tAudioChnAttr);

    // MI_AO_Enable   (AO_DEV_ID);
    // MI_AO_EnableChn(AO_DEV_ID, AO_CHN_ID    );
    // MI_AO_SetVolume(AO_DEV_ID, HW_SPK_VOLUME);
    ExecFunc(MI_AO_Enable   (AO_DEV_ID),MI_SUCCESS);
    ExecFunc(MI_AO_EnableChn(AO_DEV_ID, AO_CHN_ID),MI_SUCCESS);
    ExecFunc(MI_AO_SetVolume(AO_DEV_ID, 1),MI_SUCCESS);
    printf("waveout_init finish\n");
    return 0;
}

void waveout_exit(void)
{
    MI_AO_DisableChn(AO_DEV_ID, AO_CHN_ID);
    MI_AO_Disable   (AO_DEV_ID);
}

uint8_t pcm2alaw(int16_t pcm)
{
    uint8_t sign = (pcm >> 8) & (1 << 7);
    int     mask, eee, wxyz, alaw;
    if (sign) pcm = -pcm;
    for (mask=0x4000,eee=7; (pcm&mask)==0&&eee>0; eee--,mask>>=1);
    wxyz  = (pcm >> ((eee == 0) ? 4 : (eee + 3))) & 0xf;
    alaw  = sign | (eee << 4) | wxyz;
    return (alaw ^ 0xd5);
}

int16_t alaw2pcm(uint8_t alaw)
{
    int sign, exponent, data;
    alaw    ^= 0xD5;
    sign     = alaw & 0x80;
    exponent = (alaw & 0x70) >> 4;
    data     = alaw & 0x0f;
    data   <<= 4;
    data    += 8;
    if (exponent != 0) data  += 0x100;
    if (exponent  > 1) data <<= (exponent - 1);
    return (int16_t)(sign == 0 ? data : -data);
}

void waveout_file(char *file)
{
    MI_AUDIO_Frame_t  frame  = {};
    FILE         *fp     = NULL;
    int           alaw   = 0;
    char         *str    = NULL;
    uint32_t      s32Ret;
    uint8_t       buffer1[DEF_AO_BUFSIZE];
    uint16_t      buffer2[DEF_AO_BUFSIZE];
    int           len, i;

    // check if it is a alaw (g711a) pcm file
    if ((str = strstr(file, ".alaw")) || str + 5 == file + strlen(file)) {
        alaw = 1;
    }

    frame.apVirAddr[0] = (uint8_t*)buffer2;
    frame.apVirAddr[1] = NULL;
    fp = fopen(file, "rb");
    if (fp) {
        while (!feof(fp)) {
            if (alaw) {
                len = fread(buffer1, 1, sizeof(buffer1), fp);
                for (i=0; i<len; i++) buffer2[i] = alaw2pcm(buffer1[i]);
                frame.u32Len = len * 2;
            } else {
                frame.u32Len = fread(buffer2, 1, sizeof(buffer2), fp);
            }
            if (frame.u32Len > 0 && frame.u32Len <= sizeof(buffer2)) {
                do{
                    s32Ret = MI_AO_SendFrame(AO_DEV_ID, AO_CHN_ID, &frame, -1);
                }while(s32Ret == MI_AO_ERR_NOBUF);
            }
        }
        fclose(fp);
    }
}

static uint16_t g_ulaw_dec_tab[] =
{
    0x8284, 0x8684, 0x8A84, 0x8E84, 0x9284, 0x9684, 0x9A84, 0x9E84,
    0xA284, 0xA684, 0xAA84, 0xAE84, 0xB284, 0xB684, 0xBA84, 0xBE84,
    0xC184, 0xC384, 0xC584, 0xC784, 0xC984, 0xCB84, 0xCD84, 0xCF84,
    0xD184, 0xD384, 0xD584, 0xD784, 0xD984, 0xDB84, 0xDD84, 0xDF84,
    0xE104, 0xE204, 0xE304, 0xE404, 0xE504, 0xE604, 0xE704, 0xE804,
    0xE904, 0xEA04, 0xEB04, 0xEC04, 0xED04, 0xEE04, 0xEF04, 0xF004,
    0xF0C4, 0xF144, 0xF1C4, 0xF244, 0xF2C4, 0xF344, 0xF3C4, 0xF444,
    0xF4C4, 0xF544, 0xF5C4, 0xF644, 0xF6C4, 0xF744, 0xF7C4, 0xF844,
    0xF8A4, 0xF8E4, 0xF924, 0xF964, 0xF9A4, 0xF9E4, 0xFA24, 0xFA64,
    0xFAA4, 0xFAE4, 0xFB24, 0xFB64, 0xFBA4, 0xFBE4, 0xFC24, 0xFC64,
    0xFC94, 0xFCB4, 0xFCD4, 0xFCF4, 0xFD14, 0xFD34, 0xFD54, 0xFD74,
    0xFD94, 0xFDB4, 0xFDD4, 0xFDF4, 0xFE14, 0xFE34, 0xFE54, 0xFE74,
    0xFE8C, 0xFE9C, 0xFEAC, 0xFEBC, 0xFECC, 0xFEDC, 0xFEEC, 0xFEFC,
    0xFF0C, 0xFF1C, 0xFF2C, 0xFF3C, 0xFF4C, 0xFF5C, 0xFF6C, 0xFF7C,
    0xFF88, 0xFF90, 0xFF98, 0xFFA0, 0xFFA8, 0xFFB0, 0xFFB8, 0xFFC0,
    0xFFC8, 0xFFD0, 0xFFD8, 0xFFE0, 0xFFE8, 0xFFF0, 0xFFF8, 0x0000,
    0x7D7C, 0x797C, 0x757C, 0x717C, 0x6D7C, 0x697C, 0x657C, 0x617C,
    0x5D7C, 0x597C, 0x557C, 0x517C, 0x4D7C, 0x497C, 0x457C, 0x417C,
    0x3E7C, 0x3C7C, 0x3A7C, 0x387C, 0x367C, 0x347C, 0x327C, 0x307C,
    0x2E7C, 0x2C7C, 0x2A7C, 0x287C, 0x267C, 0x247C, 0x227C, 0x207C,
    0x1EFC, 0x1DFC, 0x1CFC, 0x1BFC, 0x1AFC, 0x19FC, 0x18FC, 0x17FC,
    0x16FC, 0x15FC, 0x14FC, 0x13FC, 0x12FC, 0x11FC, 0x10FC, 0x0FFC,
    0x0F3C, 0x0EBC, 0x0E3C, 0x0DBC, 0x0D3C, 0x0CBC, 0x0C3C, 0x0BBC,
    0x0B3C, 0x0ABC, 0x0A3C, 0x09BC, 0x093C, 0x08BC, 0x083C, 0x07BC,
    0x075C, 0x071C, 0x06DC, 0x069C, 0x065C, 0x061C, 0x05DC, 0x059C,
    0x055C, 0x051C, 0x04DC, 0x049C, 0x045C, 0x041C, 0x03DC, 0x039C,
    0x036C, 0x034C, 0x032C, 0x030C, 0x02EC, 0x02CC, 0x02AC, 0x028C,
    0x026C, 0x024C, 0x022C, 0x020C, 0x01EC, 0x01CC, 0x01AC, 0x018C,
    0x0174, 0x0164, 0x0154, 0x0144, 0x0134, 0x0124, 0x0114, 0x0104,
    0x00F4, 0x00E4, 0x00D4, 0x00C4, 0x00B4, 0x00A4, 0x0094, 0x0084,
    0x0078, 0x0070, 0x0068, 0x0060, 0x0058, 0x0050, 0x0048, 0x0040,
    0x0038, 0x0030, 0x0028, 0x0020, 0x0018, 0x0010, 0x0008, 0x0000
};

void waveout_buffer(char *buf, int len, int alawulaw)
{
    MI_AUDIO_Frame_t  frame = {};
    uint16_t          decbuf[DEF_AO_BUFSIZE];
    int               i;
    int32_t           s32Ret;

    switch (alawulaw) {
    case 0: // pcm
        frame.apVirAddr[0] = (uint8_t*)buf;
        frame.apVirAddr[1] = NULL;
        frame.u32Len  = len;
        break;
    case 1: // alaw
        for (i=0; i<len; i++) decbuf[i] = alaw2pcm(buf[i]);
        frame.apVirAddr[0] = (uint8_t*)decbuf;
        frame.apVirAddr[1] = NULL;
        frame.u32Len  = len * 2;
        break;
    case 2: // ulaw
        for (i=0; i<len; i++) decbuf[i] = g_ulaw_dec_tab[(uint8_t)buf[i]];
        frame.apVirAddr[0] = (uint8_t*)decbuf;
        frame.apVirAddr[1] = NULL;
        frame.u32Len  = len * 2;
        break;
    }
    do{
        s32Ret = MI_AO_SendFrame(AO_DEV_ID, AO_CHN_ID, &frame, -1);
    }while(s32Ret == MI_AO_ERR_NOBUF);
}


