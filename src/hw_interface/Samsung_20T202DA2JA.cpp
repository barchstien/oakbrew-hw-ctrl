
#include "hw_interface/Samsung_20T202DA2JA.h"
//#include "util.h"

//default SPI properties
// NB : 1 000 000 is too high !!
#define Samsung_20T202DA2JA_SPEED 100000//50000//100000 very few times a letter is missing
                                        //i think that was due to bad 3.3v
#define Samsung_20T202DA2JA_DELAY 0
#define Samsung_20T202DA2JA_WORD_LENGTH 0 //8 + 0 = 8bit
#define Samsung_20T202DA2JA_MODE 3 //CPOL 1 CPHA 1

//register values for divers command
#define OFF 0
#define BLINK_ON 1
#define CURSOR_ON 2
#define DISPLAY_ON 4

#define SPI_CMD_SHIFT_LEFT 0x14

#define SCROLL_2ND_LINE 0x1

#define DELAY_SCROLL_END 3000
#define DELAY_SCROLL_MIDDLE 300

using namespace std;

Samsung_20T202DA2JA::Samsung_20T202DA2JA(int channel) 
    : SPI_Device(channel, 
                Samsung_20T202DA2JA_MODE, 
                Samsung_20T202DA2JA_SPEED, 
                Samsung_20T202DA2JA_DELAY, 
                Samsung_20T202DA2JA_WORD_LENGTH), 
    position(0), blink_ctrl(OFF), cursor_ctrl(OFF), display_ctrl(OFF)
{
    init_SPI();
    clear();
    on();
}

Samsung_20T202DA2JA::~Samsung_20T202DA2JA(){
    close_SPI();
}

void Samsung_20T202DA2JA::clear(){
    //bad idea coz it takes time to take effect and lead to conflicts
    //scroll_on_2nd_line(std::string(""));
    disable_scroll();
    
    //send_spi_word(0xf8, 0x01);
    char buff[] = {0xf8, 0x01};
    m_screen_mutex.lock();
    send_buff(buff, 2);
    position = 0;
    m_screen_mutex.unlock();
}


void Samsung_20T202DA2JA::blink(bool enable){
    m_screen_mutex.lock();
    if (enable)
        blink_ctrl = BLINK_ON;
    else
        blink_ctrl = OFF;
    //send_spi_word(0xf8, 0x8 + display_ctrl + cursor_ctrl + blink_ctrl);
    char buff[] = {0xf8, static_cast<char>(0x8 + display_ctrl + cursor_ctrl + blink_ctrl)};
    send_buff(buff, 2);
    m_screen_mutex.unlock();
}

void Samsung_20T202DA2JA::cursor(bool enable){
    m_screen_mutex.lock();
    if (enable)
        cursor_ctrl = CURSOR_ON;
    else
        cursor_ctrl = OFF;
    //send_spi_word(0xf8, 0x8 + display_ctrl + cursor_ctrl + blink_ctrl);
    char buff[] = {0xf8, static_cast<char>(0x8 + display_ctrl + cursor_ctrl + blink_ctrl)};
    send_buff(buff, 2);
    m_screen_mutex.unlock();
}

void Samsung_20T202DA2JA::cursor_home(){
    m_screen_mutex.lock();
    cursor_home_no_lock();
    m_screen_mutex.unlock();
}

void Samsung_20T202DA2JA::cursor_home_no_lock(){
    char buff[] = {0xf8, 0x02};
    send_buff(buff, 2);
    position = 0;
}

void Samsung_20T202DA2JA::on(){
    char buff[] = {0xf8, 0x0c};
    
    m_screen_mutex.lock();
    display_ctrl = DISPLAY_ON;
    send_buff(buff, 2);
    m_screen_mutex.unlock();
}

void Samsung_20T202DA2JA::off(){
    char buff[] = {0xf8, 0x08};
    
    m_screen_mutex.lock();
    display_ctrl = OFF;
    send_buff(buff, 2);
    m_screen_mutex.unlock();
}

void Samsung_20T202DA2JA::cursor_shift(int n){
    m_screen_mutex.lock();
    cursor_shift_no_lock(n);
    m_screen_mutex.unlock();
}
void Samsung_20T202DA2JA::cursor_shift_no_lock(int n){
    //cout << "shifting from " << position << " to " << position + n << endl;
    
    //this is shift left
    int increment = 1;
    char shift_cmd = SPI_CMD_SHIFT_LEFT;
    if (n < 0){
        //shift to right
        n = -1 * n;
        shift_cmd -= 0x4;
        increment = -1;
    }
    //remove loops of shifting
    n = n%40;

    for (int i=0; i<n; i++){
        //spi shift
        char buff[] = {0xf8, shift_cmd};
        send_buff(buff, 2);
        position += increment;
        if (position == 20 && increment > 0){
            //passed end of first line, pass the 20 hidden positions
            for (int j=0; j<20; j++){
                //shift left spi
                char buff[] = {0xf8, shift_cmd};
                send_buff(buff, 2);
            }
        }else if (position == 40 && increment > 0){
            //passed end of 2nd line, back to 0
            cursor_home_no_lock();
        }else if (position == -1 && increment < 0){
            //passed beginning of 1st line, go up the 20 hidden positions
            position += 40;
            for (int j=0; j<20; j++){
                //spi shift right
                char buff[] = {0xf8, shift_cmd};
                send_buff(buff, 2);
            }
        }else if (position == 19 && increment < 0){
            //passed beginning of 2nd line, go up the 20 hidden positions
            for (int j=0; j<20; j++){
                //spi shift right
                char buff[] = {0xf8, shift_cmd};
                send_buff(buff, 2);
            }
        }
    }
}

