#ifndef _MPC_CONTROL_
#define _MPC_CONTROL_

#include "module.h"
#include <vector>
#include <string>

/** call mpc functions */
class MpcControl : public module {
public:
    static void play(int position=1);
    static void stop();
    static void pause();
    static void next();
    static void prev();
    static void clear();
    static std::shared_ptr<std::vector<std::string> > get_playlist();
    static std::string current();
    static void repeat(bool b);
    
    static void save_current_playlist();
    static void load_current_playlist();
    static void load_all_radio_as_playlist();
};

#endif
