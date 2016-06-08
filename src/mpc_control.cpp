#include "mpc_control.h"
#include "util.h"

#define SAVE_PLAYLIST_NAME "mp_hw_control.pls"
#define RADIO_PLAYLIST_NAME "Webradio"

void MpcControl::play(int position){
    std::string cmd = "mpc play " + std::to_string(position);
    Util::exec_read_bash(cmd);
}
void MpcControl::stop(){
    Util::exec_read_bash("mpc stop");
}

void MpcControl::pause(){
    Util::exec_read_bash("mpc pause");
}
void MpcControl::next(){
    Util::exec_read_bash("mpc next");
}

void MpcControl::prev(){
    Util::exec_read_bash("mpc prev");
}

void MpcControl::clear(){
    Util::exec_read_bash("mpc clear");
}

std::shared_ptr<std::vector<std::string> > MpcControl::get_playlist(){
    std::shared_ptr<std::vector<std::string> > arr(new std::vector<std::string>);
    
    return arr;
}

std::string MpcControl::current(){
    std::string str;
    str = Util::exec_read_bash("mpc current");
    return str;
}

void MpcControl::repeat(bool b){
    if (b){
        Util::exec_read_bash("mpc repeat on");
    }else{
        Util::exec_read_bash("mpc repeat off");
    }
}


void MpcControl::save_current_playlist(){
    Util::exec_read_bash("mpc rm " SAVE_PLAYLIST_NAME);
    Util::exec_read_bash("mpc save " SAVE_PLAYLIST_NAME);
}

void MpcControl::load_current_playlist(){
    Util::exec_read_bash("mpc load " SAVE_PLAYLIST_NAME);
}

void MpcControl::load_all_radio_as_playlist(){
    Util::exec_read_bash("mpc ls " RADIO_PLAYLIST_NAME " | mpc load");
}


