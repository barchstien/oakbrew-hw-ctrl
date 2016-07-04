
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

#include <libconfig.h++>
#include <iostream>
#include <iomanip>
#include <cstdlib>
//using namespace std;

#define ADC_THRESHOLD 5
#define ADC_DELAY_BETWEEN_READ 50

int main(int argc, char *argv[]){
    LOG << "-------------------------\nStarting oakbrew-hw-control " << std::endl;
    
    
    libconfig::Config cfg;

    // Read the file. If there is an error, report it and exit.
    try
    {
        cfg.readFile("../oakbrew-hw-ctrl/config.cfg");
    }
    catch(const libconfig::FileIOException &fioex)
    {
        std::cerr << "I/O error while reading file." << std::endl;
        return(EXIT_FAILURE);
    }
    catch(const libconfig::ParseException &pex)
    {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
        << " - " << pex.getError() << std::endl;
        return(EXIT_FAILURE);
    }
    
    libconfig::Setting &root = cfg.getRoot();
    if(! root.exists("adc")){
        LOG << "Cannot find adc in config file" << std::endl;
        exit(51);
    }

    Config config;
    libconfig::Setting &mcp3008_config = root["adc"];
    mcp3008_config.lookupValue("spi-channel", config.spi_channel);
    mcp3008_config.lookupValue("volume-channel", config.volume_channel);
    mcp3008_config.lookupValue("balance-channel", config.balance_channel);
    mcp3008_config.lookupValue("bass-channel", config.bass_channel);
    mcp3008_config.lookupValue("treble-channel", config.treble_channel);
    
    LOG << "spi-channel : " << config.spi_channel << std::endl;
    LOG << "volume-channel : " << config.volume_channel << std::endl;
    LOG << "balance-channel : " << config.balance_channel << std::endl;
    LOG << "bass-channel : " << config.bass_channel << std::endl;
    LOG << "treble-channel : " << config.treble_channel << std::endl;
    
    MCP3008 adc(config.spi_channel);
    int volume = 0, balance = 0, bass = 0, treble = 0;
    int tmp;
    
    while (true){
        //balance
        tmp = adc.get_value(config.balance_channel);
        if (tmp > (balance + ADC_THRESHOLD) || tmp < (balance - ADC_THRESHOLD)){
            balance = tmp;
            LOG << "balance : " << balance << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ADC_DELAY_BETWEEN_READ));
        
        //volume
        tmp = adc.get_value(config.volume_channel);
        if (tmp > (volume + ADC_THRESHOLD) || tmp < (volume - ADC_THRESHOLD)){
            volume = tmp;
            LOG << "volume : " << volume << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ADC_DELAY_BETWEEN_READ));
        
        
        //bass
        tmp = adc.get_value(config.bass_channel);
        if (tmp > (bass + ADC_THRESHOLD) || tmp < (bass - ADC_THRESHOLD)){
            bass = tmp;
            LOG << "bass : " << bass << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ADC_DELAY_BETWEEN_READ));
        
        //treble
        tmp = adc.get_value(config.treble_channel);
        if (tmp > (treble + ADC_THRESHOLD) || tmp < (treble - ADC_THRESHOLD)){
            treble = tmp;
            LOG << "treble : " << treble << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(ADC_DELAY_BETWEEN_READ));
        
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    return 0;
    
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
        if(std::cin.fail())
        {
            // user didn't input a number
            std::cin.clear(); // reset failbit
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //skip bad input
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
    
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    LOG << "End World" << std::endl;
    return 0;
}




