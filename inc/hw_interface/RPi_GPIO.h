#ifndef _RPi_GPIO_
#define _RPi_GPIO_

#define NUM_OF_GPIO 54

#include <fstream>

struct pin;

/** values to set to GPPUD register */
#define RPi_PULL_OFF 0b00
#define RPi_PULL_UP 0b10
#define RPi_PULL_DOWN 0b01

/** Access/Set GPIO on Raspberry Pi
Mostly uses sysfs, but also nmap specifically for setting pull up/down resistors */
struct RPi_GPIO {
    //TODO check tht it's ok if called multiple times
    /** export the pin. WARNING is required for each pin prior use ! 
    @warning exportPin() setInput() setOutput() disable any alt function (including i2c) */
    static void exportPin(unsigned int pin);
    
    /** unexport the GPIO, does sysfs unexport */
    static void unexportPin(unsigned int pin);

    /** define pin as input
    @warning exportPin() setInput() setOutput() disable any alt function (including i2c) */
    static void setInput(unsigned int pin);
    /** define pin as output
    @warning exportPin() setInput() setOutput() disable any alt function (including i2c) */
    static void setOutput(unsigned int pin);

    /** set pull up resistor */
    static void setPullup(unsigned int pin);
    /** set pull down resistor */
    static void setPulldown(unsigned int pin);
    /** disable pull up or down resistor */
    static void setPulloff(unsigned int pin);
    
    /** Set pin to alternate function alt 
    GPIO 2, 3 with alt0 are SDA, SCL for i2c*/
    static void setAlt(unsigned int pin, unsigned int alt);

    /** write value (0, 1) to pin. Only works if pin is output
    @param value 0 or 1 */
    static void write(unsigned int pin, int value);
    /** read value from pin. Only works if pin is input
    @return 0 or 1 */
    static int read(unsigned int pin);
    
    /** Print pin state that have been exported
    or all if all set to true 
    WARNING : pin setup with other system are considered unexported */
    static void print_pin_array(bool all=false);
    
    /** unexport all pins that have been exported */
    static void unexport_all();
    
protected:
    /** get the descriptor to GPIO control register
    do the nmap(). close_gpio_ptr() should be used work on GPIO control is done
    @return the pointer to the map memory (using unsigned int ptr has in datasheet (32bit))*/
    static unsigned int* get_gpio_ptr();
    
    /** unmap() 
    @param ptr the ptr returned byt get_gpio_ptr() */
    static void close_gpio_ptr(unsigned int* ptr);

    /** hold state info and fd about each pin */
    static pin pin_array[NUM_OF_GPIO];
    
    /** set pull up/down/off resistor
    @param pin the pin number
    @param pull RPi_PULL_UP/RPi_PULL_DOWN/RPi_PULL_OFF */
    static void setPullResistor(unsigned int pin, int pull);
};

/** state of a GPIO pin
Remember state and hold fstream to avoid constant open/close */
struct pin{
    pin() : exported(false), in(false), out(false),
            pull_up(false), pull_down(false), value(-1),
            alt(-1) {};
    std::fstream fd_direction;
    std::fstream fd_value;
    bool exported;
    bool in;
    bool out;
    bool pull_up;
    bool pull_down;
    int value;
    /** alternate function [0-5] -1 if not in alt mode */
    int alt;
    
    static void print_header();
    static void print_footer();
    void print(int index);
};
#endif
