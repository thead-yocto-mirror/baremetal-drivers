// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018-2019 Synopsys, Inc. and/or its affiliates.
 *
 * Synopsys DesignWare MIPI D-PHY controller driver
 * Core functions
 *
 * Author: Luis Oliveira <luis.oliveira@synopsys.com>
 */

#include "dw-dphy-rx.h"
#include "bm_csi_dphy.h"
#include "bm_printk.h"
#include <linux/io.h>

struct range_dphy_gen2 {
	u32 freq;
	u8 hsfregrange;
};

struct range_dphy_gen2 range_gen2[] = {
	{ 80, 0x00 }, { 90, 0x10 }, { 100, 0x20 }, { 110, 0x30 },
	{ 120, 0x01 }, { 130, 0x11 }, { 140, 0x21 }, { 150, 0x31 },
	{ 160, 0x02 }, { 170, 0x12 }, { 180, 0x22 }, { 190, 0x32 },
	{ 205, 0x03 }, { 220, 0x13 }, { 235, 0x23 }, { 250, 0x33 },
	{ 275, 0x04 }, { 300, 0x14 }, { 325, 0x05 }, { 350, 0x15 },
	{ 400, 0x25 }, { 450, 0x06 }, { 500, 0x16 }, { 550, 0x07 },
	{ 600, 0x17 }, { 650, 0x08 }, { 700, 0x18 }, { 750, 0x09 },
	{ 800, 0x19 }, { 850, 0x29 }, { 900, 0x39 }, { 950, 0x0A },
	{ 1000, 0x1A }, { 1050, 0x2A }, { 1100, 0x3A }, { 1150, 0x0B },
	{ 1200, 0x1B }, { 1250, 0x2B }, { 1300, 0x3B }, { 1350, 0x0C },
	{ 1400, 0x1C }, { 1450, 0x2C }, { 1500, 0x3C }, { 1550, 0x0D },
	{ 1600, 0x1D }, { 1650, 0x2D }, { 1700, 0x0E }, { 1750, 0x1E },
	{ 1800, 0x2E }, { 1850, 0x3E }, { 1900, 0x0F }, { 1950, 0x1F },
	{ 2000, 0x2F },
};

