#include "ssd1306.h"

#include <cstring>
#include <cstdlib>

static uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];


// Send one command to the SSD1306
static void command(
    i2c_inst_t *i2c,
    uint8_t address,
    uint8_t cmd
) {
    uint8_t data[2] = {
        0x00,
        cmd
    };

    i2c_write_blocking(
        i2c,
        address,
        data,
        2,
        false
    );
}


// Initialize the display
void ssd1306_init(
    i2c_inst_t *i2c,
    uint8_t address
) {
    sleep_ms(100);

    command(i2c, address, 0xAE); // Display OFF

    command(i2c, address, 0xD5); // Clock
    command(i2c, address, 0x80);

    command(i2c, address, 0xA8); // Multiplex
    command(i2c, address, 0x3F);

    command(i2c, address, 0xD3); // Display offset
    command(i2c, address, 0x00);

    command(i2c, address, 0x40); // Start line

    command(i2c, address, 0x8D); // Charge pump
    command(i2c, address, 0x14);

    command(i2c, address, 0xA1); // Segment remap
    command(i2c, address, 0xC8); // COM scan direction

    command(i2c, address, 0xDA);
    command(i2c, address, 0x12);

    command(i2c, address, 0x81); // Contrast
    command(i2c, address, 0xCF);

    command(i2c, address, 0xD9);
    command(i2c, address, 0xF1);

    command(i2c, address, 0xDB);
    command(i2c, address, 0x40);

    command(i2c, address, 0xA4); // Use display RAM
    command(i2c, address, 0xA6); // Normal display

    command(i2c, address, 0xAF); // Display ON

    ssd1306_clear();
    ssd1306_show(i2c, address);
}


// Clear our RAM buffer
void ssd1306_clear() {
    memset(buffer, 0, sizeof(buffer));
}


// Turn one pixel on/off in our RAM buffer
void ssd1306_pixel(
    int x,
    int y,
    bool on
) {
    if (
        x < 0 ||
        x >= SSD1306_WIDTH ||
        y < 0 ||
        y >= SSD1306_HEIGHT
    ) {
        return;
    }

    int index =
        x + (y / 8) * SSD1306_WIDTH;

    uint8_t mask =
        1 << (y % 8);

    if (on) {
        buffer[index] |= mask;
    } else {
        buffer[index] &= ~mask;
    }
}


// Draw a line using pixels
void ssd1306_line(
    int x0,
    int y0,
    int x1,
    int y1
) {
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;

    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int error = dx + dy;

    while (true) {

        ssd1306_pixel(
            x0,
            y0,
            true
        );

        if (
            x0 == x1 &&
            y0 == y1
        ) {
            break;
        }

        int e2 = 2 * error;

        if (e2 >= dy) {
            error += dy;
            x0 += sx;
        }

        if (e2 <= dx) {
            error += dx;
            y0 += sy;
        }
    }
}

// 5x7 font
// Each byte represents one vertical column of pixels.

static const uint8_t font[][5] = {

    // Space
    {0x00, 0x00, 0x00, 0x00, 0x00},

    // !
    {0x00, 0x00, 0x5F, 0x00, 0x00},

    // "
    {0x00, 0x07, 0x00, 0x07, 0x00},

    // #
    {0x14, 0x7F, 0x14, 0x7F, 0x14},

    // $
    {0x24, 0x2A, 0x7F, 0x2A, 0x12},

    // %
    {0x23, 0x13, 0x08, 0x64, 0x62},

    // &
    {0x36, 0x49, 0x55, 0x22, 0x50},

    // '
    {0x00, 0x05, 0x03, 0x00, 0x00},

    // (
    {0x00, 0x1C, 0x22, 0x41, 0x00},

    // )
    {0x00, 0x41, 0x22, 0x1C, 0x00},

    // *
    {0x14, 0x08, 0x3E, 0x08, 0x14},

    // +
    {0x08, 0x08, 0x3E, 0x08, 0x08},

    // ,
    {0x00, 0x50, 0x30, 0x00, 0x00},

    // -
    {0x08, 0x08, 0x08, 0x08, 0x08},

    // .
    {0x00, 0x60, 0x60, 0x00, 0x00},

    // /
    {0x20, 0x10, 0x08, 0x04, 0x02},

    // 0
    {0x3E, 0x51, 0x49, 0x45, 0x3E},

    // 1
    {0x00, 0x42, 0x7F, 0x40, 0x00},

    // 2
    {0x42, 0x61, 0x51, 0x49, 0x46},

    // 3
    {0x21, 0x41, 0x45, 0x4B, 0x31},

    // 4
    {0x18, 0x14, 0x12, 0x7F, 0x10},

    // 5
    {0x27, 0x45, 0x45, 0x45, 0x39},

    // 6
    {0x3C, 0x4A, 0x49, 0x49, 0x30},

    // 7
    {0x01, 0x71, 0x09, 0x05, 0x03},

    // 8
    {0x36, 0x49, 0x49, 0x49, 0x36},

    // 9
    {0x06, 0x49, 0x49, 0x29, 0x1E},

    // :
    {0x00, 0x36, 0x36, 0x00, 0x00},

    // ;
    {0x00, 0x56, 0x36, 0x00, 0x00},

    // <
    {0x08, 0x14, 0x22, 0x41, 0x00},

    // =
    {0x14, 0x14, 0x14, 0x14, 0x14},

    // >
    {0x00, 0x41, 0x22, 0x14, 0x08},

    // ?
    {0x02, 0x01, 0x51, 0x09, 0x06},

    // @
    {0x32, 0x49, 0x79, 0x41, 0x3E},

    // A-Z
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x03, 0x04, 0x78, 0x04, 0x03}, // Y
    {0x61, 0x51, 0x49, 0x45, 0x43}  // Z
};


void ssd1306_char(
    int x,
    int y,
    char c
) {
    // Our table begins at ASCII 32
    if (c < 32 || c > 90) {
        c = '?';
    }

    int fontIndex = c - 32;

    for (int column = 0; column < 5; column++) {

        uint8_t columnData =
            font[fontIndex][column];

        for (int row = 0; row < 7; row++) {

            if (columnData & (1 << row)) {
                ssd1306_pixel(
                    x + column,
                    y + row,
                    true
                );
            }
        }
    }
}


void ssd1306_text(
    int x,
    int y,
    const char *text
) {
    while (*text) {

        ssd1306_char(
            x,
            y,
            *text
        );

        // 5 pixels for character
        // + 1 pixel spacing
        x += 6;

        text++;
    }
}
// Send our RAM buffer to the OLED
void ssd1306_show(
    i2c_inst_t *i2c,
    uint8_t address
) {
    // Horizontal addressing mode
    command(i2c, address, 0x20);
    command(i2c, address, 0x00);

    // Columns 0-127
    command(i2c, address, 0x21);
    command(i2c, address, 0);
    command(i2c, address, 127);

    // Pages 0-7
    command(i2c, address, 0x22);
    command(i2c, address, 0);
    command(i2c, address, 7);

    // Send one page at a time
    for (int page = 0; page < 8; page++) {

        uint8_t data[129];

        // 0x40 means:
        // "the following bytes are display data"
        data[0] = 0x40;

        memcpy(
            &data[1],
            &buffer[page * 128],
            128
        );

        i2c_write_blocking(
            i2c,
            address,
            data,
            sizeof(data),
            false
        );
    }
}