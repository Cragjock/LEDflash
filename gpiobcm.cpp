
#include "gpiobcm.h"


extern volatile uint32_t *gpio;

uint16_t GDFSELn[]={0,1,2,3,4,5};

using namespace std;

int gpiobcm::obj_count=0;
volatile uint32_t* gpiobcm::ptr_gpio= new uint32_t;


///****************************************************************/

gpiobcm::gpiobcm(uint16_t pin_number)
{
    obj_count++;
    this->gpio_pin = pin_number;
    cout<<"obj count: "<<obj_count<<endl;
    printf("gpio  %x from pin %i \n", ptr_gpio, this->gpio_pin);
}

gpiobcm:: ~gpiobcm()
{

    cout<<"gpiobcm destructor"<<endl;
    int mmm= munmap((void*)ptr_gpio, getpagesize());
    if(mmm != 0)
            printf("munmap failed");

}


int gpiobcm::GPIO_set_FSR(G_function_set mode)
{
    uint32_t FS_shift_index = 3*(this->gpio_pin % 10);

    *(gpio + GDFSELn[(this->gpio_pin/10)]) = (*(gpio + this->gpio_pin/10) & ~(7 << FS_shift_index) | (mode<< FS_shift_index));

    return 1;
}


uint32_t gpiobcm::GPIO_toggle()
{
    uint32_t rtn_value = -1;

    rtn_value = (*(gpio+(GPLVL0/4)) & (L_MASK(this->gpio_pin)));

    if(rtn_value == (1<<this->gpio_pin))
        *(gpio + (GPCLR0/4)) = (1<< this->gpio_pin);

    else if(rtn_value == (0<<this->gpio_pin))
        *(gpio + (GPSET0/4)) = (1 << this->gpio_pin);

    printf("at toggle= %i value= %x\n",this->gpio_pin, rtn_value);

    return rtn_value;
}