struct range_dphy_gen3 {
	u32 freq;
	u8 hsfregrange;
	u32 osc_freq_target;
};

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
struct range_dphy_gen3 range_gen3[] = {
	{ 80, 0x00, 0x1B6 }, { 90, 0x10, 0x1B6 }, { 100, 0x20, 0x1B6 },
	{ 110, 0x30, 0x1B6 }, { 120, 0x01, 0x1B6 }, { 130, 0x11, 0x1B6 },
	{ 140, 0x21, 0x1B6 }, { 150, 0x31, 0x1B6 }, { 160, 0x02, 0x1B6 },
	{ 170, 0x12, 0x1B6 }, { 180, 0x22, 0x1B6 }, { 190, 0x32, 0x1B6 },
	{ 205, 0x03, 0x1B6 }, { 220, 0x13, 0x1B6 }, { 235, 0x23, 0x1B6 },
	{ 250, 0x33, 0x1B6 }, { 275, 0x04, 0x1B6 }, { 300, 0x14, 0x1B6 },
	{ 325, 0x25, 0x1B6 }, { 350, 0x35, 0x1B6 }, { 400, 0x05, 0x1B6 },
	{ 450, 0x16, 0x1B6 }, { 500, 0x26, 0x1B6 }, { 550, 0x37, 0x1B6 },
	{ 600, 0x07, 0x1B6 }, { 650, 0x18, 0x1B6 }, { 700, 0x28, 0x1B6 },
	{ 750, 0x39, 0x1B6 }, { 800, 0x09, 0x1B6 }, { 850, 0x19, 0x1B6 },
	{ 900, 0x29, 0x1B6 }, { 950, 0x3A, 0x1B6 }, { 1000, 0x0A, 0x1B6 },
	{ 1050, 0x1A, 0x1B6 }, { 1100, 0x2A, 0x1B6 }, { 1150, 0x3B, 0x1B6 },
	{ 1200, 0x0B, 0x1B6 }, { 1250, 0x1B, 0x1B6 }, { 1300, 0x2B, 0x1B6 },
	{ 1350, 0x3C, 0x1B6 }, { 1400, 0x0C, 0x1B6 }, { 1450, 0x1C, 0x1B6 },
	{ 1500, 0x2C, 0x1B6 }, { 1550, 0x3D, 0x10F }, { 1600, 0x0D, 0x118 },
	{ 1650, 0x1D, 0x121 }, { 1700, 0x2E, 0x12A }, { 1750, 0x3E, 0x132 },
	{ 1800, 0x0E, 0x13B }, { 1850, 0x1E, 0x144 }, { 1900, 0x2F, 0x14D },
	{ 1950, 0x3F, 0x155 }, { 2000, 0x0F, 0x15E }, { 2050, 0x40, 0x167 },
	{ 2100, 0x41, 0x170 }, { 2150, 0x42, 0x178 }, { 2200, 0x43, 0x181 },
	{ 2250, 0x44, 0x18A }, { 2300, 0x45, 0x193 }, { 2350, 0x46, 0x19B },
	{ 2400, 0x47, 0x1A4 }, { 2450, 0x48, 0x1AD }, { 2500, 0x49, 0x1B6 }
};
#else
//for asic
struct range_dphy_gen3 range_gen3[] = {         //TODO 根据手册更新osc_freq_target的值。
	{ 80, 0x00, 460 }, { 90, 0x10, 460 }, { 100, 0x20, 460 },
	{ 110, 0x30, 460 }, { 120, 0x01, 460 }, { 130, 0x11, 460 },
	{ 140, 0x21, 460 }, { 150, 0x31, 460 }, { 160, 0x02, 460 },
	{ 170, 0x12, 460 }, { 180, 0x22, 460 }, { 190, 0x32, 460 },
	{ 205, 0x03, 460 }, { 220, 0x13, 460 }, { 235, 0x23, 460 },
	{ 250, 0x33, 460 }, { 275, 0x04, 460 }, { 300, 0x14, 460 },
	{ 325, 0x25, 460 }, { 350, 0x35, 460 }, { 400, 0x05, 460 },
	{ 450, 0x16, 460 }, { 500, 0x26, 460 }, { 550, 0x37, 460 },
	{ 600, 0x07, 460 }, { 650, 0x18, 460 }, { 700, 0x28, 460 },
	{ 750, 0x39, 460 }, { 800, 0x09, 460 }, { 850, 0x19, 460 },
	{ 900, 0x29, 460 }, { 950, 0x3A, 460 }, { 1000, 0x0A, 460 },
	{ 1050, 0x1A, 460 }, { 1100, 0x2A, 460 }, { 1150, 0x3B, 460 },
	{ 1200, 0x0B, 460 }, { 1250, 0x1B, 460 }, { 1300, 0x2B, 460 },
	{ 1350, 0x3C, 460 }, { 1400, 0x0C, 460 }, { 1450, 0x1C, 460 },
	{ 1500, 0x2C, 460 }, { 1550, 0x3D, 285 }, { 1600, 0x0D, 295 },
	{ 1650, 0x1D, 304 }, { 1700, 0x2E, 313 }, { 1750, 0x3E, 322 },
	{ 1800, 0x0E, 331 }, { 1850, 0x1E, 341 }, { 1900, 0x2F, 350 },
	{ 1950, 0x3F, 359 }, { 2000, 0x0F, 368 }, { 2050, 0x40, 377 },
	{ 2100, 0x41, 387 }, { 2150, 0x42, 396 }, { 2200, 0x43, 405 },
	{ 2250, 0x44, 414 }, { 2300, 0x45, 423 }, { 2350, 0x46, 432 },
	{ 2400, 0x47, 442 }, { 2450, 0x48, 451 }, { 2500, 0x49, 460 }
};
#endif

u8 dw_dphy_setup_config(struct dw_dphy_rx *dphy)
{
#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
#if 0
	int ret;

	if (dphy->max_lanes == CTRL_4_LANES) {
		dev_vdbg(&dphy->phy->dev, "CONFIG 4L\n");
		return CTRL_4_LANES;
	}
	if (IS_ENABLED(CONFIG_OF)) {
		ret = gpio_request(dphy->config_8l, "config");
		if (ret < 0) {
			dev_vdbg(&dphy->phy->dev,
				 "could not acquire config (err=%d)\n", ret);
			return ret;
		}
		ret = gpio_get_value(dphy->config_8l);
		gpio_free(dphy->config_8l);
	} else {
		ret = dphy->config_8l;
	}

	dev_vdbg(&dphy->phy->dev,
		 "Booting in [%s] mode\n",
		 ret == CTRL_8_LANES ? "8L" : "4+4L");
	return ret;
#endif
#endif /* CONFIG_DWC_MIPI_TC_DPHY_GEN3 */
	return CTRL_4_LANES;
}

extern int k_bm_visys_write_reg(uint32_t offset, uint32_t value);
extern int k_bm_visys_read_reg(uint32_t offset, uint32_t *value);

void dw_dphy_if_write(struct dw_dphy_rx *dphy, u32 address, u32 data)
{
    //writel(data, dphy->dphy1_if_addr + address);
    k_bm_visys_write_reg(address, data);
}

u32 dw_dphy_if_read(struct dw_dphy_rx *dphy, u32 address)
{
	u32 if1 = 0;
    //if1 = readl(dphy->dphy1_if_addr + address);
    k_bm_visys_read_reg(address, &if1);
	return if1;
}