#include <sstream>
#include "util.h"
void Samsung_20T202DA2JA::write(std::string str, bool center){
    //write(str.c_str(), str.size(), center);
    clear();
    int len = str.size();
    if (center){
        std::stringstream ss;
        for (int i=0; i<(20-len)/2; i++){
            ss << " ";
        }
        ss << str;
        append(ss.str().c_str(), ss.str().size());
    }else{
        append(str.c_str(), len);
    }
}

void Samsung_20T202DA2JA::append(const char* str, int len){
    m_screen_mutex.lock();
    append_no_lock(str, len);
    m_screen_mutex.unlock();
}
void Samsung_20T202DA2JA::append_no_lock(const char* str, int len){
    for (int i=0; i<len; i++){
        //send_spi_word(0xfa, str[i]);
        char buff[] = {0xfa, str[i]};
        send_buff(buff, 2);
        position ++;
        if (position == 20){
            //passed end of first line, pass the 20 hidden positions
            for (int j=0; j<20; j++){
                char buff[] = {0xf8, 0x14};
                send_buff(buff, 2);
            }
        }else if (position == 40){
            //passed end of 2nd line, back to 0
            cursor_home_no_lock();
        }
    }
}

void Samsung_20T202DA2JA::scroll_on_2nd_line(std::string str){
    enqueMessage(std::shared_ptr<Message> (new Message(SCROLL_2ND_LINE, str)));
}

void Samsung_20T202DA2JA::write_line(int line_number, std::string str, int v_align){
    cursor_home();
    if (1 == line_number){
        cursor_shift(20);
    }
    const char* buff = str.c_str();
    int len = str.size();
    if (len > 20)
        str = str.substr(0, 20);
    if (v_align == 0){
        //align left
        append(str.c_str(), str.size());
        //blank the rest of the line
        std::stringstream ss;
        for (int i=0; i<(20-len); i++){
            ss << " ";
        }
        append(ss.str().c_str(), ss.str().size());
    }else if (v_align == 1){
        //align right
        std::stringstream ss;
        for (int i=0; i<(20-len); i++){
            ss << " ";
        }
        ss << str;
        append(ss.str().c_str(), ss.str().size());
    }else if (v_align == 2){
        //align center
        std::stringstream ss;
        for (int i=0; i<((20-len)/2 + ((20-len)%2) ); i++){
            ss << " ";
        }
        ss << str;
        //blank the rest of the line
        for (int i=0; i<((20-len)/2); i++){
            ss << " ";
        }
        append(ss.str().c_str(), ss.str().size());
    }
}


int Samsung_20T202DA2JA::init(){
    return 0;
}

int Samsung_20T202DA2JA::threadHandler(){
    LOG << "staring Samsung_20T202DA2JA thread handler" << std::endl;
    
    //2nd line string to scroll horizontally
    std::string string_line_2;
    m_enable_scroll = false;
    std::shared_ptr<Message> m = 0;
    int offset = 0;
    int increment = 1; //or -1 if scrolling backward
    
    int64_t next_anim_timestamp;
    
    //doing screen effects
    while (m_shouldRun){
        m = m_q.deque();
        if (0 != m){
            if (SCROLL_2ND_LINE == m->label){
                string_line_2 = m->data_string;
                LOG << "SCROLL_2ND_LINE : " << string_line_2 << std::endl;
                if (string_line_2.size() > 0){
                    m_enable_scroll = true;
                }else{
                    m_enable_scroll = false;
                    //blank second line
                    m_screen_mutex.lock();
                    this->cursor_home_no_lock();
                    this->cursor_shift_no_lock(20);
                    this->append_no_lock("                    ", 20);
                    m_screen_mutex.unlock();
                }
                offset = 0;
                increment = -1;
                next_anim_timestamp = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
            }
        }
        
        int64_t now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
        if (m_enable_scroll && ((now - next_anim_timestamp) >= 0)){
            
            m_screen_mutex.lock();
            
            this->cursor_home_no_lock();
            this->cursor_shift_no_lock(20);
            std::string tmp = string_line_2;
            if (string_line_2.size() > 20){
                tmp = string_line_2.substr(offset, 20);
            }
            this->append_no_lock(tmp.c_str(), tmp.size());
            
            m_screen_mutex.unlock();
            
            if (offset == 0 || offset == (string_line_2.size() - 20) ){
                next_anim_timestamp = now + DELAY_SCROLL_END;
                //inverse scrollng direction
                increment = increment * -1;
            }else{
                next_anim_timestamp = now + DELAY_SCROLL_MIDDLE;
            }
            offset += increment;
        }else{
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    return 0;
}

void Samsung_20T202DA2JA::disable_scroll(){
    m_enable_scroll = false;
}


