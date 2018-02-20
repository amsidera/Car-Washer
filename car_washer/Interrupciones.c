#include "Funciones_Defines_Macros.h"

//Lo diseñamos procurando que el tiempo de ejecucion de la rutina fuese el minimo
ISR(PCINT2_vect)
{
	//Se añadio 'static' para evitar declarar variables de forma repetida
	static short int SiL=1,ScL=1,SdL=1,SiS=1,ScS=1;
	
	//La configuración de los puertos IO 'K' se configuran como entradas
	//////////////////
	int backupK;	//Esta configuracion se restablece al acabar la rutina
	backupK = DDRK;
	DDRK &= 0b00000111;
	//////////////////

	int PINKi;		//Se realiza una sola lectura para evitar problemas debidos a
	PINKi = PINK;	//cambios en los sensores durante la ejecucion de la rutina

/****************LAVADO*****************/
	if(PIN(Ki,7)!= SiL){
		SiL = !SiL;
		LavadoHorizontal(SiL, ScL, SdL);
	}
	else if(PIN(Ki,6)!= ScL){
		ScL = !ScL;
		LavadoHorizontal(SiL, ScL, SdL);
	}
	else if(PIN(Ki,5)!= SdL){
		SdL = !SdL;
		LavadoHorizontal(SiL, ScL, SdL);
	}
/****************SECADO*****************/
	else if(PIN(Ki,4)!= SiS){
		SiS = !SiS;
		asm_secado(SiS,ScS);
	}
	else if(PIN(Ki,3)!= ScS){
		ScS = !ScS;
		asm_secado(SiS,ScS);		
		
	}
	/***********************************************/
	//* Por seguridad*/ else{Secador(SiS,ScS);LavadoHorizontal(SiL, ScL, SdL);}
	////////////
	DDRK = backupK;
	////////////

}