void dw_dphy_write(struct dw_dphy_rx *dphy, u32 address, u32 data)
{
	writel(data, dphy->base_address + address);

	if (dphy->lanes_config == CTRL_4_LANES)
		return;
	return;

	if (address == R_CSI2_DPHY_TST_CTRL0)
		iowrite32(data, dphy->base_address + R_CSI2_DPHY2_TST_CTRL0);
	else if (address == R_CSI2_DPHY_TST_CTRL1)
		iowrite32(data, dphy->base_address + R_CSI2_DPHY2_TST_CTRL1);
}

u32 dw_dphy_read(struct dw_dphy_rx *dphy, u32 address)
{
	int dphy1 = 0, dphy2 = 0;

	dphy1 = readl(dphy->base_address + address);

	if (dphy->lanes_config == CTRL_4_LANES)
		goto end;

    goto end;

	if (address == R_CSI2_DPHY_TST_CTRL0)
		dphy2 = ioread32(dphy->base_address + R_CSI2_DPHY2_TST_CTRL0);
	else if (address == R_CSI2_DPHY_TST_CTRL1)
		dphy2 = ioread32(dphy->base_address + R_CSI2_DPHY2_TST_CTRL1);
	else
		return -ENODEV;
end:
	return dphy1;
}

void dw_dphy_write_msk(struct dw_dphy_rx *dev, u32 address, u32 data, u8 shift,
		       u8 width)
{
	u32 temp = dw_dphy_read(dev, address);
	u32 mask = (1 << width) - 1;

	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	dw_dphy_write(dev, address, temp);
}

static void dw_dphy_te_12b_write(struct dw_dphy_rx *dphy, u16 addr, u8 data)
{
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0x00, PHY_TESTDIN, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, (u8)(addr >> 8),
			  PHY_TESTDIN, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, (u8)addr, PHY_TESTDIN,
			  8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, (u8)data, PHY_TESTDIN,
			  8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
}

static void dw_dphy_te_8b_write(struct dw_dphy_rx *dphy, u8 addr, u8 data)
{
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write(dphy, R_CSI2_DPHY_TST_CTRL1, addr);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write(dphy, R_CSI2_DPHY_TST_CTRL1, data);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
}

static void dw_dphy_te_write(struct dw_dphy_rx *dphy, u16 addr, u8 data)
{
    dphy->dphy_te_len = BIT12;
	if (dphy->dphy_te_len == BIT12)
		dw_dphy_te_12b_write(dphy, addr, data);
	else
		dw_dphy_te_8b_write(dphy, addr, data);
}

static int dw_dphy_te_12b_read(struct dw_dphy_rx *dphy, u32 addr)
{
	u8 ret;
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0x00, PHY_TESTDIN, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, (u8)(addr >> 8),
			  PHY_TESTDIN, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, (u8)addr, PHY_TESTDIN,
			  8);

	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0x00, 0, PHY_TESTDIN);

	ret = dw_dphy_read_msk(dphy, R_CSI2_DPHY_TST_CTRL1, PHY_TESTDOUT, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);

	return ret;
}

static int dw_dphy_te_8b_read(struct dw_dphy_rx *dphy, u32 addr)
{
	u8 ret;

	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 0);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 1, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, addr, PHY_TESTDIN, 8);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTEN, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL1, 0, PHY_TESTDIN, 8);
	ret = dw_dphy_read_msk(dphy, R_CSI2_DPHY_TST_CTRL1, PHY_TESTDOUT, 8);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 1);

	return ret;
}

int dw_dphy_te_read(struct dw_dphy_rx *dphy, u32 addr)
{
	int ret;

	if (dphy->dphy_te_len == BIT12)
		ret = dw_dphy_te_12b_read(dphy, addr);
	else
		ret = dw_dphy_te_12b_read(dphy, addr);
		//ret = dw_dphy_te_8b_read(dphy, addr);

	return ret;
}

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
static void dw_dphy_if_init(struct dw_dphy_rx *dphy)
{
    bm_info("enter %s\n", __func__);
	//dw_dphy_if_write(dphy, dphy->dphy1_if_addr, RESET);
	//dw_dphy_if_write(dphy, dphy->dphy1_if_addr, TX_PHY);
	//dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLR, 1);
	//dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLR, 1);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 0);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 1);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, GLUELOGIC);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLR, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLR, 1);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 0);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 1);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RX_PHY);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLR, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLR, 1);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 0);
	//dw_dphy_if_write(dphy, DPHYZCALCTRL, 1);
}
#endif

