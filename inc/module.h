#ifndef _MODULE_H_
#define _MODULE_H_

#include <thread>
#include "util.h"

#include <memory>

class module{
public:
    module() : shouldRun_(false) {}
    ~module();

    void start();
    void stop();

    int enqueMessage(std::shared_ptr<Message>);

protected:
    volatile bool shouldRun_;

    virtual int threadHandler()=0;
    std::thread module_thread_;

    ThreadSafeQ<std::shared_ptr<Message> > q_;
};
#endif
