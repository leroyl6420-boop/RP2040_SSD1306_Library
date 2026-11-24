#pragma once

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

void ssd1306_init(i2c_inst_t *i2c, uint8_t address);

void ssd1306_clear();

void ssd1306_pixel(int x, int y, bool on);

void ssd1306_line(int x0, int y0, int x1, int y1);

void ssd1306_show(i2c_inst_t *i2c, uint8_t address);

void ssd1306_char(
    int x,
    int y,
    char c
);

void ssd1306_text(
    int x,
    int y,
    const char *text
);