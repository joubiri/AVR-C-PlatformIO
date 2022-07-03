#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define F_CPU 16000000
#define BAUD 9600
#define UBRR ((F_CPU/16/BAUD)-1)
#define BUFFER_SIZE 128

char SerialBuffer[BUFFER_SIZE];
uint8_t timer=0;
uint8_t readPos=0;
uint8_t writePos=0;

void appendSerial(char c){
  SerialBuffer[writePos]=c;
  writePos++;
  if (writePos >= BUFFER_SIZE){
    writePos=0;
  }
}

void USART_WRITE(char data[]){
  for (uint8_t i=0; i<strlen(data); i++){
    appendSerial(data[i]);
    //UDR0= data; 
    }
  if(UCSR0A & (1<<UDRE0)){
    UDR0=0;
  }
}

void USART_INIT(){
  UBRR0H= UBRR >>8;
  UBRR0L= UBRR;
  UCSR0B= (1 << TXEN0) | (1 << TXCIE0);
  UCSR0C= (1 << UCSZ00) | (1 << UCSZ01);
}

void CLK_INIT(){
  TCCR0A= (1 << WGM01); //clear timer on compare (CTC mode)
  OCR0A= 195;
  TIMSK0= (1 << OCIE0A);
}

int main(void){
  DDRB=0x20;
  USART_INIT();
  CLK_INIT();
  sei();
  TCCR0B= (1 << CS00) | (1 << CS02); //1024 prescalar
  while(1){
  if(timer>100){
    timer=0;
    USART_WRITE("Hello");
    // PORTB ^= (1 << PORTB5);
    
  }
  // USART_WRITE("Test");
  // _delay_ms(500);

  }
}

ISR(TIMER0_COMPA_vect){
  timer++;
}

ISR(USART_TX_vect){
  if(readPos != writePos){
    UDR0= SerialBuffer[readPos];
    readPos++;
    if(readPos >= BUFFER_SIZE){
      readPos=0;
    }
  }
}
