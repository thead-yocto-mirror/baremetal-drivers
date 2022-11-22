/*
 * Copyright (C) 2021 Alibaba Group Holding Limited
 * Author: liuyitong <yitong.lyt@alibaba-inc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _BM_CSI_IOCTL_H_
#define _BM_CSI_IOCTL_H_

#include <linux/ioctl.h>

enum {
    BMCSI_IOC_S_RESET = 0x100,
    BMCSI_IOC_S_POWER,
    BMCSI_IOC_G_POWER,
    BMCSI_IOC_S_CLOCK,
    BMCSI_IOC_G_CLOCK,
    BMCSI_IOC_S_STREAM,
    BMCSI_IOC_G_STREAM,
    BMCSI_IOC_S_FMT,
    BMCSI_IOC_G_FMT,
    BMCSI_IOC_S_VC_SELECT,
    BMCSI_IOC_G_VC_SELECT,
    BMCSI_IOC_IPI_START,
    BMCSI_IOC_IPI_STOP,
    BMCSI_IOC_S_LANE_CFG,
    BMCSI_IOC_S_HSA_CFG,
    BMCSI_IOC_S_HBP_CFG,
    BMCSI_IOC_S_HSD_CFG,
	BMCSI_IOC_WRITE_REG,
	BMCSI_IOC_READ_REG,
	BMCSI_IOC_READ_ARRAY,
	BMCSI_IOC_SET_PIXCLK,
    BMCSI_IOC_GET_PIXCLK,
    BMCSI_IOC_GET_ERROR,
    BMCSI_IOC_MAX,
};

struct bm_csi_reg_t {
	unsigned int offset;
	unsigned int value;
};

struct bm_csi_reg_arry_t {
	unsigned int start_addr;
	unsigned int words;
    unsigned int *buf;
};

struct csi_vc_select_context {
    unsigned int ipi_idx;
    unsigned int vc_ch;
};

struct dw_csi_err_mask {
    unsigned int src;
    unsigned int ipi1_phy_fatal_mask;
    unsigned int ipi2_phy_fatal_mask;
    unsigned int ipi3_phy_fatal_mask;
    unsigned int pkt_fatal;
    unsigned int frame_fatal;
    unsigned int phy;
    unsigned int line;
    unsigned int ipi;
    unsigned int ipi2;
    unsigned int ipi3;
    unsigned int bndry_frame_fatal;
    unsigned int seq_frame_fatal;
    unsigned int crc_frame_fatal;
    unsigned int pld_crc_fatal;
    unsigned int data_id;
    unsigned int ecc_corrected;
};

#endif /* _BM_CSI_IOCTL_H_*/
