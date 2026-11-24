# Leroy-s_SSD1306_Library_for_Waveshare_RP2040_Devboard_Guide
This tutorial is for Waveshare RP2040 Zero's and their clones, but it should apply to more RP2040 Devboards (:

# DECIDING WHICH PINS TO USE

Pick an I2C pair from this diagram, both I2C1 and I2C0 are fine:

![Pinout Diagram for Waveshare RP2040 Zero](Assets/900px-RP2040-Zero-details-7.jpg)

For this guide, I'm using GPIO 8 for SDA and GPIO 9 for SCL, making an I2C0 pair. 

# SETTING THINGS UP IN YOUR CODE

In your code, include:

![#include's you need to add](Assets/image1.png)

and in your main:

![Pins you need to define and initialize](Assets/image2.png)

In your CMakeLists.txt file, make sure that add_executable includes:

![ssd1306.cpp in add_executable](Assets/image3.png)

and that your target_link_libraries has:

![hardware_i2c in target_link_libraries](Assets/image4.png)

Remember to put the two files from the Library folder in this repo into your project folder!

# USE

Start with: 

![Writing text example](image5.png)

ssd1306_text works by (x, y, "text"). Remember, 0, 0 is the top-left of the screen. 


