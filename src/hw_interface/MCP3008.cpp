

#include "hw_interface/MCP3008.h"
#include <cstring>
#include <arpa/inet.h>

#include <bitset>

using namespace std;

#define MCP3008_SPI_CPOL_CPHA 0b00
#define MCP3008_SPI_SPEED 10000000
#define MCP3008_SPI_DELAY 0
#define MCP3008_SPI_WL 0

MCP3008::MCP3008(int channel) 
    : SPI_Device(channel, 
                MCP3008_SPI_CPOL_CPHA, 
                MCP3008_SPI_SPEED, 
                MCP3008_SPI_DELAY, 
                MCP3008_SPI_WL)
{
    LOG << "init MCP 3008..." << endl;
    init_SPI();
    //char buff[2];
    
#if 0
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
#endif
    LOG << "MCP 3008 finish init" << endl;
}

MCP3008::~MCP3008(){
    close_SPI();
}

uint16_t MCP3008::get_value(unsigned int channel){
    uint8_t tmp = channel & 0x111;
    tmp = tmp << 4;
    tmp = 0b10000000 | tmp;
    char cmd[3]     = {0b00000001, tmp, 0};
    //char rx_buff[3] = {0, 0, 0};
#if 0
    for (int i=0; i<3; i++){
        std::bitset<8> x(cmd[i]);
        std::cout << "cmd[" << i << "] : " << x << std::endl;
    }
#endif
    char* rx_buff = 0;
    rx_buff = send_buff(cmd, 3);
#if 0
    for (int i=0; i<3; i++){
        std::bitset<8> x(rx_buff[i]);
        std::cout << "rx[" << i << "] : " << x << std::endl;
    }
#endif
    uint16_t v = 0;
    memcpy(&v, rx_buff + 1, 2);
    v = ntohs(v);
    v = v & 0b1111111111;
    return v;
}






