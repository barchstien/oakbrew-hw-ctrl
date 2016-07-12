#include "hw_interface/TDA7468.h"
#include <stdlib.h>
#include "util.h"

#include <thread>
#include <chrono>

//debug
#include <bitset>

//not sure how it is used
#define POWER_ON_RESET  0b11111110

//use like this : INCREMENTAL_BUS_ACTIVE | SUB_ADDR_XXX
#define INCREMENTAL_BUS_ACTIVE 0b00010000

#define SUB_ADDR_INPUT_SELECT_MIC 0b00000000
#define SUB_ADDR_INPUT_GAIN       0b00000001
#define SUB_ADDR_SURROUND         0b00000010
#define SUB_ADDR_VOLUME_LEFT      0b00000011
#define SUB_ADDR_VOLUME_RIGHT     0b00000100
#define SUB_ADDR_TREBLE_BASS      0b00000101
#define SUB_ADDR_OUTPUT           0b00000110
#define SUB_ADDR_BASS_ALC         0b00000111

//INPUT SELECTION & MIC
#define INPUT_SELECT_1 0b00000000
#define INPUT_SELECT_2 0b00000001
#define INPUT_SELECT_3 0b00000010
#define INPUT_SELECT_4 0b00000011
//? OR with INPUT_SELECT_X ?
//#define INPUT_MUTE_ON        0b00000100
//#define INPUT_MUTE_OFF       0b00000000
//to OR with previous
#define INPUT_MIC_ON  0b00000000
#define INPUT_MIC_OFF 0b00100000

//INPUT GAIN
//0 to 14db, 2db steps, 3 lowest bit
//this is just after input select

//VOLUME
//[0db; -64db[ on lowest 6 bit (applied before treble/bass)
//- n * 8db on highest 2 bit (applied after treble/bass)

//TREBLE & BASS
//[-14db; 14db] by step 2db, 4bit each
//bass 4 hihest bit, treble 4 lowest
//0 = -14db, 7 = 0db, MSb is sign (0 means -, 1 means +)

//OUTPUT
#define OUTPUT_MUTE_ON  0b00000000
#define OUTPUT_MUTE_OFF 0b00000001

#define SLEEP_MSEC 1

TDA7468::TDA7468(uint8_t addr, int channel)
    : I2C_Device(addr, channel),
    volume_(0xffff), input_gain_(0), bass_(0), treble_(0),
    mute_(false), input_(1), balance_(0)
{
    LOG << "start init TDA 7468" << std::endl;
    init_I2C();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //DAC is input 0
    input(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //volume to lowest
    volume(-87);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //disable suround
    write_byte_data(SUB_ADDR_SURROUND, 0b00011000);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //trebel & bass to mid point
    write_byte_data(SUB_ADDR_TREBLE_BASS, 0b11111111);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //un-mute output
    mute_output(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    //turn off bass ALC
    write_byte_data(SUB_ADDR_BASS_ALC, 0b00000000);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));

    LOG << "end init TDA 7468" << std::endl;
}

TDA7468::~TDA7468()
{
    close_I2C();
}

int TDA7468::volume(){
    return volume_;
}

void TDA7468::volume(int v){
    if (v < -87){
        v = -87;
    }else if (v > 14){
        v = 14;
    }

    volume_ = v;
    int volume_1 = v, volume_2 = v;

    //apply balance
    //LOG << "balance : " << balance_ << " abs(balance_) * v / 50 : " << abs(balance_) * v / 50 << std::endl;
    if (balance_ < 0){
        volume_1 = v - abs(balance_ * (87 - abs(v))) / 50;
        if (volume_1 < -87){
            volume_1 = -87;
        }
    }else if (balance_ > 0){
        volume_2 = v - abs(balance_ * (87 - abs(v))) / 50;
        if (volume_2 < -87){
            volume_2 = -87;
        }
    }
    //LOG << "Setting volume_1 to " << volume_1  << "  volume_2 to " << volume_2 << std::endl;

    set_volume_to_channel(SUB_ADDR_VOLUME_LEFT, volume_1);
    set_volume_to_channel(SUB_ADDR_VOLUME_RIGHT, volume_2);
}

