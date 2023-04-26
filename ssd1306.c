#include "ssd1306.h"
#include "font8x8_basic.h"

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];


/*Private Variable*/
static SSD1306_t SSD1306;

void ssd1306_init(i2c_port_t i2c_num)
{ static const char *TAG = "i2c-lib";
	esp_err_t espRc;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR<< 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_STREAM, true);

	i2c_master_write_byte(cmd, SSD1306_SET_CHARGE_PUMP, true);
	i2c_master_write_byte(cmd, 0x14, true);

	i2c_master_write_byte(cmd, SSD1306_SEG_REMAP_OP , true); // reverse left-right mapping
	i2c_master_write_byte(cmd, SSD1306_COM_SCAN_DIR_OP , true); // reverse up-bottom mapping

	i2c_master_write_byte(cmd, SSD1306_DISPLAY_NORMAL  , true);
	i2c_master_write_byte(cmd, SSD1306_DISPLAY_ON , true);
	//i2c_master_write_byte(cmd, OLED_CMD_DISPLAY_OFF, true);

	i2c_master_stop(cmd);

	espRc = i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS); //config thời gian delay thành 1s
	if (espRc == ESP_OK) {
		SSD1306.Initialized = 1;
		ESP_LOGI(TAG, "OLED configured successfully");
	} else {
		ESP_LOGE(TAG, "OLED configuration failed. code: 0x%.2X", espRc);
	}
	i2c_cmd_link_delete(cmd);

	/*Clear screen*/
	ssd1306_Fill(SSD1306_COLOR_BLACK);

	/*Update Screen*/
	ssd1306_UpdateScreen(i2c_num);

	/*Set default Value*/
	SSD1306.Current_X = 0;
	SSD1306.Current_Y = 0;
	
	//Set defdfault display screen
	SSD1306.Inverted = 0; //Display normal mode
}
//ssd1306_init done

void ssd1306_clear(i2c_port_t i2c_num)
{
	ssd1306_Fill(0);
	ssd1306_UpdateScreen(i2c_num);	
}

void ssd1306_Fill(SSD1306_COLOR_t color) 
{
	/*Add color to buffer*/
	memset(SSD1306_Buffer, (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(SSD1306_Buffer));
}

void ssd1306_Stopscroll(i2c_port_t i2c_num)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, SSD1306_ACK);
	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_SINGLE , SSD1306_ACK);
	i2c_master_write_byte(cmd,SSD1306_DEACT_SCROLL , SSD1306_ACK); 
	i2c_master_stop(cmd);
	
	i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd); 
}

//Thuat toan ve line: https://vi.wikipedia.org/wiki/Gi%E1%BA%A3i_thu%E1%BA%ADt_Bresenham_v%E1%BA%BD_%C4%91o%E1%BA%A1n_th%E1%BA%B3ng
//https://blog.luyencode.net/thuat-toan-breshenham/
//https://www.iostream.vn/giai-thuat-lap-trinh/thuat-toan-ve-duong-thang-bresenham-p2BLE1

/* Example Usage:
 	ssd1306_DrawLine(64,0,64,60,1); //x0,y0,x1,y1
    ssd1306_DrawLine(0,32,128,32,1);
    ssd1306_DrawLine(0,0,128,64,1);
    ssd1306_DrawLine(128,0,0,64,1);

    ssd1306_UpdateScreen(I2C_MASTER_NUM);

	Result visualization:
			  * * * 
			   ***
			*********
			   ***
			  * * *
*/
void ssd1306_DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, SSD1306_COLOR_t color)
{	
	uint8_t dx, dy, sx, sy, err, e2, i, tmp;
	//sx: amount to scale in x
	//sy: amount to scale in y
	//dx: x coordinator
	//dy: y coordinator

	/*Limit overflow*/
	if(x0 >= SSD1306_WIDTH)
	{
		x0 = SSD1306_WIDTH - 1;
	} 
	if(x1 >= SSD1306_WIDTH)
	{
		x1 = SSD1306_WIDTH - 1;
	}
	if(y0 >= SSD1306_HEIGHT)
	{
		y0 = SSD1306_HEIGHT - 1;
	}
	if(y1 >= SSD1306_HEIGHT)
	{
		y1 = SSD1306_HEIGHT - 1;
	}

	dx = (x0 < x1) ? (x1 - x0) : (x0 - x1); 
	dy = (y0 < y1) ? (y1 - y0) : (y0 - y1); 
	sx = (x0 < x1) ? 1 : -1; 
	sy = (y0 < y1) ? 1 : -1; 
	err = ((dx > dy) ? dx : -dy) / 2; 

	if (dx == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Vertical line */
		for (i = y0; i <= y1; i++) {
			ssd1306_DrawPixel(x0, i, color);
		}
		
		/* Return from function */
		return;
	}
	
	if (dy == 0) {
		if (y1 < y0) {
			tmp = y1;
			y1 = y0;
			y0 = tmp;
		}
		
		if (x1 < x0) {
			tmp = x1;
			x1 = x0;
			x0 = tmp;
		}
		
		/* Horizontal line */
		for (i = x0; i <= x1; i++) {
			ssd1306_DrawPixel(i, y0, color);
		}
		
		/* Return from function */
		return;
	}
	
	while (1) {
		ssd1306_DrawPixel(x0, y0, color);
		if (x0 == x1 && y0 == y1) {
			break;
		}
		e2 = err; 
		if (e2 > -dx) {
			err -= dy;
			x0 += sx;
		} 
		if (e2 < dy) {
			err += dx;
			y0 += sy;
		} 
	}
}



