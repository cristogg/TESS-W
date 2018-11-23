//Compilador Keil C51
//uP AT89C51
//version operativa desde 11/04/2014

//-------------------------------------------------------------
//-------   Medidas de frecuencia 20-03-2014     --------------
//------  lecturas medias promediando lecutras durante 60sg ---
//-------------------------------------------------------------
//  f IN Hz	  f OUT Hz		 Mag V estimada
//     0,080   0,0800 			 23,78
//     0,100   0,1000 			 23,54
//     0,200   0,2000 			 22,77
//     0,500   0,5000 		     21,77
//     1 	   1 				 21,01
//     2       2  		 		 20,26
//     5 	   5,0018  			 19,26
//    10 	  10,0048	 		 18,50
//    20 	  19,975 	 		 17,74
//    50   	  49,841			 16,75
//   100	 100,0			 	 15,99
//   200	 200,0			 	 15,23
//   500	 500,0			 	 14,23
//  1000	1002 			     13,47
//  2000	2003 			     12,74
//  5000    5007			     11,72
// 10000   10015  				 10,96
// 20000   20031		 		 10,20
// 50000   50078		  	      9,20
//---------------------------------------------------------
// Version LSM303_4051d.c 
// Si no hay acelerometro ni IR solo sale <fm 04970><mZ -0000>
// Comando <S>  para enviar lecturas acelerometro siempre que pueda. Para cancelar: <s>
// Comando <V>	para leer fecha de compilacion
// Comando <Pxxx> entre 001 y 10 para cambiar periodo de lectura, poco efecto segun f
// --------------------------------------------------------


#include <REG51.h>
#include <math.h> 
#include <stdlib.h>

typedef unsigned char byte;
typedef unsigned int word;

#define TRUE 1
#define FALSE 0

#define LSMAce 0x32 //Direccion WR	( RD: 0x33)
#define CTRL_REG1Ace 0x20 
#define OUT_X_LA_msb 0xA8 
#define diraX_L 0x28
#define diraX_H 0x29
#define diraY_L 0x2A
#define diraY_H 0x2B
#define diraZ_L 0x2C
#define diraZ_H 0x2D
//MAGNETOMETRO
#define LSMMag 0x3C   //Direccion WR  ( RD: 0x3D )
#define CRA_REG_M 0x00
#define CRB_REG_M 0x01
#define MR_REG_M 0x02 
#define SR_REG_M 0x09
#define IR_REG_M1 0x0A
#define IR_REG_M2 0x0B
#define IR_REG_M3 0x0C
#define dirmX_H 0x03
#define dirmX_L 0x04
#define dirmZ_H 0x05
#define dirmZ_L 0x06
#define dirmY_H 0x07
#define dirmY_L 0x08
#define dirmT_H 0x31
#define dirmT_L 0x32

#define  tam_buf  7
#define READ 1
#define WRITE 0


sbit SCL = P1^1;
sbit SDA = P1^0;

sbit TEST = P3^7;

sbit TSL = P3^5;

byte comand;
int Periodo;
char num1631; 
char aux;
int auxi, datos, ax, ay, az, mx, my, mz;
float fTSL, mv;
double Timer2A;

byte cad[20];
char bufferR[8];//buffer de recepcion
bit LSM_ok;
bit orientar;

bit fserie;  //indica mensaje completo recibido por puerto serie
bit RTCint;
bit puedoTr; // permiso de transmision por puerto serie
byte contar_R = 0; // contador de recepciones del puerto serie
word auxiT;
void Delay(word a);


//----- funciones I2C-----
void I2CSendAddr(unsigned char addr, unsigned char rd);
void I2CSendByte(unsigned char bt);
unsigned char I2CGetByte(unsigned char lastone);
void I2CSendStop(void);
void I2CSendStart(void);
void I2CSCLHigh(void);
void LeeMlAcMa(void);

void xputchar (char c)
{
  while (!puedoTr)
  ;
  SBUF=c;
  puedoTr=0; 
}

void xprint (const char *strp)
{
  while (*strp !=0)  {
     xputchar (*strp);
     strp++;
  }
}

byte extrae_byte ()
{
	byte num;
	num =     (bufferR[4]	& 0x0F) * 1;
	num  +=  ((bufferR[3]	& 0x0F)) * 10;
	num  +=  ((bufferR[2]	& 0x0F)) * 100;
	return(num);
}



