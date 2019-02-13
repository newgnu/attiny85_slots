#include <avr/pgmspace.h>  //needed for PROGMEM variables


//Faster i2c library - https://github.com/felias-fogg/SoftI2CMaster
#if defined(__AVR_ATtiny25__) | defined(__AVR_ATtiny45__) | defined(__AVR_ATtiny85__) | defined(__AVR_AT90Tiny26__) | defined(__AVR_ATtiny26__)
  #define SCL_PIN 2
  #define SCL_PORT PORTB
  #define SDA_PIN 0
  #define SDA_PORT PORTB
#else
  #define SCL_PIN 5
  #define SCL_PORT PORTC
  #define SDA_PIN 4
  #define SDA_PORT PORTC
#endif
#define I2C_PULLUP 1
#define I2C_TIMEOUT 10000
#define I2C_FASTMODE 1
#include <SoftWire.h>
#undef SCL_PIN //colision with spi/sd junk
#undef SDA_PIN //luckily unset works.

SoftWire SSD1306Wire = SoftWire();

/******** CHANGE SCREEN SIZE HERE ********/
#define SCREEN_128x64
//#define SCREEN_128x32

#ifdef SCREEN_128x64
  #define SSD1306_COLUMNS 127
  #define SSD1306_PAGES 7
  #define SSD1306_SCREEN_BYTES 1024
#else //128x32
  #define SSD1306_COLUMNS 127
  #define SSD1306_PAGES 3
  #define SSD1306_SCREEN_BYTES 512
#endif

#ifndef SSD1306_ADDRESS
  #define SSD1306_ADDRESS 0x3C
#endif

#define SSD1306_COMMAND 0x00 //command mode
#define SSD1306_DATA    0x40 //data mode

static const uint8_t SSD1306_CONFIG[] PROGMEM = {
//  0xAE,        //'display off'
  0xD5, 0x80,  //'set display clock ratio'. suggested default is 0x80

#ifdef SCREEN_128x64
  0xA8, 63,    //'set multiplex ratio'. line height -1. in this case, a 64px tall screen
  0xDA, 0x12,  //'set com pins' (see data sheet) -- set to 0x12 at reset.
#else //128x32
  0xA8, 31,    //'set multiplex ratio'. line height -1. in this case, a 32px tall screen
  0xDA, 0x02,  //'set com pins' (see data sheet) -- set to 0x12 at reset. use 0x02 for 32px tall oled?
#endif

  0xD3, 0x00,  //'set display offset' to 0 -- this is returned to 0 at reset
  0x40 | 0x00, //'set start line' to 0 (second byte, but really only 6 bits) -- this is returned to 0 at reset

  0x20, 0x00,  //'set memory mode' to 0x00 (horizontal addressing)
  0xA0 | 0x01, //'set segment re-map' to 1. column 127 is mapped to seg0
  0xC8,        //'set com output scan direction' to top-down. C0 would be bottom-up
  0x8D, 0x14,  //'set charge pump' to 0x14 (use 0x10 for 'externalvcc')
  0x81, 0xCF,  //'set contrast' to 0xCF. Use 0x9F if external vcc, or 0x8F if 32px tall oled (int or ext vcc)
  0xD9, 0xF1,  //'set precharge' to 0xF1. Use 0x22 if external vcc
//  0xDB, 0x40,  //'set vcom deselect level' to 0x40 (datasheet says this is out of bounds?)
  0xA4,        //'resume to ram display' -- default at reset. 0xA5 ignores ram
  0xA6,        //'display normal' (0xA7 for inverted) -- default at reset
  0xAF         //and finally, 'display on' command
};

class SSD1306 {
  public:
    void begin(void);
    void write(uint8_t mode, uint8_t data[], int size);
    void writeP(uint8_t mode, int data, int size);  //same as 'write', but for progmem variables
    void clear();
    void area(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end); //sets a specific draw area
    void fill(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end, uint8_t data);
    void display_off();
    void display_on();
    void contrast(uint8_t value);
    void inverted(bool inv);
};

void SSD1306::begin(){
  SSD1306Wire.begin();
  SSD1306::writeP(SSD1306_COMMAND, SSD1306_CONFIG, sizeof(SSD1306_CONFIG));
  SSD1306::clear();
}

void SSD1306::write(uint8_t mode, uint8_t data[], int size){
  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(mode);
  for (int i = 0; i < size; i++){
    SSD1306Wire.write(data[i]);
  }
  SSD1306Wire.endTransmission();
}

void SSD1306::writeP(uint8_t mode, int data, int size){
  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(mode);
  for (int i = 0; i < size; i++){
    SSD1306Wire.write(pgm_read_byte_near(data +i));
  }
  SSD1306Wire.endTransmission();
}

void SSD1306::clear(){
  SSD1306::fill(0, SSD1306_PAGES, 0, SSD1306_COLUMNS, 0x00);
}

void SSD1306::display_on(){
  uint8_t data[] = {0xAF};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::display_off(){
  uint8_t data[] = {0xAE};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::contrast(uint8_t value){
  uint8_t data[] = {0x81, value};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}

void SSD1306::area(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end){
  uint8_t location[] = {
    0x20, 0x00,                //horizontal addressing mode
    0x21, col_start, col_end,  //set start/end column
    0x22, page_start, page_end //set start_end page
  };
  SSD1306::write(SSD1306_COMMAND, location, sizeof(location));
}

void SSD1306::fill(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end, uint8_t data){
  SSD1306::area(page_start, page_end, col_start, col_end);

  SSD1306Wire.beginTransmission(SSD1306_ADDRESS);
  SSD1306Wire.write(SSD1306_DATA);
  for (int i = 0; i < ((page_end-page_start)+1) * ((col_end-col_start)+1); i++){
    SSD1306Wire.write(data);
  }
  SSD1306Wire.endTransmission();
}

void SSD1306::inverted(bool inv){
  uint8_t data[] = {0xA6 | inv};
  SSD1306::write(SSD1306_COMMAND, data, sizeof(data));
}
