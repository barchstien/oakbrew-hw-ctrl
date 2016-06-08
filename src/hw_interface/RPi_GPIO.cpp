/**  Extracted from http://elinux.org/RPi_Low-level_peripherals 5/12/2013
//  ... and modified to suite better c++
**/


#include <hw_interface/RPi_GPIO.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "util.h"
#include <sstream>

#include <chrono>
#include <thread>

using namespace std;

/** Resources
http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
http://elinux.org/RPi_Low-level_peripherals
*/

/** base address for chip peripherals */
#define BCM2708_PERI_BASE 0x20000000
/** offset to GPIO */
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000)
#define BLOCK_SIZE (4*1024)

//value is extracted from datasheet
//value is divided by 4 because pointer to register is unsigned int (4 bytes)
/** relative GPPUD register (where the pullup/down/none is set) */
#define REG_GPPUD (0x94/sizeof(unsigned int))
/** relative GPPUDCLK0 register (where which pin the GPPUD is applied to) */
#define REG_GPPUDCLK0 (0x98/sizeof(unsigned int))
/** relative GPPUDCLK1 register (rest of the pin) */
#define REG_GPPUDCLK1 (0x9C/sizeof(unsigned int))

//datasheet says wait 150 cycles. At 700Mhz, it's 0.2 usec. Let's take 1usec
#define WAIT_150_CYCLES std::this_thread::sleep_for(std::chrono::microseconds(1));

pin RPi_GPIO::pin_array[NUM_OF_GPIO];

unsigned int* RPi_GPIO::get_gpio_ptr(){
    int mem_fd = 0;

    /* open /dev/mem */
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("can't open /dev/mem. Try with sudo \n");
        exit(-1);
    }

    /* mmap GPIO */
    void* gpio_map = mmap(
        NULL,             //Any adddress in our space will do
        BLOCK_SIZE,       //Map length
        PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
        MAP_SHARED,       //Shared with other processes
        mem_fd,           //File to map
        GPIO_BASE         //Offset to GPIO peripheral
    );

    close(mem_fd); //No need to keep mem_fd open after mmap

    if (gpio_map == MAP_FAILED) {
        printf("mmap error %d\n", (int)gpio_map);//errno also set!
        exit(-1);
    }

    return (unsigned int*)gpio_map;
}

void RPi_GPIO::close_gpio_ptr(unsigned int* ptr){
    munmap(ptr, BLOCK_SIZE);
}

void RPi_GPIO::unexport_all(){
    //close all fd for each exported pin, and unexport theme
    for (int i = 0; i < NUM_OF_GPIO; i++){
        if (pin_array[i].exported){
            unexportPin(i);
        }
    }
}

/**Macro for all void fuctions 
This avoids to access out of bound memory of pin_array */
#define PIN_NUM_CHECK(x) if (x > NUM_OF_GPIO - 1){\
                            LOG << x << " is not a valid pin number : [0 - " << NUM_OF_GPIO << "]"<< endl;\
                            return;\
                        }

void RPi_GPIO::exportPin(unsigned int pin){
    PIN_NUM_CHECK(pin);
    std::fstream fs;
    fs.open ("/sys/class/gpio/export", std::fstream::out | std::fstream::binary);
    if (! fs.is_open()){
        LOG << "Failed openning /sys/class/gpio/export" << endl;
        return;
    }
    fs << std::to_string(pin);
    fs.close();
    //open the direction...
    std::stringstream ss;
    ss << "/sys/class/gpio/gpio" << pin << "/direction";
    pin_array[pin].fd_direction.open(ss.str());
    if (! pin_array[pin].fd_direction.is_open()){
        LOG << "Failed openning " << ss.str() << endl;
        return;
    }
    //... and value files for the pin_array
    ss.str( std::string() );
    ss << "/sys/class/gpio/gpio" << pin << "/value";
    pin_array[pin].fd_value.open(ss.str());
    if (! pin_array[pin].fd_direction.is_open()){
        LOG << "Failed openning " << ss.str() << endl;
        return;
    }
    pin_array[pin].exported = true;
    pin_array[pin].alt = -1;
}

void RPi_GPIO::unexportPin(unsigned int pin){
    PIN_NUM_CHECK(pin);
    //close fd
    //direction...
    pin_array[pin].fd_direction.close();
    //... and value files for the pin_array
    pin_array[pin].fd_value.close();
    //unexport
    std::fstream fs;
    fs.open ("/sys/class/gpio/unexport", std::fstream::out | std::fstream::binary);
    if (! fs.is_open()){
        LOG << "Failed openning /sys/class/gpio/unexport" << endl;
        return;
    }
    fs << std::to_string(pin);
    fs.close();
    pin_array[pin].exported = false;
    pin_array[pin].in = false;
    pin_array[pin].out = false;
    pin_array[pin].alt = -1;
}

void RPi_GPIO::setInput(unsigned int pin){
    PIN_NUM_CHECK(pin);
    if (! pin_array[pin].exported)
        LOG << "Error : pin " << pin << " is not exported" << endl;
    pin_array[pin].in = true;
    pin_array[pin].out = false;
    pin_array[pin].alt = -1;
    pin_array[pin].fd_direction << "in" << endl;
}
void RPi_GPIO::setOutput(unsigned int pin){
    PIN_NUM_CHECK(pin);
    if (! pin_array[pin].exported)
        LOG << "Error : pin " << pin << " is not exported" << endl;
    pin_array[pin].in = false;
    pin_array[pin].out = true;
    pin_array[pin].alt = -1;
    pin_array[pin].fd_direction << "out" << endl;
}