void conv_num_cad(char * cadena, word num, byte tamano) //no funciona si hace reentrante
{
  cadena[tamano]=0; 
	while(tamano)
	{
		cadena[tamano-1] = (num % 10) + 0x30;
		num = num / 10;
		tamano--;
	}
}
void retardo10us (void) 	//  Retardo fijo de 10us 
{
   unsigned char l;
   l=0;
   l=1;
   l=0;
   l=1;
}

//----------------------------------------------------------------------------------
/*  Funcion de retardo en milisegundos */
void retardo (byte duracion)
{
   byte l,s;
     for (s=0; s<duracion; s++)
	 {
        for (l=0; l<111; l++);
	 }
}



init_timer1_TSL(void)  // timer1 para contar pulsos del TSL
{
EA=0;
    TMOD  &= 0X0F;     // clear Timer 1 control
    TMOD |= 0X50 ;							   
    TL1 = 0X0 ;        // value set by user      
    TH1 = 0X0 ;        // value set by user      
    TR1 = 1;           // TCON.6  start timer    
EA=1;
}

 

init_serie(void)	// timer1 baudios puerto serie
{
EA=0;
  	SCON  = 0x50;        /* SCON: mode 1, 8-bit UART, enable rcvr    */
    TMOD  &= 0X0F;       /* clear Timer 1 control    */
  	TMOD |= 0x20;        /* TMOD: timer 1, mode 2, 8-bit reload      */
  	TH1   = 0xFD;        /* TH1:  para 9600: 0xFA  con 22MHz, 0xFD con 11MHz  */
  	TR1   = 1;           /* TR1:  timer 1 run                        */
  	ES=1;   			 /* habilita la interrupcion 0 de puerto serie*/
EA=1;
}




//--------------------------------------------------------------------
// Interrupcion del puerto serie
void serie0 () interrupt 4 using 2
{
  static bit empezado = FALSE;
  byte aux_rec;

  if (RI)  
  {                         // if receiver interrupt 
    aux_rec = SBUF;
	if ((empezado) && (aux_rec != '<'))
	{
      bufferR[contar_R++] = aux_rec;
      if(contar_R >tam_buf)
		contar_R = 0;
	  if (aux_rec == '>')  //fin de mensaje
      {
		 empezado = FALSE;
		 fserie = TRUE; //flag mensaje almacenado	
    	 bufferR[contar_R] =0;
	  }                                          
    }
    else if (aux_rec == '<') // inicio de mensaje
	{
		empezado = TRUE;  
		bufferR[0] = aux_rec;
		contar_R = 1;
	}
	RI = FALSE;
  }
  else {
    TI= FALSE;
	puedoTr = TRUE;
  }
}
//--------------------------------------------------------------------
						     			
void I2CSendStart(void)
{
  SCL = 1;
  retardo10us();
  SDA = 0;
  retardo10us();
  retardo10us();
  SCL = 0;
  retardo10us();
}

void I2CSendStop(void)
{
  SDA = 0;
  retardo10us();
  SCL = 1;
  retardo10us();
  SDA = 1;
  retardo10us();
}
 
void I2CSCLHigh(void) // Set SCL high, and wait for it to go high
{
register char err;
  SCL = 1;
  retardo10us();
  while (!SCL)
  {
	err++;
	if (!err)
    {
  		return;
    }
  }
}


unsigned char I2CGetByte(unsigned char lastone) // last one == 1 for last byte; 0 for any other byte
{
register unsigned char i, res;
  res = 0;

  for (i=0;i<8;i++) // Each bit at a time, MSB first
  {
    I2CSCLHigh();
    retardo10us();
    res *= 2;
    if (SDA) res++;
    SCL = 0;
    retardo10us();
  }
  SDA = lastone; // Send ACK according to 'lastone'
  I2CSCLHigh();
  retardo10us();
  SCL = 0;
  retardo10us();
  SDA = 1; // end transmission
  retardo10us();
  retardo10us();

  return(res);
}


void I2CSendAddr(unsigned char addr, unsigned char rd)
{
  I2CSendStart();
  I2CSendByte(addr+rd); // send address byte
}


