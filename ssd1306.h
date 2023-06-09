/****************************************************************************************************************
   .Title SSD1306 OLED Driver
  ***************************************************************************************************************
  File:	      ssd1306.h
  Modifier:   Ngo Le Tuyet Hoa
  Last Modified:    27th APR 2023
  ***************************************************************************************************************
  Copyright (C) 2023 https://github.com/tuyethoa1011
  This is a free software under the GNU license, you can redistribute it and/or modify it under the terms
  of the GNU General Public License version 3 as published by the Free Software Foundation.
  This software library is shared with public for educational purposes, without WARRANTY and Author is not liable for any damages caused directly
  or indirectly by this software, read more about this on the GNU General Public License.
  ***************************************************************************************************************
    .Usage: Basic setup for uses OLED Display
*/  

#ifndef SSD1306_H
#define SSD1306_H

//Includes
#include <string.h>
#include <stdlib.h>
#include "driver/i2c.h"
#include "esp_log.h"


//define ssd1306 buffer
#define SSD1306_BUFFER_SIZE SSD1306_WIDTH * SSD1306_HEIGHT / 8
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set, color depend on OLED
} SSD1306_COLOR_t;


//Define Color Fill
#define SSD1306_COLOR_BLACK 0x00 //black color
#define SSD1306_COLOR_WHITE 0x01 //white color



/*Private SSD1306 Structure*/
typedef struct {
	uint16_t Current_X;
	uint16_t Current_Y;
	uint8_t Inverted;
	uint8_t Initialized;
} SSD1306_t;

//Define SSD1306 width and length
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

//Success/Errors
#define SSD1306_OK 1
#define SSD1306_FAIL 0

//ACK/NACK
#define SSD1306_ACK 1
#define SSD1306_NACK

//OLED SSD1306 Address definition
#define SSD1306_OLED_ADDR 0x3C

//Command definition
//-------------------- Control byte ---------------------------
  #define SSD1306_CONTROL_BYTE_CMD_SINGLE    0x80 //Control byte dùng để thông báo các byte I2C OLED nhận tiếp theo nhận duy nhất 1 byte lệnh
  #define SSD1306_CONTROL_BYTE_CMD_STREAM    0x00 //Control byte dùng để thông báo các byte I2C OLED nhận tiếp theo là 1 luồng lệnh nhiều bytes
  #define SSD1306_CONTROL_BYTE_DATA_STREAM   0x40 //Control byte dùng để thông báo các byte I2C OLED nhập tiếp theo là 1 luồng stream toàn các byte dữ liệu (DATA STREAM)
  #define SSD1306_CONTROL_BYTE_DATA_SINGLE   0xC0 //Continuation bit = 1, D/C = 1100 0000
//-------------------------------------------------------------
  #define SSD1306_SET_MUX_RATIO     0xA8
  #define SSD1306_DISPLAY_OFFSET    0xD3
  #define SSD1306_DISPLAY_ON        0xAF
  #define SSD1306_DISPLAY_OFF       0xAE
  #define SSD1306_DISPLAY_ENT_DISP_ON   0xA4
  #define SSD1306_DISPLAY_IGNORE_RAM    0xA5
  #define SSD1306_DISPLAY_NORMAL        0xA6
  #define SSD1306_DISPLAY_INVERSE       0xA7
  #define SSD1306_DEACT_SCROLL      0x2E
  #define SSD1306_ACTIVE_SCROLL     0x2F

  #define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
  #define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26

  #define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
  #define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
  #define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A
  #define SSD1306_SET_START_LINE    0x40
  #define SSD1306_MEMORY_ADDR_MODE  0x20
  #define SSD1306_SET_COLUMN_ADDR   0x21
  #define SSD1306_SET_PAGE_ADDR     0x22
  #define SSD1306_SEG_REMAP         0xA0
  #define SSD1306_SEG_REMAP_OP      0xA1
  #define SSD1306_COM_SCAN_DIR      0xC0
  #define SSD1306_COM_SCAN_DIR_OP   0xC8
  #define SSD1306_COM_PIN_CONF      0xDA
  #define SSD1306_SET_CONTRAST      0x81
  #define SSD1306_SET_OSC_FREQ      0xD5
  #define SSD1306_SET_CHARGE_PUMP     0x8D
  #define SSD1306_SET_PRECHARGE     0xD9
  #define SSD1306_VCOM_DESELECT     0xDB



//Function prototype definition
void ssd1306_init(i2c_port_t i2c_num);
void ssd1306_clear(i2c_port_t i2c_num);
void ssd1306_normal_screen(i2c_port_t i2c_num, int inv);
void ssd1306_SetContrast(i2c_port_t i2c_num, uint8_t value);
void ssd1306_string_text(const void *arg_text, i2c_port_t i2c_num);
void ssd1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);
void ssd1306_drawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, uint16_t color);
void ssd1306_UpdateScreen(i2c_port_t i2c_num);
void ssd1306_setCursor(uint16_t x, uint16_t y); //set x,y coordinate
void ssd1306_Fill(SSD1306_COLOR_t color);
void ssd1306_Stopscroll(i2c_port_t i2c_num);
char ssd1306_Putchar(char ch, SSD1306_COLOR_t color);
void ssd1306_PutString(const void *arg_text, SSD1306_COLOR_t color);
void ssd1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR_t color);
void ssd1306_DrawRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR_t color);
void ssd1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t color);
void SSD1306_DrawTriangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR_t color);
/*ON/OFF function use for saving energy purpose*/
void ssd1306_on(void);
void ssd1306_off(void);

void ssd1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);
void ssd1306_DrawFilledRectangle(uint8_t x, uint8_t y, uint8_t w, uint8_t h, SSD1306_COLOR_t color);
void ssd1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

void ssd1306_ScrollRight(uint8_t start_row, uint8_t end_row);
void ssd1306_ScrollLeft(uint8_t start_row, uint8_t end_row);

void ssd1306_ScrolldiagrRight(uint8_t start_row, uint8_t end_row);
void SSD1306_ScrolldiagrLeft(uint8_t start_row, uint8_t end_row);

#endif                                      
