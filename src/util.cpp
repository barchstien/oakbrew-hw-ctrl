
#include "util.h"
#include <unistd.h>

std::string Util::exec_read_bash(std::string cmd) {
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return result;
}

int Util::fork_exec_bash(const char* cmd) {
    LOG << "fork_exec_bash : " << cmd << std::endl;
    int pid = fork();
    if (pid < 0){
        LOG << "ERROR : fork didn't succeed" << std::endl;
    }else if (pid == 0){
        //child process
        execl("/bin/sh","sh","-c", cmd, (char*) NULL);
        exit(0);
    }
    // pid > 0
    return pid;
}

#include <sstream>
std::string Util::get_time_string(){
    std::stringstream ss;
    
    time_t t;
    time(&t);
    struct tm * timeinfo;
    timeinfo = localtime (&t);
    char buff[80];
    strftime (buff, 80 , "%c", timeinfo);
    //LOG << "\n\n-------------------------\nStarting WebRadio " << buff << std::endl;
    ss << buff;
    
    return ss.str();
}

