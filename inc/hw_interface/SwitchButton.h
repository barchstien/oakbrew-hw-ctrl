#ifndef _SWITCH_BUTTON_H_
#define _SWITCH_BUTTON_H_

#include "hw_interface/Button.h"

#define DELAY_BETWEEN_SAMPLE_SWITCH_BUTTON 50

/** a push button
It can be pressed for a time t to trigger an LONG_PRESS event
It also have a default value when not pressed */
class SwitchButton : public Button{
public:
    /** Create a Switch Button
    @param gpio_pin read from this pin
    @param pull_resistor <0 is pull down, >0 is pull up, 0 is no pull */
    SwitchButton(int gpio_n, 
        std::shared_ptr<ThreadSafeQ<std::shared_ptr<Message> > > bus, 
        int pull_resistor=0, 
        unsigned int wait_millisec=DELAY_BETWEEN_SAMPLE_SWITCH_BUTTON)
        : Button(gpio_n, bus, pull_resistor, wait_millisec)
    {}
    //~SwitchButton();
    virtual int process_read(int v);
    
protected:
    std::shared_ptr<ButtonEvent> last_event;
};

#endif
