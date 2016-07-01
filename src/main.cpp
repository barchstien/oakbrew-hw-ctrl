
#include "util.h"

#include "hw_interface/RPi_GPIO.h"
#include "hw_interface/MCP3008.h"
#include "hw_interface/TDA7468.h"

#include <chrono>
#include <vector>
#include <thread>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include <cmath>

/** pin map */
#define GPIO_UNMUTE 26


using namespace std;

#define PCM512X_SPI_CHAN 0


int main(int argc, char *argv[]){
    LOG << "-------------------------\nStarting oakbrew-hw-control " << endl;
        
    MCP3008 adc(0);
    TDA7468 sound_ctrl;
    
    //this_thread::sleep_for(chrono::milliseconds(500));
    //sound_ctrl.mute(false);
    //this_thread::sleep_for(chrono::milliseconds(500));
    //set to 14 to enable input gain
    //sound_ctrl.volume(0);
    //this_thread::sleep_for(chrono::milliseconds(10));

    //DAC is input 1
    //sound_ctrl.input(1);
    //this_thread::sleep_for(chrono::milliseconds(500));
    
#if 0
    for (int i=-87; i<15; i++){
        sound_ctrl.volume(i);
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
#endif

    int v = -87;
    sound_ctrl.volume(v);
    while (true){
        std::cout << "volume : " << std::endl;
        std::cin >> v;
        if(cin.fail())
        {
            // user didn't input a number
            cin.clear(); // reset failbit
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
            // next, request user reinput
        }else{
            sound_ctrl.volume(v);
        }
    }

#if 0
    ///
    RPi_GPIO::exportPin(GPIO_UNMUTE);
    RPi_GPIO::setOutput(GPIO_UNMUTE);
    
    
    this_thread::sleep_for(chrono::milliseconds(1000));
    shared_ptr<PCM512x_spi> pcm512x_spi_ptr;
    pcm512x_spi_ptr = shared_ptr<PCM512x_spi>(new PCM512x_spi(PCM512X_SPI_CHAN));
    this_thread::sleep_for(chrono::milliseconds(1000));
    
    //unmute
    LOG << "unmuting DAC" << endl;
    RPi_GPIO::write(GPIO_UNMUTE, 0);
    this_thread::sleep_for(chrono::milliseconds(500));
    RPi_GPIO::write(GPIO_UNMUTE, 1);
    //this_thread::sleep_for(chrono::milliseconds(500));
    //RPi_GPIO::write(GPIO_UNMUTE, 0);
    //this_thread::sleep_for(chrono::milliseconds(500));

    LOG << "init finished" << std::endl;

    while (true){
        //RPi_GPIO::write(GPIO_UNMUTE, 1);
        this_thread::sleep_for(chrono::milliseconds(2000));
        //RPi_GPIO::write(GPIO_UNMUTE, 0);
        //this_thread::sleep_for(chrono::milliseconds(2000));
    }
#endif
    
    this_thread::sleep_for(chrono::milliseconds(10000));
    LOG << "End World" << endl;
    return 0;
}




