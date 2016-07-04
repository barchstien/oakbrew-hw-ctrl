

#include "hw_interface/MCP3008.h"
#include "util.h"
#include <cstring>
#include <arpa/inet.h>

#include <bitset>

using namespace std;

#define MCP3008_SPI_CPOL_CPHA 0b00
//datasheet p14, Figure 4.2, Max clock freq to maintain less than 0.1 LS deviation
#define MCP3008_SPI_SPEED 500000
#define MCP3008_SPI_DELAY 0
#define MCP3008_SPI_WL 0

#define ADC_DELAY_BETWEEN_READ 50

MCP3008::MCP3008(int channel)
    : SPI_Device(channel,
                MCP3008_SPI_CPOL_CPHA,
                MCP3008_SPI_SPEED,
                MCP3008_SPI_DELAY,
                MCP3008_SPI_WL)
{
    LOG << "init MCP 3008..." << std::endl;

    shouldRun_ = false;
    for (int i=0; i<NUM_OF_CHANNELS; i++){
        enabled_[i] = false;
        values_[i] = -1;
    }

    init_SPI();
    LOG << "MCP 3008 finish init" << std::endl;
}

MCP3008::~MCP3008(){
    close_SPI();
}

void MCP3008::enable_channel(unsigned int channel){
    if (channel >= 0 && channel < NUM_OF_CHANNELS){
        enabled_[channel] = true;
        LOG "enable ADC chan : "  << channel << std::endl;
    }
}

void MCP3008::disable_channel(unsigned int channel){
    if (channel >= 0 && channel < NUM_OF_CHANNELS){
        enabled_[channel] = false;
        values_[channel] = -1;
    }
}

int MCP3008::get_value(unsigned int channel){
    if (channel >= 0 && channel < NUM_OF_CHANNELS){
        return values_[channel];
    }
    return -1;
}

int MCP3008::threadHandler(){
    while (shouldRun_){
        for (int i=0; i<NUM_OF_CHANNELS; i++){
            if (! enabled_[i]){
                //keep that ?
                values_[i] = -1;
                continue;
            }
            uint8_t tmp = i & 0b00000111;
            tmp = tmp << 4;
            tmp = 0b10000000 | tmp;

            char cmd[3] = {0b00000001, tmp, 0};

#if 0
            for (int i=0; i<3; i++){
                std::bitset<8> x(cmd[i]);
                std::cout << "cmd[" << i << "] : " << x << std::endl;
            }
#endif
            char rx_buff[3];
            send_buff(cmd, rx_buff, 3);
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
            values_[i] = v;

            //LOG << "adc read : " << v << "  / values_[i] : " << values_[i] << std::endl;

            std::this_thread::sleep_for(std::chrono::milliseconds(ADC_DELAY_BETWEEN_READ));
        }
    }
    return 0;
};
