#include <iostream>

#include "gpiobcm.h"

volatile uint32_t *gpio;
static uint32_t led_level;
static uint16_t GPIO_input = 16;
static uint16_t GPIO_output = 26;
volatile uint32_t *wtf;

char buffer[81];

using namespace std;


int main()
{
    cout << "Hello world!" << endl;
    int x;
    wtf=gpiobcm::set_vmem();

    printf("wtf = %x\n", wtf);
    gpiobcm g16input(16);
    gpiobcm g26output(26);



    printf("Start of GPIO memory-manipulation test program.\n");

    if(getuid()!=0)
    {
        printf("You must run this program as root. Exiting.\n");
        return -EPERM;
    }



    printf("gpio after mmap call %x \n", gpio);

    if ((int32_t) gpio < 0)
    {
        printf("Memory mapping failed: %s\n", strerror(errno));
        return -EBUSY;
    }


    g26output.GPIO_set_FSR(goutput);
    g16input.GPIO_set_FSR(ginput);


    cin>>buffer;


    g26output.GPIO_toggle();
    cin>>buffer;


    g26output.GPIO_toggle();
    cin>>buffer;


    g26output.GPIO_toggle();
    cin>>buffer;

    g26output.GPIO_toggle();
    //cin>>buffer;




    do
    {

        uint16_t gpioRTN = g26output.GPIO_write(GPSET0);

        for(x=0;x<50;x++){}  // blocking delay hack using a simple loop

        gpioRTN = g26output.GPIO_write(GPCLR0);


        for(x=0;x<49;x++){}  // delay hack -- balanced for while()

        led_level = *(gpio+(GPLVL0/4));
        printf("switch value = %x\n", (led_level & L_MASK(GPIO_input)));

        uint32_t dododod= g16input.GPIO_read(GPLVL0);
        dododod = g26output.GPIO_read(GPLVL0);
        dododod = g26output.GPIO_read(GPFSELn);
        dododod= g16input.GPIO_read(GPFSELn);
        dododod= g26output.GPIO_read(GPSET0);
        dododod = g26output.GPIO_read(GPCLR0);
        dododod= g16input.GPIO_read(GPSET0);
        dododod= g16input.GPIO_read(GPCLR0);
        dododod= g16input.GPIO_read(GPEDS0);
        dododod = g26output.GPIO_read(GPEDS0);


        led_level = (*(gpio+(GPLVL0/4)) & (L_MASK(GPIO_input)));
        ///printf("while condition check %x \n", led_level);

    }

    while((*(gpio+(GPLVL0/4)) & (1<<16)) !=0); // only true if bit 27 high

    g26output.GPIO_write(GPSET0);

    printf("Button was pressed - end of example program.\n");



    int mmm= munmap((void*)gpio, getpagesize());
    if(mmm != 0)
            printf("munmap failed");


    return 0;
}


