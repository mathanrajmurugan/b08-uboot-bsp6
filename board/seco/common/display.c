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
#include <ipu_pixfmt.h>
#include <micrel.h>
#include <i2c.h>

#include "proto_seco.h"
#include "muxing.h"




#if defined(CONFIG_VIDEO_IPUV3)


static int detect_hdmi (struct display_info_t const *dev) {
	struct hdmi_regs *hdmi  = (struct hdmi_regs *)HDMI_ARB_BASE_ADDR;
	return readb(&hdmi->phy_stat0) & HDMI_DVI_STAT;
}


static void enable_hdmi (struct display_info_t const *dev) {
	disable_lvds(dev);
	imx_enable_hdmi_phy ();
}


static int detect_lvds (struct display_info_t const *dev) {
	return 0;
}


static struct display_info_t const displays[] = {
	{
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB24,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			.name           = "LDB-WXGA",
			.refresh        = 59,
			.xres           = 1368,
			.yres           = 768,
			.pixclock       = 13890,
			.left_margin    = 93,
			.right_margin   = 33,
			.upper_margin   = 22,
			.lower_margin   = 7,
			.hsync_len      = 40,
			.vsync_len      = 4,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	}, {
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB24,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			.name           = "LDB-SVGA-BA",
			.refresh        = 60,
			.xres           = 800,
			.yres           = 600,
			.pixclock       = 13252,
			.left_margin    = 45,
			.right_margin   = 45,
			.upper_margin   = 25,
			.lower_margin   = 25,
			.hsync_len      = 10,
			.vsync_len      = 10,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	 }, {
		.bus	= 0,
		.addr	= 0,
		.pixfmt	= IPU_PIX_FMT_RGB24,
		.detect	= NULL,
		.enable	= enable_rgb,
		.mode	= {
			.name           = "CLAA-WVGA",
			.refresh        = 60,
			.xres           = 800,
			.yres           = 480,
			.pixclock       = 29850,
			.left_margin    = 89,
			.right_margin   = 164,
			.upper_margin   = 23,
			.lower_margin   = 10,
			.hsync_len      = 10,
			.vsync_len      = 10,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	}, {
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. 800x480 Panel UMSH-8596MD-20T
			.name           = "LDB-WVGA",
			.refresh        = 60,
			.xres           = 800,
			.yres           = 480,
			.pixclock       = 15385,
			.left_margin    = 220,
			.right_margin   = 40,
			.upper_margin   = 21,
			.lower_margin   = 7,
			.hsync_len      = 60,
			.vsync_len      = 10,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		} 
	}, {

		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. Panel 1024x768 Panel UMSH-8596MD-15T
			.name           = "LDB-XGA",
			.refresh        = 60,
			.xres           = 1024,
			.yres           = 768,
			.pixclock       = 15385,
			.left_margin    = 220,
			.right_margin   = 40,
			.upper_margin   = 21,
			.lower_margin   = 7,
			.hsync_len      = 60,
			.vsync_len      = 10,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	}, {
		.bus    = -1,
		.addr   = -1,
		.pixfmt = IPU_PIX_FMT_RGB666,
		.detect = detect_lvds,
		.enable = enable_lvds,
		.mode   = {
			// Rif. 1366x768 Panel CHIMEI M156B3-LA1
			.name           = "LDB-WXGA",
			.refresh        = 59,
			.xres           = 1368,
			.yres           = 768,
			.pixclock       = 13890,
			.left_margin    = 93,
			.right_margin   = 33,
			.upper_margin   = 22,
			.lower_margin   = 7,
			.hsync_len      = 40,
			.vsync_len      = 4,
			.sync           = 0,
			.vmode          = FB_VMODE_NONINTERLACED
		}
	},{
		.bus    = -1,
		.addr   = 0,
		.pixfmt = IPU_PIX_FMT_RGB24,
		.detect = detect_hdmi,
		.enable = enable_hdmi,
		.mode   = {
        	.name           = "HDMI",
        	.refresh        = 60,
        	.xres           = 1024,
        	.yres           = 768,
        	.pixclock       = 15385,
        	.left_margin    = 220,
        	.right_margin   = 40,
        	.upper_margin   = 21,
        	.lower_margin   = 7,
        	.hsync_len      = 60,
        	.vsync_len      = 10,
        	.sync           = FB_SYNC_EXT,
        	.vmode          = FB_VMODE_NONINTERLACED
		}
	},
};
size_t display_count = ARRAY_SIZE(displays);


