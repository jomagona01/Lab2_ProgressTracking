#include <avr/io.h>

int main(void)
{
    // Configuración de los registros de inicialización

    // Configuración del registro DDRB para establecer los pines PB0 y PB1 como salidas
    DDRB |= (1 << PB0) | (1 << PB1);

    // Configuración del registro PORTB para establecer el valor inicial de los pines PB0 y PB1 en bajo
    PORTB &= ~(1 << PB0);
    PORTB &= ~(1 << PB1);

    // Configuración del registro TIMSK para habilitar la interrupción por desbordamiento del temporizador 0
    TIMSK |= (1 << TOIE0);

    // Configuración del registro TCCR0A para establecer el modo de operación del temporizador 0 en normal (sin PWM)
    TCCR0A = 0;

    // Configuración del registro TCCR0B para establecer la prescaler del temporizador 0 en 1024
    TCCR0B |= (1 << CS02) | (1 << CS00);

    // Habilitación global de interrupciones
    sei();

    // Bucle infinito
    while (1)
    {
        // Enciende el LED conectado al pin PB0 durante 500ms
        PORTB |= (1 << PB0);
        _delay_ms(500);

        // Apaga el LED conectado al pin PB0 y enciende el LED conectado al pin PB1 durante 500ms
        PORTB &= ~(1 << PB0);
        PORTB |= (1 << PB1);
        _delay_ms(500);

        // Apaga el LED conectado al pin PB1 durante 500ms
        PORTB &= ~(1 << PB1);
        _delay_ms(500);
    }

    return 0;
}

// Rutina de interrupción por desbordamiento del temporizador 0
ISR(TIMER0_OVF_vect)
{
    // Realiza alguna acción en la interrupción, si es necesario
}