/*Despcripcion:  Este código configura los pines PB0, PB1 y PB2 como entrada para los botones de 
carga baja, media y alta, respectivamente. Los pines PB3, PB4 y PB5 se configuran como
 salida para los LEDs correspondientes. Dentro del bucle principal, se verifica el
  estado de cada botón y se enciende o apaga el LED correspondiente según sea necesario.
   El temporizador _delay_ms se usa para evitar vibraciones de los botones y asegurarse 
   de que solo se cuenta como un único evento. Puedes ajustar el tiempo de espera según 
   tus necesidades. 

*/



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BUTTON_LOW PB0
#define BUTTON_MED PB1
#define BUTTON_HIGH PB2
#define LED_LOW PB3
#define LED_MED PB4
#define LED_HIGH PB5

int main(void) {
  // Configuración de pines de entrada/salida

  // Configura botones como entrada
  DDRB &= ~(1 << BUTTON_LOW) & ~(1 << BUTTON_MED) & ~(1 << BUTTON_HIGH); 

// Configura LEDs como salida
  DDRB |= (1 << LED_LOW) | (1 << LED_MED) | (1 << LED_HIGH); 
  

  
    // Activar las interrupciones 
    GIMSK |= (1 << INT0) | (1 << INT1); // 
        MCUCR |= (1 << ISC01) | (1 << ISC11); // Trigger on falling edge

    // activar interrupciones globales
    sei();

  while (1) {
    // Comprueba el estado de los botones
    if (bit_is_clear(PINB, BUTTON_LOW)) { // Si se presiona el botón de carga baja
      PORTB |= (1 << LED_LOW); // Enciende el LED correspondiente
      _delay_ms(500); // Espera 500ms para evitar vibraciones
    } else {
      PORTB &= ~(1 << LED_LOW); // Apaga el LED correspondiente
    }
    
    if (bit_is_clear(PINB, BUTTON_MED)) { // Si se presiona el botón de carga media
      PORTB |= (1 << LED_MED); // Enciende el LED correspondiente
      _delay_ms(500); // Espera 500ms para evitar vibraciones
    } else {
      PORTB &= ~(1 << LED_MED); // Apaga el LED correspondiente
    }
    
    if (bit_is_clear(PINB, BUTTON_HIGH)) { // Si se presiona el botón de carga alta
      PORTB |= (1 << LED_HIGH); // Enciende el LED correspondiente
      _delay_ms(500); // Espera 500ms para evitar vibraciones
    } else {
      PORTB &= ~(1 << LED_HIGH); // Apaga el LED correspondiente
    }
  }
}


