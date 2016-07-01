#include "hw_interface/TDA7468.h"
#include <stdlib.h>
#include "util.h"

#include <thread>
#include <chrono>

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

// Conclusion :
// Scale volume from -87db to (0db) +14db, total (88) 102 steps. !! last 1 steps by 2db
// volume = 8 * V2_8db + 8 * V1_8db + V1_1db + 
// from ADC, get targeted volume, that uses 1024 steps
// divide ADC by div = (1024 / num_steps). Avoid "edge" values leading to lots of volume change like this :
// |--> keep current_val, which is ((old_read + div/2) % div)
// |--> Only change volume when ((new_read + div/2) % div) changes from current_val
// 
// Considering Target_V as absolute value [0; 87]
// V1_1db = Target_V % 8
// V1_8db = (Target_V / 8) % 8
// V2_8db = Target_V / 8 - V1_8db
//
// if Target_V > 0
//   sel V1_1db, V1_8db and V2_8db to 0
//   set V_in_gain = Target_V / 2

#define SLEEP_MSEC 50

TDA7468::TDA7468(uint8_t addr, int channel) 
    : I2C_Device(addr, channel), 
    volume_(0xffff), input_gain_(0xffff), bass_(0xffff), treble_(0xffff), 
    mute_(false), input_(0xffff), balance_(0xffff)
{
    LOG << "start init TDA 7468" << std::endl;
    init_I2C();
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));
    //write_byte(POWER_ON_RESET);
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    //disable suround
    ///write_byte_data(SUB_ADDR_SURROUND, 0b00000000);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //trebel & bass to mid point for test
    //write_byte_data(SUB_ADDR_TREBLE_BASS, 0b11111111);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    //turn off bass ALC
    //write_byte_data(SUB_ADDR_BASS_ALC, 0b00000000);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    //DAC is input 1
    input(1);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));
    
    //meaning 0 attenuation
    volume(0);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));
    
    //un-mute output
    mute_output(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MSEC));
    
    
    //write_byte(POWER_ON_RESET);
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
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
    /*if (volume_ == v){
        return;
    }*/
    
    volume_ = v;
    uint8_t v1_1db, v1_8db, v2_8db, input_gain;
    if (volume_ < 0){
        v1_1db = volume_ % 8;
        v1_8db = (volume_ / 8) % 8;
        v2_8db = volume_ / 8 - v1_8db;
        input_gain = 0;
    }else{
        v1_1db = 0;
        v1_8db = 0;
        v2_8db = 0;
        input_gain = volume_ / 2;
    }
    
    //chip addr has been set with ioctl(...)
    
    //TODO consider balance
    
    uint8_t data = 0;
    data = (v2_8db << 6) | (v1_8db << 3) | v1_1db;
    write_byte_data(SUB_ADDR_VOLUME_LEFT, data);
    
    write_byte_data(SUB_ADDR_VOLUME_RIGHT, data);
    
    write_byte_data(SUB_ADDR_INPUT_GAIN, input_gain);
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
    if (n < 1){
        n = 1;
    }else if (n > 4){
        n = 4;
    }
    LOG << "switching to input number : " << n << std::endl;
    /*if (input_ == n){
        return;
    }*/
    
    input_ = n;

    uint8_t data = (n - 1) | INPUT_MIC_OFF;
    write_byte_data(SUB_ADDR_INPUT_SELECT_MIC, data);
}


int TDA7468::balance(){
    return balance_;
}

void TDA7468::balance(int b){
    //TODO
}



uint8_t TDA7468::encode_bass_treble(int bass, int treble){
    uint8_t tmp_b = abs(abs(bass) / 2 - 7);
    if (tmp_b > 0){
        tmp_b = tmp_b | 0b1000;
    }
    
    uint8_t tmp_t = abs(abs(treble) / 2 - 7);
    if (tmp_t > 0){
        tmp_t = tmp_t | 0b1000;
    }
    
    return ((tmp_b << 4) | tmp_t);
}

