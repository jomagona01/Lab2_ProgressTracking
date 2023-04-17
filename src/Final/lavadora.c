#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t segundos = 0;
volatile uint8_t count0 = 0;
volatile uint8_t count1 = 0;
volatile uint8_t count2 = 0;
volatile uint8_t count3 = 0;
volatile uint8_t button0 = 0;
volatile uint8_t button1 = 0;
volatile uint8_t button2 = 0;
volatile uint8_t counting_enabled = 0;
volatile uint8_t state_bit0 = 0;
volatile uint8_t state_bit1 = 0;
volatile uint8_t flag_finish = 0;
volatile uint8_t flag_prueba = 0;


ISR(INT0_vect){
    // Button 1 pressed
    if(!counting_enabled){
        counting_enabled = 1;
        //_delay_ms(50);
        count0=0;
        count1=0;
        count2=0;
        button0=1;
    }
}

ISR(INT1_vect){
    // Button 2 pressed
    if(!counting_enabled){
        counting_enabled = 1;
        //_delay_ms(50);
        count1=0;
        button1=1;
    }
}

ISR(PCINT2_vect){
    if(!counting_enabled){
        counting_enabled = 1;
        //_delay_ms(50);
        count0=0;
        count1=0;
        count2=0;
        button2=1;
        flag_prueba=1;
    }
}

ISR(TIMER0_COMPA_vect){
    segundos++; // incrementa la variable de segundos cada vez que se genera la interrupción cada 1 segundo
    
}

