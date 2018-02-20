#include <avr/interrupt.h>
#include <avr/io.h>

//VARIABLES GLOBALES
const int vel=100;

//"rebotadores" a cero
int rebotador_SO10=0;
int rebotador_SO12=0;

//banderas para la interrupcion temporal inicialmente bajadas
int f_SO10=0;
int f_SO12=0;

//inicializamos la cuenta de coches y la cola a 0 
int coches=0;
int cola=0;
//entrada a 1 (1=permitida, 0=no permitida)
int entrada=1;


//MANU - DANI
//Asm_secado.s" -> esta incluido en el proyecto
#include "Funciones_Defines_Macros.h"
#include "Interrupciones.c"
#include "LavadoH.c"


//ANA - ANGELA
#include "barreradefinitivo.c"
#include "luzdeestado.c"

//JUAN - ALEX
// asm_filtrarSO10.s -> incluido en el proyecto

void Inicializacion_General() {
	//pone a 0 todos los registros que se usaran para configuraciones o salidas
	//para posteriormente escribir el valor deseado mediante un |=
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TIMSK1 = 0x00;
	TCNT1 = 0;
	OCR1A = 0;
	OCR1B = 0;
	OCR1C = 0;
	
	TCCR3B = 0x00;
	TIMSK3 = 0x00;
	TCNT3 = 0;
	
	EIFR =0b11111111;
	EICRB=0b00000000;
	EIMSK=0b00000000;
	
	PCICR  = 0b00000000;
	PCMSK0 = 0b00000000;
	PCMSK2 = 0b00000000;
	
	DDRB=0b00000000;
	DDRE=0b00000000;
	DDRK=0b00000000;
	DDRL=0b00000000;
	
	PORTE=0b00000000;
	PORTB=0b00000000;
	PORTK=0b00000000;
	PORTL=0b00000000;
}

void ConfigurarCinta() {
	cli();
	DDRB|=0b00100000;		//PB5= 0C1A M6_en
	DDRE|=0b00000010;		//PE1= M6_di
	
	// Configuramos el Timer1 en modo FastPWM
	TCCR1A |= 0b10000001;	// COM1A1,COM1A0,COM1B1,COM1B0,COM1C1,COM1C0,WGM11,WGM10
	// A=10-> enciende OC1A en 0, reinicia en OC; B y C no los usamos; WGM= 01 01->Fast PWM 8 bits
	TCCR1B |= 0b00001011;	// ICNC1 ,ICES1 ,  -   , WGM13, WGM12,  CS12, CS11,CS10
	// IC=Input Capture NC=Reductor de ruido ES=selector de flanco, WGm..., CS= preescaling (001 desactivado)
	
	OCR1A = 0;			//OCR1A = Valor del OCR (fin de cada ciclo)
	
	TCNT1 = 0;				// Resetear la cuenta del timer
	sei();
}

void ConfigurarSemaforo() {
	//las luces del semaforo comparten timer con la cinta, por lo que la declaración es análoga
	//se usa el mismo modo (FASTPWM 8 bits) y el mismo preescaler
	
	cli();
	DDRB|=0b11000000;		//PB7= 0C1c L5, PB6= OC1B L4
	
	// Configuramos el Timer1 en modo FastPWM
	TCCR1A |= 0b00101001;	// COM1A1,COM1A0,COM1B1,COM1B0,COM1C1,COM1C0,WGM11,WGM10
	// B,C =10-> enciende OC1B,C en 0, reinicia en OC; WGM= 01 01->Fast PWM 8 bits
	TCCR1B |= 0b00001011;	// ICNC1 ,ICES1 ,  -   , WGM13, WGM12,  CS12, CS11,CS10
	// IC=Input Capture NC=Reductor de ruido ES=selector de flanco, WGm..., CS= preescaling (001 desactivado)
	
	OCR1B = 0;			//OCR1x = Valor del OCR (fin de cada ciclo)
	OCR1C = 0;
	
	TCNT1 = 0;				// Resetear la cuenta del timer
	sei();

}

void ConfigurarTimerRebotes() {
	//habilita la cuenta del timer 3 y una interrupcion por desbordamiento para filtrar rebotes
	cli();
	
	TCCR3B = 0x01;
	TIMSK3 = 0x01;
	
	TCNT3 = 0;				// Resetear la cuenta del timer
	sei();
}

void MoverCinta(int velocidad) {
	OCR1A = velocidad;			// Valor del OCR (fin de cada ciclo)
	PORTE|=0b00000010;			//habilitar movimiento de la cinta (no haria falta, se fija mediante el switch fisico)
}

void PararCinta() {
	PORTE&=0b11111101;			//deshabilitar movimiento de la cinta (no sería necesario, ya que el switch fisico
								//esta fijado y esta acción no realiza nada sobre la maqueta
	TCCR1A=0b00000000;			//paro la cuenta del timer 1. En ningun caso el  semaforo esta encendido cuando la cinta
								//esta apagada, por lo tanto puedo parar la cuenta del timer sin estropear la funcionalidad
								//del semaforo
	OCR1A=0;					
	
	
}

