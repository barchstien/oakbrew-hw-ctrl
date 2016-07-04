#ifndef _MCP3008_
#define _MCP3008_

#include <string>
#include "hw_interface/SPI_Device.h"
#include "module.h"

#define NUM_OF_CHANNELS 8

/** SPI control of MCP3008, that has 8 channels ADC
 * Internal thread polls enabled channels, cached values can be retrieved with get_value(n)
 * Thread is used because MCP3008 requires 50msec between each read, making it hard to read sequentially without loosing time, ie responsiveness */
class MCP3008 : public SPI_Device, public module {
public:
    /** Constructor
    @param channel SPI channel (0 & 1 for raspberry pi) */
    MCP3008(int channel);
    ~MCP3008();

    /** enable a channel to be poll by thread */
    void enable_channel(unsigned int channel);
    void disable_channel(unsigned int channel);

    /** get previously polled cahed values (approx every <enabled_channel> * 50msec */
    int get_value(unsigned int channel);

    static const unsigned int MAX_VALUE = 1023;

protected:
    /** poll MCP 3008 for enabled channel(s) */
    virtual int threadHandler();

    /** channels polled by thread, default to false */
    bool enabled_[NUM_OF_CHANNELS];

    /** Hold values polled from the 8 available Channels, -1 means never read */
    int values_[NUM_OF_CHANNELS];
};
#endif
