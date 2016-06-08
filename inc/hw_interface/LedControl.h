#ifndef _LED_CONTROL_
#define _LED_CONTROL_

#include "module.h"

/** Control a LED,
including a dedicated thread to blink it */
class LedControl : public module {
public:
    /** Object to control a LED
    @param pin gpio pin number to use
    @param period milliseconds between each period
    @dutty_cycle % of the time LED is one */
    LedControl(int pin, int period, int dutty_cycle) : 
        m_gpio_pin(pin), m_period(period), m_dutty_cycle(dutty_cycle), m_enable_blink(false)
    {init();}
    
    /** enable blinking thread */
    void enable_blink();
    /** disable blinking thread, mostly sleeps to save CPU 
    DO NOT turn off light when disabling ! */
    void disable_blink();
    
    void set_period(int period){ m_period = period; }
    void set_dutty_cycle(int dc){ m_dutty_cycle = dc; }
    
    /** set led and disable blink */
    void set_on_no_blink();
    /** set led off disable blink */
    void set_off_no_blink();
    
protected:
    int init();
    virtual int threadHandler();
    
private:
    int m_gpio_pin;
    int m_period;
    int m_dutty_cycle;
    
    /** control led thread, if disabled do not blink */
    bool m_enable_blink;
};
#endif