void ssd1306_normal_display(i2c_port_t i2c_num, int inv)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, SSD1306_ACK);
	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_SINGLE , SSD1306_ACK);

	if(inv) {
		i2c_master_write_byte(cmd,SSD1306_DISPLAY_INVERSE, SSD1306_ACK); 
	} else {
		i2c_master_write_byte(cmd,SSD1306_DISPLAY_NORMAL, SSD1306_ACK); 
	}

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd); 
}

//Value with format: 0xXX; reset: 0x7F
void ssd1306_SetContrast(i2c_port_t i2c_num, uint8_t value)
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, SSD1306_ACK);
	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_STREAM , SSD1306_ACK);
	
	i2c_master_write_byte(cmd,SSD1306_SET_CONTRAST,SSD1306_ACK);
	i2c_master_write_byte(cmd,value,SSD1306_ACK);

	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}

char ssd1306_Putchar(char ch, SSD1306_COLOR_t color) //hiện tại font đanh được set cứng với kích thước 8x8
{
	uint16_t height = 0, width = 0;
	uint8_t b;
	//font hiện tại đang được set cứng với kích thước 8x8
	for(height=0;height<8;height++)  
	{	
		b = font8x8_basic[(uint8_t)ch][height];
		for(width = 0;width<8;width++) 
		{	
			//shift right là để đẩy bit sang phải
			//and với 0x01 mục đích là để lấy bit ở trọng số thấp nhẩt sau khi được đẩy để xét set pixel hoặc not set pixel
			if((b>>width) & 0x01) {
				ssd1306_DrawPixel(SSD1306.Current_X + width,SSD1306.Current_Y + height,color);
			} else {
				ssd1306_DrawPixel(SSD1306.Current_X + width,SSD1306.Current_Y + height,!color);
			}
		}
	}

	/*Increase pointer*/
	SSD1306.Current_X += 8; 
	return ch;
}

void ssd1306_PutString(const void *arg_text, SSD1306_COLOR_t color)
{
	char *text = (char*)arg_text;
	uint8_t text_len = strlen(text);

	uint8_t index = 0;

	while(index<text_len)
	{	
		if(text[index] == '\n') 
		{	
			SSD1306.Current_X = 0; 
			SSD1306.Current_Y += 8;
		} else {
			ssd1306_Putchar(text[index],color);
		}
		index++;
	}

}



void ssd1306_string_text(const void *arg_text, i2c_port_t i2c_num) //Display function ver 1.0 (gonna change this)
{
	char *text = (char*)arg_text;
	uint8_t text_len = strlen(text);

	i2c_cmd_handle_t cmd;

	uint8_t cur_page = 0;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR<< 1) | I2C_MASTER_WRITE, true);

	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_STREAM, true);
	i2c_master_write_byte(cmd, 0x00, true); // reset column - choose column --> 0
	i2c_master_write_byte(cmd,0x10, true); // reset line - choose line --> 0

	i2c_master_write_byte(cmd, 0xB0 | cur_page, true); // reset page
	//0xB0 = 1011 0000 
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	for (uint8_t i = 0; i < text_len; i++) {
		if (text[i] == '\n') {
			cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, true);

			i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_STREAM, true);
			i2c_master_write_byte(cmd, 0x00, true); // reset column
			i2c_master_write_byte(cmd, 0x10, true);
			i2c_master_write_byte(cmd, 0xB0 | ++cur_page, true); // increment page

			i2c_master_stop(cmd);
			i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);
		} else {
			cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, true);

			i2c_master_write_byte(cmd,SSD1306_CONTROL_BYTE_DATA_STREAM , true);
			i2c_master_write(cmd, font8x8_basic_tr[(uint8_t)text[i]], 8, true);

			i2c_master_stop(cmd);
			i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);
		}
	}
}


void ssd1306_setCursor(uint16_t x, uint16_t y)
{
	SSD1306.Current_X = x;
	SSD1306.Current_Y = y;
}

void ssd1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color)
{
	if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
	{
		/*Throw ERROR*/
		return;
	}

	/*Check if pixel is inverted*/
	/*Nếu đang ở chế độ inverted chúng ta sẽ thực hiện việc đảo màu cho phù hợp với chế độ hiển thị hiện tại*/
	if (SSD1306.Inverted) {
		color = (SSD1306_COLOR_t)!color;
	}

	/* Set color */
	/*Chủ yếu là để set đúng màu pixel được vẽ ra*/
	if (color == SSD1306_COLOR_WHITE) {
		//set phần tử in white for sure
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8); //oke understood, CHƯA HIỂU ỦA CÁI INDEX LỰA CHỌN NÓ TÍNH NHƯ NÀO MÀ HAY QUÁ NHỜ
	} else { //set phần tử in black for sure 
		SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
	}	
}


//Software bitmap converter recommend: https://sourceforge.net/projects/lcd-image-converter/
void ssd1306_drawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, uint16_t color) //Display up to bottom style gonna make this more flexible
{
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if (byte & 0x80) {
                ssd1306_DrawPixel(x+i,y, color);
            }
        }
    }
    return;
}

void ssd1306_UpdateScreen(i2c_port_t i2c_num)
{
	uint8_t page;
	i2c_cmd_handle_t cmd;

	for(page = 0; page < SSD1306_HEIGHT/8; page++)
	{
		cmd = i2c_cmd_link_create();
		i2c_master_start(cmd);
		i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, true);
		i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_SINGLE , true);
		
		i2c_master_write_byte(cmd, 0xB0 | page, true);

		i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_DATA_STREAM, true);
		i2c_master_write(cmd, &SSD1306_Buffer[SSD1306_WIDTH*page],SSD1306_WIDTH, true);
		i2c_master_stop(cmd);
		i2c_master_cmd_begin(i2c_num, cmd, 10/portTICK_PERIOD_MS);
		i2c_cmd_link_delete(cmd);
	}
	
}