void TDA7468::set_volume_to_channel(uint8_t chan, int v){
    uint8_t v1_1db, v1_8db, v2_8db, input_gain;
    if (v < 0){
        int v_abs = -1 * v;
        v1_1db = v_abs % 8;
        v1_8db = ((v_abs - v1_1db) / 8);
        if (v1_8db > 7){
            v1_8db = 7;
        }
        v2_8db = ((v_abs - v1_1db) / 8) - v1_8db;
        //LOG << "v1_1db : "<<(int)v1_1db <<"   v1_8db : "<<(int)v1_8db <<"   v2_8db : "<<(int)v2_8db << std::endl;
        input_gain = 0;
    }else{
        v1_1db = 0;
        v1_8db = 0;
        v2_8db = 0;
        //input gain ignored for now (same for both channels)
        input_gain = v / 2;
    }

    uint8_t data = 0;
    data = (v2_8db << 6) | (v1_8db << 3) | v1_1db;
    write_byte_data(chan, data);

    //TODO ? input_gain_ ???
    //not used for now, for both channels
    //write_byte_data(SUB_ADDR_INPUT_GAIN, input_gain);
}


int TDA7468::bass(){
    return bass_;
}

void TDA7468::bass(int b){
    if (b < -14){
        b = -14;
    }else if (b > 14){
        b = 14;
    }
    if (bass_ == b){
        return;
    }

    bass_ = b;
    //LOG << "setting bass to : " << bass_ << std::endl;
    uint8_t data = encode_bass_treble(bass_, treble_);

    write_byte_data(SUB_ADDR_TREBLE_BASS, data);
}


int TDA7468::treble(){
    return treble_;
}

void TDA7468::treble(int t){
    if (t < -14){
        t = -14;
    }else if (t > 14){
        t = 14;
    }
    if (treble_ == t){
        return;
    }

    treble_ = t;
    //LOG << "setting treble to : " << treble_ << std::endl;
    uint8_t data = encode_bass_treble(bass_, treble_);

    write_byte_data(SUB_ADDR_TREBLE_BASS, data);
}


bool TDA7468::mute_output(){
    return mute_;
}

void TDA7468::mute_output(bool m){
    mute_ = m;
    uint8_t data = 0;
    if (mute_){
        data = OUTPUT_MUTE_ON;
    }else{
        data = OUTPUT_MUTE_OFF;
    }
    write_byte_data(SUB_ADDR_OUTPUT, data);
}

int TDA7468::input(){
    return input_;
}

void TDA7468::input(int n){
    if (n < 0){
        n = 0;
    }else if (n > 3){
        n = 3;
    }
    LOG << "switching to input number : " << n << std::endl;
    /*if (input_ == n){
        return;
    }*/

    input_ = n;

    uint8_t data = n | INPUT_MIC_OFF;
    write_byte_data(SUB_ADDR_INPUT_SELECT_MIC, data);
}


int TDA7468::balance(){
    return balance_;
}

bool TDA7468::balance(int b){
    if (b < -50){
        b = -50;
    }else if (b > 50){
        b = 50;
    }
    if (balance_ != b){
        balance_ = b;
        return true;
    }
    return false;
}



uint8_t TDA7468::encode_bass_treble(int bass, int treble){
    uint8_t tmp_b;
    //uint8_t tmp_b = 0;
    if (bass <= 0){
        tmp_b = bass / 2 + 7;
    }else{
        tmp_b = 7 - (bass / 2);
        tmp_b = tmp_b | 0b1000;
    }

    uint8_t tmp_t;
    //uint8_t tmp_b = 0;
    if (treble <= 0){
        tmp_t = treble / 2 + 7;
    }else{
        tmp_t = 7 - (treble / 2);
        tmp_t = tmp_t | 0b1000;
    }

    return ((tmp_b << 4) | tmp_t);
}