static void dw_dphy_gen3_12bit_tc_power_up(struct dw_dphy_rx *dphy, uint8_t hsfregrange)
{
#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, GLUELOGIC);  //用fpga时候是写到子板专有寄存器内，芯片要写到visys reg
	dw_dphy_te_write(dphy, CFGCLKFREQRANGE_RX, 0x1C);   //TODO ASCI 改为通过vi sys寄存器去配置src clk
	dw_dphy_te_write(dphy, 0x6, hsfregrange);           //TODO ASCI 改为通过vi sys寄存器去配置src clk
#endif


#if !IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
    u32 reg_val = 0;
	reg_val = dw_dphy_if_read(dphy, dphy->sysreg_mipi_csi_ctrl);   //TODO ASCI 改为通过vi sys寄存器去配置src clk
    reg_val &= ~0x3f;
    reg_val |= 0x1c;
	dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, reg_val);   //TODO ASCI 改为通过vi sys寄存器去配置src clk
    reg_val &= ~(0x7f << 8);
    reg_val |= (hsfregrange << 8);
	dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, reg_val);   //TODO ASCI 改为通过vi sys寄存器去配置src clk
#endif

	/* CLKSEL | UPDATEPLL | SHADOW_CLEAR | SHADOW_CTRL | FORCEPLL */
	//dw_dphy_te_write(dphy, BYPASS, 0x3F);
	/* IO_DS3 | IO_DS2 | IO_DS1 | IO_DS0 */

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	if ((dphy->dphy_freq/1000) > 1500) {
		dw_dphy_te_write(dphy, IO_DS, 0x0F);
    }

	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RX_PHY | 0x2);
#endif
}

static void dw_dphy_gen3_8bit_tc_power_up(struct dw_dphy_rx *dphy)
{
	u32 input_freq = dphy->dphy_freq / 1000;
#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, GLUELOGIC);
	dw_dphy_te_write(dphy, CFGCLKFREQRANGE_RX, 0x1C);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RESET);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, RX_PHY);
#endif
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX0_MSB, 0x03);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX0_LSB, 0x02);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX1_MSB, 0x03);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX1_LSB, 0x02);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX2_MSB, 0x03);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX2_LSB, 0x02);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX3_MSB, 0x03);
	dw_dphy_te_write(dphy, OSC_FREQ_TARGET_RX3_LSB, 0x02);
	dw_dphy_te_write(dphy, BANDGAP_CTRL, 0x80);

	if (input_freq < 2000)
		dw_dphy_te_write(dphy, HS_RX_CTRL_LANE0, 0xC0);

	if (input_freq < 1000) {
		dw_dphy_te_write(dphy, HS_RX_CTRL_LANE1, 0xC0);
		dw_dphy_te_write(dphy, HS_RX_CTRL_LANE2, 0xC0);
		dw_dphy_te_write(dphy, HS_RX_CTRL_LANE3, 0xC0);
	}
}

int dw_dphy_g118_settle(struct dw_dphy_rx *dphy)
{
	u32 input_freq, total_settle, settle_time, byte_clk, lp_time;

	lp_time = dphy->lp_time;
	input_freq = dphy->dphy_freq / 1000;

	settle_time = (8 * (1000000 / (input_freq))) + 115000;
	byte_clk = (8000000 / (input_freq));
	total_settle = (settle_time + lp_time * 1000) / byte_clk;

	if (total_settle > 0xFF)
		total_settle = 0xFF;

	return total_settle;
}

static void dw_dphy_pwr_down(struct dw_dphy_rx *dphy)
{
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);

	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLK, 1);
	if (dphy->lanes_config == CTRL_8_LANES)
		dw_dphy_write_msk(dphy, R_CSI2_DPHY2_TST_CTRL0, 0, PHY_TESTCLK, 1);

	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 0);
}

static void dw_dphy_pwr_up(struct dw_dphy_rx *dphy)
{
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLK, 1);
	if (dphy->lanes_config == CTRL_8_LANES)
		dw_dphy_write_msk(dphy, R_CSI2_DPHY2_TST_CTRL0, 1, PHY_TESTCLK,
				  1);
	dev_vdbg(&dphy->phy->dev, "DPHY power up.\n");
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 1);
    udelay(1);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);
    udelay(1);
}

static int dw_dphy_gen3_12bit_configure(struct dw_dphy_rx *dphy, int abphy_flag)
{
	u32 input_freq = dphy->dphy_freq;
	u8 range = 0;
    u32 reg_val = 0;

	dev_vdbg(&dphy->phy->dev, "12bit: PHY GEN 3: Freq: %u\n", input_freq);
	for (range = 0; (range < ARRAY_SIZE(range_gen3) - 1) &&
	     ((input_freq / 1000) > range_gen3[range].freq);
	     range++);

    input_freq  /= 1000;

	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 0);


    if (dphy->phy_id == MIPI_A_PHY) {
        reg_val = dw_dphy_te_read(dphy, RX_RX_CLKLANE_LANE_6);
        reg_val &= ~(3 << 4);
	    dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, reg_val);
    }

	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLR, 1);
    udelay(1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLR, 1);

	dw_dphy_gen3_12bit_tc_power_up(dphy, range_gen3[range].hsfregrange);

	//uint8_t lp_time = dphy->lp_time;
	//dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_17, lp_time);//SET SETTLE TIME

    //位置1

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_te_write(dphy, RX_SYS_1, range_gen3[range].hsfregrange);
	dw_dphy_te_write(dphy, RX_SYS_0, 0x20);
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_2, 0xcc);
#else
    //used for asic
    if (input_freq > 1500) {
	    dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_2, (u8)(range_gen3[range].osc_freq_target & 0xff));
	    dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_3, (u8)(range_gen3[range].osc_freq_target >> 8));
    }