void RPi_GPIO::setPullup(unsigned int pin){
    setPullResistor(pin, RPi_PULL_UP);
}
void RPi_GPIO::setPulldown(unsigned int pin){
    setPullResistor(pin, RPi_PULL_DOWN);
}
void RPi_GPIO::setPulloff(unsigned int pin){
    setPullResistor(pin, RPi_PULL_OFF);
}

void RPi_GPIO::setPullResistor(unsigned int pin, int pull){
    PIN_NUM_CHECK(pin);
    unsigned int* gpio = get_gpio_ptr();
    
    if (gpio == 0)
        LOG << "Error : setup_io() was not called, aborting setPullResistor()" << endl;
    if (! pin_array[pin].exported)
        LOG << "Error : pin " << pin << " is not exported" << endl;
    if (! (pin_array[pin].in || pin_array[pin].out))
        LOG << "Error : pin " << pin << " is not an input or output" << endl;
    pin_array[pin].pull_up = false;
    pin_array[pin].pull_down = false;
    if (pull == RPi_PULL_UP){
        pin_array[pin].pull_up = true;
    }else if (pull == RPi_PULL_DOWN){
        pin_array[pin].pull_down = true;
    }
    //set pull mode
    *(gpio + REG_GPPUD) = pull;
    WAIT_150_CYCLES;
    
    //enable clock on pin
    *(gpio + REG_GPPUDCLK0 + (pin/32)) = 1 << (pin%32);
    WAIT_150_CYCLES;

    //clean registers    
    *(gpio + REG_GPPUD) = 0;
    *(gpio + REG_GPPUDCLK0 + (pin/32)) = 0;

    close_gpio_ptr(gpio);
}

void RPi_GPIO::setAlt(unsigned int pin, unsigned int alt){
    PIN_NUM_CHECK(pin);
    unsigned int* gpio = get_gpio_ptr();
    
    if (gpio == 0)
        LOG << "Error : setup_io() was not called, aborting setAlt()" << endl;
    if (alt < 0 || alt > 5){
        LOG << "Error : alt should be within [0-5] but is now : " << (unsigned int)alt << endl;
        return;
    }
    if (pin_array[pin].exported){
        unexportPin(pin);
    }
    //get register offset
    unsigned int reg_offset = pin / 10;
    //get bit offset within register
    unsigned int bit_offset = (pin % 10)*3;
    //get register value
    uint32_t tmp = *(gpio + reg_offset);
    //set new values to the relevant 3 bit
    //first create a mask to zero those 3 bits
    uint32_t mask = ~(0b111 << bit_offset);
    tmp &= mask;
    //compute the new value according to BCM2835 Peripherals doc page 91
    unsigned int new_val = 0b100 + alt;
    if (alt == 4)
        new_val = 0b011;
    if (alt == 5)
        new_val = 0b010;
    //set the value to the register
    tmp |= new_val << bit_offset;
    *(gpio + reg_offset) = tmp;
    pin_array[pin].alt = alt;
    
    close_gpio_ptr(gpio);
}

void RPi_GPIO::write(unsigned int pin, int value){
    PIN_NUM_CHECK(pin);
    if (! pin_array[pin].out){
        LOG << pin << " is not an output, cannot write value : " << value << endl;
        return;
    }
    pin_array[pin].fd_value << value << endl;
    pin_array[pin].value = value;
}

int RPi_GPIO::read(unsigned int pin){
    if (pin > NUM_OF_GPIO - 1){
        LOG << pin << " is not a valid pin number : [0 - " << NUM_OF_GPIO - 1 << "]"<< endl;
        return -1;
    }
    if (! pin_array[pin].in){
        LOG << pin << " is not an input, cannot read value" << endl;
        return -1;
    }
    int val = -1;
    pin_array[pin].fd_value.seekg(0);
    // ? sync()
    pin_array[pin].fd_value >> val;
    return val;
}

void RPi_GPIO::print_pin_array(bool all){
    pin::print_header();
    for (int i=0; i<NUM_OF_GPIO; i++){
        //only print GPIO state of pin which have been exported
        if (pin_array[i].exported || all || pin_array[i].alt != -1)
            pin_array[i].print(i);
    }
    pin::print_footer();
}

/////////////////// Pin Class //////////////////

#include <iomanip>

void pin::print_header(){
    LOG << "+-----+----------+--------+------+-------+" << endl;
    LOG << "| PIN | EXPORTED | in/out | PULL | VALUE |" << endl;
    LOG << "+-----+----------+--------+------+-------+" << endl;
}

void pin::print_footer(){
    LOG << "+-----+----------+--------+------+-------+" << endl;
}

void pin::print(int index){
    //direction print
    string direction;
    if (in){
        direction = "in";
        //if input mode read value
        value = RPi_GPIO::read(index);
    }else if (out){
        direction = "out";
    }else if (alt != -1){
        stringstream ss;
        ss << "alt" << alt;
        direction = ss.str();
    }else{
        direction = "none";
    }
    //pull up/down print
    string pull;
    if (pull_up){
        pull = "up";
    }else if (pull_down){
        pull = "down";
    }else{
        pull = "off";
    }
    LOG << "| " \
        << setw(3) << index << " | " \
        << setw(8) << exported << " | " \
        << setw(6) << direction << " | " \
        << setw(4) << pull << " | " \
        << setw(5) << value << " |" \
        << endl;
}
