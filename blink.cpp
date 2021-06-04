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
 *
 * Please don't judge the code quality: its all just proof-of-concept code.
 */

#include <cstdint>

#define RSTCTL          0x10000034      /* Reset control register */

#define GPIO2_MODE      0x10000064      /* GPIO2 purpose selection */

#define TGLB_REG        0x10000100      /* Timers global control register */

#define TGBL_T0RST      (1 << 8)        /* Timer 0 reset */
#define TGBL_T1RST      (1 << 10)       /* Timer 1 reset */

#define T0CTL_REG       0x10000110      /* Timer 0 control register */
#define T0LMT_REG       0x10000114      /* Timer 0 limit register */
#define T0_REG          0x10000118      /* Timer 0 counter register */

#define T1CTL_REG       0x10000130      /* Timer 1 control register */
#define T1LMT_REG       0x10000134      /* Timer 1 limit register */
#define T1_REG          0x10000138      /* Timer 1 counter register */

#define TxCTL_Tx_PRES_OFFSET 16         /* Timer 0/1 prescale offset */
#define TxCTL_Tx_PRES_MASK   0xFFFF     /* Timer 0/1 prescale mask */
#define TxCTL_TxEN           (1 << 7)   /* Timer 0/1 enable */
#define TxCTL_TxAL           (1 << 4)   /* Timer 0/1 auto-load enable */
#define TxCTL_TxAL_STATUS    (1 << 3)   /* Timer 0/1 auto load enable status */

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


static inline void write_l(uint32_t addr, unsigned int val)
{
    volatile auto* ptr = reinterpret_cast<uint32_t *>(addr);
    *ptr = val;
}


static inline unsigned int read_l(uint32_t addr)
{
    volatile auto* ptr = reinterpret_cast<uint32_t *>(addr);
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
        auto received = read_l(UART0_RBR) & 0xFF;
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

/*
 * Delay using timer 0 for exact timeout.
 *
 * Still a dumb delay, but this time with exact timeout.
 */
static void timer_delay(unsigned int delay_in_ms)
{
    // start by disabling timer
    write_l(T0CTL_REG, 0);
    write_l(TGLB_REG, 0);

    write_l(T0LMT_REG, delay_in_ms);

    // enable the timer, set prescale to 1ms
    write_l(T0CTL_REG, (1000 << TxCTL_Tx_PRES_OFFSET) | TxCTL_TxEN);

    // Busy wait until timer expires.
    while ((read_l(T0CTL_REG) & TxCTL_TxEN) != 0) {
        // sleep
    }
}


/* A dumb delay routine */
static void delay()
{
    const auto milliseconds = 500;
    const auto granularity = 100;
    static_assert(milliseconds % granularity == 0, "milliseconds should be a multiple of granularity");
    for (auto i = 0; i < (milliseconds / granularity); i++) {
        reset_if_button_pressed();
        serial_input();
        timer_delay(granularity);
    }
}


/* Send the given string to the UART */
static void print_string(const char *s)
{
    for (auto* p = s; *p; ++p) {
        while ((read_l(UART0_LSR) & UART_LSR_THRE) == 0) {
            /* Wait until there's some space in the FIFO */
        }
        write_l(UART0_THR, *p);
    }
}

extern "C"
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
