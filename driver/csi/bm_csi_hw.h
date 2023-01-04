/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 * Author: liuyitong <yitong.lyt@alibaba-inc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _BM_CSI_HW_H_
#define _BM_CSI_HW_H_

#include <linux/cdev.h>
#include <linux/ioctl.h>
#include "dw-dphy-rx.h"
#include "dw-mipi-csi.h"
#include "dw-csi-data.h"
#include "dw-dphy-data.h"

struct bm_csi_drvdata {
	struct cdev cdev;
	dev_t devt;
	struct class *class;
	struct mutex mutex;
	unsigned int device_idx;
	void __iomem *base;
    struct regmap *csia_reg;
    struct regmap *visys_clk_reg;
	void __iomem *reset;
    phy_id_e phy_id;
    struct dw_dphy_rx *dphy;
    struct dw_csi csi_dev;
	struct dw_csih_pdata csi_pdata;
    struct dw_phy_pdata dphy_pdata;
	int irq_num;
	struct clk *pclk;
    struct clk *pixclk;
	struct clk *cfg_clk;
	struct platform_device *pdev;
    uint32_t csi_power_on_sta;
    wait_queue_head_t irq_wait;
	void *private;	// can be bm_csi_drvdata_private, but not use now
};

struct bm_csi_drvdata_private {
	int private_tmp;
};

int bm_csi_write_reg(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_read_reg(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_read_array(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_init(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_exit(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_reset(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_power(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_power(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_en_power(struct bm_csi_drvdata *drvdata);
int bm_csi_dis_power(struct bm_csi_drvdata *drvdata);
int bm_csi_set_clock(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_clock(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_pixclk(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_pixclk(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_stream(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_stream(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_error(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_fmt(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_fmt(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_vc_select(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_get_vc_select(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_ipi_enable(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_ipi_disable(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_lane_cfg(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_hsa(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_hbp(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_set_hsd(struct bm_csi_drvdata *drvdata, void *__user args);


/*csi dphy*/
int bm_csi_dphy_write_reg(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_dphy_init(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_dphy_uinit(struct bm_csi_drvdata *drvdata, void *__user args);
int bm_csi_dphy_reset(struct bm_csi_drvdata *drvdata, void *__user args);

void dw_csi_soc_reset(void);
int dw_csi_probe(struct platform_device *pdev);
int dw_csi_remove(struct platform_device *pdev);

int dw_csi_set_pixclk(uint32_t freq_hz);
int dw_csi_get_pixclk(uint32_t *freq_hz);

#endif /* _BM_CSI_HW_H_*/
