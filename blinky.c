//*****************************************************************************
// blinky.c - Simple example to blink the on-board LED.
// EK-TM4C123GXL | PF1=RED | PF2=BLUE | PF3=GREEN
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
// Objeto LED
//*****************************************************************************

typedef struct led_t led_t;

struct led_t {
    uint32_t port;
    uint8_t  pin;

    void (*init)  (led_t *self);
    void (*set)   (led_t *self);
    void (*reset) (led_t *self);
    void (*toggle)(led_t *self);
};

#define LED_CALL(led, method) (led).method(&(led))

//*****************************************************************************
// Implementação dos métodos
//*****************************************************************************

static void led_init(led_t *self)
{
    GPIOPinTypeGPIOOutput(self->port, self->pin);
}

static void led_set(led_t *self)
{
    GPIOPinWrite(self->port, self->pin, self->pin);
}

static void led_reset(led_t *self)
{
    GPIOPinWrite(self->port, self->pin, 0x00);
}

static void led_toggle(led_t *self)
{
    uint8_t state = GPIOPinRead(self->port, self->pin);
    GPIOPinWrite(self->port, self->pin, state ? 0x00 : self->pin);
}

//*****************************************************************************
// Construtor
//*****************************************************************************

static led_t led_create(uint32_t port, uint8_t pin)
{
    return (led_t){
        .port   = port,
        .pin    = pin,
        .init   = led_init,
        .set    = led_set,
        .reset  = led_reset,
        .toggle = led_toggle,
    };
}

//*****************************************************************************
// Funções do sistema
//*****************************************************************************

static void sys_init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
}

static void delay_ms(uint32_t ms)
{
    SysCtlDelay(ms * (SysCtlClockGet() / 3000));
}

//*****************************************************************************
// Main
//*****************************************************************************

int main(void)
{
    sys_init();

    led_t led_red   = led_create(GPIO_PORTF_BASE, GPIO_PIN_1);
    led_t led_blue  = led_create(GPIO_PORTF_BASE, GPIO_PIN_2);
    led_t led_green = led_create(GPIO_PORTF_BASE, GPIO_PIN_3);

    LED_CALL(led_red,   init);
    LED_CALL(led_blue,  init);
    LED_CALL(led_green, init);

    while (1)
    {
        LED_CALL(led_red,   set);
        LED_CALL(led_green, reset);
        LED_CALL(led_blue,  reset);
        delay_ms(500);

        LED_CALL(led_red,   reset);
        LED_CALL(led_green, set);
        delay_ms(500);

        LED_CALL(led_green, reset);
        LED_CALL(led_blue,  set);
        delay_ms(500);
    }
}
