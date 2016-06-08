
#include "util.h"

#include "hw_interface/RPi_GPIO.h"
//#include "hw_interface/SPI_Device.h"
#include "hw_interface/PCM512x_spi.h"

#include <chrono>
#include <vector>
#include <thread>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

/** pin map */
#define GPIO_UNMUTE 26


using namespace std;

#define PCM512X_SPI_CHAN 0


int main(int argc, char *argv[]){
    LOG << "-------------------------\nStarting music-player-hw-control " << endl;
    
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
    
    LOG << "End World" << endl;
    return 0;
}




