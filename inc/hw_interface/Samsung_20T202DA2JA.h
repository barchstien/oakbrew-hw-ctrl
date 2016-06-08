#ifndef _Samsung_20T202DA2JA_
#define _Samsung_20T202DA2JA_

#include <string>
#include "hw_interface/SPI_Device.h"

//class Samsung_20T202DA2JA;
//class module;
#include "module.h"

/**
This class allows to drive a Samsung 20T202DA2JA independently from SPI interface
It required that SPI has been enabled (modprobe spi-bcm2708 fro raspberryPi)
... and that current user has r/w permission on /dev/spidev0.0 and /dev/spidev0.1

Note : after performance test, it looks like SPI with this scren performs around 160KHz
This allows very fast switching of the screen

Note : some cursor position math looks funny coz screen layout is like :
[0-19] 1st line
[20-39] hidden
[40-59] 2nd line
[60-79] hidden

These hidden zone are managed within the current class. From outside position and cursor_shift is 
[0-19] 1st line
[20-39] 2nd line
*/

class Samsung_20T202DA2JA : public SPI_Device, public module {
public:
    /** Constructor
    @param channel SPI channel (0 & 1 for raspberry pi) */
    Samsung_20T202DA2JA(int channel);
    ~Samsung_20T202DA2JA();

    /** clear screen */
    void clear();

    /** blink the char at cursor position */
    void blink(bool enable);
    /** enable/disable cursor */
    void cursor(bool enable);
    /** set cursor to home */
    void cursor_home();
    /** enable display */
    void on();
    /** disable display */
    void off();

    /** shift cursor n character, to the left if positive, else to the right */
    void cursor_shift(int n);

    /** clear screen and write string. wraps lines */
    void write(std::string, bool center=false);
    
    /** append to screen from where ever th cursor is */
    void append(const char*, int len);
    
    /** write str to line_number (no wrap)
    @param line_number 0 or 1
    @param str string cut at length 20 if longer
    @param v_align 0 left, 1 right, 2 center */
    void write_line(int line_number, std::string str, int v_align=0);
    
    /** scroll a string longer than 20 on 2nd line
    call disable_scroll() to disable scrolling */
    void scroll_on_2nd_line(std::string);
    /** disable scrolling */
    void disable_scroll();
    
    static const int V_ALIGN_LEFT=0;
    static const int V_ALIGN_RIGHT=1;
    static const int V_ALIGN_CENTER=2;
    
private:
    /** where the cursor is */
    int position;

    int blink_ctrl;
    int cursor_ctrl;
    int display_ctrl;
    
    std::mutex m_screen_mutex;
    
    /** set cursor to home, no lock, for internal use */
    void cursor_home_no_lock();
    /** shift cursor n character, to the left if positive, else to the right
    no lock, for internal use */
    void cursor_shift_no_lock(int n);
    
    void append_no_lock(const char*, int len);
    
protected:
    int init();
    bool m_enable_scroll;
    virtual int threadHandler();
};
#endif
