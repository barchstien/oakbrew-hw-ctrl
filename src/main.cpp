
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

    libconfig::Setting &mux_config = root["mux"];
    mux_config.lookupValue("rpi-gpio-switch-dac", config.rpi_gpio_swith_dac);
    LOG << "rpi-gpio-switch-dac : " << config.rpi_gpio_swith_dac << std::endl;
    RPi_GPIO::exportPin(config.rpi_gpio_swith_dac);
    RPi_GPIO::setInput(config.rpi_gpio_swith_dac);

    TDA7468 sound_ctrl;

    MCP3008 adc(config.spi_channel);
    adc.enable_channel(config.volume_channel);
    adc.enable_channel(config.balance_channel);
    adc.enable_channel(config.treble_channel);
    adc.enable_channel(config.bass_channel);
    adc.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));

    int volume = 0, balance = 0, bass = 0, treble = 0;
    int audio_input = -1;
    int tmp = -1;

    LOG << "main loop ..." << std::endl;

    while (true){
        //volume
        tmp = adc.get_value(config.volume_channel);
        if (tmp > (volume + ADC_THRESHOLD) || tmp < (volume - ADC_THRESHOLD)){
            volume = tmp;

            //TODO debug
            int v = volume * 87 / MCP3008::MAX_VALUE -87;
            LOG << "volume : " << volume << "  set db : " << v << std::endl;
            sound_ctrl.volume(v);
        }

        //balance
        tmp = adc.get_value(config.balance_channel);
        if (tmp > (balance + ADC_THRESHOLD) || tmp < (balance - ADC_THRESHOLD)){
            balance = tmp;
            int b = balance / 10 - 50;
            LOG << "balance : " << balance << " set balance : " << b << std::endl;
            sound_ctrl.balance(b);
            //update volume
            sound_ctrl.volume(sound_ctrl.volume());
        }

        //bass
        tmp = adc.get_value(config.bass_channel);
        if (tmp > (bass + ADC_THRESHOLD) || tmp < (bass - ADC_THRESHOLD)){
            bass = tmp;
            //map to [0; 14], value is rounded @ 2db by sound_ctrl
            int b = (bass * 15 / MCP3008::MAX_VALUE -7) * 2;
            //LOG << "bass : " << bass << "  set db : " << b << std::endl;
            sound_ctrl.bass(b);
        }

        //treble
        tmp = adc.get_value(config.treble_channel);
        if (tmp > (treble + ADC_THRESHOLD) || tmp < (treble - ADC_THRESHOLD)){
            treble = tmp;
            //map to [0; 14], value is rounded @ 2db by sound_ctrl
            int t = (treble * 15 / MCP3008::MAX_VALUE -7) * 2;
            //LOG << "treble : " << treble << "  set db : " << t << std::endl;
            sound_ctrl.treble(t);
        }

        //input
        tmp = RPi_GPIO::read(config.rpi_gpio_swith_dac);
        if (tmp != audio_input){
            audio_input = tmp;
            sound_ctrl.input(audio_input);
        }

        //LOG << "blop" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //DAC is input 1
    //sound_ctrl.input(1);
    //this_thread::sleep_for(chrono::milliseconds(500));

    LOG << "End World" << std::endl;
    return 0;
}