#endif



#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_4, 0x21);         //TODO fpga val=0x21, chip val=0x11
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_5, 0x01);         //TODO 顺序位置配置可能需要调到位置1
#else

	reg_val = dw_dphy_te_read(dphy, RX_RX_STARTUP_OVR_4);
    reg_val &= ~(0xf0);
    reg_val |= (1 << 4);
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_4, reg_val);

    if (input_freq > 1500) {
	    reg_val = dw_dphy_te_read(dphy, RX_RX_STARTUP_OVR_4);
        reg_val |= 1;
	    dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_4, reg_val);         //TODO fpga val=0x21, chip val=0x11
    }

	reg_val = dw_dphy_te_read(dphy, RX_RX_STARTUP_OVR_5);
    reg_val |= 1;
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_5, reg_val);         //TODO 顺序位置配置可能需要调到位置1

	reg_val = dw_dphy_te_read(dphy, RX_RX_CB_1);                  //TODO used to chip
    reg_val &= ~0x3;
    reg_val |= 2;
	dw_dphy_te_write(dphy, RX_RX_CB_1, reg_val);                  //TODO used to chip


	reg_val = dw_dphy_te_read(dphy, RX_RX_CB_2);                  //TODO used to chip
    reg_val |= (1 << 6);
	dw_dphy_te_write(dphy, RX_RX_CB_2, reg_val);              //TODO used to chip


    if (abphy_flag) {
	    reg_val = dw_dphy_te_read(dphy, RX_RX_CLKLANE_LANE_6);                  //TODO used to chip
        reg_val |= (1 << 7);
	    dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, reg_val);    //TODO used to chip
    }

    if (input_freq <= 1500) {
        reg_val = dw_dphy_te_read(dphy, 0x607); //TODO used to chip
        reg_val |= 0x3f;
	    dw_dphy_te_write(dphy, 0x607, reg_val); //TODO used to chip

        reg_val = dw_dphy_te_read(dphy, 0x807); //TODO used to chip
        reg_val |= 0x3f;
	    dw_dphy_te_write(dphy, 0x807, reg_val); //TODO used to chip

        reg_val = dw_dphy_te_read(dphy, 0xa07); //TODO used to chip
        reg_val |= 0x3f;
	    dw_dphy_te_write(dphy, 0xa07, reg_val); //TODO used to chip

        reg_val = dw_dphy_te_read(dphy, 0xc07); //TODO used to chip
        reg_val |= 0x3f;
	    dw_dphy_te_write(dphy, 0xc07, reg_val);    //TODO used to chip
    } else  {
	    dw_dphy_te_write(dphy, RX_SYS_9, 0x43);                    //TODO used to chip
    }

    //for chip dphy->sysreg_mipi_csi_ctrl: csi2_0, csi2_1, csi2_2 -> 0x140, 0x144, 0x148
    reg_val  = dw_dphy_if_read(dphy, dphy->sysreg_mipi_csi_ctrl);
    reg_val |= (1 << 28) | (1 << 25) | (7 << 20);
	dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, reg_val); //TODO set visys reg basedir_0 =1, set visys reg forcerxmode_0123=1
#endif

    udelay(1);

	if (dphy->phy_type) {
#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
		dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_1, 0x01);
		dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_0, 0x80);
#endif
	}

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_te_write(dphy, RX_SYS_3, 0x59);
#endif

	if (dphy->phy_type || input_freq <= 1500) {
		reg_val = dw_dphy_te_read(dphy, RX_SYS_7);
        reg_val |= (1 << 5);
		dw_dphy_te_write(dphy, RX_SYS_7, reg_val);
    }

    if (dphy->phy_id == MIPI_A_PHY) {
        reg_val = dw_dphy_te_read(dphy, RX_RX_CLKLANE_LANE_6);
        reg_val |= (1 << 5);
	    dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, reg_val);
    }

	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);
    udelay(1);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 1);

    udelay(600);

    //TODO set visys reg mipi_csi2x2_a_forcerxmode_0=0 这个先不要配置，应该不需要设置为0

    reg_val  = dw_dphy_if_read(dphy, dphy->sysreg_mipi_csi_ctrl);
    reg_val &= ~((1 << 25) | (7 << 20));
	dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, reg_val); //TODO set visys reg basedir_0 =1, set visys reg forcerxmode_0123=0
	return 0;
}

static void dw_dphy_combination_4lane_switch_master(struct dw_dphy_rx *dphy)
{
    u32 sys_reg_val = 0;
    sys_reg_val = dw_dphy_if_read(dphy, dphy->sysreg_mipi_csi_ctrl);
    sys_reg_val |= 1;
    dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, sys_reg_val);    //切换寄存器到master
}

