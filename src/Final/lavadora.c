#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t seconds = 0;
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
volatile uint8_t flag = 0;


ISR(INT0_vect){
    // Button 1 pressed
    if(!counting_enabled){
        counting_enabled = 1;
        button0=1;
    }
    else{
        counting_enabled = 0; //If the button is pressed, press again to pause the count
    }
}

ISR(INT1_vect){
    // Button 2 pressed
    if(!counting_enabled){
        counting_enabled = 1;
        button1=1;
    }
    else{
        counting_enabled = 0; //If the button is pressed, press again to pause the count
    }
}

ISR(PCINT2_vect){
    if(!counting_enabled){
        counting_enabled = 1;
        flag_prueba=1;
        button2=1;
    }
    /*else if(counting_enabled && flag_prueba){
        counting_enabled = 0;
        flag_prueba = 0;
    }*/
}



ISR(TIMER0_COMPA_vect){
    seconds++; //Increase seconds variable each time the interrupt is triggered 
} 

int main() {
    // Set up ports
    DDRB = 0xFF; // Set PORTB as output
    DDRD = 0x00; // Set PORTD as intput
    //PORTD = 0x03; // Enable pull-up resistors on PD0 and PD1
    PORTD |= (1 << PD2) | (1 << PD3) | (1 << PD6); // Enable pull-up resistor for PD2 (INT0), PD3 (INT1), and PD6(INT17)
    DDRA = 0XFF; //Set PORTA as output
    DDRD |= (1 << DDD0) | (1 << DDD1); //Set D0 and D1 as outputs


    // Set up interrupts
    GIMSK |= (1 << PCIE2) | (1 << INT0) | (1 << INT1); // Enable INT0, INT1, and INT17
    MCUCR |= (1 << ISC01) | (1 << ISC11); // Trigger on falling edge
    PCMSK2 |= (1 << PCINT17); //Set PCINT17

    // Set ATtiny4313 internal osciller at 1MHz
    CLKPR = 0x80;  // Set CLKPR register in 0x80
    CLKPR = 0x01;  // Set prescaler to 2 (Divide the frequency by 2)

    //Set Timer0
    TCCR0A = 0;    // Set Timer0 in normal mode 
    TCCR0B |= (1 << CS02) | (1 << CS00); // Set prescaler at 1024
    OCR0A = 78; //Set OCR0A register value for an interrupt each 1 second
    PORTB &= ~(1 << PB7);


    //Enables the Timer0 interrupt by comparison
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
                break;
                case 10:
                PORTB = 0b00111111;
                break;
            }

            //Asks if is more than a second to repeat the iteration
            if(seconds>=78){

                // Boton nivel de carga alto
                if(button0){ // When press button 0
                    PORTB &= ~(1 << PB7);
                    if(count0<3 && state_bit0 == 0 && state_bit1 == 0){ 
                        count0++; // Increment count 0 by 1 while is within the range
                        PORTA |= (1 << PA0); // Turns ON LED0
                        state_bit0 = 0; //'Suministro' state flags
                        state_bit1 = 0;
                        
                    }
                    else{
                        state_bit0 = 1; //'Lavado' state flags
                        state_bit1 = 0;
                        count0 = 0; 
                        PORTA &= ~(1 << PA0); //Turns OFF LED0
                        PORTB &= ~(1 << PB7); //Keeps second display in 0
                        

                        if(count1 < 11 && state_bit0 == 1 && state_bit1 == 0 && count2 == 0 && count3 == 0){ // Starts the count for the second state
                            count1++;
                            PORTA |= (1 << PA1); //Turns ON LED1 
                            state_bit0 = 1; //'Lavado' state flags
                            state_bit1 = 0;
                            while(count1==10){
                                PORTB = 0b00111111; //Displays 0 in first display
                                PORTB |= (1 << PB7); // When the count reaches ten, displays number 1 in second display
                                count1++; // Increase the count to get out of the while
                            }
                        }
                        else{
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            state_bit0 = 0;//'Enjuagado' state flags
                            state_bit1 = 1;
                            count1 = 0;
                            PORTB &= (1 << PB7);
                            if(count2 < 5 && state_bit0 == 0 && state_bit1 == 1 && flag_finish == 0 && count3 == 0){
                                count2++;
                                PORTD |= (1 << PD1); //Turns ON LED2
                                state_bit1 = 1;// 'Enjuagado' state flags
                                state_bit0 = 0;
                                count1 = 0;
                                PORTB &= ~(1 << PB7);
                            }
                            else{
                                count2 = 0;
                                PORTA &= ~(1 << PA1); //Turns OFF LED1
                                state_bit0 = 1;// 'Centrifugado' state flags
                                state_bit1 = 1;
                                flag_finish = 1;
                                PORTD &= ~(1 << PD1); //Turns OFF LED2
                                PORTB &= ~(1 << PB7);
                                
                                if(count3 < 9 && state_bit0 == 1 && state_bit1 == 1 && flag_finish == 1){
                                    count3++;
                                    PORTD |= (1 << PD0); //Turns ON LED3
                                    state_bit0 = 1;// 'Centrifugado' state flags
                                    state_bit1 = 1;
                                    count2 = 0;
                                    
                                    }
                                else{
                                    count3=0;
                                    button0 = 0;
                                    PORTB=0b00111111;
                                    PORTD &= ~(1 << PD0); //Turns OFF LED3
                                    counting_enabled = 0;
                                    state_bit0=0;//Restart states machine
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
                        state_bit0 = 0; //'Suministro' state flags
                        state_bit1 = 0;
                    }
                    else{
                        state_bit0 = 1; //'Lavado' state flags
                        state_bit1 = 0;
                        count0 = 0; 
                        PORTA &= ~(1 << PA0); //Turns OFF LED0 to indicates first state is done
                        

                        if(count1 < 7 && state_bit0 == 1 && state_bit1 == 0 && count2 == 0 && count3 == 0){ // Starts the count for the second state
                            count1++;
                            PORTA |= (1 << PA1); //Turns ON LED1 while is within the second range
                            state_bit0 = 1; //'Lavado' state flags
                            state_bit1 = 0;
                        }
                        else{
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            state_bit0 = 0;//'Enjuagado' state flags
                            state_bit1 = 1;
                            //count1 = 0;
                            if(count2 < 4 && state_bit0 == 0 && state_bit1 == 1 && flag_finish == 0 && count3 == 0){
                                count2++;
                                PORTD |= (1 << PD1); //Turns ON LED2
                                state_bit1 = 1;//'Enjuagado' state flags
                                state_bit0 = 0;
                                count1 = 0;
                            }
                            else{
                                //count2 = 0;
                                PORTA &= ~(1 << PA1); //Turns OFF LED1
                                state_bit0 = 1;// 'Centrifugado' state flags
                                state_bit1 = 1;
                                flag_finish = 1;
                                PORTD &= ~(1 << PD1); //Turns OFF LED2
                                
                                if(count3 < 6 && state_bit0 == 1 && state_bit1 == 1 && flag_finish == 1){
                                    count3++;
                                    PORTD |= (1 << PD0); //Turns ON LED3
                                    state_bit0 = 1;// 'Centrifugado' state flags
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
                                    state_bit0=0;//Restart states machine
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
            seconds =0;  
            }
            
        }
    }
    return 0;
}