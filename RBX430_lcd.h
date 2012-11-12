//******************************************************************************
//	LCD
//******************************************************************************
#ifndef LCD_H_
#define LCD_H_

//	initialization constants
#define EV					24			// LCD volume
#define EXT					1
#define BE					1
#define FR					7
#define RB_RA				0x06 		// internal resistor ratio
//#define RB_RA				0x27

//	display size
#define HD_X_MAX			160
#define HD_Y_MAX			160

#define CHAR_SIZE			8

//	print buffer size
#define PRINT_BUFFER_SIZE	32

#define	SINGLE_PEN_OFF	0
#define	SINGLE_PEN		1
#define	DOUBLE_PEN_OFF	2
#define	DOUBLE_PEN		3
#define	READ_POINT		4

#define M2B3P(P0,P1,P2)	((0xc0*P1|0x1f*P2)^0xff),((0Xf8*P0|0x07*P1)^0xff)

//	lcd modes
#define lcd_display lcd_mode
uint16 lcd_mode(int16 mode);

#define LCD_PROPORTIONAL	0x01
#define LCD_REVERSE_FONT	0x02
#define LCD_2X_FONT			0x04
#define LCD_FRAM_CHARACTER	0x08
#define LCD_REVERSE_DISPLAY	0x10
#define LCD_OR_CHAR			0x20

//	lcd prototypes
uint8 lcd_init(void);
void lcd_clear(void);
void lcd_set(uint16 value);
void lcd_backlight(uint8 backlight);
void lcd_volume(uint16 volume);

//	lcd character data
char lcd_putchar(char c);
uint16 lcd_printf(const char* fmt, ...);
uint8 lcd_cursor(int16 x, int16 y);
uint8 lcd_image(const uint8* image, int16 x, int16 y);
uint8 lcd_image1(const uint8* image, int16 x, int16 y, uint8 flag);
uint8 lcd_image2(const uint8* image, int16 x, int16 y, uint8 flag);
uint8 lcd_blank(int16 x, int16 y, uint16 width, uint16 height);
uint8 lcd_fill(int16 x, int16 y, uint16 width, uint16 height, uint8 flag);

uint16 lcd_read_word(int16 x, int16 y);
void lcd_write_word(int16 x, int16 y, uint16 data);

uint8 lcd_point(int16 x, int16 y, int16 flag);
void lcd_circle(int16 x, int16 y, uint16 radius, uint8 pen);
void lcd_square(int16 x, int16 y, uint16 side, uint8 pen);
void lcd_rectangle(int16 x, int16 y, uint16 w, uint16 h, uint8 pen);

unsigned divu3(unsigned n);

#endif /*LCD_H_*/