int main() {
    // Set up ports
    DDRB = 0xFF; // Set PORTB as output
    DDRD = 0x00; // Set PORTD as intput
    //PORTD = 0x03; // Enable pull-up resistors on PD0 and PD1
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD6); // Enable pull-up resistor for PD2 (INT0), PD3 (INT1), and PD6(INT17)
    DDRA = 0XFF;
    DDRD |= (1 << DDD0) | (1 << DDD1); //Set D0 and D1 as outputs


    // Set up interrupts
    GIMSK |= (1 << PCIE2) | (1 << INT0) | (1 << INT1); // Enable INT0, INT1, and INT17
    MCUCR |= (1 << ISC01) | (1 << ISC11); // Trigger on falling edge
    PCMSK2 |= (1 << PCINT17);

    // Configura el oscilador interno del tiny4313 a 1MHz
    CLKPR = 0x80;  // Configura el registro CLKPR en 0x80
    CLKPR = 0x01;  // Establece el preescalador a 2 (divide la frecuencia por 2)

    // Configura el Timer0
    TCCR0A = 0;    // Establece el Timer0 en modo normal
    TCCR0B |= (1 << CS02) | (1 << CS00); // Establece el prescaler en 1024
    OCR0A = 78; // Establece el valor del registro OCR0A para una interrupción cada 1 segundo



    // Habilita la interrupción por comparación del Timer0
    TIMSK |= (1 << OCIE0A);


    // Enable global interrupts
    sei();

    while (1) {

        // Display the count value on the 7-segment display
        if(counting_enabled){
            switch(count0 | count1 | count2 | count3)
            {   
                case 0:
                PORTB = 0b00111111;
                break;
                case 1:
                PORTB = 0b00000110;
                break;
                case 2:
                PORTB = 0b01011011;
                break;
                case 3:
                PORTB = 0b01001111;
                break;
                case 4:
                PORTB = 0b01100110;
                break;
                case 5:
                PORTB = 0b01101101;
                break;
                case 6:
                PORTB = 0b01111101;
                break;
                case 7:
                PORTB = 0b00000111;
                break;
                case 8:
                PORTB = 0b01111111;
                break;
                case 9:
                PORTB = 0b01101111;
                //counting_enabled = 0; // Disable counting when count reaches 9
                break;
            }

            //Asks if is more than a second
            if(segundos>=78){

                // Boton nivel de carga alto
                if(button0){ // When press button 0
                    if(count0<3 && state_bit0 == 0 && state_bit1 == 0){ 
                        count0++; // Increment count 0 by 1 while is within the range
                        PORTA |= (1 << PA0); // Turns ON LED0
                        state_bit0 = 0; //When is within the first range flag is in 0
                        state_bit1 = 0;
                    }
                    else{
                        state_bit0 = 1; // If it reaches the boundary of first range sets the flag
                        state_bit1 = 0;
                        count0 = 0; 
                        PORTA &= ~(1 << PA0); //Turns OFF LED0 to indicates first state is done
                        

                        if(count1 < 10 && state_bit0 == 1 && state_bit1 == 0 && count2 == 0 && count3 == 0){ // Starts the count for the second state
                            count1++;
                            PORTA |= (1 << PA1); //Turns ON LED1 while is within the second range
                            state_bit0 = 1;
                            state_bit1 = 0;
                        }
                        else{
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            state_bit0 = 0;
                            state_bit1 = 1;
                            count1 = 0;
                            if(count2 < 5 && state_bit0 == 0 && state_bit1 == 1 && flag_finish == 0 && count3 == 0){
                                count2++;
                                PORTD |= (1 << PD1); //Turns ON LED2
                                state_bit1 = 1;
                                state_bit0 = 0;
                                count1 = 0;
                            }
                            else{
                                count2 = 0;
                                PORTA &= ~(1 << PA1); //Turns OFF LED1
                                state_bit0 = 1;
                                state_bit1 = 1;
                                flag_finish = 1;
                                PORTD &= ~(1 << PD1); //Turns OFF LED2
                                
                                if(count3 < 9 && state_bit0 == 1 && state_bit1 == 1 && flag_finish == 1){
                                    count3++;
                                    PORTD |= (1 << PD0); //Turns ON LED3
                                    state_bit0 = 1;
                                    state_bit1 = 1;
                                    count2 = 0;
                                    
                                    }
                                else{
                                    count3=0;
                                    button0 = 0;
                                    PORTB=0b00111111;
                                    PORTD &= ~(1 << PD0); //Turns OFF LED3
                                    counting_enabled = 0;
                                    state_bit0=0;
                                    state_bit1=0;
                                    flag_finish=0;
                                }
                            }    
                        }
                    
                    }
                }

                // Boton nivel de carga alto
                else if(button1){
                    if(count0<2 && state_bit0 == 0 && state_bit1 == 0){ 
                        count0++; // Increment count 0 by 1 while is within the range
                        PORTA |= (1 << PA0); // Turns ON LED0
                        state_bit0 = 0; //When is within the first range flag is in 0
                        state_bit1 = 0;
                    }
                    else{
                        state_bit0 = 1; // If it reaches the boundary of first range sets the flag
                        state_bit1 = 0;
                        count0 = 0; 
                        PORTA &= ~(1 << PA0); //Turns OFF LED0 to indicates first state is done
                        

                        if(count1 < 7 && state_bit0 == 1 && state_bit1 == 0 && count2 == 0 && count3 == 0){ // Starts the count for the second state
                            count1++;
                            PORTA |= (1 << PA1); //Turns ON LED1 while is within the second range
                            state_bit0 = 1;
                            state_bit1 = 0;
                        }
                        else{
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            state_bit0 = 0;
                            state_bit1 = 1;
                            //count1 = 0;
                            if(count2 < 4 && state_bit0 == 0 && state_bit1 == 1 && flag_finish == 0 && count3 == 0){
                                count2++;
                                PORTD |= (1 << PD1); //Turns ON LED2
                                state_bit1 = 1;
                                state_bit0 = 0;
                                count1 = 0;
                            }
                            else{
                                //count2 = 0;
                                PORTA &= ~(1 << PA1); //Turns OFF LED1
                                state_bit0 = 1;
                                state_bit1 = 1;
                                flag_finish = 1;
                                PORTD &= ~(1 << PD1); //Turns OFF LED2
                                
                                if(count3 < 6 && state_bit0 == 1 && state_bit1 == 1 && flag_finish == 1){
                                    count3++;
                                    PORTD |= (1 << PD0); //Turns ON LED3
                                    state_bit0 = 1;
                                    state_bit1 = 1;
                                    count2 = 0;
                                    
                                    }
                                else{
                                    button1 = 0;
                                    count0=0;
                                    count1=0;
                                    count2=0;
                                    count3=0;
                                    PORTB=0b00111111;
                                    PORTD &= ~(1 << PD0); //Turns OFF LED3
                                    counting_enabled = 0;
                                    state_bit0=0;
                                    state_bit1=0;
                                    flag_finish=0;
                                }
                            }    
                        }
                    
                    }
                }
                else if(button2 && flag_prueba == 1){ // When press button 0
                    if(count0<1 && state_bit0 == 0 && state_bit1 == 0){ 
                        count0++; // Increment count 0 by 1 while is within the range
                        PORTA |= (1 << PA0); // Turns ON LED0
                        state_bit0 = 0; //When is within the first range flag is in 0
                        state_bit1 = 0;
                        PORTB |= (1 << PB7);
                    }
                    else{
                        state_bit0 = 1; // If it reaches the boundary of first range sets the flag
                        state_bit1 = 0;
                        count0 = 0; 
                        PORTA &= ~(1 << PA0); //Turns OFF LED0 to indicates first state is done
                        

                        if(count1 < 3 && state_bit0 == 1 && state_bit1 == 0 && count2 == 0 && count3 == 0){ // Starts the count for the second state
                            count1++;
                            PORTA |= (1 << PA1); //Turns ON LED1 while is within the second range
                            state_bit0 = 1;
                            state_bit1 = 0;
                            PORTB |= (1 << PB7);
                        }
                        else{
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            state_bit0 = 0;
                            state_bit1 = 1;
                            count1 = 0;
                            if(count2 < 2 && state_bit0 == 0 && state_bit1 == 1 && flag_finish == 0 && count3 == 0){
                                count2++;
                                PORTD |= (1 << PD1); //Turns ON LED2
                                state_bit1 = 1;
                                state_bit0 = 0;
                                count1 = 0;
                                PORTB |= (1 << PB7);
                            }
                            else{
                                count2 = 0;
                                PORTA &= ~(1 << PA1); //Turns OFF LED1
                                state_bit0 = 1;
                                state_bit1 = 1;
                                flag_finish = 1;
                                PORTD &= ~(1 << PD1); //Turns OFF LED2
                                
                                if(count3 < 3 && state_bit0 == 1 && state_bit1 == 1 && flag_finish == 1){
                                    count3++;
                                    PORTD |= (1 << PD0); //Turns ON LED3
                                    state_bit0 = 1;
                                    state_bit1 = 1;
                                    count2 = 0;
                                    }
                                else{
                                    PORTB=0b00111111;
                                    PORTD &= ~(1 << PD0); //Turns OFF LED3
                                    count0=0;
                                    count1=0;
                                    count2=0;
                                    count3=0;
                                    button2 = 0;
                                    counting_enabled = 0;
                                    state_bit0=0;
                                    state_bit1=0;
                                    flag_finish=0;
                                }
                            }    
                        }
                        
                    }
                
                }
                else{
                    PORTB=0b00111111;
                    counting_enabled = 0;
                } 

            //Restarts time counter
            segundos =0;  
            }
            
        }
    }
    return 0;
}