#ifndef _MODULE_H_
#define _MODULE_H_

#include <thread>
#include "util.h"

class Samsung_20T202DA2JA;
//#include "hw/Samsung_20T202DA2JA.h"

#include <memory>

class module{
public:
    module() {}
    module(std::shared_ptr<Samsung_20T202DA2JA> screen)
        : m_screen(screen), m_shouldRun(false) {}
    ~module();
     
    void start();
    void stop();
    void pause();
    
    int enqueMessage(std::shared_ptr<Message>);
    
    //static ThreadSafeQ<Message> static_bus;

protected:
    volatile bool m_shouldRun;
    
    std::shared_ptr<Samsung_20T202DA2JA> m_screen;
    
    virtual int threadHandler()=0;
    std::thread m_module_thread;
    
    ThreadSafeQ<std::shared_ptr<Message> > m_q;
};
#endif
