/*esta funcion en ensamblador es equivalente a:
		
		void Secador(int SiS, int ScS) {
			static short int Men=0,Mdi=0;
			//Control del motor Arriba/Abajo/Para
			//Arriba
				if (!SiS || !ScS)
			{
				Men = ENCENDIDO;
				Mdi = ARRIBA;
			}
	
			//Abajo
			else
			{
				Men = ENCENDIDO;
				Mdi = ABAJO;
			}
			PORT(L,7,Men);
			PORT(L,6,Mdi);
		
		}
		*/

 #include <avr/io.h>
  .global asm_secado

 asm_secado:
 //Negamos SiS, primer bit, el resto seran ignorados
 com r24
 //Negamos ScS, primer bit, el resto seran ignorados
 com r22

 //Suma logica de !SiS con !ScS
 or r24, r22
 
 sbrs r24, 0x00
 jmp else
 //if(!SiS||!ScS):
 lds r24, PORTL		//Motor sube
 ori r24, 0b11000000//0b Men Mdi 00 0000 
 sts PORTL, r24 
 ret
 
 else:
 lds r24, PORTL		//Motor baja
 ori r24, 0b10000000//0b Men Mdi 00 0000 
 andi r24, 0b10111111
 sts PORTL, r24
 ret