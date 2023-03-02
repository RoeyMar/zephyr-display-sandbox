/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

static const struct gpio_dt_spec rs_pin = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(a0), gpios, {0});
static const struct gpio_dt_spec reset_pin = GPIO_DT_SPEC_GET_OR(DT_NODELABEL(nrst), gpios, {0});

void st7565_comm(struct device *spi, const struct spi_config *spi_cfg, unsigned char data) {
	gpio_pin_set_dt(&rs_pin, 0);
	const struct spi_buf_set tx = {
		.buffers = &(const struct spi_buf){
			.buf = &data,
			.len = 1,
		},
		.count = 1,
	};
	spi_write(spi, spi_cfg, &tx);
}

void main(void)
{
	printk("Program start: %s\n", CONFIG_BOARD);

	gpio_pin_configure_dt(&rs_pin, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&reset_pin, GPIO_OUTPUT_INACTIVE);

	struct spi_config spi_cfg = {};
	struct device *spi = DEVICE_DT_GET(DT_NODELABEL(spi2));
	spi_cfg.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER; // TODO half-duplex
	spi_cfg.frequency = 1000000U;
	printk("SPI init complete\n");

	// first display reset
	gpio_pin_set_dt(&reset_pin, 0);
	k_msleep(100);
	gpio_pin_set_dt(&reset_pin, 1);
	k_msleep(100);
	printk("GPIO init complete\n");

	// init
	st7565_comm(spi, &spi_cfg, 0xA0);
	st7565_comm(spi, &spi_cfg, 0xAE);
	st7565_comm(spi, &spi_cfg, 0xC0);
	st7565_comm(spi, &spi_cfg, 0xA2);
	st7565_comm(spi, &spi_cfg, 0x2F);
	st7565_comm(spi, &spi_cfg, 0x26);
	st7565_comm(spi, &spi_cfg, 0x81);
	st7565_comm(spi, &spi_cfg, 0x11);
	st7565_comm(spi, &spi_cfg, 0xAF);
	printk("Disp config complete\n");




}
