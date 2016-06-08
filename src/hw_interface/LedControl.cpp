#include "hw_interface/LedControl.h"
#include "hw_interface/RPi_GPIO.h"
//#include <chrono>
#include "util.h"

//TODO make dutty cycle 0% and 100% more energy/cycle saving

int LedControl::init(){
    //open GPIO as output
    RPi_GPIO::exportPin(m_gpio_pin);
    RPi_GPIO::setOutput(m_gpio_pin);
    return 0;
}

int LedControl::threadHandler(){
    LOG << "Start LedControl::threadHandler() gpio" << m_gpio_pin 
        << " period " << m_period << " d-c " << m_dutty_cycle << std::endl;
    
    int64_t next_action = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
    //off by default
    bool is_on = false;
    RPi_GPIO::write(m_gpio_pin, 0);
    
    while(m_shouldRun){
        if (m_enable_blink){
            //blink mode
        
            ////TODO if dc 0% or 100%, next_action tiestamp is not updted and CPU are used a lot !!
            ////... for now should disable afer set to d-c to 0 or 100%
            
            //check if next action timestamp is past, and should do next anim step
            int64_t diff = (std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1)) - next_action;
            if (diff >= 0){
                if (is_on ){
                    //next anim is turn off
                    if (m_dutty_cycle != 100){
                        //turn off except if dutty cycle is 100%
                        is_on = false;
                        RPi_GPIO::write(m_gpio_pin, 0);
                        next_action = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1) + m_period * (100 - m_dutty_cycle) / 100;
                    }else{
                        //duty cycle is 100%, do not turn off
                        //update next action timestamp after a full period
                        next_action = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1) + m_period;
                    }
                }else{
                    //next anim is turn on
                    if (m_dutty_cycle != 0){
                        //turn on except if dutty cycle 0%
                        is_on = true;
                        RPi_GPIO::write(m_gpio_pin, 1);
                        next_action = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1) + m_period * m_dutty_cycle / 100;
                    }else{
                        //duty cycle is 0%, do not turn on
                        //update next action timestamp after a full period
                        next_action = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1) + m_period;
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }else{
            //// no blink mode, not much CPU used
            if (m_dutty_cycle == 100 && !is_on){
                //dutty cycle is 100, should turn on led
                RPi_GPIO::write(m_gpio_pin, 1);
                is_on = true;
            }else if (m_dutty_cycle == 0 && is_on){
                //dutty_cycle is 0, should turn off led
                RPi_GPIO::write(m_gpio_pin, 0);
                is_on = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
    return 0;
}

void LedControl::enable_blink(){
    m_enable_blink = true;
}

void LedControl::disable_blink(){
    m_enable_blink = false;
}

void LedControl::set_on_no_blink(){
    m_dutty_cycle = 100;
    m_enable_blink = false;
}

void LedControl::set_off_no_blink(){
    m_dutty_cycle = 0;
    m_enable_blink = false;
}


