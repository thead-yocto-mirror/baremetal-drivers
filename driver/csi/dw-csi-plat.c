// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018-2019 Synopsys, Inc. and/or its affiliates.
 *
 * Synopsys DesignWare MIPI CSI-2 Host controller driver.
 * Platform driver
 *
 * Author: Luis Oliveira <luis.oliveira@synopsys.com>
 */

#include "dw-csi-data.h"
#include "dw-dphy-data.h"
#include "dw-csi-plat.h"

#include "bm_csi_hw.h"

const struct mipi_dt csi_dt[] = {
	{
		.hex = CSI_2_YUV420_8,
		.name = "YUV420_8bits",
	}, {
		.hex = CSI_2_YUV420_10,
		.name = "YUV420_10bits",
	}, {
		.hex = CSI_2_YUV420_8_LEG,
		.name = "YUV420_8bits_LEGACY",
	}, {
		.hex = CSI_2_YUV420_8_SHIFT,
		.name = "YUV420_8bits_SHIFT",
	}, {
		.hex = CSI_2_YUV420_10_SHIFT,
		.name = "YUV420_10bits_SHIFT",
	}, {
		.hex = CSI_2_YUV422_8,
		.name = "YUV442_8bits",
	}, {
		.hex = CSI_2_YUV422_10,
		.name = "YUV442_10bits",
	}, {
		.hex = CSI_2_RGB444,
		.name = "RGB444",
	}, {
		.hex = CSI_2_RGB555,
		.name = "RGB555",
	}, {
		.hex = CSI_2_RGB565,
		.name = "RGB565",
	}, {
		.hex = CSI_2_RGB666,
		.name = "RGB666",
	}, {
		.hex = CSI_2_RGB888,
		.name = "RGB888",
	}, {
		.hex = CSI_2_RAW6,
		.name = "RAW6",
	}, {
		.hex = CSI_2_RAW7,
		.name = "RAW7",
	}, {
		.hex = CSI_2_RAW8,
		.name = "RAW8",
	}, {
		.hex = CSI_2_RAW10,
		.name = "RAW10",
	}, {
		.hex = CSI_2_RAW12,
		.name = "RAW12",
	}, {
		.hex = CSI_2_RAW14,
		.name = "RAW14",
	}, {
		.hex = CSI_2_RAW16,
		.name = "RAW16",
	},
};

static struct mipi_fmt *
find_dw_mipi_csi_format(uint32_t mbus_code)
{
	unsigned int i;

	pr_info("%s entered mbus: 0x%x\n", __func__, mbus_code);

	for (i = 0; i < ARRAY_SIZE(dw_mipi_csi_formats); i++)
		if (mbus_code == dw_mipi_csi_formats[i].mbus_code) {
			pr_info("Found mbus 0x%x\n", dw_mipi_csi_formats[i].mbus_code);
			return &dw_mipi_csi_formats[i];
		}
	return NULL;
}

int dw_mipi_csi_enum_mbus_code(int index, uint32_t *code)
{
	if (index != 0)
		return -EINVAL;

	*code = dw_mipi_csi_formats[index].mbus_code;
	return 0;
}

static struct mipi_fmt *
dw_mipi_csi_try_format(uint32_t mbus_code)
{
	struct mipi_fmt *fmt;

	fmt = find_dw_mipi_csi_format(mbus_code);
	if (!fmt)
		fmt = &dw_mipi_csi_formats[0];

	return fmt;
}

struct mipi_fmt *
dw_mipi_csi_get_format(struct dw_csi *dev)
{
	dev_vdbg(dev->dev,
		"%s got v4l2_mbus_pixelcode. 0x%x\n", __func__,
		dev->fmt->mbus_code);
	dev_vdbg(dev->dev,
		"%s got width. 0x%x\n", __func__,
		dev->fmt->width);
	dev_vdbg(dev->dev,
		"%s got height. 0x%x\n", __func__,
		dev->fmt->height);
    return dev->fmt;
}

