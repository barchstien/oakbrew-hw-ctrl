#ifndef _SPI_DEVICE_
#define _SPI_DEVICE_


/** Abstract class for all SPI devices
It simplifies SPI use, wrapping /dev/spidevA.B with function call
Class implementing SPI_Device MUST use init_SPI() and close_SPI() function */
class SPI_Device {
public:
    SPI_Device(int channel, int mode, int speed, int delay_usec, int w_length) 
        : channel_(channel), mode_(mode), speed_(speed), delay_usec_(delay_usec), word_length_(w_length) {}
protected:
    /** file descriptor to SPI device */
    int fd;
    
    /** Ensure that /dev/spidevA.B exist
    set SPI parameters : clock phase/polarity, word length ... 
    @param channel 0 or 1 for raspberry pi */
    int init_SPI();
    
    /** close the SPI communication by closing the file descriptor on /dev/spidevA.B */
    int close_SPI();
    
    /** send and receive buff_len bytes (SPI style) 
    @param tx_buff data sent, can be dummy if only want to receive
    @param rx_buff data received, should be allocated (or static) memory
    @param buff_len length of tx and rx buff
    @return result of ioctl(...) call */
    int send_buff(char* tx_buff, char* rx_buff, int buff_len);
    
    int channel_;
    /** clock polarity & phase CPOL CPHA 
    b00 CPOL 0 CPHA 0
    b01 CPOL 0 CPHA 1
    b10 CPOL 0 CPHA 1
    b11 CPOL 1 CPHA 1 */
    int mode_;
    int speed_;
    int delay_usec_;
    /** 0 means 8bit long */
    int word_length_;
    
};
#endif
