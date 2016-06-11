#ifndef _TDA_7468_
#define _TDA_7468_

#include "hw_interface/I2C_Device.h"

class TDA7468 : public I2C_Device {
public:
    TDA7468(uint8_t addr=0x44, int channel=0) 
        : I2C_Device(addr, channel), 
        volume_(0xffff), input_gain_(0xffff), bass_(0xffff), treble_(0xffff), 
        mute_(false), input_(0xffff), balance_(0xffff)
    {}

    /** get/set volume [-87db : +14db], 1db step for v<=0, 2db step v>0
    set using Volume1 in 1db and 8db steps, Volume2 and input gain */
    int volume();
    void volume(int v);
    
    /** [-14db : +14db] 2db step */
    int bass();
    void bass(int b);
    
    /** [-14db : +14db] 2db step */
    int treble();
    void treble(int t);
    
    bool mute();
    void mute(bool m);
    
    /** (1 : 4] **/
    int input();
    void input(int n);
    
    /** [-100 : 100] 
      0 means mid-point
      -100 : right channel -100% volume
      +100 : left channel -100% volume*/
    int balance();
    void balance(int b);
    
private:
    int volume_;
    int input_gain_;
    
    int bass_;
    int treble_;
    
    bool mute_;
    
    int input_;
    
    int balance_;
    
    uint8_t encode_bass_treble(int bass, int treble);
};

#endif
