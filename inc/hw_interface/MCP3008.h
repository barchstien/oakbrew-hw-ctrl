#ifndef _MCP3008_
#define _MCP3008_

#include <string>
#include "hw_interface/SPI_Device.h"

/** SPI control of MCP3008, that has 8 channels ADC */
class MCP3008 : public SPI_Device {
public:
    /** Constructor
    @param channel SPI channel (0 & 1 for raspberry pi) */
    MCP3008(int channel);
    ~MCP3008();
    
    /** poll MCP 3008 for channel */
    uint16_t get_value(unsigned int channel);
    
};
#endif