int board_video_skip (void) {
	int i;
	int ret;
	char const *panel = getenv("panel");

	if ( !panel ) {
		panel = display_name;
	}

	if ( !panel ) {
		for ( i = 0 ; i < ARRAY_SIZE(displays) ; i++ ) {
			struct display_info_t const *dev = displays+i;
			if ( dev->detect && dev->detect(dev) ) {
				panel = dev->mode.name;
				printf ("auto-detected panel %s\n", panel);
				break;
			}
		}
		if ( !panel ) {
			panel = displays[0].mode.name;
			printf ("No panel detected: default to %s\n", panel);
			i = 0;
		}
	} else {
		for ( i = 0 ; i < ARRAY_SIZE(displays) ; i++ ) {
			if ( !strcmp(panel, displays[i].mode.name) )
				break;
		}
	}

	if ( i < ARRAY_SIZE(displays) ) {
		ret = ipuv3_fb_init(&displays[i].mode, 0,
				    displays[i].pixfmt);
		if ( !ret ) {
			if ( displays[i].enable )
				displays[i].enable (displays+i);
			printf ("Display: %s (%ux%u)\n",
			       displays[i].mode.name,
			       displays[i].mode.xres,
			       displays[i].mode.yres);
		} else
			printf ("LCD %s cannot be configured: %d\n",
			       displays[i].mode.name, ret);
	} else {
		printf ("unsupported panel %s\n", panel);
		return -EINVAL;
	}

	return 0;
}



void setup_display (void) {

	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR;
	struct iomuxc *iomux = (struct iomuxc *)IOMUXC_BASE_ADDR;
	int reg;

	setup_iomux_backlight();
	enable_ipu_clock();
	imx_setup_hdmi();

	/* Turn on LDB0, LDB1, IPU,IPU DI0 clocks */
	reg = readl(&mxc_ccm->CCGR3);
	reg |=  MXC_CCM_CCGR3_LDB_DI0_MASK | MXC_CCM_CCGR3_LDB_DI1_MASK;
	writel(reg, &mxc_ccm->CCGR3);

	/* set LDB0, LDB1 clk select to 011/011 */
	reg = readl(&mxc_ccm->cs2cdr);
	reg &= ~(MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_MASK
		 | MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_MASK);
	reg |= (3 << MXC_CCM_CS2CDR_LDB_DI0_CLK_SEL_OFFSET)
	      | (3 << MXC_CCM_CS2CDR_LDB_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->cs2cdr);

	reg = readl(&mxc_ccm->cscmr2);
	reg |= MXC_CCM_CSCMR2_LDB_DI0_IPU_DIV | MXC_CCM_CSCMR2_LDB_DI1_IPU_DIV;
	writel(reg, &mxc_ccm->cscmr2);

	reg = readl(&mxc_ccm->chsccdr);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI0_CLK_SEL_OFFSET);
	reg |= (CHSCCDR_CLK_SEL_LDB_DI0
		<< MXC_CCM_CHSCCDR_IPU1_DI1_CLK_SEL_OFFSET);
	writel(reg, &mxc_ccm->chsccdr);

	reg = IOMUXC_GPR2_BGREF_RRMODE_EXTERNAL_RES
	     | IOMUXC_GPR2_DI1_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_DI0_VS_POLARITY_ACTIVE_LOW
	     | IOMUXC_GPR2_BIT_MAPPING_CH1_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH1_18BIT
	     | IOMUXC_GPR2_BIT_MAPPING_CH0_SPWG
	     | IOMUXC_GPR2_DATA_WIDTH_CH0_18BIT
	     | IOMUXC_GPR2_LVDS_CH0_MODE_ENABLED_DI0
	     | IOMUXC_GPR2_LVDS_CH1_MODE_ENABLED_DI0;
	writel(reg, &iomux->gpr[2]);

	reg = readl(&iomux->gpr[3]);
	reg = (reg & ~(IOMUXC_GPR3_LVDS1_MUX_CTL_MASK
			| IOMUXC_GPR3_HDMI_MUX_CTL_MASK))
	    | (IOMUXC_GPR3_MUX_SRC_IPU1_DI0
	       << IOMUXC_GPR3_LVDS1_MUX_CTL_OFFSET);
	writel(reg, &iomux->gpr[3]);
}



#endif /* CONFIG_VIDEO_IPUV3 */


