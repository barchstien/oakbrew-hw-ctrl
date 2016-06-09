#include "hw_interface/SPI_Device.h"

#include "util.h"

//io control headers
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
//#include <linux/spi/spidev.h>
#include "linux/spi/spidev.h"
//for std::strerror
#include <cstring>

#define SPI_CHNL_0 "/dev/spidev0.0"
#define SPI_CHNL_1 "/dev/spidev0.1"

using namespace std;

int SPI_Device::init_SPI(){
    //Make channel_ string
    std::string channel_str;
    if (0 == channel_){
        channel_str = SPI_CHNL_0;
    }else if (1 == channel_){
        channel_str = SPI_CHNL_1;
    }else{
        LOG << "Error, channel_ = " << channel_ << " valid values are 0 and 1" << std::endl;
    }
    
    //assume that SPI module has been loaded (modprobe ...)
    //check that SPI fd are created
    std::string str = Util::exec_read_bash("ls /dev/spidev*");
    if (str.compare(0, 14, channel_str) != 0){
        LOG << "Error loading SPI, " << channel_str << " not created..." << endl;
        exit (0);
    }

    //open spi channel_
    if ((fd = open(channel_str.c_str(), O_RDWR)) < 0){
        LOG << "Error openning SPI fd..." << endl;
        exit (0);
    }
    if (ioctl(fd, SPI_IOC_WR_MODE, &mode_) < 0){
        LOG << "Error setting SPI mode_ to " << mode_ <<": " << strerror(errno) << endl;
        exit (0);
    }
    
    //seams to be un-necessary coz redundant with send data function
    //... delete later, or keep that ?
    //set word length (0 means 8bit long)
    if (ioctl (fd, SPI_IOC_WR_BITS_PER_WORD, &word_length_) < 0){
        LOG << "Error setting word length to " << (8+word_length_) << "bit : " << strerror(errno) << endl;
        exit (0);
    }
    if (ioctl (fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_)   < 0){
        LOG << "Error setting clock speed to " << speed_ << " : " << strerror(errno) << endl;
        exit (0);
    }
    
    return 0;
}

int SPI_Device::close_SPI(){
    close(fd);
}

//debug
//#include <bitset>
//#include <netinet/in.h>

char* SPI_Device::send_buff(char* tx_buff, int tx_buff_len){
    struct spi_ioc_transfer spi ;
    
    //debug
    /*std::bitset<8> data(buff[1]);
    std::bitset<8> cmd(buff[0]);
    int reg = ((int)buff[0]) >> 1;
    int r_w = (int)(buff[0] & 0b00000001);
    LOG << reg << " - W=0? " << r_w << " -cmd: " << cmd << " -data: " << data << endl;
    */
    
    char* rx_buff = (char*)malloc(tx_buff_len);
    
    spi.tx_buf        = (__u64)tx_buff;
    spi.rx_buf        = (__u64)rx_buff;
    spi.len           = tx_buff_len;
    spi.delay_usecs   = 0;//delay_usec_;
    spi.speed_hz      = 0;//speed_;
    spi.bits_per_word = 0;//word_length_;
    spi.cs_change     = 0;//true;

    ioctl (fd, SPI_IOC_MESSAGE(1), &spi);
    
    return rx_buff;
}