static void dw_dphy_combination_4lane_switch_slave(struct dw_dphy_rx *dphy)
{
    u32 sys_reg_val = 0;
    sys_reg_val = dw_dphy_if_read(dphy, dphy->sysreg_mipi_csi_ctrl);
    sys_reg_val &= ~1;
    dw_dphy_if_write(dphy, dphy->sysreg_mipi_csi_ctrl, sys_reg_val);    //切换寄存器到slave
}

#if !IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
//用于dphy 两个2lane组合成4lane的模式下phy初始化
static int dw_dphy_12bit_combination_4lane_configure(struct dw_dphy_rx *dphy)
{
	u32 input_freq = dphy->dphy_freq;
    u32 phy_reg_val = 0;
	u8 range = 0;

    dw_dphy_pwr_down(dphy);
	dev_vdbg(&dphy->phy->dev, "12bit: PHY GEN 3: Freq: %u\n", input_freq);
	for (range = 0; (range < ARRAY_SIZE(range_gen3) - 1) &&
	     ((input_freq / 1000) > range_gen3[range].freq);
	     range++);


    //Define master PHY and slave PHY and configure each accordingly
    //(this choice needs to be coherent with wiring connections mentioned above):
	dw_dphy_combination_4lane_switch_master(dphy);
    dw_dphy_te_write(dphy, RX_RX_DUAL_PHY_0, 0x01);                 //2lane合并为4lane，设置master
	dw_dphy_combination_4lane_switch_slave(dphy);
	dw_dphy_te_write(dphy, RX_RX_DUAL_PHY_0, 0x00);                 //2lane合并为4lane，设置slave

    //Configure master PHY clock lane to drive long channel clock
	dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, RX_RX_CLKLANE_LANE_6);
    phy_reg_val |= (1 << 2);
	dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, phy_reg_val);                     //2lane合并为4lane，设置master

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val &= ~(1 << 2);
	dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, phy_reg_val);                     //2lane合并为4lane，设置slave

    //Configure both master and slave PHY data lanes to get clock from long channel:
	dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x508);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x508, phy_reg_val);                     //2lane合并为4lane，设置slave

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x508);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x508, phy_reg_val);                     //2lane合并为4lane，设置slave

    dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x708);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x708, phy_reg_val);                     //2lane合并为4lane，设置slave

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x708);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x708, phy_reg_val);                     //2lane合并为4lane，设置slave

    dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x908);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x908, phy_reg_val);                     //2lane合并为4lane，设置slave

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x908);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0x908, phy_reg_val);                     //2lane合并为4lane，设置slave

    dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0xB08);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0xB08, phy_reg_val);                     //2lane合并为4lane，设置slave

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0xB08);
    phy_reg_val |= (1 << 5);
	dw_dphy_te_write(dphy, 0xB08, phy_reg_val);                     //2lane合并为4lane，设置slave

    //Configure slave PHY clock lane to bypass long channel clock to DDR clock:
    dw_dphy_combination_4lane_switch_master(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x308);
    phy_reg_val &= ~(1 << 3);
	dw_dphy_te_write(dphy, 0x308, phy_reg_val);                     //2lane合并为4lane，设置slave

	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x308);
    phy_reg_val |= (1 << 3);
	dw_dphy_te_write(dphy, 0x308, phy_reg_val);                     //2lane合并为4lane，设置slave

    //Override slave PHY DDR clock lane enable (DPHYHSRX_div124 module):
	dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x0e1);
    phy_reg_val |= (1 << 1);
	dw_dphy_te_write(dphy, 0x0e1, phy_reg_val);                     //2lane合并为4lane，设置slave

    phy_reg_val = dw_dphy_te_read(dphy, RX_RX_CLKLANE_LANE_6);
    phy_reg_val |= (1 << 3);
	dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, phy_reg_val);                     //2lane合并为4lane，设置slave

    //Turn off slave PHY LP-RX clock lane:
    dw_dphy_combination_4lane_switch_slave(dphy);
    phy_reg_val = dw_dphy_te_read(dphy, 0x304);
    phy_reg_val &= ~0x1f;
    phy_reg_val |= 0xa;
	dw_dphy_te_write(dphy, 0x304, phy_reg_val);                     //2lane合并为4lane，设置slave


    //Configure both PHYs as described in“Start-Up Sequence” on page 81
    //until step enable_n and enableclk=1'b1 (skip step Configure register
    //0x307 to set rxclk_rxhs_pull_long_channel_if_rw signal (bit 8) to 1'b1).
    dw_dphy_gen3_12bit_configure(dphy, 1);


    //Set shutdownz=1'b1 in master and slave PHYs.
    dw_dphy_combination_4lane_switch_master(dphy);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 1);
    dw_dphy_combination_4lane_switch_slave(dphy);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 1);

    //Release reset for master PHY (rstz=1'b1).
    dw_dphy_combination_4lane_switch_master(dphy);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);

    udelay(1);
    //Release reset for slave PHY (rstz=1'b1).
    dw_dphy_combination_4lane_switch_slave(dphy);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);

    udelay(600);
    //Set forcerxmode_n in master and slave PHY = 1'b0.
    //???




