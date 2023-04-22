#include "ssd1306.h"
#include "font8x8_basic.h"

/* SSD1306 data buffer */
static uint8_t SSD1306_Buffer[SSD1306_BUFFER_SIZE];


/*Private Variable*/
static SSD1306_t SSD1306;

void ssd1306_init(i2c_port_t i2c_num)
{
    	static const char *TAG = "i2c-lib";
	esp_err_t espRc;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (SSD1306_OLED_ADDR << 1) | I2C_MASTER_WRITE, SSD1306_ACK);
	i2c_master_write_byte(cmd, SSD1306_CONTROL_BYTE_CMD_STREAM , SSD1306_ACK);

    	i2c_master_write_byte(cmd, SSD1306_DISPLAY_OFF, SSD1306_ACK);
    	i2c_master_write_byte(cmd,SSD1306_MEMORY_ADDR_MODE, SSD1306_ACK);
    	i2c_master_write_byte(cmd,SSD1306_SET_MUX_RATIO,SSD1306_ACK); //set mux ratio
    	i2c_master_write_byte(cmd,SSD1306_DISPLAY_OFFSET, SSD1306_ACK); //set offset
    	i2c_master_write_byte(cmd,SSD1306_SET_START_LINE, SSD1306_ACK);

    	i2c_master_write_byte(cmd, SSD1306_SEG_REMAP_OP, SSD1306_ACK); // reverse left-right mapping
	i2c_master_write_byte(cmd, SSD1306_COM_SCAN_DIR_OP, SSD1306_ACK); // reverse up-bottom mapping

    	i2c_master_write_byte(cmd,SSD1306_COM_PIN_CONF,SSD1306_ACK);
    	i2c_master_write_byte(cmd,SSD1306_SET_CONTRAST,SSD1306_ACK); //set contrast

   	 i2c_master_write_byte(cmd,SSD1306_DISPLAY_ENT_DISP_ON,SSD1306_ACK);
    
	i2c_master_write_byte(cmd, SSD1306_SET_CHARGE_PUMP , SSD1306_ACK);
	i2c_master_write_byte(cmd, SSD1306_DISPLAY_NORMAL, SSD1306_ACK);
    	i2c_master_write_byte(cmd,SSD1306_SET_OSC_FREQ,SSD1306_ACK);

    	i2c_master_write_byte(cmd, 0x14, SSD1306_ACK); //enable charge pump 

	i2c_master_write_byte(cmd,SSD1306_DISPLAY_ON, SSD1306_ACK); //turn on light panel
	i2c_master_write_byte(cmd,SSD1306_DEACT_SCROLL, SSD1306_ACK); //deactivate scroll

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

