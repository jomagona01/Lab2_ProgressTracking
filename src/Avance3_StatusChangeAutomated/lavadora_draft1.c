#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t count0 = 0;
volatile uint8_t count01 = 0;
volatile uint8_t count02 = 0;
volatile uint8_t count03 = 0;
volatile uint8_t count1 = 0;
volatile uint8_t button0 = 0;
volatile uint8_t button1 = 0;
volatile uint8_t counting_enabled0 = 0;
volatile uint8_t counting_enabled01 = 0;
volatile uint8_t flag0 = 0;
volatile uint8_t flag1 = 0;
volatile uint8_t flag_finish = 0;


ISR(INT0_vect){
    // Button 1 pressed
    if(!counting_enabled0){
        counting_enabled0 = 1;
        _delay_ms(50);
        count0=0;
        count01=0;
        count02=0;
        button0=1;
    }
}

ISR(INT1_vect){
    // Button 2 pressed
    if(!counting_enabled0){
        counting_enabled0 = 1;
        _delay_ms(50);
        count1=0;
        button1=1;
    }
}

int main() {
    // Set up ports
    DDRB = 0xFF; // Set PORTB as output
    DDRD = 0x00; // Set PORTD as intput
    //PORTD = 0x03; // Enable pull-up resistors on PD0 and PD1
    PORTD |= (1 << PD2) | (1 << PD3); // Enable pull-up resistor for PD2 (INT0) and PD3 (INT1)
    DDRA = 0XFF;
    DDRD |= (1 << DDD0) | (1 << DDD1); //Set D0 and D1 as outputs


    // Set up interrupts
    GIMSK |= (1 << INT0) | (1 << INT1); // Enable INT0 and INT1
    MCUCR |= (1 << ISC01) | (1 << ISC11); // Trigger on falling edge

    // Enable global interrupts
    sei();

    while (1) {
        // Display the count value on the 7-segment display
        if(counting_enabled0){
            switch(count0 | count01 | count02 | count03)
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
            if(button0){ // When press button 0
                if(count0<4 && flag0 == 0 && flag1 == 0){ 
                    count0++; // Increment count 0 by 1 while is within the range
                    PORTA |= (1 << PA0); // Turns ON LED0
                    flag0 = 0; //When is within the first range flag is in 0
                    flag1 = 0;
                }
                else{
                    flag0 = 1; // If it reaches the boundary of first range sets the flag
                    flag1 = 0;
                    count0 = 0; 
                    PORTA &= ~(1 << PA0); //Turns OFF LED0 to indicates first state is done
                    

                    if(count01 < 10 && flag0 == 1 && flag1 == 0 && count02 == 0 && count03 == 0){ // Starts the count for the second state
                        count01++;
                        PORTA |= (1 << PA1); //Turns ON LED1 while is within the second range
                        flag0 = 1;
                        flag1 = 0;
                    }
                    else{
                        PORTA &= ~(1 << PA1); //Turns OFF LED1
                        //
                        flag0 = 0;
                        flag1 = 1;
                        //count01 = 0;
                        //PORTB=0b00111111; // If the count recahes the boundary set 0 in the display
                        if(count02 <= 6 && flag0 == 0 && flag1 == 1 && flag_finish == 0 && count03 == 0){
                            count02++;
                            PORTD |= (1 << PD1); //Turns ON LED2
                            flag1 = 1;
                            flag0 = 0;
                            count01 = 0;
                            //PORTA &= ~(1 << PA1); //Turns OFF LED1
                        }
                        else{
                            //count02 = 0;
                            PORTA &= ~(1 << PA1); //Turns OFF LED1
                            flag0 = 1;
                            flag1 = 1;
                            flag_finish = 1;
                            PORTD &= ~(1 << PD1); //Turns OFF LED2
                            
                            if(count03 <= 9 && flag0 == 1 && flag1 == 1 && flag_finish == 1){
                                count03++;
                                PORTD |= (1 << PD0); //Turns ON LED3
                                flag0 = 1;
                                flag1 = 1;
                                count02 = 0;
                                
                                }
                            else{
                                PORTB=0b00111111;
                                PORTD &= ~(1 << PD0); //Turns OFF LED3
                                counting_enabled0 = 0;
                            }
                        }    
                    }
                      
                }
                
            }
            
            else if(button1){
                if(count1 < 7){
                    count1++; // Increment count 1 by 1 while is within the range
                    PORTA |= (1 << PA1); //Turns ON LED1
                    PORTD |= (1 << PD1); //Turns ON LED2
                }
                else{
                    PORTB=0b00111111;
                    count1 = 0;
                    
                    PORTA &= ~(1 << PA1); //Turns OFF LED1
                    PORTD &= ~(1 << PD1); //Turns OFF LED2
                    counting_enabled0 = 0; //When pass the threshold disenable the count, to be able to start again
                }
            }
            else{
                PORTB=0b00111111;
                counting_enabled0 = 0;
                //PORTA &= ~(1 << PA1);
                //PORTA &= ~(1 << PA0);
            }        
            _delay_ms(1000); //Delays 1 second
        }
    }
}