#if 0
	dw_dphy_gen3_12bit_tc_power_up(dphy, range_gen3[range].hsfregrange);

	//uint8_t lp_time = dphy->lp_time;
	//dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_17, lp_time);//SET SETTLE TIME

    //位置1
	dw_dphy_te_write(dphy, RX_SYS_1, range_gen3[range].hsfregrange);
	dw_dphy_te_write(dphy, RX_SYS_0, 0x20);

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_2, 0xcc);
#else
    //used for asic
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_2, (u8)(range_gen3[range].osc_freq_target & 0xff));
#endif

	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_3,
			 (u8)(range_gen3[range].osc_freq_target >> 8));


#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_4, 0x21);         //TODO fpga val=0x21, chip val=0x11
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_5, 0x01);         //TODO 顺序位置配置可能需要调到位置1
#else
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_4, 0x11);         //TODO fpga val=0x21, chip val=0x11
	dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_5, 0x01);         //TODO 顺序位置配置可能需要调到位置1
	dw_dphy_te_write(dphy, RX_RX_CB_1, 0x02);                  //TODO used to chip
	dw_dphy_te_write(dphy, RX_RX_CB_2, 0x1 << 6);              //TODO used to chip
	dw_dphy_te_write(dphy, RX_SYS_9, 0x43);                    //TODO used to chip
	dw_dphy_te_write(dphy, RX_RX_CLKLANE_LANE_6, 0x1 << 7);    //TODO used to chip

    //for chip dphy->dphyglueiftester : csi2_0, csi2_1, csi2_2 -> 0x140, 0x144, 0x148
    reg_val  = dw_dphy_if_read(dphy, dphy->dphyglueiftester);
    reg_val |= (1 << 28) | (1 << 25);
	dw_dphy_if_write(dphy, dphy->dphyglueiftester, reg_val); //TODO set visys reg basedir_0 =1, set visys reg forcerxmode_0=1
#endif

	if (dphy->phy_type) {
		dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_1, 0x01);
		dw_dphy_te_write(dphy, RX_RX_STARTUP_OVR_0, 0x80);
	}

	dw_dphy_te_write(dphy, RX_SYS_3, 0x59);

	dw_dphy_te_write(dphy, RX_SYS_7, 0x38);
	if (dphy->phy_type || input_freq <= 1500)
		dw_dphy_te_write(dphy, RX_SYS_7, 0x38);

    //TODO set visys reg mipi_csi2x2_a_forcerxmode_0=0 这个先不要配置，应该不需要设置为0
    //reg_val &= ~(1 << 25);
	//dw_dphy_if_write(dphy, dphy->dphyglueiftester, reg_val);
#endif
	return 0;
}
#endif


static int dw_dphy_gen3_8bit_configure(struct dw_dphy_rx *dphy)
{
	u32 input_freq = dphy->dphy_freq;
	u8 data;
	u8 range = 0;

	dev_vdbg(&dphy->phy->dev, "8bit: PHY GEN 3: Freq: %u\n", input_freq);
	for (range = 0; (range < ARRAY_SIZE(range_gen3) - 1) &&
	     ((input_freq / 1000) > range_gen3[range].freq);
	     range++)
		;

	dw_dphy_te_write(dphy, RX_SKEW_CAL, dw_dphy_g118_settle(dphy));
	data = 1 << 7 | range_gen3[range].hsfregrange;
	dw_dphy_te_write(dphy, HSFREQRANGE_8BIT, data);
	dw_dphy_gen3_8bit_tc_power_up(dphy);

	return 0;
}

static int dw_dphy_gen2_configure(struct dw_dphy_rx *dphy)
{
	u32 input_freq = dphy->dphy_freq;
	u8 data;
	u8 range = 0;

	/* provide an initial active-high test clear pulse in TESTCLR  */
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 1, PHY_TESTCLR, 1);
	dw_dphy_write_msk(dphy, R_CSI2_DPHY_TST_CTRL0, 0, PHY_TESTCLR, 1);

	dev_vdbg(&dphy->phy->dev, "PHY GEN 2: Freq: %u\n", input_freq);
	for (range = 0; (range < ARRAY_SIZE(range_gen2) - 1) &&
	     ((input_freq / 1000) > range_gen2[range].freq); range++)
		;

	data = range_gen2[range].hsfregrange << 1;
	dw_dphy_te_write(dphy, HSFREQRANGE_8BIT, data);

	return 0;
}