int
dw_mipi_csi_set_fmt(struct platform_device *pdev,
		    uint32_t mbus_code, uint32_t width, uint32_t height)
{
    struct bm_csi_drvdata *drvdata  = platform_get_drvdata(pdev);
    struct dw_csi *dev = &drvdata->csi_dev;

	struct mipi_fmt *dev_fmt = NULL;
	int i;
	dev_vdbg(dev->dev,
		"%s got mbus_pixelcode. 0x%x\n", __func__,
		mbus_code);

	dev_fmt = dw_mipi_csi_try_format(mbus_code);

	dev_vdbg(dev->dev,
		"%s got v4l2_mbus_pixelcode. 0x%x\n", __func__,
		dev_fmt->mbus_code);
	if (!dev_fmt)
		return -EINVAL;

	if (dev_fmt) {
		dev->fmt->mbus_code =  dev_fmt->mbus_code;
        dev->fmt->width = width;
        dev->fmt->height = height;
		dw_mipi_csi_set_ipi_fmt(dev);
	}
	dev_vdbg(dev->dev, "Width: %d, Height: %d of Demo\n", width, height);
	if (width > 0 && height > 0) {
        dw_mipi_csi_fill_timings(dev, width, height);
/*
		demo_writel(dev,
			    DEMO_ACTIVE_WIDTH, fmt->format.width);
		demo_writel(dev,
			    DEMO_ACTIVE_HEIGHT, fmt->format.height);
*/
		dev_vdbg(dev->dev,
			 "(core/demosaic) : width=%d/%d, height=%d/%d\n",
			 dev->hw.htotal - (dev->hw.hbp
					   + dev->hw.hsd
					   + dev->hw.hsa),
			 width,
			 dev->hw.vactive,
			 height);
		dev_vdbg(dev->dev,
			 "(core/demosaic) : width=%d/%d, height=%d/%d\n",
			 dev->hw.htotal - (dev->hw.hbp
					   + dev->hw.hsd
					   + dev->hw.hsa),
			 width,
			 dev->hw.vactive,
			 height);

	} else {
		dev_vdbg(dev->dev, "%s unacceptable values 0x%x.\n",
		       __func__, width);
		dev_vdbg(dev->dev, "%s unacceptable values 0x%x.\n",
		       __func__, height);
		dev_vdbg(dev->dev, "%s unacceptable values 0x%x.\n",
		       __func__, width);
		dev_vdbg(dev->dev, "%s unacceptable values 0x%x.\n",
		       __func__, height);
		return -EINVAL;
	}

	for (i = 0; i < ARRAY_SIZE(csi_dt); i++)
		if (csi_dt[i].hex == dev->ipi_dt) {
			dev_vdbg(dev->dev, "Using data type %s\n",
				 csi_dt[i].name);
			dev_vdbg(dev->dev, "Using data type %s\n",
				 csi_dt[i].name);
		}
	return 0;
}

int
dw_mipi_csi_get_fmt(struct platform_device *pdev,
                    struct mipi_fmt *fmt)
{
    struct bm_csi_drvdata *drvdata  = platform_get_drvdata(pdev);
    struct dw_csi *dev = &drvdata->csi_dev;

    struct mipi_fmt *mf = dw_mipi_csi_get_format(dev);
	if (!mf)
		return -EINVAL;

	mutex_lock(&dev->lock);
	*fmt = *mf;
	mutex_unlock(&dev->lock);

	return 0;
}

int dw_mipi_csi_s_power(struct dw_csi *dev, int on)
{
	dev_vdbg(dev->dev, "%s: on=%d\n", __func__, on);

	if (on) {
        dw_dphy_power_on(dev->phy);
		dw_mipi_csi_hw_stdby(dev);
		dw_mipi_csi_start(dev);
	} else {
        dw_dphy_power_off(dev->phy);
		dw_mipi_csi_mask_irq_power_off(dev);
		/* reset data type */
		dev->ipi_dt = 0x0;
	}
	return 0;
}

