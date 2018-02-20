/* Codigo equivalente en C

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

	#include <avr/io.h>
	// declarar variables globales de c
	.extern f_SO10
	.extern rebotador_SO10
	.extern coches

	//Declarar funciones externas de C que se llaman
	.extern PonerSemaforoVerde
	.extern ActivarIntSO10

	//declarar funcion asm para que sea visible desde el programa en c
	.global asm_filtrarSO10 

	asm_filtrarSO10:


	//if de entrada (if(f_SO10==1)...)
	lds r27, f_SO10
	SBRS r27, 0
	jmp else1 
	//if1: (
	ldi r18, 0b00000000
	lds r21, rebotador_SO10
	cp  r18 , r21
	brlo if2
	//else2
	lds r19 , PINE
	sbrs r19, 6
	jmp else3
	
	
	//if3
	sts f_SO10, r18
	call ActivarIntSO10
	ldi r20, 0b00000000
	lds r26, coches
	cp  r20 , r26
	brlo if4
	
	ret
	
	
	if4:
	call PonerSemaforoVerde
	ret
	
	
	else3:
	ret
	
	
	if2:
	dec r21
	sts rebotador_SO10, r21
	ret




	else1:
	ret