static int dw_dphy_configure(struct dw_dphy_rx *dphy)
{

    bm_info("enter %s\n", __func__);
	dw_dphy_pwr_down(dphy);
    dphy->dphy_gen = GEN3;
	if (dphy->dphy_gen == GEN3) {
#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)
		dw_dphy_if_init(dphy);
#endif
        dphy->dphy_te_len = BIT12;
		if (dphy->dphy_te_len == BIT12)
			dw_dphy_gen3_12bit_configure(dphy, 1);
		else
			dw_dphy_gen3_8bit_configure(dphy);
	} else {
		dw_dphy_gen2_configure(dphy);
	}
	dw_dphy_pwr_up(dphy);

	return 0;
}

#if IS_ENABLED(CONFIG_DWC_MIPI_TC_DPHY_GEN3)

#if 0
int dw_dphy_if_set_idelay(struct dw_dphy_rx *dphy, u8 dly, u8 cells)
{
	u32 val = 0;

	dw_dphy_if_write(dphy, IDLYCFG, 0);
	dw_dphy_if_write(dphy, IDLYSEL, cells);
	dw_dphy_if_write(dphy, IDLYCNTINVAL, dly);

	/* Pulse Value Set */
	dw_dphy_if_write(dphy, IDLYCFG, 1);
	usleep_range(10, 20);
	dw_dphy_if_write(dphy, IDLYCFG, 0);

	/* Pulse IDELAY CTRL Reset */
	dw_dphy_if_write(dphy, DPHY1REGRSTN, 0);
	usleep_range(10, 20);
	dw_dphy_if_write(dphy, DPHY1REGRSTN, 1);

	/* Get Value*/
	val = dw_dphy_if_read(dphy, IDLYCNTOUTVAL);

	if (val != dly) {
		dev_vdbg(&dphy->phy->dev,
			 "odelay config failed, set %d get %d", dly, val);
		return -EINVAL;
	}

	return 0;
}

int dw_dphy_if_get_idelay(struct dw_dphy_rx *dphy)
{
	return dw_dphy_if_read(dphy, IDLYCNTOUTVAL);
}

int dw_dphy_if_set_idelay_lane(struct dw_dphy_rx *dphy, u8 dly, u8 lane)
{
	int cell;

	switch (lane) {
	case 0:
		for (cell = 3; cell <= 10; cell++)
			dw_dphy_if_set_idelay(dphy, dly, cell);
		break;
	case 1:
		for (cell = 14; cell <= 21; cell++)
			dw_dphy_if_set_idelay(dphy, dly, cell);
		break;
	case 2:
		for (cell = 24; cell <= 31; cell++)
			dw_dphy_if_set_idelay(dphy, dly, cell);
		break;
	case 3:
		for (cell = 34; cell <= 41; cell++)
			dw_dphy_if_set_idelay(dphy, dly, cell);
		break;
	case 4: /* ALL */
		dw_dphy_if_set_idelay(dphy, dly, 0x7F);
		break;
	default:
		dev_err(&dphy->phy->dev, "Lane Value not recognized\n");
		return -1;
	}
	return 0;
}
#endif
#endif

int dw_dphy_init(struct phy *phy)
{
	struct dw_dphy_rx *dphy = phy_get_drvdata(phy);

	dev_warn(&dphy->phy->dev, "Init DPHY.\n");

	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);
	dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 0);

	return 0;
}

static int dw_dphy_set_phy_state(struct dw_dphy_rx *dphy, u32 on)
{

	u8 hs_freq;

    bm_info("enter %s\n", __func__);
	dphy->lanes_config = dw_dphy_setup_config(dphy);

	if (dphy->dphy_te_len == BIT12)
		hs_freq = RX_SYS_1;
	else
		hs_freq = HSFREQRANGE_8BIT;

	if (on) {
		dw_dphy_configure(dphy);
		dev_vdbg(&dphy->phy->dev,
			 "HS Code: 0X%x\n", dw_dphy_te_read(dphy, hs_freq));
	} else {
		dw_dphy_write(dphy, R_CSI2_DPHY_SHUTDOWNZ, 0);
		dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);
	}

	return 0;
}

int dw_dphy_power_on(struct phy *phy)
{
	struct dw_dphy_rx *dphy = phy_get_drvdata(phy);

	dev_vdbg(&dphy->phy->dev, "DPHY Power ON\n");
    bm_info("enter %s\n", __func__);

    dphy->phy_type = 1;
	return dw_dphy_set_phy_state(dphy, 1);
}

int dw_dphy_power_off(struct phy *phy)
{
	struct dw_dphy_rx *dphy = phy_get_drvdata(phy);

	return dw_dphy_set_phy_state(dphy, 0);
}

int dw_dphy_reset(struct phy *phy)
{
	struct dw_dphy_rx *dphy = phy_get_drvdata(phy);

	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 0);
	usleep_range(100, 200);
	dw_dphy_write(dphy, R_CSI2_DPHY_RSTZ, 1);

	return 0;
}
