#include <avr/io.h>
#include <avr/interrupt.h>
extern int coches;
extern int entrada;
extern int cola;

volatile int contadorsw1=1;		//inicialización de la variable que cuenta los pulsos del sensor SW1


void arrancarm1(){PORTB=PORTB | 0b00010000;}	//Función que arranca el motor de la barrera
void pararm1(){PORTB=PORTB & 0b11101111;}		//Función que para el motor la barrera

void arrancarm2(){PORTL=PORTL | 0b00000001;}	//Función que arranca el motor del lavado vertical
void pararm2(){PORTL=PORTL & 0b11111110;}		//Función que para el motor del lavado vertical


/*===============================================================*/
/* Interrupción por pin change del sensor S01 que detecta cuando */
/* 					hay un coche en la barrera,					 */
/*===============================================================*/
ISR(PCINT0_vect){
	if (entrada ==1) {
		arrancarm1();
		arrancarm2();		
	}
	else if (cola==0) {cola++;}
	ConfigurarSemaforo();
	PonerSemaforoRojo();
	ConfigurarCinta();
	MoverCinta(100);	
}

/*===============================================================*/
/* Interrupción por flanco de bajada del sensor S02 que detecta  */
/*	 				el cierre de la barrera						 */
/*===============================================================*/
ISR(INT5_vect){
	pararm1();
	TCCR4B=0x01;	//Activación del timer en el que se desactiva la interrupción del sensor S02
	TIMSK4 =0x01;	//y se activa la interrupción de SW1
	coches++;		//Incremento del número de coches en el lavadero tras cerrarse la barrera
	entrada=0;		//No se perite la entrada de un nuevo coche hasta que entrada sea 1
	
}

/*===============================================================*/
/* Interrupción por flanco de subida del sensor SW1 que detecta  */
/*	 			  la posición de la barrera						 */
/*===============================================================*/
ISR(INT4_vect){

	TCCR4B=0x01;	//Activamos el timer para filtrar rebotes y gestionar el 
	TIMSK4 =0x01;	//funcionamiento de la barrera

}

/*====================================================================*/
/*   Filtro de rebotes de la interrupción del sensor SW1, en la que   */
/*  se gestiona el funcionamiento de la barrera, de manera que cuando */
/*  se encuentra cerrada desactiva la interrupción de S02 y activa la */
/* de SW1 filtrándola y, cuando está en su posición más alta desactiva*/
/* 				la interrupción de SW1 y activa la de S02 			  */
/*====================================================================*/
volatile int contador1=10;
ISR(TIMER4_OVF_vect){
	
	contador1--;		//
	EIMSK&=0b11001111;	//desactiva las interrupciones
	
	if((contador1<=0)&&(contadorsw1<=5)){//Cuando ha filtrado los rebotes
		contador1=10;			//Ahora entramos a la interrupcion y reiniciamos la cuenta	
		TIMSK4 =0x00;			//Desactivamos el timer
		contadorsw1++;			//Suma que se corresponde con la posición de la barrera
		
		if(contadorsw1==5){		//Cuando la barrera llega a la posición superior
			contadorsw1=0;		//Reinicio de la cuenta
			pararm1();			//Parada de la barrera
			EIFR=0b00100000;	//Desactivación de la interrupción SW1 y activación de S02
			EICRB|=0b00001011;
			EIMSK|=0b00100000;}
		else{
			EIFR=0b00010000;	//Activación de la interrupción de SW1
			EICRB|=0b00001011;
			EIMSK|=0b00010000;
		}	
	}
}
/*====================================================================*/
/*   	Función que coloca la barrera en su posición de inicio 		  */
/*====================================================================*/
void InicioBarrera(){
	
	int inicio=0;

	while((PINE & 0b00100000)==0b00100000 && inicio==0)	//Mientras no esté cerrada la barrera, se mantiene activo el motor
	{
		arrancarm1();
	}
	if((PINE & 0b00100000)==0b00000000 && inicio==0)	//Cuando el sensor S02 detecta el cierre de la barrera se apaga el motor
	{
		pararm1();
		inicio=1;
		contadorsw1=0;	//Inicio de la cuenta de la posición de la barrera
	}
}
/*===============================================================*/
/*	 	Configuración de interrupción por pin change del S01   	 */
/*===============================================================*/
void ActivarPCIntS01(){
	PCICR|=0x01;
	PCMSK0=0b00001000;
}


/*===============================================================*/
/* Función que arranca la configuración de la barrera			 */
/*===============================================================*/
void Encender () {
	cli();
	
/*====================================================================*/
/* Configuración de los pines de entrada y salida: sensor SW1 PE4 (13)*/
/*   sensor S01 PB3(22); S02 PE5 (14); M1_en: PB4 (23); M2_en: PL0    */
/*====================================================================*/
 
	DDRB=0xf0;
	DDRE|=0b00001110;
	DDRL|=0b00000001;
			
	InicioBarrera();
	ActivarPCIntS01();
		
/*====================================================================*/
/*   Configuración de las interrupciones del sensor SW1 por flanco.   */
/*====================================================================*/
	
	EICRB|=0b00001011;
	EIMSK|=0b00010000;
							
	sei();
}