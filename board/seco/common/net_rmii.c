/*
 * (C) Copyright 2015 Seco
 *
 * Author: Davide Cardillo <davide.cardillo@seco.com>
 *
 * SPDX-License-Identifier:     GPL-2.0+
 */


#include <common.h>
#include <malloc.h>
#include <asm/io.h>
#include <asm/arch/clock.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/crm_regs.h>
#include <asm/errno.h>
#include <asm/gpio.h>
#include <asm/imx-common/iomux-v3.h>
#include <asm/imx-common/mxc_i2c.h>
#include <asm/imx-common/boot_mode.h>
#include <mmc.h>
#include <fsl_esdhc.h>
#include <miiphy.h>
#include <netdev.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/mxc_hdmi.h>
#include <linux/fb.h>
#include <micrel.h>
#include <i2c.h>

#include "proto_seco.h"
#include "muxing.h"

/*  __________________________________________________________________________
 * |                                                                          |
 * |	                        Ethernet RGMII		                      |
 * |__________________________________________________________________________|
 */


iomux_v3_cfg_t const enet_pads1[] = {

MX6_PAD_ENET_CRS_DV__ENET_RX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_MDC__ENET_MDC | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_MDIO__ENET_MDIO | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_REF_CLK__GPIO1_IO23 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_RX_ER__ENET_RX_ER | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_TX_EN__ENET_TX_EN | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_RXD0__ENET_RX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_RXD1__ENET_RX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_TXD0__ENET_TX_DATA0 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_ENET_TXD1__ENET_TX_DATA1 | MUX_PAD_CTRL(ENET_PAD_CTRL),
/* RMII Reset */
MX6_PAD_GPIO_2__GPIO1_IO02 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_GPIO_3__GPIO1_IO03 | MUX_PAD_CTRL(ENET_PAD_CTRL),
MX6_PAD_GPIO_16__ENET_REF_CLK | MUX_PAD_CTRL(ENET_PAD_CTRL), 


//	MX6_PAD_ENET_MDIO__ENET_MDIO		| MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_ENET_MDC__ENET_MDC		| MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TXC__RGMII_TXC	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TD0__RGMII_TD0	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TD1__RGMII_TD1	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TD2__RGMII_TD2	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TD3__RGMII_TD3	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_TX_CTL__RGMII_TX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_ENET_REF_CLK__ENET_TX_CLK	| MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RXC__RGMII_RXC	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	/* RGMII reset */
//	MX6_PAD_EIM_D23__GPIO3_IO23		| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
//	/* alimentazione ethernet*/
//	MX6_PAD_EIM_EB3__GPIO2_IO31		| MUX_PAD_CTRL(NO_PAD_CTRL),
//	/* pin 32 - 1 - (MODE0) all */
//	MX6_PAD_RGMII_RD0__GPIO6_IO25		| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
//	/* pin 31 - 1 - (MODE1) all */
//	MX6_PAD_RGMII_RD1__GPIO6_IO27		| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
//	/* pin 28 - 1 - (MODE2) all */
//	MX6_PAD_RGMII_RD2__GPIO6_IO28		| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
//	/* pin 27 - 1 - (MODE3) all */
//	MX6_PAD_RGMII_RD3__GPIO6_IO29		| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
//	/* pin 3 - 1 - (CLK125_EN) 125Mhz clockout enabled */
//	MX6_PAD_RGMII_RX_CTL__GPIO6_IO24	| MUX_PAD_CTRL(NO_PAD_CTRL | PAD_CTL_PUS_47K_UP),
};

iomux_v3_cfg_t const enet_pads2[] = {
//	MX6_PAD_RGMII_RXC__RGMII_RXC 	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RD0__RGMII_RD0	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RD1__RGMII_RD1	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RD2__RGMII_RD2	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RD3__RGMII_RD3	        | MUX_PAD_CTRL(ENET_PAD_CTRL),
//	MX6_PAD_RGMII_RX_CTL__RGMII_RX_CTL	| MUX_PAD_CTRL(ENET_PAD_CTRL),
};


int mx6_rmii_rework (struct phy_device *phydev) {
	phy_write (phydev, MDIO_DEVAD_NONE, MII_BMCR, 0x2100);
	return 0;
}


void setup_iomux_enet () {

	imx_iomux_v3_setup_multiple_pads (enet_pads1, ARRAY_SIZE(enet_pads1));
	udelay(20);
	
	gpio_direction_output (IMX_GPIO_NR(1, 3), 0); /* assert PHY rst */
	udelay (10000);

	gpio_set_value (IMX_GPIO_NR(1, 3), 1); /* deassert PHY rst */

	/* Need delay 100ms to exit from reset. */
	udelay (1000 * 100);
}



int board_phy_config (struct phy_device *phydev) {
	mx6_rmii_rework(phydev);

	if (phydev->drv->config)
		phydev->drv->config(phydev);

	return 0;
}


int board_eth_init (bd_t *bis) {
	uint32_t base = IMX_FEC_BASE;
	struct mii_dev *bus = NULL;
	struct phy_device *phydev = NULL;
	int ret;
	
	enable_ethernet_rail ();
	setup_iomux_enet();

#ifdef CONFIG_FEC_MXC
	bus = fec_get_miibus(base, -1);
	if (!bus)
		return 0;
	/* scan phy 4,5,6,7 */
	//phydev = phy_find_by_mask(bus, (0xf << 4), PHY_INTERFACE_MODE_RGMII);
	phydev = phy_find_by_mask(bus, (0xFF), PHY_INTERFACE_MODE_RMII);

	if (!phydev) {
		free(bus);
		return 0;
	}
	printf("using phy at %d\n", phydev->addr);
	ret  = fec_probe(bis, -1, base, bus, phydev);
	if (ret) {
		printf("FEC MXC: %s:failed\n", __func__);
		free(phydev);
		free(bus);
	}
#endif
	return 0;
}
