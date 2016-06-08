#ifndef _UTIL_
#define _UTIL_

#include <string>
#include <sstream>
#include <iostream>
#include <deque>
#include <memory>
#include <mutex>


/** Utilities that cannot be classed in other files */
class Util {
public:
    /** execute a bash command and read output until the end
    WARNING : this may block execution if bash command never ends
    @param cmd the command to execute
    @return the string returned by the bash command or "ERROR" if Pipe couldn't be created */
    static std::string exec_read_bash(std::string cmd);
    
    /** execute a bash command and return the pid of the new process
    DOES NOT read the output of the forked process
    @param cmd the command to execute
    @return pid ofthe forked process*/
    static int fork_exec_bash(const char* cmd);
    
    static std::string get_time_string();
};

//#define LOG std::cout
#define LOG std::cout << Util::get_time_string() << " : "

struct Message{
    Message(int label) : label(label) {}
    Message(int label, std::string str) : label(label), data_string(str) {}
    ~Message() {}
    
    /** the command to execute */
    int label;
    /** data to be used for executing cmd */
    std::string data_string;
    
    static const int MESSAGE_TYPE = 0;
    virtual int message_type(){return MESSAGE_TYPE;};
    virtual std::string print(){
        std::stringstream ss;
        ss << label << " : " << data_string;
        return ss.str();
    };
};

template <class T> class ThreadSafeQ {
public:
    ThreadSafeQ(){};
    int enque(T m);
    T deque();
private:
    std::mutex m_mutex;
    std::deque<T> m_fifo;
};

template <class T> int ThreadSafeQ<T>::enque(T m){
    m_mutex.lock();
    m_fifo.push_back(m);
    m_mutex.unlock();
    return 0;
}

template <class T> T ThreadSafeQ<T>::deque(){
    m_mutex.lock();
    if (0 == m_fifo.size()){
        m_mutex.unlock();
        return 0;
    }
    T m = m_fifo.front();
    m_fifo.pop_front();
    m_mutex.unlock();
    return m;
}

#endif