void PonerSemaforoVerde() {

	//para ponerlo en verde -> anula el rojo y luego activa el de verde
	OCR1B=80;
	OCR1C=0;
}

void PonerSemaforoRojo() {
	//para ponerlo en verde -> anula el rojo y luego activa el de verde
	//los valores son distintos al de verde ya que los leds no tienen la misma intensidad de luz
	OCR1B=0;
	OCR1C=100;
	
}

void ApagarSemaforo() {
	//para apagarlo del todo -> anula los 2
	OCR1B=0;
	OCR1C=0;
}

void ActivarIntSO10() {
	//para evitar posibles rebotes, bajo la bandera asociada a una interrupción pendiente en SO10
	EIFR = 0b01000000;
	//y despues habilito de nuevo la interrupcion por flanco de subida (lo que equivale a un flanco 
	//de "bajada de coche"
	EIMSK|=0b01000000;
	EICRB|=0b00110000;	// 00, 11 - INT6 f.s, 00, 00
}

void DesactivarIntSO10() {
	//deshabilito la interrupcion bajando la mascara, pero sin cambiar el modo de funcionamiento en EICRB
	EIMSK&=0b10111111;
}

void ActivarIntSO12() {
	//analogo a ActivarIntSO10
	EIFR = 0b10000000;
	EIMSK|=0b10000000;
	EICRB|= 0b11000000;	// 11 - INT7 f.s, 00, 00, 00
}

void DesactivarIntSO12() {
	//analogo a DesactivarIntSO10
	EIMSK&=0b01111111;
}

ISR(TIMER3_OVF_vect) {
	
	asm_filtrarSO10();
	/*
	//la funcion en ensamblador gestiona el antirrebotes del SO10:
	//el codigo en c equivalente es:
	
	if (f_SO10==1){
		if (rebotador_SO10 > 0)	{rebotador_SO10--;}
		else if ( (PINE&0b01000000)!=0 ) {		//Si SO10 esta a 1 (no ve coche)
			if (coches>0) {PonerSemaforoVerde();}
			
			//bajar la bandera para que no entre mas veces en el if una vez la cuenta llega a 0
			f_SO10=0;
			
			ActivarIntSO10();
		}
	}
	*/
	
	
	if(f_SO12==1) {										//Si se ha levantado la bandera para filtrar SO12
		if (rebotador_SO12 > 0)		{rebotador_SO12--;}	//Si el rebotador aun no es 0 lo decrementa. Cuando llega a 0
		else if ( (PINE&0b10000000)!=0 ) {				//Si SO12 esta a 1 (no ve coche)
			if(coches>=1) {								//Si quedan coches en el lavadero
				coches--;								//resta 1 a la cuenta de coches (se considera que la salida del coche
														//de la zona de SO12 es la salida del lavadero
				PonerSemaforoRojo();					//como quedaban coches pongo el semaforo en rojo
			}
			
			if (coches==0) { //si no quedan coches -> apaga del todo (cambiar luz de funcionamiento, cinta y semaforo)
				ApagarSemaforo();
				
				PararCinta();
				
				//la luz de funcionamiento se apaga ya que en su rutina de interrupción temporal se consulta si coches es 0
							
				entrada=1;	//habilito de nuevo la entrada al lavadero
			}
			
			//bajamos la bandera para que no entre mas veces en el if una vez la cuenta ha llegado a 0 y lanzamos de nuevo la IntSO12
			f_SO12=0;
			
			ActivarIntSO12();
		}
	}

}

ISR( INT6_vect ){	//Int_SO10
	//si hay un flanco de subida en SO10 (el sensor funciona por nivel bajo, lo que
	//equivale a un flanco de bajada real) => pon semaforo en verde
	//Para filtrar rebotes se hace en una interrupcion temporal y aqui solo se
	//incrementa la cuenta y se levanta la bandera de SO12
	//Se desactiva la interrupción para que no salte varias veces mientras se espera
	//al tiempo del antirrebotes
	rebotador_SO10=3;
	f_SO10=1;
	DesactivarIntSO10();
	
	//este momento es tambien el elegido para parar el rodillo de lavado vertical
	pararm2();
	
}

ISR( INT7_vect ){	//Int_SO12
	//si hay un flanco de subida en SO12 (el sensor funciona por nivel bajo, lo que
	//equivale a un flanco de bajada real) => pon semaforo en rojo si hay coches
	//si no hay mas coches se apaga el semaforo
	//Para filtrar rebotes se hace en una interrupcion temporal y aqui solo se
	//incrementa la cuenta y se levanta la bandera de SO12
	//Se desactiva la interrupción para que no salte varias veces mientras se espera
	//al tiempo del antirrebotes
	rebotador_SO12=3;
	f_SO12=1;
	DesactivarIntSO12();
	
}

int main(void)
{	cli();
	Inicializacion_General();
	
	//JUAN - ALEX
	ConfigurarTimerRebotes();
	ActivarIntSO10();
	ActivarIntSO12();
			
	//MANU - DANI
	Activar();
	
	//ANA - ANGELA
	luzdeestado();
	Encender();
	
	sei();
	
	while(1)
	{	
			
	}
}