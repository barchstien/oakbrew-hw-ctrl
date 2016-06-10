#include "hw_interface/TDA7468.h"


//use like this : INCREMENTAL_BUS_ACTIVE | SUB_ADDR_XXX
#define INCREMENTAL_BUS_ACTIVE 0b00010000

#define SUB_ADDR_INPUT_SELECT_MIC 0b00000000
#define SUB_ADDR_INPUT_GAIN       0b00000001
#define SUB_ADDR_SURROUND         0b00000010
#define SUB_ADDR_VOLUME_LEFT      0b00000011
#define SUB_ADDR_VOLUME_RIGHT     0b00000100
#define SUB_ADDR_TREBLE_BASS      0b00000101
#define SUB_ADDR_OUTPUT           0b00000110
#define SUB_ADDR_BASS_ALC         0b00000111

//INPUT SELECTION & MIC
#define INPUT_SELECT_1 0b00000000
#define INPUT_SELECT_2 0b00000001
#define INPUT_SELECT_3 0b00000010
#define INPUT_SELECT_4 0b00000011
//? OR with INPUT_SELECT_X ?
#define MUTE_ON        0b00000100
#define MUTE_OFF       0b00000000

//INPUT GAIN
//0 to 14db, 2db steps, 3 lowest bit
//this is just after input select

//VOLUME (I guess 1 and 2 mean left and right ?
//[0db; -64db[ on lowest 6 bit (applied before treble/bass)
//+ n * 8db on highest 2 bit (applied after treble/bass)

//TREBLE & BASS
//[-14db; 14db] by step 2db, 4bit each
//bass 4 hihest bit, treble 4 lowest
//0 = -14db, 7 = 0db, MSb is sign (0 means -, 1 means +)

//OUTPUT
#define OUTPUT_MUTE_ON  0b00000000
#define OUTPUT_MUTE_OFF 0b00000001


