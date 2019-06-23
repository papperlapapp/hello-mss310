#include <stdint.h>

#define RSTCTL          0x10000034
#define GPIO2_MODE      0x10000064
#define GPIO_CTRL_1     0x10000604
#define GPIO_DATA_1     0x10000624
#define GPIO_DSET_1     0x10000634
#define GPIO_DCLR_1     0x10000644

#define SYS_RST            (1 << 0)

#define GPIO_RELAY         (1 << 0)
#define GPIO_LED_PIN_RED   (1 << 1)
#define GPIO_LED_PIN_GREEN (1 << 2)
#define GPIO_BUTTON        (1 << 3)

static inline void write_l(unsigned int addr, unsigned int val)
{
    volatile uint32_t *ptr = (uint32_t *)(addr);
    *ptr = val;
}

static inline unsigned int read_l(unsigned int addr)
{
    volatile uint32_t *ptr = (uint32_t *)(addr);
    return *ptr;
}

static void reset_if_button_pressed()
{
    if (!(read_l(GPIO_DATA_1) & GPIO_BUTTON)) {
        write_l(RSTCTL, SYS_RST);
    }
}

static void delay()
{
    volatile int i, j;
    for (i = 0; i < 500; i++) {
        reset_if_button_pressed();
        for (j = 0; j < 500; j++) {
            /* Do nothing */
        }
    }
}

#ifdef __cplusplus
extern "C"
#endif
void entrypoint(void)
{
    write_l(GPIO2_MODE, 0x05550550);
    write_l(GPIO_CTRL_1, GPIO_RELAY | GPIO_LED_PIN_RED | GPIO_LED_PIN_GREEN);
    while (1) {
        write_l(GPIO_DCLR_1, GPIO_LED_PIN_RED);
        delay();
        write_l(GPIO_DCLR_1, GPIO_LED_PIN_GREEN);
        delay();
        write_l(GPIO_DSET_1, GPIO_LED_PIN_RED);
        delay();
        write_l(GPIO_DSET_1, GPIO_LED_PIN_GREEN);
        delay();
    }
}
