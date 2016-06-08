#include "module.h"
#include "util.h"

module::~module(){
    stop();
}

void module::start(){
    LOG << "module::start()" << std::endl;
    if (!m_shouldRun){
        m_shouldRun = true;
        m_module_thread = std::thread(&module::threadHandler, this);
    }
}

void module::stop(){
    if (m_shouldRun){
        m_shouldRun = false;
        if (m_module_thread.joinable())
            m_module_thread.join();
    }
}

int module::enqueMessage(std::shared_ptr<Message> m){
    m_q.enque(m);
}
