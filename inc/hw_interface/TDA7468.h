#ifndef _TDA_7468_
#define _TDA_7468_

#include "hw_interface/I2C_Device.h"

class TDA7468 : public I2C_Device {
public:
    TDA7468(uint8_t addr=0x44, int channel=0) 
        : I2C_Device(addr, channel) {}

    
    
};

#endif
