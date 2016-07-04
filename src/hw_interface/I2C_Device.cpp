#include "hw_interface/I2C_Device.h"

#include "util.h"

//io control headers
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>
//#include <i2c/smbus.h>
//#include "linux/i2c-dev.h"
//#include "i2c/smbus.h"

//for std::strerror
#include <cstring>

#define I2C_CHNL_0 "/dev/i2c-0"
#define I2C_CHNL_1 "/dev/i2c-1"

using namespace std;

int I2C_Device::init_I2C(){
    //Make channel_ string
    std::string channel_str;
    if (0 == channel_){
        channel_str = I2C_CHNL_0;
    }else if (1 == channel_){
        channel_str = I2C_CHNL_1;
    }else{
        LOG << "Error, channel_ = " << channel_ << " valid values are 0 and 1" << std::endl;
    }

    //assume that I2C module has been loaded (modprobe ...)
    //check that I2C fd are created
    std::stringstream ss;
    ss << "ls " << channel_str;
    std::string str = Util::exec_read_bash(ss.str());
    if (str.compare(0, 10, channel_str) != 0){
        LOG << "Error loading I2C, " << channel_str << " does not exist..." << endl;
        exit (0);
    }

    //open spi channel_
    if ((fd_ = open(channel_str.c_str(), O_RDWR)) < 0){
        LOG << "Error openning I2C device : " << channel_str << endl;
        exit (0);
    }
    if (ioctl(fd_, I2C_SLAVE, addr_) < 0){
        int err = errno;
        LOG << "Error setting I2C slave address to 0x" << std::hex << (int)addr_ << std::dec <<": " << strerror(err) << endl;
    }
    LOG << "init_I2C() done" << std::endl;
    return 0;
}

int I2C_Device::close_I2C(){
    close(fd_);
    LOG << "close_I2C() done" << std::endl;
}

//debug
#include <bitset>

int I2C_Device::write_byte(uint8_t cmd){

    //debug
    std::bitset<8> c(cmd);
    //LOG << "writing to chan (" << channel_ << ") addr (" << std::hex << (int)addr_ << ") : " <<  c << std::dec << std::endl;

    int ret = 0;
    struct i2c_smbus_ioctl_data args;
    //read or write
    args.read_write = I2C_SMBUS_WRITE;
    //sub-addr
    args.command = cmd;
    args.size = I2C_SMBUS_BYTE;
    args.data = nullptr;

    ret = ioctl(fd_, I2C_SMBUS, &args);

    if (ret < 0){
        int err = errno;
        LOG << "error writing to i2c device chan (" << channel_ << ") addr (" << std::hex << (int)addr_ << std::dec << ") : " << strerror(err) << std::endl;
    }
    //LOG << "ret : " << ret << std::endl;

    return ret;
}

int I2C_Device::write_byte_data(uint8_t cmd, uint8_t data){

    //debug
    std::bitset<8> c(cmd);
    std::bitset<8> d(data);
    //LOG << "writting to chan (" << channel_ << ") addr (" << std::hex << (int)addr_ << ") : " << c << " : " << d << std::dec << std::endl;

    int ret = 0;

    struct i2c_smbus_ioctl_data args;
    //read or write
    args.read_write = I2C_SMBUS_WRITE;
    //sub-addr
    args.command = cmd;
    args.size = I2C_SMBUS_BYTE_DATA;

    //args.size = I2C_SMBUS_BLOCK_DATA;
    //array [length, data0, ... dataN]
    union i2c_smbus_data i2c_data;
    //i2c_data.block[0] = 1;
    //i2c_data.block[1] = data;
    i2c_data.byte = data;

    args.data = &i2c_data;

    ret = ioctl(fd_, I2C_SMBUS, &args);

    if (ret < 0){
        int err = errno;
        LOG << "error writing to i2c device chan (" << channel_ << ") addr (" << std::hex << (int)addr_ << std::dec << ") : " << strerror(err) << std::endl;
    }

    return ret;
}
