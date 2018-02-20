
#ifndef FUNCIONES_DEFINES_MACROS_H_
#define FUNCIONES_DEFINES_MACROS_H_


#include <avr/io.h>
#include <avr/interrupt.h>

//Se usa para las macros realizadas a continuación
#define P0 0b00000001
#define P1 0b00000010
#define P2 0b00000100
#define P3 0b00001000

#define P4 0b00010000
#define P5 0b00100000
#define P6 0b01000000
#define P7 0b10000000

//Devuelve el valor del pin NUM del puerto X
//OPCION 1
#define PIN(X,NUM) ((PIN ## X & P ## NUM) == P ## NUM)

//OPCION 2
//#define PIN(X,NUM) ((PIN ## X >> NUM) & 0x01)


//Otorga el valor 'DATO' al pin NUM del puerto X (se debe poner ; al final)

//OPCION 1
#define PORT(X,NUM,DATO) if(DATO)  PORT ## X |=  P ## NUM; else    PORT ## X &= ~P ## NUM
//		EJEMPLO:PORT(E,3,Mg);
//		if(Mg)  PORTE |=  P3;
//		else    PORTE &= ~P3;

//OPCION 2
//#define PORT(X,NUM,DATO) PORT ## X = (((PORT ## X) & ~(0x01 << NUM)) | (DATO << NUM)) 
//		EJEMPLO:PORT(E,3,Mg);
//		PORTE = ((PORTE & ~(0x01 << 3)) | (Mg << 3));

//Se usa en la funcione de lavado horizontal:
#define ENCENDIDO 1
#define APAGADO 0
#define ARRIBA 1
#define ABAJO 0

//Funciondes:
void Activar();							//Activar: Configura/habilita los pines usados en LavadoHorizontal y asm_secado.
void LavadoHorizontal(int, int, int);	//LavadoHorizontal: Actua sobre los motores que controlan el rodillo horizontal de acuerdo a  
										// tres parametros que representan los valores de los sensores ópticos izquierdo,centro y derecho respectivamente.
void asm_secado(short int, short int);	//asm_secado: realizada en ensamblador. Actua sobre los motores que controlan el secador de acuerdo a
										// tres parametros que representan los valores de los sensores ópticos izquierdo y centro respectivamente.




#endif /* FUNCIONES_DEFINES_MACROS_H_ */