void I2CSendByte(unsigned char bt)
{
unsigned char i;

  for (i=0; i<8; i++)
  {
	if (bt & 0x80) SDA = 1; // Send each bit, MSB first changed 0x80 to 0x01
	  else SDA = 0;
	I2CSCLHigh();
	retardo10us();
	SCL = 0;
	retardo10us();
	bt = bt << 1;
  }
  SDA = 1; // Check for ACK
  retardo10us();
  I2CSCLHigh();
  retardo10us();
  SCL = 0;
  retardo10us();
}



void LSM_on(void)  //  se configura el modo de funcionamiento
{
	//  ACELEROMETRO
I2CSendAddr(LSMAce, WRITE);
I2CSendByte (CTRL_REG1Ace);	//i2c_write_byte(0x20); // CTRL_REG1_A
I2CSendByte (0x27);	//normal power mode, 50 Hz data rate, all axes enabled
I2CSendStop();
	// MAGNETOMETRO
I2CSendAddr(LSMMag, WRITE);	
I2CSendByte (MR_REG_M);	
I2CSendByte (0x00);	// continuous conversion mode
I2CSendStop();

I2CSendAddr(LSMMag, WRITE);
I2CSendByte (CRA_REG_M);	
//I2CSendByte (0x10);	// Temperatura OFF y velocidad salida datos 15
I2CSendByte (0x90);	// Temperatura ON y velocidad salida datos 15
I2CSendStop();

I2CSendAddr(LSMMag, WRITE);
I2CSendByte (CRB_REG_M);	
I2CSendByte (0x20);	// Ganancia +-1.3
I2CSendStop();
}

void xprint5(int val)
{
   if(val>0) {
	   xputchar('+');
       conv_num_cad(cad, val/16, 4);	 
   }
   else {
	   xputchar('-');
       conv_num_cad(cad, (val*-1)/16, 4);	 
   }
   xprint(cad);
   xputchar('>');
}

void xprint5M(int val)
{
   if(val>0) {
	   xputchar('+');
       conv_num_cad(cad, (word)(val), 4);	 
   }
   else {
	   xputchar('-');
       conv_num_cad(cad, (word)(val*-1), 4);	 
   }
   xprint(cad);
   xputchar('>');
}


int LSM_leeWord(byte lsm, byte dir)
{

  I2CSendAddr(lsm,WRITE); // 0x3C selecciona dispositivo para escribir
  I2CSendByte(dir);
  I2CSendAddr(lsm,READ);
  datos = I2CGetByte(1); 	   //  lee registro MS
  I2CSendStop(); 			   // send stop

  I2CSendAddr(lsm,WRITE); // 0x3C selecciona dispositivo para escribir
  I2CSendByte(dir+1);
  I2CSendAddr(lsm,READ);
  auxi = I2CGetByte(1); 	   //  lee registro MS
  I2CSendStop(); 			   // send stop

  if(lsm == LSMAce) {
  datos+= auxi<<8;
	if (auxi >= 0x80)
	  datos = (0xFFFF-datos) * -1;
  }
  else {
  auxi+= datos<<8;
	if (datos >= 0x80)
	  auxi = (0xFFFF-auxi) * -1;
  datos =  auxi;
  }
  return datos;
}


void LeeAce()  // Lee 3 ejes ACELEROMETRO
{
  ax = LSM_leeWord(LSMAce, diraX_L);
  ay = LSM_leeWord(LSMAce, diraY_L);
  az = LSM_leeWord(LSMAce, diraZ_L);

  if((ax==0) && (ay==0) && (az==0)) {
   LSM_on();   //Intenta reiniciar acel-mag.
   LSM_ok = 0;
  }
  else {
	LSM_ok = 1;
    xprint("<aX ");	 xprint5(ax);
    xprint("<aY ");	 xprint5(ay);
    xprint("<aZ ");	 xprint5(az);
  }
}



void LeeMag() //Lee 3 ejes MAGNETOMETRO
{
  if (LSM_ok) {
    mx = LSM_leeWord(LSMMag, dirmX_H);  //l
    my = LSM_leeWord(LSMMag, dirmY_H);  //l
    mz = LSM_leeWord(LSMMag, dirmZ_H);  //l
  
    xprint("<mX ");
    xprint5M(mx);
    xprint("<mY ");
    xprint5M(my);
  }
  xprint("<mZ "); //Siempre saca mZ para terminar la serie
  xprint5M(mz);

}

