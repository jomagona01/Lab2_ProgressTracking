#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BUTTON_LOW PB0
#define BUTTON_MED PB1
#define BUTTON_HIGH PB2
#define LED_LOW PB3
#define LED_MED PB4
#define LED_HIGH PB5

// Variables globales para el control del temporizador
volatile uint8_t timer_count = 0;
volatile uint8_t timer_target = 0;
volatile uint8_t timer_running = 0;

// Constantes para los tiempos de espera
const uint8_t WATER_TIME[] = {1, 2, 3}; // Tiempo de suministro de agua en segundos
const uint8_t WASH_TIME[] = {3, 7, 10}; // Tiempo de lavado en segundos
const uint8_t RINSE_TIME[] = {2, 4, 5}; // Tiempo de enjuague en segundos
const uint8_t SPIN_TIME[] = {3, 6, 9}; // Tiempo de centrifugado en segundos

// Función para inicializar el temporizador
void timer_init(void) {
  TCCR0B |= (1 << CS02) | (1 << CS00); // Configura el preescalador del temporizador a 1024
  TIMSK0 |= (1 << OCIE0A); // Habilita la interrupción de comparación A
  OCR0A = 156; // Configura el registro de comparación para generar una interrupción cada 10ms
}

// Función para iniciar el temporizador con un tiempo objetivo en segundos
void timer_start(uint8_t time) {
  timer_running = 1;
  timer_count = 0;
  timer_target = time * 100; // Convierte el tiempo en segundos a unidades de 10ms
  TCNT0 = 0; // Reinicia el contador del temporizador
}

// Función para detener el temporizador
void timer_stop(void) {
  timer_running = 0;
}

// Función de interrupción de comparación A del temporizador
ISR(TIMER0_COMPA_vect) {
  if (timer_running) {
    timer_count++;
    if (timer_count >= timer_target) {
      timer_stop();
    }
  }
}

int main(void) {
  // Configuración de pines de entrada/salida
  DDRB &= ~(1 << BUTTON_LOW) & ~(1 << BUTTON_MED) & ~(1 << BUTTON_HIGH); // Configura botones como entrada
  DDRB |= (1 << LED_LOW) | (1 << LED_MED) | (1 << LED_HIGH); // Configura LEDs como salida
  
  // Inicialización del temporizador
  timer_init();
  
  while (1) {
    // Comprueba el estado de los botones
    if (bit_is_clear(PINB, BUTTON_LOW)) { // Si se presiona el botón de carga baja
      PORTB |= (1 << LED_LOW); // Enciende el LED correspondiente
      timer_start(WATER_TIME[0]); // Inicia el temporizador para el suministro de agua
      _delay_ms(500); // Espera 500ms para evitar vibraciones
    } else {
      PORTB &= ~(1 << LED_LOW); // Apaga el LED correspondiente
    }
    
    if (bit_is_clear(PINB, BUTTON_MED)) { // Si se presiona el
