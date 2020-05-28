#ifndef ___PADMUX_H
#define ___PADMUX_H

//#define PINMUX_FOR_GPIO_MODE       0x00

typedef enum
{
    PINMUX_FOR_GPIO_MODE   = 0,
    PINMUX_FOR_GT0_MODE,
    PINMUX_FOR_GT1_MODE,
    PINMUX_FOR_SEC_MSPI2_MODE,
    PINMUX_FOR_SPDIF_OUT_MODE,
    PINMUX_FOR_SPDIF_IN_MODE,
    PINMUX_FOR_I2S_IN_MODE,
    PINMUX_FOR_I2S_TRX_MODE,
    PINMUX_FOR_I2S_OUT_MODE,
    PINMUX_FOR_I2S_OUT_MODE2,
    PINMUX_FOR_NAND_MODE,
    PINMUX_FOR_NAND_CS1_EN,
    PINMUX_FOR_I2CM0_EN,
    PINMUX_FOR_I2CM1_MODE,
    PINMUX_FOR_I2CM2_MODE,
    PINMUX_FOR_I2CM3_MODE,
    PINMUX_FOR_I2CM4_MODE,
    PINMUX_FOR_SECOND_UART_MODE,
    PINMUX_FOR_THIRD_UART_MODE,
    PINMUX_FOR_FORTH_UART_MODE,
    PINMUX_FOR_FAST_UART_RT_MODE,
    PINMUX_FOR_FUART_MODE,
    PINMUX_FOR_FUART_EMMC_MODE,
    PINMUX_FOR_MSPI1_MODE1,
    PINMUX_FOR_MSPI2_MODE1,
    PINMUX_FOR_MSPI3_MODE1,
    PINMUX_FOR_SD_CONFIG,
    PINMUX_FOR_SDIO_MODE,
    PINMUX_FOR_SDIO30_MODE,
    PINMUX_FOR_EMMC_RSTN_EN,
    PINMUX_FOR_EMMC_CONFIG,
    PINMUX_FOR_PWM0_MODE,
    PINMUX_FOR_PWM1_MODE,
    PINMUX_FOR_PWM2_MODE,
    PINMUX_FOR_PWM3_MODE,
    PINMUX_FOR_PWM4_MODE,
    PINMUX_FOR_PWM5_MODE,
    PINMUX_FOR_PWM6_MODE,
    PINMUX_FOR_PWM7_MODE,
    PINMUX_FOR_USB30VCTL_MODE,
    PINMUX_FOR_USB30VCTL_MODE1,
    PINMUX_FOR_HDMITX_DDC_MODE,
    PINMUX_FOR_HDMITX_ARC_MODE,
    PINMUX_FOR_HDMIRX_ARC_MODE,
    PINMUX_FOR_HSYNC_EN,
    PINMUX_FOR_VSYNC_EN,
    PINMUX_FOR_SATA_LED_MODE,
    PINMUX_FOR_SATA1_LED_MODE,
    PINMUX_FOR_SNR0_MODE,
    PINMUX_FOR_SNR1_MODE,
    PINMUX_FOR_SNR2_MODE,
    PINMUX_FOR_SNR3_MODE,
    PINMUX_FOR_CCIR0_CTRL_MODE,
    PINMUX_FOR_CCIR1_CTRL_MODE,
    PINMUX_FOR_CCIR2_CTRL_MODE,
    PINMUX_FOR_CCIR3_CTRL_MODE,
    PINMUX_FOR_CCIR0_CLK_MODE,
    PINMUX_FOR_CCIR1_CLK_MODE,
    PINMUX_FOR_CCIR2_CLK_MODE,
    PINMUX_FOR_CCIR3_CLK_MODE,
    PINMUX_FOR_CCIR0_8B_MODE,
    PINMUX_FOR_CCIR1_8B_MODE,
    PINMUX_FOR_CCIR2_8B_MODE,
    PINMUX_FOR_CCIR3_8B_MODE,
    PINMUX_FOR_CCIR0_16B_MODE,
    PINMUX_FOR_CCIR2_16B_MODE,
    PINMUX_FOR_MIPI_LVDS_TX_2CH_MODE,
    PINMUX_FOR_MIPI_LVDS_TX_4CH_MODE,
    PINMUX_FOR_RGB_16B_MODE,
    PINMUX_FOR_RGB_24B_MODE,
    PINMUX_FOR_TTL_OUT,
    PINMUX_FOR_CODEC_I2S_RX_MCK_MODE,
    PINMUX_FOR_CODEC_I2S_RX_0_MODE,
    PINMUX_FOR_CODEC_I2S_RX_1_MODE,
    PINMUX_FOR_CODEC_I2S_RX_2_MODE,
    PINMUX_FOR_CODEC_I2S_RX_3_MODE,
    PINMUX_FOR_CODEC_I2S_TX_0_MODE,
    PINMUX_FOR_MISC_I2S_TX_MUTE_MODE,
    PINMUX_FOR_MISC_I2S_RX_0_MODE,
    PINMUX_FOR_MISC_I2S_TX_0_MODE,
    PINMUX_FOR_MISC_I2S_TX_1_MODE,
    PINMUX_FOR_MISC_I2S_TX_2_MODE,
    PINMUX_FOR_BT_I2S_TRX_0_MODE,
    PINMUX_FOR_BT_I2S_RX_0_MODE,
    PINMUX_FOR_BT_I2S_TX_0_MODE,
    PINMUX_FOR_BT_DMIC_0_MODE,
    PINMUX_FOR_BT_DMIC_1_MODE,
    PINMUX_FOR_BT_DMIC_2_MODE,
    PINMUX_FOR_BT_DMIC_3_MODE,
    PINMUX_FOR_EJ_MODE,
    PINMUX_FOR_EJ_CEVA_MODE,
    PINMUX_FOR_PMSPI_MODE,
    PINMUX_FOR_VID_MODE,
    PINMUX_FOR_GPU_VID_MODE,
    PINMUX_FOR_MIIC_MODE,
    PINMUX_FOR_HDMI_HPD_BYPASS_MODE,
    PINMUX_FOR_SD_CDZ_MODE,
    PINMUX_FOR_PM_PWM0_MODE,
    PINMUX_FOR_PM_PWM1_MODE,
    PINMUX_FOR_MISC_I2S_RX_MCK_MODE,
    PINMUX_FOR_LED_MODE,
    PINMUX_FOR_UNKNOWN_MODE,
    PINMUX_FOR_SAR_MODE,
    PINMUX_FOR_I2C1_MODE,
    PINMUX_FOR_I2C1_MODE2,
    PINMUX_FOR_I2C1_MODE3,
    PINMUX_FOR_I2C1_MODE4,
    PINMUX_FOR_I2C2_MODE,
    PINMUX_FOR_I2C2_MODE3,
    PINMUX_FOR_I2C2_MODE4,
    PINMUX_FOR_I2C2_MODE5,
    PINMUX_FOR_I2C2_MODE6,
    PINMUX_FOR_I2C2_MODE7,
    PINMUX_FOR_I2C3_MODE4,
    PINMUX_FOR_EMMC_MODE,
    PINMUX_FOR_DMIC_MODE,
    PINMUX_FOR_PM_MSPI_MODE,
    PINMUX_FOR_SNR1_IN_MODE,
    PINMUX_FOR_SPI1_MODE,
}PINMUX_MODE;


S32 halPadGetVal(U32 padID, U32* mode);
S32 halPadSetVal(U32 padID, U32 mode);
S32 halCheckPin(U32 padID);

#endif