int dw_mipi_csi_log_status(struct dw_csi *dev)
{
	dw_mipi_csi_dump(dev);

	return 0;
}

#if IS_ENABLED(CONFIG_VIDEO_ADV_DEBUG)
int
dw_mipi_csi_g_register(struct v4l2_subdev *sd, uint32_t reg)
{
    struct bm_csi_drvdata *drvdata  = platform_get_drvdata(pdev);
    struct dw_csi *dev = &drvdata->csi_dev;

	dev_vdbg(dev->dev, "%s: reg=%llu\n", __func__, reg);
	reg->val = dw_mipi_csi_read(dev, reg);

	return 0;
}
#endif

#if  0
static int dw_mipi_csi_init_cfg(struct v4l2_subdev *sd,
				struct v4l2_subdev_pad_config *cfg)
{
	struct v4l2_mbus_framefmt *format =
	    v4l2_subdev_get_try_format(sd, cfg, 0);

	format->colorspace = V4L2_COLORSPACE_SRGB;
	format->code = MEDIA_BUS_FMT_RGB888_1X24;
	format->field = V4L2_FIELD_NONE;

	return 0;
}
#endif

unsigned csi_poll(struct file *file, struct poll_table_struct *wait)
{
    unsigned int mask = 0;
	struct bm_csi_drvdata *drvdata = file->private_data;
	struct dw_csi *csi_dev = &drvdata->csi_dev;
    struct dw_csi_err_mask *err_mask = &csi_dev->error_mask;

    poll_wait(file, &drvdata->irq_wait, wait);

    if (csi_dev->has_error) {
        csi_dev->has_error = 0;
        mask |= POLLIN | POLLRDNORM;
    }

    return mask;
}


static irqreturn_t dw_mipi_csi_irq1(int irq, void *dev_id)
{

	struct bm_csi_drvdata *drvdata = dev_id;
	struct dw_csi *csi_dev = &drvdata->csi_dev;

	dw_mipi_csi_irq_handler(csi_dev);
    wake_up_interruptible(&drvdata->irq_wait);

	return IRQ_HANDLED;
}

extern int k_bm_visys_write_reg(uint32_t offset, uint32_t value);
extern int k_bm_visys_read_reg(uint32_t offset, uint32_t *value);

void dw_csi_soc_reset(void)
{
#define VISYS_SW_RST 0x100
    uint32_t  reg_val = 0;
    k_bm_visys_read_reg(VISYS_SW_RST, &reg_val);
    reg_val &= ~(1 << 16);
    k_bm_visys_write_reg(VISYS_SW_RST, reg_val);
    reg_val |= 1 << 16;
    k_bm_visys_write_reg(VISYS_SW_RST, reg_val);
}

#define MIPI_CSI_PIXELCLK 0x30
#define MIPI_CSI_PIXELSRC_CLK 2376000000 //hz

int dw_csi_set_pixclk(uint32_t freq_hz)
{
    uint32_t reg_val = 0;
    uint32_t div = (MIPI_CSI_PIXELSRC_CLK + (MIPI_CSI_PIXELSRC_CLK % freq_hz)) / freq_hz;

    if (freq_hz > MIPI_CSI_PIXELSRC_CLK || div > 0xf) {
        return  -1;
    }

    k_bm_visys_read_reg(MIPI_CSI_PIXELCLK, &reg_val);
    reg_val &= ~0xf;
    reg_val |= div;
    k_bm_visys_write_reg(MIPI_CSI_PIXELCLK, reg_val);

    return 0;
}

