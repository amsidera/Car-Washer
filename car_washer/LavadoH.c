#include "Funciones_Defines_Macros.h"

void Activar()
{
	cli();
	//Se usa el puerto L (salidas) y el puerto K (entradas)
	PCICR  |= 0b00000100;	//Mascara. Activar PCINT2
	PCMSK2 |= 0b11111000;	//Mascara. Activar pines 7..3: Sensores opticos del lavadoH y secado

	DDRL |= 0b11101100;		//Mascara. Configurar pines 7,6,5,3 y 2 como salida:  Motores del lavadoH y secado
	DDRK &= 0b00000111;		//Mascara. Configurar pines 7...3 como entrada: Sensores opticos del lavadoH y secado

	sei();
}

void LavadoHorizontal(int SiL,int ScL, int SdL)
{

	static short int Men=0,Mdi=0,Mg=0;

	
	//Control del motor de giro: Enciende si se detecta algo
	if(SiL && ScL && SdL)  Mg = APAGADO;
	else				Mg = ENCENDIDO;
	
	//Control del motor Arriba/Abajo/Para
	//Arriba
	if (!SiL)
	{
		Men = ENCENDIDO;
		Mdi = ARRIBA;
	}
	
	//Abajo
	else if (ScL /*|| !SdL*/)	//El coche podia resultar aplastado
	{
		Men = ENCENDIDO;
		Mdi = ABAJO;
	}
	
	//Para
	else
	{
		Men = APAGADO;
	}
	
	
	//Actuador: Mover motores
	/*Puerto del motor M4*/
	PORT(L,5,Mg);
	/*Puerto del motor M3*/
	PORT(L,3,Men);
	PORT(L,2,Mdi);
	
}