uint32_t gpiobcm::GPIO_read(uint8_t g_reg)
{
    uint32_t rtn_value = -1;
    switch (g_reg)
    {

        case GPEDS0:
            {
                rtn_value = (*(gpio+(GPEDS0/4)) & (L_MASK(this->gpio_pin)));
                printf("at GPEDS0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                if(rtn_value == L_MASK(this->gpio_pin))
                {
                    printf("event detected\n");
                    *(gpio + (GPEDS0/4)) = (1 << this->gpio_pin);
                }
                break;
            }

        case GPFSELn:
            {
                rtn_value = *(gpio + GDFSELn[(this->gpio_pin/10)]);
                printf("at GPFSELn=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }

        case GPSET0:
            {
                rtn_value = *(gpio + (GPSET0/4));
                printf("at GPSET0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }
        case GPCLR0:
            {
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at GPCLR0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }
        case GPLVL0:
            {
                rtn_value = (*(gpio+(GPLVL0/4)) & (L_MASK(this->gpio_pin)));
                printf("at GPLVL0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                //volatile uint32_t* paddr = bcm2835_gpio + BCM2835_GPLEV0/4 + pin/32;
                break;
            }
        default:
            printf("error on switch!\n");
    }

    return rtn_value;
}


uint32_t gpiobcm::GPIO_write(uint8_t g_reg)
{
    uint32_t rtn_value = -1;
    switch (g_reg)
    {
        case GPSET0:
            {
                *(gpio + (GPSET0/4)) = (1 << this->gpio_pin);
                rtn_value = *(gpio + (GPSET0/4)) & (L_MASK(this->gpio_pin));
                printf("at wr GPSET0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }
        case GPCLR0:
            {
                *(gpio + (GPCLR0/4)) = (1 << this->gpio_pin);
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at wr GPCLR0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }

         case GPLEN0:
            {
                *(gpio + (GPLEN0/4)) = (0 << this->gpio_pin); // set event
                rtn_value = *(gpio + (GPLEN0/4));
                printf("at wr GPLEN0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }

         case GPHEN0:
            {
                *(gpio + (GPHEN0/4)) = (0 << this->gpio_pin); // set event
                rtn_value = *(gpio + (GPHEN0/4));
                printf("at wr GPHEN0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }
          case GPEDS0:
            {
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at GPCLR0=pin %i value= %x\n",this->gpio_pin, rtn_value);
                break;
            }

        default:
            printf("error on switch!\n");
    }

    return rtn_value;
}


volatile uint32_t* gpiobcm::set_vmem()
{

    int fd;

    if(getuid()!=0)
    {
        printf("You must run this program as root. Exiting.\n");
        //return -EPERM;
    }


    fstream my_fstream("/dev/mem");
    if(!my_fstream.is_open())
    {
        cout<<"my_fstream failed to open"<<endl;
    }
    cout<<"my_fstream.is_open()"<<my_fstream.is_open()<<endl;
    my_fstream.close();

    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
    {
        printf("Unable to open /dev/mem: %s\n", strerror(errno));
        //return -EBUSY;
    }


   // get a pointer that points to the peripheral base for the GPIOs
    gpio = (uint32_t *) mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED, fd, GPIO_BASE);
    ptr_gpio= gpio;


    //cout<<"GPIO wtf vmem set: "<<hex<<wtf<<endl;
    //printf("gpio wtf %x \n", wtf);

    printf("gpio after v_mmap call %x \n", gpio);
    printf("ptr_gpio after v_mmap call %x \n", ptr_gpio);

    if ((int32_t) gpio < 0)
    {
        printf("Memory mapping failed: %s\n", strerror(errno));
        //return -EBUSY;
    }

    return gpio;

}

/*****************************************
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);

uint32_t GPIO_toggle(uint16_t GNumber)
{
    uint32_t rtn_value = -1;

    rtn_value = (*(gpio+(GPLVL0/4)) & (L_MASK(GNumber)));

    if(rtn_value == (1<<GNumber))
        *(gpio + (GPCLR0/4)) = (1<< GNumber);

    else if(rtn_value == (0<<GNumber))
        *(gpio + (GPSET0/4)) = (1 << GNumber);

    printf("at toggle= %i value= %x\n",GNumber, rtn_value);

    return rtn_value;
}

int GPIO_set_FSR(uint16_t GNumber, enum G_function_set mode)
{
    uint32_t FS_shift_index = 3*(GNumber % 10);

    *(gpio + GDFSELn[(GNumber/10)]) = (*(gpio + GNumber/10) & ~(7 << FS_shift_index) | (mode<< FS_shift_index));

    return 1;
}



uint32_t GPIO_read(uint16_t GNumber, uint8_t g_reg)
{
    uint32_t rtn_value = -1;
    switch (g_reg)
    {

        case GPEDS0:
            {
                rtn_value = (*(gpio+(GPEDS0/4)) & (L_MASK(GNumber)));
                printf("at GPEDS0=pin %i value= %x\n",GNumber, rtn_value);
                if(rtn_value == L_MASK(GNumber))
                {
                    printf("event detected\n");
                    *(gpio + (GPEDS0/4)) = (1 << GNumber);
                }
                break;
            }

        case GPFSELn:
            {
                rtn_value = *(gpio + GDFSELn[(GNumber/10)]);
                printf("at GPFSELn=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }

        case GPSET0:
            {
                rtn_value = *(gpio + (GPSET0/4));
                printf("at GPSET0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }
        case GPCLR0:
            {
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at GPCLR0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }
        case GPLVL0:
            {
                rtn_value = (*(gpio+(GPLVL0/4)) & (L_MASK(GNumber)));
                printf("at GPLVL0=pin %i value= %x\n",GNumber, rtn_value);
                //volatile uint32_t* paddr = bcm2835_gpio + BCM2835_GPLEV0/4 + pin/32;
                break;
            }
        default:
            printf("error on switch!\n");
    }

    return rtn_value;
}

uint32_t GPIO_write(uint16_t GNumber, uint8_t g_reg)
{
    uint32_t rtn_value = -1;
    switch (g_reg)
    {
        case GPSET0:
            {
                *(gpio + (GPSET0/4)) = (1 << GNumber);
                rtn_value = *(gpio + (GPSET0/4)) & (L_MASK(GNumber));
                printf("at wr GPSET0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }
        case GPCLR0:
            {
                *(gpio + (GPCLR0/4)) = (1 << GNumber);
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at wr GPCLR0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }

         case GPLEN0:
            {
                *(gpio + (GPLEN0/4)) = (0 << GNumber); // set event
                rtn_value = *(gpio + (GPLEN0/4));
                printf("at wr GPLEN0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }

         case GPHEN0:
            {
                *(gpio + (GPHEN0/4)) = (0 << GNumber); // set event
                rtn_value = *(gpio + (GPHEN0/4));
                printf("at wr GPHEN0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }
          case GPEDS0:
            {
                rtn_value = *(gpio + (GPCLR0/4));
                printf("at GPCLR0=pin %i value= %x\n",GNumber, rtn_value);
                break;
            }

        default:
            printf("error on switch!\n");
    }

    return rtn_value;
}

****************************************************/


