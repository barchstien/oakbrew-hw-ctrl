#ifndef _TDA_7468_
#define _TDA_7468_

#include "hw_interface/I2C_Device.h"

class TDA7468 : public I2C_Device {
public:
    TDA7468(uint8_t addr=0x44, int channel=1);
    ~TDA7468();

    /** get/set volume [-87db : +14db], 1db step for v<=0, 2db step v>0
    set using Volume1 in 1db and 8db steps, Volume2 and input gain */
    void volume(int v);
    int volume();

    /** [-14db : +14db] 2db step */
    int bass();
    void bass(int b);

    /** [-14db : +14db] 2db step */
    int treble();
    void treble(int t);

    bool mute_output();
    void mute_output(bool m);

    /** (1 : 4] **/
    int input();
    void input(int n);

    /** [-50 : 50]
      0 means mid-point
      -50 : right channel -100% volume
      +50 : left channel -100% volume
      @return true if balance was changed */
    int balance();
    bool balance(int b);

private:
    int volume_;

    //not used for now...
    int input_gain_;

    int bass_;
    int treble_;

    bool mute_;

    int input_;

    int balance_;

    uint8_t encode_bass_treble(int bass, int treble);

    void set_volume_to_channel(uint8_t chan, int v);
};

#endif
