#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <thread>
#include <deque>
#include <chrono>
#include <mutex>
#include <memory>

#include "module.h"
#include "util.h"

#define LONG_PRESS_MILLISEC 3000

namespace input{
enum event_type{
    EVENT_VALUE_0 = 0,
    EVENT_VALUE_1,
    EVENT_FALL_EDGE,
    EVENT_RISE_EDGE,
    EVENT_PRESSED,
    EVENT_RELEASED,
    EVENT_LONG_PRESS
};

};

/** used to store value in fifo  */
struct ButtonEvent : public Message{
    /*ButtonEvent(int origin, input::event_type type)
        : Message(origin), origin(origin), type(type), tp(std::chrono::high_resolution_clock::now())
    {}*/
    
    ButtonEvent(int origin, input::event_type type, int v)
        : Message(origin), origin(origin), type(type), tp(std::chrono::high_resolution_clock::now())
        , value(v)
    {}
    
    int origin;
    std::chrono::high_resolution_clock::time_point tp;
    input::event_type type;
    int value;
    
    static const int MESSAGE_TYPE = 1;
    virtual int message_type(){return MESSAGE_TYPE;};
    virtual std::string print(){
        std::stringstream ss;
        ss << " - origin:" << origin << " - type:" << type << " - value:" << value;
        return ss.str();
    };
};

#define DELAY_BETWEEN_SAMPLE 5//2////5good//2//5//1

/**
Button is an abstract class used to implement behaviour when a button used by human
Button :
    _ permanently samples/read the input
    _ keep timestamp for value change
    _ averages input read to avoid false positive
    _ ensure low latency
    _ permit custom behaviour by realizing handler() function
*/
class Button{
public:
    /** Create a Button
    WARNING : Button instances should be created in a single thread, one at-a-time. Else weird GPIO read occur
    @param gpio_pin read from this pin
    @param pull_resistor <0 is pull down, >0 is pull up, 0 is no pull */
    Button(int gpio_n, std::shared_ptr<ThreadSafeQ<std::shared_ptr<Message> > > bus, int pull_resistor=0, unsigned int wait_millisec=DELAY_BETWEEN_SAMPLE) 
        : m_gpio_num(gpio_n), m_bus(bus), shouldRun(true) , m_pull_resistor(pull_resistor), m_last_read(-1), m_paused(false), m_wait_millisec(wait_millisec)
    {
        init();
    }
    
    ~Button();
    virtual int process_read(int v)=0;
    
    /** when paused, stop sampling, save CPU cycle --> save energy */
    void pause();
    /** resume sample */
    void resume();

protected:
    /** gpio pin used as input */
    int m_gpio_num;
    /** pull up(>0), down (<0), or no pull (0) */
    int m_pull_resistor;
    
    //last read_value
    int m_last_read;
    
    //std::shared_ptr<module> m_coordinator;
    std::shared_ptr<ThreadSafeQ<std::shared_ptr<Message> > > m_bus;
    
    /** init GPIO and start the thread */
    int init();
    
    bool shouldRun;
    /** read the input, do average to avoid false positive */
    std::thread sample_thread;
    void sample_thread_handler();
    
    /** if paused, save CPU cycle --> save energy */
    bool m_paused;
    
    /** millisec to wait between sample
    useful with different types of buttons that requires different sample for accuracy */
    unsigned int m_wait_millisec;
};

#endif
