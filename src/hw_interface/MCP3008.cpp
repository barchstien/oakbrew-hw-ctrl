

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






