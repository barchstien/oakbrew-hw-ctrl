#ifndef _SPI_DEVICE_
#define _SPI_DEVICE_

////TODO redo the clean up and changes that were lost when RPi SD card went wrong...
////... meaning cleaner send SPI

/** Abstract class for all SPI devices
It simplifies SPI use, wrapping /dev/spidevA.B with function calle
Class implementing SPI_Device MUST use init_SPI() and close_SPI() function */
class SPI_Device {
public:
    SPI_Device(int channel, int mode, int speed, int delay_usec, int w_length) 
        : channel(channel), mode(mode), speed(speed), delay_usec(delay_usec), word_length(w_length) {}
protected:
    /** file descriptor to SPI device */
    int fd;
    
    /** Ensure that /dev/spidevA.B exist
    set SPI parameters : clock phase/polarity, word length ... 
    @param channel 0 or 1 for raspberry pi */
    int init_SPI();
    
    /** close the SPI communication by closing the file descriptor on /dev/spidevA.B */
    int close_SPI();
    
    //void send_spi_word(char c, char d);
    void send_buff(char* buff, int buff_len);
    
    int channel;
    /** clock polarity & phase CPOL CPHA 
    b00 CPOL 0 CPHA 0
    b01 CPOL 0 CPHA 1
    b10 CPOL 0 CPHA 1
    b11 CPOL 1 CPHA 1 */
    int mode;
    int speed;
    int delay_usec;
    /** 0 means 8bit long */
    int word_length;
    
};
#endif
