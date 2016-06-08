

#include "hw_interface/PCM512x_spi.h"
//#include "linux/sound/soc/codecs/pcm512x.h"

using namespace std;

#define PCM512X_SPI_CPOL_CPHA 0b01
#define PCM512X_SPI_SPEED 100000
#define PCM512X_SPI_DELAY 0
//TODO as for screen, it's 2 x 8(WL) rather than 1 x 16(WL) as in data sheet
//? do define with reg num + config ?? #define PCM512X_PLL_D_0000
#define PCM512X_SPI_WL 8

//shift one bit left to insert write(0) or read(1) control bit
#define REG_WRITE(x) ((x << 1) & 0b11111110)
#define REG_READ(x)  ((x << 1) | 0b00000001)

//register address
#define PCM512x_PAGE_LEN        0x100
#define PCM512x_PAGE_BASE(n)    (PCM512x_PAGE_LEN * n)
#define PCM512X_PAGE_SELECT     0

#define PCM512x_RESET               (PCM512x_PAGE_BASE(0) +   1)
#define PCM512x_POWER               (PCM512x_PAGE_BASE(0) +   2)
#define PCM512x_MUTE                (PCM512x_PAGE_BASE(0) +   3)
#define PCM512X_PLL_SOURCE          (PCM512x_PAGE_BASE(0) +   13)
#define PCM512X_PLL_P               (PCM512x_PAGE_BASE(0) +   20)
#define PCM512X_PLL_J               (PCM512x_PAGE_BASE(0) +   21)
#define PCM512X_PLL_D               (PCM512x_PAGE_BASE(0) +   23)
#define PCM512X_PLL_R               (PCM512x_PAGE_BASE(0) +   24)

#define PCM512X_PLL_DDSP            (PCM512x_PAGE_BASE(0) +   27)
#define PCM512X_PLL_DDAC            (PCM512x_PAGE_BASE(0) +   28)
#define PCM512X_PLL_DNCP            (PCM512x_PAGE_BASE(0) +   29)
#define PCM512X_PLL_DOSR            (PCM512x_PAGE_BASE(0) +   30)

#define PCM512X_I2S_FORMAT_LENGTH   (PCM512x_PAGE_BASE(0) +   40)
#define PCM512X_I2S_SHIFT           (PCM512x_PAGE_BASE(0) +   41)


PCM512x_spi::PCM512x_spi(int channel) 
    : SPI_Device(channel, 
                PCM512X_SPI_CPOL_CPHA, 
                PCM512X_SPI_SPEED, 
                PCM512X_SPI_DELAY, 
                PCM512X_SPI_WL)
{
    LOG << "PCM512x_spi" << endl;
    init_SPI();
    char buff[2];
    
    //reset
    register_write(PCM512x_RESET, 0b00010001);
    
    //request standby
    register_write(PCM512x_POWER, 0b00010000);
    this_thread::sleep_for(chrono::milliseconds(1000));
    
    //reg 13, set PLL source to BCK (1), default SCK (0)
    register_write(PCM512X_PLL_SOURCE, 0b00010000);
        
    //P, keep default 0000: P=1
    register_write(PCM512X_PLL_P, 0b00010000);    
    
    //J 100000: J=32
    register_write(PCM512X_PLL_J, 0b100000);
    
    //D, keep default 0000: D=0
    register_write(PCM512X_PLL_D, 0b00010000);
        
    //R 0001: R=2
    register_write(PCM512X_PLL_R, 0b0001);
    
    ////values from pcm512x datasheet - page 28 - 44,1kHz RSCK:32
    //DDSP 2
    register_write(PCM512X_PLL_DDSP, 0b0000001);
    
    //DDAC 16 : 
    register_write(PCM512X_PLL_DDAC, 0b0001111);
    
    //DNCP 4
    register_write(PCM512X_PLL_DNCP, 0b0000011);
    
    //DOSR 8
    register_write(PCM512X_PLL_DOSR, 0b0000111);
    
    //i2s format and word length (16bit) 0b00000000
    register_write(PCM512X_I2S_FORMAT_LENGTH, 0b00000000);

    LOG << "PCM512x_spi -- end" << endl;
}

PCM512x_spi::~PCM512x_spi(){
    close_SPI();
}


void PCM512x_spi::register_write(uint16_t reg, uint8_t data){
    char buff[2];
    //select page, page(reg and data) stored on 2 bytes higher than reg value
    buff[0] = PCM512X_PAGE_SELECT;
    buff[1] = (reg >> 8) & 0xff;
    //word length is 16 bits
    send_buff(buff, 1);
    this_thread::sleep_for(chrono::milliseconds(10));
    //send register and data
    buff[0] = REG_WRITE((reg & 0xff));
    buff[1] = data;
    //word length is 16 bits
    send_buff(buff, 1);
    this_thread::sleep_for(chrono::milliseconds(500));
}


