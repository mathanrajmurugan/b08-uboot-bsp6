/*
 * Copyright (C) UDOO Team
 *
 * Author: Francesco Montefoschi <francesco.monte@gmail.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <asm/arch/mx6-pins.h>
#include <i2c.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include "detectboard.h"

#define DIO_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_DOWN | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_34ohm | PAD_CTL_HYS | PAD_CTL_SRE_FAST)

#define DIO_PAD_CFG   (MUX_PAD_CTRL(DIO_PAD_CTRL) | MUX_MODE_SION)

#define BOARD_REV_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |		\
	PAD_CTL_PUS_100K_DOWN | PAD_CTL_SPEED_MED |		\
	PAD_CTL_DSE_34ohm | PAD_CTL_HYS | PAD_CTL_SRE_FAST)

#define BOARD_REV_PAD_CFG   (MUX_PAD_CTRL(BOARD_REV_PAD_CTRL) | MUX_MODE_SION)

iomux_v3_cfg_t const board_recognition_pads[] = {
    MX6_PAD_ENET1_COL__GPIO2_IO_0 | DIO_PAD_CFG,  // ENET1_COL conn to ENET2_RST
};
#define GPIO_ON_ENET1_COL IMX_GPIO_NR(2, 0)

iomux_v3_cfg_t const board_revision_pads[] = {
    MX6_PAD_ENET2_COL__GPIO2_IO_6 | BOARD_REV_PAD_CFG,  // BOARD_REV_D0
    MX6_PAD_ENET2_CRS__GPIO2_IO_7 | BOARD_REV_PAD_CFG,  // BOARD_REV_D1
    MX6_PAD_KEY_ROW3__GPIO2_IO_18 | BOARD_REV_PAD_CFG,  // BOARD_REV_D2
};
#define BOARD_REVISION_BIT_0 IMX_GPIO_NR(2, 6)
#define BOARD_REVISION_BIT_1 IMX_GPIO_NR(2, 7)
#define BOARD_REVISION_BIT_2 IMX_GPIO_NR(2, 18)

/**
 * Detects board revision.
 * Rev_B = 0x0
 * Rev_C = 0x1
 * Rev_D = 0x2
 * Rev_E = 0x3
 * Rev_F = 0x4
 * Rev_G = 0x5
 * Rev_H = 0x6
 * Rev_I = 0x7
 */
int detect_revision(void)
{
    imx_iomux_v3_setup_multiple_pads(board_revision_pads,
        ARRAY_SIZE(board_revision_pads));
    
    gpio_direction_input(BOARD_REVISION_BIT_0);
    gpio_direction_input(BOARD_REVISION_BIT_1);
    gpio_direction_input(BOARD_REVISION_BIT_2);
    
    int board_rev = gpio_get_value(BOARD_REVISION_BIT_0) << 0 |
		    gpio_get_value(BOARD_REVISION_BIT_1) << 1 |
		    gpio_get_value(BOARD_REVISION_BIT_2) << 2 ;

    return board_rev;
}

/**
 * Detects the board model.
 * FULL   - 1GB RAM, 2xEth, WiFi, motion sensors  -> GPIO2_0 = 1
 * BASIC  - 512 RAM, 1xEth, WiFi, motion sensors  -> GPIO2_0 = 0
 */
int detect_board(void)
{
    imx_iomux_v3_setup_multiple_pads(board_recognition_pads,
        ARRAY_SIZE(board_recognition_pads));
    
    gpio_direction_input(GPIO_ON_ENET1_COL);
    
    int enet2_rst = gpio_get_value(GPIO_ON_ENET1_COL);

    if (enet2_rst == 0)
    	return SECO_B08_TYPE_BASIC;
    else
    	return SECO_B08_TYPE_FULL;
}

/**
 * Autodetect video output searching for connected I2C touchsreen address
 * 0x14 = RGB 7''
 * 0x55 = LVDS 7''
 */
int detect_video(void)
{
        I2C_SET_BUS(TOUCH_I2C_BUS);
        int j, ret=0;
        struct udevice *bus;

        printf("\ndevice 0x%02x: ", j);
        for (j = 128; j > 0; j--) {
           ret = i2c_probe(j);
           if (ret == 0) {
                switch(j) {
                   case 0x14:
                        printf("Auto-select RGB 7'' video output.\n");
                        return 0x14;
                   case 0x55:
                        printf("Auto-select LVDS 7'' video output.\n");
                        return 0x55;
                }
           }
        }
        return 0;
}