int dw_csi_get_pixclk(uint32_t *freq_hz)
{
    uint32_t reg_val = 0;
    uint32_t div = 0;
    k_bm_visys_read_reg(MIPI_CSI_PIXELCLK, &reg_val);
    div = reg_val & 0xf;
    *freq_hz = MIPI_CSI_PIXELSRC_CLK / div;
    return 0;
}

static const struct of_device_id dw_mipi_csi_of_match[];

int dw_csi_probe(struct platform_device *pdev)
{

	struct device *dev = &pdev->dev;
	struct dw_csi *csi;
    struct dw_dphy_rx *dphy;
    struct dw_csih_pdata *pdata;
    struct bm_csi_drvdata *drvdata;
	int ret;

    if (!IS_ENABLED(CONFIG_OF))

	dev_vdbg(dev, "Probing started\n");

	/* Resource allocation */
    drvdata  = platform_get_drvdata(pdev);

    csi = &drvdata->csi_dev;
    dphy = drvdata->dphy;
    pdata = &drvdata->csi_pdata;

	mutex_init(&csi->lock);
	spin_lock_init(&csi->slock);
	csi->dev = dev;

    /*set csi phy*/
    csi->phy = dphy->phy;
	csi->base_address = drvdata->base;
	if (IS_ERR(csi->base_address)) {
		dev_err(dev, "base address not set.\n");
		return PTR_ERR(csi->base_address);
	}

	csi->ctrl_irq_number = drvdata->irq_num;
	if (csi->ctrl_irq_number < 0) {
		dev_err(dev, "irq number %d not set.\n", csi->ctrl_irq_number);
		ret = csi->ctrl_irq_number;
		goto end;
	}

	ret = devm_request_irq(dev, csi->ctrl_irq_number,
			       dw_mipi_csi_irq1, IRQF_SHARED,
			       dev_name(dev), drvdata);
	if (ret) {
		dev_err(dev, "irq csi %d failed\n", pdata->id);
		goto end;
	}

	csi->fmt = &dw_mipi_csi_formats[0];
	csi->fmt->mbus_code = dw_mipi_csi_formats[0].mbus_code;
/*
 to do pad init
*/
    csi->hw.num_lanes = pdata->lanes;
	csi->hw.pclk = pdata->pclk;
	csi->hw.fps = pdata->fps;
	csi->hw.dphy_freq = pdata->hs_freq;
	csi->hw.ipi_mode = CAMERA_TIMING;
    csi->hw.ipi_color_mode = COLOR16;

    init_waitqueue_head(&drvdata->irq_wait);
    //csi soc reset
    dw_csi_soc_reset();
	dw_csi_create_capabilities_sysfs(pdev);
	dw_mipi_csi_get_version(csi);
	dw_mipi_csi_specific_mappings(csi);
	dw_mipi_csi_mask_irq_power_off(csi);

	dev_vdbg(dev, "DW MIPI CSI-2 Host registered successfully HW v%u.%u\n",
		 csi->hw_version_major, csi->hw_version_minor);

	ret = phy_reset(csi->phy);
    if (ret) {
		dev_err(&csi->phy->dev, "phy init failed --> %d\n", ret);
		goto end;
	}
	ret = phy_init(csi->phy);
	if (ret) {
		dev_err(&csi->phy->dev, "phy init failed --> %d\n", ret);
		goto end;
	}

	return 0;
end:
	return ret;
}

int dw_csi_remove(struct platform_device *pdev)
{
    struct bm_csi_drvdata *drvdata;
    drvdata  = platform_get_drvdata(pdev);
    dw_csi_remove_capabilities_sysfs(pdev);
	//struct dw_csi *mipi_csi = &drvdata->csi_dev;
    //csi soc reset
    dw_csi_soc_reset();
    dev_vdbg(&pdev->dev, "DW MIPI CSI-2 Host module removed\n");

	return 0;
}

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Luis Oliveira <luis.oliveira@synopsys.com>");
MODULE_DESCRIPTION("Synopsys DesignWare MIPI CSI-2 Host Platform driver");
