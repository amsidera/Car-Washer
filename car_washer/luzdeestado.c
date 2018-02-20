#include <avr/io.h>
#include <avr/interrupt.h>

extern int coches;
int contador=2;
long contador2=21;

/*===============================================================*/
/* 	   Interrupción temporal que gestiona el estado del led		 */
/*===============================================================*/
ISR(TIMER5_OVF_vect){
	
	if(coches==0)		//si no hay ningún coche en el lavadero se enciende el led 0.5seg y se mantiene apagado durante 10seg	
	{
		contador2--;	//Decremento de la cuenta de tiempo
		if(contador2==20){
			
			PORTB &= 0b11111101;//Apagar led
		}
		if(contador2==1){
			
		PORTB |=0b00000010;		//Encender led
		}
		if(contador2 ==0){
			contador2 =21;
		}
	}
	else{//si hay algún coche en el lavadero se enciende y apaga el led cada medio segundo
		contador--;
		
		if(contador==1){
			
			PORTB &= 0b11111101;	//Apagar led
		}
		if(contador==0){
			
			PORTB |=0b00000010;		//Encender led
			contador=2;
		}
	}
}

void luzdeestado(){		//Configuración del led como salida y del timer que gestiona su funcionamiento
	cli();
	DDRB|=0b00000010;
	TCCR5B=0x03;
	TIMSK5 =0x01;
	sei();
	
}