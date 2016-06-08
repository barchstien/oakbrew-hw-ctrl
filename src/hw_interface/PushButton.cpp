#include "hw_interface/PushButton.h"

PushButton::~PushButton(){}

int PushButton::process_read(int v){
    std::shared_ptr<ButtonEvent> e;
    if (v == no_press_value){
        e = std::shared_ptr<ButtonEvent> (new ButtonEvent(m_gpio_num, input::EVENT_RELEASED, 0));
    }else{
        e = std::shared_ptr<ButtonEvent> (new ButtonEvent(m_gpio_num, input::EVENT_PRESSED, 1));
    }
    m_bus->enque(e);
    
    return 0;
}

