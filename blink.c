/*
 * A simple test program for testing lowlevel access to the MSS310's
 * - LEDs (green LED, red LED)
 * - Button
 * - UART (debug pins)
 *
 * When run on the MSS310, this program:
 * - cycles through all LED colors (red/green/yellow)
 * - prints some text on the serial console
 * - resets the CPU when the button on the device is pressed
 * - resets the CPU when the # key is sent via the serial interface
 * - switches the relay off when the 0 key is pressed
 * - switches the relay on when the 1 key is pressed
 */

#include <stdint.h>

#define RSTCTL          0x10000034      /* Reset control register */

#define GPIO2_MODE      0x10000064      /* GPIO2 purpose selection */

#define GPIO_CTRL_1     0x10000604      /* Direction control register */
#define GPIO_DATA_1     0x10000624      /* Data register              */
#define GPIO_DSET_1     0x10000634      /* Data set register          */
#define GPIO_DCLR_1     0x10000644      /* Data clear register        */

/* UART is expected to be configured from u-boot already */
#define UART0_RBR       0x10000C00      /* RX buffer register   */
#define UART0_THR       0x10000C00      /* TX holding register  */
#define UART0_LSR       0x10000C14      /* Line status register */

#define UART_LSR_DR        (1 << 0)     /* Data ready                */
#define UART_LSR_THRE      (1 << 5)     /* TX holding register empty */

#define SYS_RST            (1 << 0)     /* Whole system reset */

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


static inline void reset_cpu()
{
  write_l(RSTCTL, SYS_RST);
}


static void reset_if_button_pressed()
{
    /* Button is pressed if port is low */
    if (!(read_l(GPIO_DATA_1) & GPIO_BUTTON)) {
      reset_cpu();
    }
}


static void serial_input()
{
    if (read_l(UART0_LSR) & UART_LSR_DR) {
        /* Something in the RX buffer */
        char received = read_l(UART0_RBR) & 0xFF;
        switch (received) {
        case '#':
            reset_cpu();
            break;
        case '0':
            write_l(GPIO_DCLR_1, GPIO_RELAY);
            break;
        case '1':
            write_l(GPIO_DSET_1, GPIO_RELAY);
            break;
        }
    }
}

/* A very dumb delay routine */
static void delay()
{
    volatile int i, j;
    for (i = 0; i < 500; i++) {
        reset_if_button_pressed();
        serial_input();
        for (j = 0; j < 500; j++) {
            /* Do nothing */
        }
    }
}


/* Send the given string to the UART */
static void print_string(const char *s)
{
    const char *p;

    for (p = s; *p; ++p) {
        while ((read_l(UART0_LSR) & UART_LSR_THRE) == 0) {
            /* Wait until there's some space in the FIFO */
        }
        write_l(UART0_THR, *p);
    }
}

#ifdef __cplusplus
extern "C"
#endif
void entrypoint(void)
{
    write_l(GPIO2_MODE, 0x05550550);  /* Value determined from original app */
    /* Configure GPIO pins as output */
    write_l(GPIO_CTRL_1, GPIO_RELAY | GPIO_LED_PIN_RED | GPIO_LED_PIN_GREEN);
    while (1) {
        write_l(GPIO_DCLR_1, GPIO_LED_PIN_RED);
        delay();
        print_string("Hello 1\r\n");
        write_l(GPIO_DCLR_1, GPIO_LED_PIN_GREEN);
        delay();
        print_string("Hello 2\r\n");
        write_l(GPIO_DSET_1, GPIO_LED_PIN_RED);
        delay();
        print_string("Hello 3\r\n");
        write_l(GPIO_DSET_1, GPIO_LED_PIN_GREEN);
        delay();
        print_string("Hello 4\r\n");
    }
}
