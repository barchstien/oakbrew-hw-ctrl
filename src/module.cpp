#include "module.h"
#include "util.h"

module::~module(){
    stop();
}

void module::start(){
    LOG << "module::start()" << std::endl;
    if (!shouldRun_){
        shouldRun_ = true;
        module_thread_ = std::thread(&module::threadHandler, this);
    }
}

void module::stop(){
    if (shouldRun_){
        shouldRun_ = false;
        if (module_thread_.joinable())
            module_thread_.join();
    }
}

int module::enqueMessage(std::shared_ptr<Message> m){
    q_.enque(m);
}
