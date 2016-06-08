#include "hw_interface/SwitchButton.h"

//SwitchButton::~SwitchButton(){}

int SwitchButton::process_read(int v){
    std::shared_ptr<ButtonEvent> e;
    if (v == 1){
        e = std::shared_ptr<ButtonEvent> (new ButtonEvent(m_gpio_num, input::EVENT_VALUE_1, v));
    }else{
        //v == 0
        e = std::shared_ptr<ButtonEvent> (new ButtonEvent(m_gpio_num, input::EVENT_VALUE_0, v));
    }
    m_bus->enque(e);
    return 0;
}

