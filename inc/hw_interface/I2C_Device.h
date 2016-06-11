#ifndef _I2C_DEVICE_
#define _I2C_DEVICE_

#include <cstdint>

/** Abstract class for all I2C devices
It simplifies I2C use, wrapping /dev/i2c-1 with function calle
Class implementing I2C_Device MUST use init_I2C() and close_I2C() function */
class I2C_Device {
public:
    I2C_Device(uint8_t addr, int channel=1) 
        : addr_(addr), channel_(channel) {}
protected:
    /** file descriptor to I2C device */
    int fd_;
    
    int init_I2C();
    
    /** close the I2C communication by closing the file descriptor on /dev/i2c-1 */
    int close_I2C();
    
    int channel_;
    uint8_t addr_;
    
    int write_buff(uint8_t* buff, int buff_len);
    //TODO read()
};
#endif