word MLX(byte dir) //Lee temperatura del MLX
{
word k;
byte t;
  I2CSendAddr(0x00,WRITE); 
  I2CSendByte(dir); 	     
  I2CSendAddr(0x00,READ);
  t = I2CGetByte(0); 	   
  k = I2CGetByte(1);
  if(0x80 & (byte)(k))	 //mira bit de error
   return(0);
  	 
  I2CSendStop(); 			 
  k = k*256 + (word)(t);
  return k;
}

void LeeMLX() //Lee Temperaturas MLX
{

	ax = MLX(0x06);
 if (ax > 0) {
    xprint("<tA ");  // T ambiente  
	ax = ax*2 - 27315; 
    if(ax>0) {
	   xputchar('+');
       conv_num_cad(cad, ax, 4);	 
    }
	else {
	   xputchar('-');
 	   conv_num_cad(cad, ax*-1, 4);	 
	}
	xprint(cad);
	xputchar('>');

	xprint("<tO "); //T objeto
	ax = MLX(0x07); 
	ax = ax*2 - 27315; 
    if(ax>0) {
	   xputchar('+');			 
       conv_num_cad(cad, ax, 4);	 
    }
	else {
	   xputchar('-');
 	   conv_num_cad(cad, ax*-1, 4);	 
	}
	xprint(cad);
	xputchar('>');
 }
}


void Tareas(char tarea)
{
//word th, tl, tt;

	static byte aux;
	switch (tarea)
	{
		case 'P': //Periodo envio
			aux = extrae_byte();
			if ((aux <=10) && (aux>=1)) {  
				Periodo = aux; 
				xprint(bufferR);     
			}
			else 
				xprint("<1-10>");
			break;

		case 'S':
	 		orientar = 1;
			xprint(bufferR);     
		break;
		case 's':
	 		orientar = 0;
			xprint(bufferR);     
		break;

		case 'M':
	 		LeeMLX();
		break;

		case 'R':
	 		LSM_on();
		break;

		case 'V':
	       xprint("<TESS ");
		   xprint(__DATE2__);
			xprint(">\r\n");
		break;
  

		case 'T':	 //---  Temperatura magnetometro
			xprint("<mT ");
			  ax = LSM_leeWord(LSMMag, dirmT_H);
xprint5M(ax>>4);
		break;

	}
  
}




/* Configure Timer 0
   - Mode                  = 1
   - Interrupt                   = ENABLED
   - Clock Source                = INTERNAL
   - Enable Gating Control    = DISABLED
*/
void init_timer0(void)
{
     TMOD &= 0XF0;                    /* clear Timer 0   */
     TMOD  |= 0X1;
     TL0 = 0XB5;                   /* value set by user    */
     TH0 = 0XFF;                  /* value set by user  */
     ET0 = 1;        /* IE.1*/
	 TR0 = 0;
    // TR0 = 1;                /* TCON.4 start timer  */
}

void int2_timer0 () interrupt 1 using 3
{
//byte aux;

	TR0 = FALSE;
    TL0 = 0xBE;           /* value set by user  FC79 para 1ms  */
    TH0 = 0xFF;           /* value set by user con xtal. 11.0592 MHz */
	if(auxiT < 0xFFFF)
  	  auxiT++;
	TR0 = 1;
}


