#include "hw_interface/Button.h"
#include "hw_interface/RPi_GPIO.h"
#include <chrono>
#include "util.h"

/** number of sample for 1 read*/
#define NUM_OF_SAMPLE 5////10good//25//10//20//5
/** milliseconds beteen sample*/
//#define DELAY_BETWEEN_SAMPLE 5//2////5good//2//5//1
/** minimum % of samples to confirm 1 read value */
#define RATIO_THRESHOLD 59//70good

#define DELAY_WHEN_PAUSED 250

Button::~Button(){
    shouldRun = false;
    if (sample_thread.joinable())
        sample_thread.join();
}

int Button::init(){
    //open GPIO as input
    RPi_GPIO::exportPin(m_gpio_num);
    RPi_GPIO::setInput(m_gpio_num);
    if (m_pull_resistor > 0){
        //LOG << "pull up for id : " << m_gpio_num << std::endl;
        RPi_GPIO::setPullup(m_gpio_num);
    }else if (m_pull_resistor < 0){
        //LOG << "pull down for id : " << m_gpio_num << std::endl;
        RPi_GPIO::setPulldown(m_gpio_num);
    }else{
        //remove any default pull up/down
        RPi_GPIO::setPulloff(m_gpio_num);
    }
    
    //start the threads
    sample_thread = std::thread(&Button::sample_thread_handler, this);
    return 0;
}

void Button::sample_thread_handler(){
    
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    
    LOG << "start button sample for GPIO num : " << m_gpio_num << std::endl;
    while(shouldRun){
        if (m_paused){
            std::this_thread::sleep_for(std::chrono::milliseconds(DELAY_WHEN_PAUSED));
            //continue;
        }
    
        //sample input, then average
        int tmp = 0;
        for(int i=0; i<NUM_OF_SAMPLE; i++){
            tmp += RPi_GPIO::read(m_gpio_num);
            std::this_thread::sleep_for(std::chrono::milliseconds(m_wait_millisec));
        }
        int v = -1;
        if (tmp*100 > NUM_OF_SAMPLE * RATIO_THRESHOLD){
            v = 1;
        }else if (tmp*100 < NUM_OF_SAMPLE * (100 - RATIO_THRESHOLD)){
            v = 0;
        }else{
            //read data is not significant 
            LOG << "$$$$$$$$$$$$$$$$$$$$$$$$$$ " << m_gpio_num << "  " << tmp*100/NUM_OF_SAMPLE << std::endl;
            //LOG << "--------> " << tmp*100/NUM_OF_SAMPLE << std::endl;
            //std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }
        
        if(m_last_read != v){
            //LOG << m_gpio_num << " ---> " << tmp*100/NUM_OF_SAMPLE << std::endl;
            //if (m_last_read != -1)
                process_read(v);
        }
        m_last_read = v;
    }
}

void Button::pause(){
    LOG << " Button::pause()" << std::endl;
    m_paused = true;
}

void Button::resume(){
    m_paused = false;
    LOG << " Button::resume()" << std::endl;
}
