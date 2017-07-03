
#ifndef GPIO_BCM_H
#define GPIO_BCM_H

#include <iostream>
#include <cstdio>
#include <cstdlib>   // #include <stdlib.h>
#include <cmath>     //#include <math.h>
#include <fcntl.h>
#include <fstream>

#include <unistd.h>     /// need this for getuid and
#include <errno.h>
//#include <string.h>
#include <cstring>
#include <sys/mman.h>
//#include <stdint.h>   // for uint32_t - 32-bit unsigned integer
#include <cstdint>

// GPIO_BASE is 0x20000000 on RPi models other than the RPi 2
#define GPIO_BASE       0x3F200000          // on the RPi 2/3
#define GPFSELn         0x00
#define GPFSEL0         0x0000
#define GPFSEL1         0x0004
#define GPFSEL2         0x0008
#define GPFSEL3         0x000c
#define GPFSEL4         0x0010
#define GPFSEL5         0x0014
#define GPSET0          0x1c
#define GPCLR0          0x28
#define GPLVL0          0x34
#define GPEDS0          0x40
#define GPHEN0          0X64
#define GPLEN0          0x70
#define LEVEL_MASK      0x10000             /// mask for bit 16
#define L_MASK(x)       (1<<x)              /// level mask for GPIO x


typedef enum G_direction {Ginput, Goutput} GPIO_direction;
typedef enum G_function_set
    {
        ginput,
        goutput,
        galt_0,
        galt_1,
        galt_2,
        galt_3,
        galt_4,
        galt_5
    } GPIO_Function_set;

typedef enum G_pin_output{Gset, Gclear} GPIO_pin_set;
typedef enum G_Pin_level {HIGH, LOW} Pin_level;
enum G_event {high, low, rising, falling};




class gpiobcm
{

private:
    int gpio_pin;



public:
    gpiobcm(uint16_t pin_number);
    ~gpiobcm();
    static volatile uint32_t* set_vmem();
    int GPIO_set_FSR(G_function_set Gdir);
    int GPIO_Pin_set_state(uint16_t GNumber, enum G_pin_output Gset);
    int GPIO_Pin_level(uint16_t GNumber, enum G_Pin_level Glevel);
    uint32_t GPIO_read(uint8_t g_reg);
    uint32_t GPIO_write(uint8_t g_reg);
    uint32_t GPIO_toggle();

    static int obj_count;
    static volatile uint32_t* ptr_gpio;
};



#endif // GPIO_BCM_H


/**************************************
extern uint16_t GDFSELn[];
***************************************/

//int GPIO_set_FSR(uint16_t GNumber,G_function_set Gdir);           /// for GPFSELn
//int GPIO_Pin_set_state(uint16_t GNumber, G_pin_output Gset);   /// for GPSETn and GPCLRn
//int GPIO_Pin_level(uint16_t GNumber, G_Pin_level Glevel);       /// for GPLEVn
//uint32_t GPIO_read(uint16_t GNumber, uint8_t g_reg);
//uint32_t GPIO_write(uint16_t GNumber, uint8_t g_reg);
//uint32_t GPIO_toggle(uint16_t GNumber);