//------------------------------------------------
//Mide frecuencia en todo el rango. 
//En funcion de esta f:
//si f<50 Hz, se usa Timer0 para medir periodo
//si f>50 y f<50000, se cuentan pulsos en 1/2 sg
//
void CuentaTSL()
{
    init_timer0();
	auxiT = 0;

	if(TSL==1) {		  //----- empieza con pin en ALTO ---
	   TR0 = 1;  //arranca timer
		  	while (TSL ==1)	 //Empieza espera al nivel bajo
			{ //espera terminar ALTO
				if ((auxiT == 20000) || (auxiT == 40000) ) 
				  LeeMlAcMa();
				else if (auxiT > 65024)  // f<0,06Hz				 
			  	break;				
		  	}
			TR0 = 0;  //para timer
			auxiT = 0;
			TR0 = 1;  //arranca timer
			while (TSL ==0) {	 //Empieza cuenta verdadera
				if ((auxiT == 20000) || (auxiT == 40000) ) 
				  LeeMlAcMa();
				else if (auxiT > 65024)  // f<0,06Hz				 
			  	break;						
			}
			TR0 = 0; //para timer

	 }
	 else {		  //----- empieza con pin en BAJO ---
	    TR0 = 1;  //arranca timer
		  	while (TSL ==0)
			{ //espera terminar ALTO
				if ((auxiT == 20000) || (auxiT == 40000) ) 
				  LeeMlAcMa();
				else if (auxiT > 65024)  // f<0,06Hz				 
			  	break;				
		  	}
			TR0=0;
			auxiT = 0;
			TR0 = 1;  //arranca timer
			while (TSL ==1) {
				if ((auxiT == 20000) || (auxiT == 40000) ) 
				  LeeMlAcMa();
				else if (auxiT > 65024)  // f<0,06Hz				 
			  	break;						
			}
			TR0 = 0; //para timer
	 }

	 if(auxiT > 100) {          //Si hay >100 pulso en el nivel f < 50Hz y calculamos 
	   if(auxiT < 65024) {      //Comprueba si no es demasiado lento 
//         fTSL = (float)(auxiT)*0.2;  //periodo en msg, el 2 para el periodo entero
// 	       fTSL = 994.0/fTSL;        //frecuencia ajustando el milisegundo
		 //fTSL = 4970.0/(float)(auxiT); //simplificando las dos lineas superiores.
		 fTSL = 5004.0/(float)(auxiT); //
	   }
	   else fTSL = 0;
	 }
	 else {	                   // Si f>= 50 contamos pulsos en 1/2 sg. 
	 	auxiT = 0;
    	init_timer0();	   //T0 base de 0.5sg para contar con T1
     	init_timer1_TSL(); //T1 cuenta pulsos. Se pierde puerto serie
		TR0 = 1; 
		while (auxiT < 5004){//4970) { //4996) //espero 1/2 sg
		  if(TH1== 0xFF) break;	  //evita desbordar contadores
		}
    	TR1	= 0; 	  //para T1 antes de leer
		TR0= 0;
    	auxiT = ((word)(TH1))<<8 | (word)(TL1);	// pulsos en 1/2 sg
		if (auxiT > 20) //si salen mas de 20Hz hay un problema
		  fTSL = 2.0*(float)(auxiT);// doble de pulsos par el sg entero
		else
		  fTSL = 0; 

		init_serie(); //recupera puerto serie
//retardo(20);
	 }

//-------------------------------------
	
 	if(fTSL<65000.0) { //si la frecuencia cabe en 5 cifras
	  if (fTSL < 50.0){
	    if (fTSL > 0) {
	      xprint("<fm ");	  
	      mv = fTSL*1000.0;      //pone mHz
	      auxiT = (word)(mv);
		}
		else {
		  xprint("<fm-"); //f demasiado baja
		  auxiT = 0;
		}
	  }
	  else {
	    xprint("<fH ");	  
	    auxiT = (word)(fTSL); // pone Hz
	  }		
      conv_num_cad(cad, auxiT, 5);
	  xprint(cad);		   
	}
	else					// f > 60KHz muy alta y no
	  xprint("<fH-00001");

   	xputchar('>');


	//----------------------
}


void LeeMlAcMa(void) {
   LeeMLX();  
   LeeAce();
   LeeMag(); 
   TEST = 1;   //apaga led
   xputchar(13); // ret
   xputchar(10); //	nl
}


void main(void)
{
	retardo(100);
	LSM_on();	//Inicializa Acelerometro

	init_serie();	
	EA=1;
   	puedoTr =1;

	Periodo = 1;	
// cadena de inicio del programa con la fecha de compilacion
	xprint("<TESS ");
	xprint(__DATE2__);
	xprint(">\r\n");

	
  while (1)
  {
 	if(fserie) {     // Recibido mensaje del PC
	 		contar_R = 0;
	  		fserie = FALSE;
	  		comand = bufferR[1];// SBUF;
	  	    Tareas(comand);
	}
 
	Timer2A++;
  	if (Timer2A > 200*Periodo)//
	{
	   TEST = 0; //enciende led
	   Timer2A = 0;
	   CuentaTSL();
	   LeeMlAcMa();	//Lee todos los sensores
	}
	if (orientar) {
      LeeAce();
      LeeMag();  	
      xputchar(13); // ret
      xputchar(10); //	nl
	  Timer2A = Timer2A + 20*Periodo; //por el tiempo perdido
   }
 }
}
