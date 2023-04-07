#include <avr/io.h>
#include <util/delay.h>

// Definición de los pines para los segmentos de la pantalla de 7 segmentos
#define SEG_A   PB0
#define SEG_B   PB1
#define SEG_C   PB2
#define SEG_D   PB3
#define SEG_E   PB4
#define SEG_F   PB5
#define SEG_G   PB6

int main(void)
{
    // Configuración de los registros de incialización

    // Configuración del registro DDRB para establecer los pines como salidas
    DDRB |= (1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G);

    // Configuración del registro PORTB para establecer el valor inicial de los pines en bajo
    PORTB &= ~((1 << SEG_A) | (1 << SEG_B) | (1 << SEG_C) | (1 << SEG_D) | (1 << SEG_E) | (1 << SEG_F) | (1 << SEG_G));

    // Bucle infinito
    while (1)
    {
        // Enciende el segmento A durante 500ms
        PORTB |= (1 << SEG_A);
        _delay_ms(500);

        // Enciende el segmento B durante 500ms
        PORTB &= ~(1 << SEG_A);
        PORTB |= (1 << SEG_B);
        _delay_ms(500);

        // Enciende el segmento C durante 500ms
        PORTB &= ~(1 << SEG_B);
        PORTB |= (1 << SEG_C);
        _delay_ms(500);

        // Enciende el segmento D durante 500ms
        PORTB &= ~(1 << SEG_C);
        PORTB |= (1 << SEG_D);
        _delay_ms(500);

        // Enciende el segmento E durante 500ms
        PORTB &= ~(1 << SEG_D);
        PORTB |= (1 << SEG_E);
        _delay_ms(500);

        // Enciende el segmento F durante 500ms
        PORTB &= ~(1 << SEG_E);
        PORTB |= (1 << SEG_F);
        _delay_ms(500);

        // Enciende el segmento G durante 500ms
        PORTB &= ~(1 << SEG_F);
        PORTB |= (1 << SEG_G);
        _delay_ms(500);

        // Apaga todos los segmentos durante 500ms
        PORTB &= ~(1 << SEG_G);
        _delay_ms(500);
    }

    return 0;
}
