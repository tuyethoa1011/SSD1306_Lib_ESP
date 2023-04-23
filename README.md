# C/C++ library for SSD1306 OLED 128x64 pixels 

# Description
SSD1306 library to support express-dif framework.

# Basic functions build plan:
- ssd1306_init (DONE)
- ssd1306_clear (DONE)
- ssd1306_normal_screen/ inverse_screen (DONE but not testing)
- ssd1306_setcursor (DONE)
- sdd1306_string_text (ver1.0 i will update this later because i want it to addapt with coordinator[x,y] change)
- ssd1306_drawpixel (DONE)
- ssd1306_drawBitmap (DONE) - need improvement
- ssd1306_scroll (Stop scroll - Done others not done) - Need testing
- ssd1306_setcontrast (DONE but not testing)
- ssd1306_updatescreen (DONE)
- ss1306_fill (DONE)
- ssd1306_drawLine
- ssd1306_drawRect
- ssd1306_drawCircle

# Reference
  <ul> 
      <li>
        Datasheet: <a href = "https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf">SSD1306.pdf </a>
      </li>
  </ul>
  
  # Author
  - Ngo Le Tuyet Hoa (Sam)
 # Dependencies
  - font8x8_basic
  - bitmap
  - bitmap converter software
 # References
 Huge thanks to STM32_lib SSD1306: https://github.com/afiskon/stm32-ssd1306/tree/master/ssd1306
