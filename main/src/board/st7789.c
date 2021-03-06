/*
 * st7789.c
 *
 *  Created on: 2018-03-16 12:30
 *      Author: Jack Chen <redchenjs@live.com>
 */

#include <string.h>

#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"

#include "chip/spi.h"
#include "board/st7789.h"

#ifdef CONFIG_SCREEN_PANEL_ST7789

#define TAG "st7789"

static spi_transaction_t hspi_trans[2];

void st7789_init_board(void)
{
    memset(hspi_trans, 0x00, sizeof(hspi_trans));

    gpio_set_direction(CONFIG_SCREEN_PANEL_DC_PIN,  GPIO_MODE_OUTPUT);
    gpio_set_direction(CONFIG_SCREEN_PANEL_RST_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(CONFIG_SCREEN_PANEL_DC_PIN,  0);
    gpio_set_level(CONFIG_SCREEN_PANEL_RST_PIN, 0);

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_8_BIT,
        .freq_hz         = 20000,
        .speed_mode      = LEDC_HIGH_SPEED_MODE,
        .timer_num       = LEDC_TIMER_0,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = CONFIG_SCREEN_PANEL_BL_PIN,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledc_channel);

    ledc_fade_func_install(0);

    ESP_LOGI(TAG, "initialized, bl: %d, dc: %d, rst: %d", CONFIG_SCREEN_PANEL_BL_PIN, CONFIG_SCREEN_PANEL_DC_PIN, CONFIG_SCREEN_PANEL_RST_PIN);
}

void st7789_set_backlight(uint8_t val)
{
    ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, val, 500);
    ledc_fade_start(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
}

void st7789_setpin_dc(spi_transaction_t *t)
{
    int dc = (int)t->user;
    gpio_set_level(CONFIG_SCREEN_PANEL_DC_PIN, dc);
}

void st7789_setpin_reset(uint8_t val)
{
    gpio_set_level(CONFIG_SCREEN_PANEL_RST_PIN, val);
}

void st7789_write_cmd(uint8_t cmd)
{
    hspi_trans[0].length = 8;
    hspi_trans[0].tx_buffer = &cmd;
    hspi_trans[0].user = (void*)0;

    spi_device_transmit(hspi, &hspi_trans[0]);
}

void st7789_write_data(uint8_t data)
{
    hspi_trans[0].length = 8;
    hspi_trans[0].tx_buffer = &data;
    hspi_trans[0].user = (void*)1;

    spi_device_transmit(hspi, &hspi_trans[0]);
}

void st7789_write_buff(uint8_t *buff, uint32_t n)
{
    hspi_trans[0].length = n * 8;
    hspi_trans[0].tx_buffer = buff;
    hspi_trans[0].user = (void*)1;

    spi_device_transmit(hspi, &hspi_trans[0]);
}

void st7789_refresh_gram(uint8_t *gram)
{
    hspi_trans[0].length = 8,
    hspi_trans[0].tx_data[0] = 0x2C;    // Set Write RAM
    hspi_trans[0].user = (void*)0;
    hspi_trans[0].flags = SPI_TRANS_USE_TXDATA;

    hspi_trans[1].length = ST7789_SCREEN_WIDTH*ST7789_SCREEN_HEIGHT*2*8;
    hspi_trans[1].tx_buffer = gram;
    hspi_trans[1].user = (void*)1;

    // Queue all transactions.
    for (int x=0; x<2; x++) {
        spi_device_queue_trans(hspi, &hspi_trans[x], portMAX_DELAY);
    }
}
#endif
