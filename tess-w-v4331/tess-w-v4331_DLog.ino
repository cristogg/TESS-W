// -------------CAMBIOS-----------------------------------------------------
//7/6/2017  Incluido Ficheros en SD, RTC y HTU21D.
    //   En el arranque se comprueba si esta presente nuestro SSID 
    //   para evitar intentos de conexion imposibles.

//12/8/2017 Es posible editar sin resetar todo el acceso wifi y RTC
    // Si hay SD el programa apaga el wifi.
    // Si No hay SD el programa funciona como siempre.

//21-8-2017 En la conexion mqtt se pone como cliente tname
  // es posible poner otro nombre de broker que contenga "tess", igual autentificacion que ucm

//22-8-2017 Sin conexion, el nombre del AP es del tipo: TESS-stars1 para permitir varios fotometros juntos.
  //Si no hay configuración el AP muestra como nombre TESS-W

//25/10/2017 Opción de no intentar mandar MQTT si en el broker ponemos off, <4 caracteres

//24/11/2017 
// - Broadcast UDP con cadena jason especifica:
//{"seq":15, "rev":2, "name":"pru21", "freq":50000.00, "mag":0.00, "tamb":23.85, "tsky":22.87, "alt":86.30, "azi":147.00, "wdBm":0, "ZP":20.50, "text":0.00, "hrel":0.00, "pres":0.00, "ain":8.50}

// ----- Compilador Arduino 1.8.3 pero librería MQTT antigua---------------------

//29/11/2017 Trato configurar desde la ap 2.0
//falta espaciar envio udp a 1 sg
//parpadea led en esp-07 

//5/12/2017 Se procesan los datos del acelerometro magnetometro, desde la web se ajusta el magnetometro
//10/12/2017 La funcion de Gestion Web se saca a un fichero aparte para facilitar la edicion
//5/1/2018  La multicast UDP con cadena JSON funciona en cualquier red

//8/1/2018 El calentador se activa siempre que la temperatura < 5
//14/01/2018 Corregido error en cambio de SSID y passw
//-------------------------------------------------------------------
//9/5/2018 --- lectura luxometro OPT3001 -------------------------
//  Las lecturas del OPT reemplazan el brillo del MLX
//-------------------------------------------------------------------
//3-12-2018 Lectura analogica A0 para anemometro mecanico----------------
// Para anemometro DC 0 a1.2v en J11 quitar R8 y en R7 100nF
//  Lectura A0 + lecturas BME en mensaje multicast y en MQTT.
//  OPT3001 comentado
//--------------------------------------------------------------------------
//--- 8-1-2019 ---------Vesion 3.0------------------------------------------------------------------
// SSID dobla longitud posible, pasa de 16 a 32 caracteres
// Alt y Az por udp si hay acelerometro
//Por puerto serie sale cadena raw :
//<fH-00001><tA +2385><tO +2287><aX +0012><aY +0068><aZ +1068><mX +0305><mY +0285><mZ -1313>
//Si hay BME, en la pagina web sale temperatura, humedad y presión
// 10-01-2019 evia UDP cada 500ms si hay mensaje
// 11-01-2019 quito refresca Oled, ya no se usa con esp8266
// 14-01-2019 paso de SHT a BME
// 17-01-2019 Opcion de desactivar WiFi colocando puente en J11.
//----------------------------------------------------------------------
//--- 01/07/2019-----Version 3.1--------------------------------------------------------------------
//Si hay BMP280 usar punto de Rocio para controlar calefactor. Hay que quitar TVS !!!!!
//----------------------------------------------------------------------

//--- 17/01/2021 ---- tess-w-v31_Lolin32-v1  -------------------------------------------------
// funciona todo con interrupcion en ESP32 TTGO (la de display blanco y negro)
// espera hasta 15sg para detectar señal

//--- 10/03/2021 ---- tess-w-v31_Lolin32-v4  -------------------------------------------------
//Funcionan 4 chanels + configuración web
//cadena jasonlocal sale por Telnet port 23
//Modulo TTGO T1 con TFT

//--- 12/07/2021 ---- tess-w-v31_Lolin32-v4rtc  -------------------------------------------------
//Añado RTC con libreria ErriezDS1307, la anterior no funcionaba con ESP32

//--- 28/07/2021 ---- tess-w-v31_D132-v4rtc  -------------------------------------------------
//Prueba en con ESP32 D1 mini en placa formato TESS-W.
//Configurable 1 Chanel ó 4 Ch.
//---27/10/2021
//Añadido cabiado exit(0) por return en busqueda de RTC, si no existe

//---24/11/2021  Version 4.0 PCB TESS-W4C
//--PCB nueva conESP32 en D1 mini, los pines 26 y 33 no van, paso señal a 18 19
//Añadida precaucion de poner un SSID muy corto.
//Se arregla la medida de entrada analógica.
//10/01/2022  
// Edit CI no salia si habia wifi, corregido.
// Se lee Analog IN en VPS pin 19 y sale en cadenas JSON
//
//-----------------------------------------------------------------------------------------
//-----  12/2/2022  Version 4.1  ESP32      -----------------------------------------------
//--  Placa WEMOS D1 MINI ESP32 en pcv v2.0   ----
//-----------------------------------------------------------------------------------------
//--Uso de SD + Sleep.------ 
//Promedia 10 lecturas o lo recibido en 30sg.
//Guarda cada 10 minutos.
//10mA apagado, 80mA encendido
//LED azul: flash largo al inicio corto antes de sleep.
//Si no hay SD funciona normal
//
//----- 20/04/2022--------------
//Solo si hay RTC se intenta configurar SD, de lo contrario el canal verde es ocupado por la SD

//----- 10/05/2022--------------
//Se añaden milesimas de hz en frecuencias bajas.
//Añadido WatchDog de 5sg
// ---- I2C con LOGGER en placas v2 --------
// Para conectar Logger Deek-Robot ID8122 directamente configurar I2C:
//  Wire.begin(22,21);  //pcb v2 con D1mini 32 con RTC y SD. Implica intercambiar Dato y CLK en termopila.

//----- 26/05/2022-- v4.11 ------------
//Se evita mag = 24 en RGB con saturacion de brillo. 
//Compilado a bin con I2C estandar (para no usar SD)

//----- 26/05/2022-- v4.12 ------------
//Corregido bug en frecuencia Blue

//----- 26/05/2022-- v4.13 ------------
//Identificacion fisicas de canales con IOs en pcb v2 
//-- Posicion Sensores en PCB v2 -----------------
//--   IOs    Frecuencias   Colores --------------
//   25  18 ->  F1  F3  ->  750  R
//   27  19 ->  F2  F4  ->  650  B 
//------------------------------------------------
//El pin de interruptPinG se intercambia con el pin de interruptPinR 
//Formato MAC corregido
//Corregida cadena json de registro.

//----- 27/06/2022--------------
//Corregido comillas en json

//----- 30/06/2022-- v4.14------------
//en 1 canal json compatible con v3

//----- 17/08/2022-- v4.15------------
//en 1 canal json compatible con v2 
//en 4 canal json v3 

//----- 17/08/2022-- v4.16------------
//descarta lecturas erroneas de temperaturas
//----- 17/08/2022-- v4.16DL------------
//I2C intercambia CLK con SDA para leer Data Logger, necesario no cruzar CLK y SDA en termopila.
//

//----- 7/11/2022-- v4.17------------
//Eliminado comando serie 'K' ya que a veces se disparaba en los arranques en modo logger y
//cambiaba la hora al RTC.
//----- 7/11/2022-- v4.18------------
//Magnitud max pasa de 24 a 25
//Se compila en dos versiones, una para el datalogger cambiando el I2C
//pin IO35 no vale para Heatpin, pasa al IO32
//----- 7/11/2022-- v4.19------------
//Aparece MAC en pagina de configuracion de cara a automatizar la configuración
//----- 12/02/2023-- v4.2------------
//----Pagina web v3 para añadir el offset mHz
//----- 23/02/2023-- v4.21------------
//---- Inicia con un solo CI en la web, en vez de 4
//----- 12/02/2023-- v4.23 ------------
//----Si hay RTC y no SD, entra en SLEEP por 10 min.
// en el contador de seq pone el numero de boot

//----- 14/06/2023-- v4.24 ------------
//---En caso de SD, corregido promediado de lecturas y Mag se calcula al final
//eliminada opcion de funcionamiento con wifi, ya que no se puede editar el RTC
//Queda que sin SD pero con RTC arranca comoo AP, para poder poner reloj en hora.
//No SD, SI RTC y encuentra wifi, conecta envia y sleep.
//--- 14/06/2023 --- Añadido hash en todos los mensajes con los tres últmos caracteres de la MAC
//  29/11/2023
//-- Añadida en linea 984     else  HayWifi = true;
//-- Añadida opción de Broker generico, aunque con user y passw igual a ucm.
// En 4C con D1 el led azul hace un parpadeo al inicio. En pcb v4.1 el led está al reves
// 12/12/2023
// Añadid posible broker genérico con las mismas credenciales de la UCM

//----- 18/02/2024  v4.26  ---------
// AnalogA0 envia el max de 5 lecturas por UDP
// Variable nueva, Analog60sg envia max de 60sg
// Añadido RESET por Watch dog para evitar el intento de reconexion continua.

//----- 18/02/2024  v4.27  ---------!!!!! BUG corregido, en caso de 4 ch
// PubSubClient.h, #define MQTT_MAX_PACKET_SIZE 400, aumenta de 300 a 400
// Medida de tiempo de reconexion MQTT corregido
// HayWifi = 1 cada vez que hay un envio mqtt ok, evitara que pag web salga como AP habiendo wifi

//----- 18/04/2024  v4.28  ------
//Desactivado WDT en la conexión MQTT, para evitar un reset cuando el servidor broker no acepta conexion
//No es suficiente, finalmente dejo WDT en 30sg durante la conexión mqtt y luego pasa a 5s
//Quizas el problema era que 
// despues de:  now2 = millis();  //cada 5 seg se comprueba si hay wifi por si se perdio
// Habia un:  mqttcon = 0; innecesario. Lo he quitado y parece que mejor.
// 
//Check connection mqtt.
//Conecting MQTT: astrix.fis.ucm.es-port: 1883
//Conecting mqtt ucm
//Connected UCM OK
//Reconecting Time ms: 2714
//Lecturas promediadas: 60.00
//{"seq":2454, "rev":2, "name":"stars2002", "freq":35340.62, "mag":9.13, "tamb":22.11, "tsky":20.75, "wdBm":-75, "hash":"1B0"}

//----- 18/04/2024  v4.29  ------
//Correccion calculo de medias de frecuencias en canales RGB
//----- 18/06/2024  v4.30  ------
//Correccion de frecuencia  en canal 1 para oscuridad total

//------6/10/2024  v4.31  -------
//Rescatado contador de canal 1, CuentaRxFrecuencia++; que daba origen a lecturas cero
//Corregido CI en mensaje de registro

//------6/10/2024  v4.32  -------
//Añadido al registro offset Hz
//Añadido al registro versión firmware y date  
//
//------17/03/2025  v4.33 DLog  -------
//Apaga el led en pcb 4.2 despues del setup. El LED se instala para Data Logger.
//En PCB v4.2 el led se apaga con HIGH

//--------------------------------------------------------------
//------9/12/2025  v4.331 DLog  -------
//IDE:  1.8.10
//CORE: 3.1.1 
//Ahora no se incrementa contador de lectura en canal 1 si no hay interrupción.
//Desactivado el WDT, con el nuevo core debe estudiarse 

#include <esp_task_wdt.h>//

#define vernum "4.331"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <PubSubClient.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <driver/adc.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

#include <SPI.h>
#include <SD.h>
//#include <pgmspace.h>
#include <Wire.h>  // c
//#include <RtcDS1307.h>
#include <ErriezDS1307.h>

//#include "SparkFunHTU21D.h"

#include <BME280_MOD-1022.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_BME280.h>

//#include "Opt3001.h"
#define USE_USCI_B1 
//Opt3001 opt3001;

//--Simple Deep Sleep with Timer Wake Up ----------
//Author: Pranav Cherukupalli <cherukupallip@gmail.com>
#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  600        /* Time ESP32 will go to sleep (10 minutos) */
//#define TIME_TO_SLEEP  120        /* Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int bootCount = 0; //Escribe en memoria del RTC numero de boots
//--------------------------------------------------


#define LimMag 25    //Magnitud maxima esperable
#define LimInteg 30  //Maximo tiempo de espera de interrupcion


int HeatPin = 32;    //esp32 D1 mini
//int HeatPin = 28;    //esp07, Incompatible con tarjeta SD, que la usa como clk
//int HeatPin = 0;   //esp12S, PIO para calefactor, en TESS-W v1.0 está en el 14.
int DonePin = 2;     //PIO DONE para nanotimer, es el LED azul.
//int Ain4 =  34;  //Entrada analogica SVP GPIO036 pin 19, con Divisor 1/10 a los 5v.
//int Ain4 =  4;  //se usa ADCread con ADC1_CHA6
//GPIO34 readADC para ain 

//const byte interruptPin = 32;  //PCB D1mini
//const byte interruptPinR = 21; //PCB D1mini

//-- Posicion Sensores en PCB v2 -----------------
//--   IOs    Frecuencias   Colores ---------
//   25  18 ->  F1  F3  ->  750  R
//   27  19 ->  F2  F4  ->  650  B 
//---------------------------------------------
const byte interruptPin = 25;    //F1   
const byte interruptPinG = 27;   //F2    
const byte interruptPinR = 18;   //F3 
const byte interruptPinB = 19;   //F4 
/* 
const byte interruptPin = 25;    //F1   PCB TFT & D1
const byte interruptPinR = 27;   //F2    PCB TFT & D1
const byte interruptPinG = 18;   //F3    26;  //19 y 18 usados por SPI   --OJO--!!!!
const byte interruptPinB = 19;   //F4    33; //por que no van 33 y 26 ? --OJO--
*/

volatile int interruptCounter = 0;
volatile int interruptCounterG = 0;
volatile int interruptCounterR = 0;
volatile int interruptCounterB = 0;
volatile int CounterR = 0;
volatile int CounterG = 0;
volatile int CounterB = 0;

int Ti1G, Ti2G, Tius1G, Tius2G, WaitTiG;
int FrecOverG = 0;
int Ti1R, Ti2R, Tius1R, Tius2R, WaitTiR;
int FrecOverR = 0;
int Ti1B, Ti2B, Tius1B, Tius2B, WaitTiB;
int FrecOverB = 0;

bool waitG = 0;
bool waitR = 0;
bool waitB = 0;

//volatile int interruptCounter = 0;
//int numberOfInterrupts = 0;
int milisg; 
int milisgAnt = 0;
int Ti1, Ti2, Tius1, Tius2, WaitTi;
int FrecOver = 0;
int inter1, inter2;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
String DirMac, DirMacHi, DirMacLo,  hash;
String DirMacL4B;

float OffsetHz = 0.0;

//Adafruit_SSD1306 display(OLED_RESET);


//--- I2C ---
//pin 13 SDA    PIO5
//pin  14 SCL   PIO4

//-- SPI tarjeta WEMOS SD ESP32 D1 mimni ---
//CLK    IO18
//MISO   IO19
//MOSI   IO23
//CS     IO5
//--------------

File myFile;
ErriezDS1307 rtc;
struct tm dt;    // Global date/time object
time_t t;
  
//HTU21D myHumidity;
float humd21;
float temp21;
bool HaySHT21;
bool HayRTC;
bool SDcardOK;
bool BME280ok;
int Ncanales =  0;

bool wait = 0;

int nHo;
int nMi;
int nSe;
int nAn;
int nMe;
int nDi;


#define mqtt_port 1883
#define mqtt_server_upm "rich.dia.fi.upm.es"  //DEBE leerse de broker, pero algo falla
#define mqtt_user_upm "guest"
#define mqtt_password_upm "ciclope"

#define mqtt_server_ucm "astrix.fis.ucm.es"  //147.96.67.45
#define mqtt_user_ucm "tess"
#define mqtt_password_ucm "k0L1vr1Sl0w"
//Para el recolector de medidas usuario: tessdb passwd: k0L1vr1F4st

#define mqtt_server_mosquito "test.mosquitto.org"
#define mqtt_server_192 "192.168.1.6"

#define mqtt_server_local "192.168.1.33"
#define mqtt_user_local "publisher" 
#define mqtt_password_local "k0L1vr1Sl0w"
 //para el recolector: subscriber k0L1vr1F4st
//#define MQTT_CONNECT_UNAVAILABLE = 1;
//#define MQTT_SOCKET_TIMEOUT 3//15

//#define reads_topic "tess/coslada4/reads"
String rdtopic;
//#define status_topic  "tess/coslada4/status"
String sttopic;
String ESP8266Client;
//#define TAMBUFF  200  //bufer rx serie medidas tess
#define EepromBuf 250  //espacio para variables de configuracion tess y AP
//#define TelnetPort 26

//const IPAddress apIP(192, 168, 4, 1);
IPAddress apIP(192, 168, 4, 1);
const char* apSSID = "TESS-W";

IPAddress IPfija(192, 168, 1, 111);
//IPAddress gateway(192, 168, 1, 1);
//IPAddress subnet(225, 225, 225, 0);

WiFiUDP Udp;
unsigned int localUdpPort = 2250; // escucha
unsigned int remoteUdpPort = 2255; // envia
char incomingPacket[500]; // buffer for incoming packets
char replyPacekt[] = "Soy el 21"; // a reply string to send back


boolean settingMode;
String ssidList;

String ssid = "";
String pass = "";
String tname = "";
String cons = "";

String consOffset = "";
String rtcDT = "";
String broker = "";
String Envio = "";
String Puerto = "";
char mqtb[32];
String offmhz = "";

int SegEnvio = 60;


boolean mqttcon;
boolean HayWifi;
WiFiClient espClient;
PubSubClient client(espClient);
int segundosWDT = 20;

//uint8_t mactess[WL_MAC_ADDR_LENGTH];  //18FE349CB4C1 modulo ofi

String CadTess;

String macID;
String Signal;
long rssi;
String jasonReads;
String jasonStatus;
String jasonLocal;
String tess_channel = "pruebas";
String revision = "3";
String rev_udp = "3";


String banda1 = "UV/IR-cut" ;
String banda2 = "Johnson-B" ;
String banda3 = "Johnson-R" ;
String banda4 = "Johnson-G" ;

long lastMsg = 0;
long lastMsg2 = 0;
long now4 = 0;
float temp = 0.0;
float mag = 0.0;
float diff = 1.0;


long lastRd = 0;
int p_wr = 0;
int p_rd = 0;
int nd;
//char buff_meteo[TAMBUFF];
long lastSearch;
char Meteo[200];
static char BufTess[200];

float TAmbienteM, TObjetoM, Frecuencia_F, MagnitudM, CuentaRx_tA, CuentaRx_tO, CuentaRxMag; //Para gráfico
float TAmbiente_F, TObjeto_F, Magnitud_F, CuentaRx_tA_F, CuentaRx_tO_F, CuentaRxMag_F; //para fichero

float F_TAmbiente, TAmbiente;
float F_TObjeto, TObjeto;
float F_Magnitud, F_MagnitudR, F_MagnitudG,F_MagnitudB;
float F_Frecuencia, Frecuencia;

float frec, frecG, frecR, frecB;
float frecMG, frecMR, frecMB;
float F_frecMG = 0;
float F_frecMR = 0;
float F_frecMB = 0;


float Magnitud = 0.0;
float MagnitudG = 0.0;
float MagnitudR = 0.0;
float MagnitudB = 0.0;

float CuentaRxFrecuencia_F, FrecuenciaM_F;




bool abreparen, RecibidoTess, BufCompleto;
int posmeteo, CuentasEma, posBuf;
float  OfsetMV =0;
int ConstI = 0;        //Tipo de filtro
//float OfsetTamb = 0;
float LUX;
float tempBME, humedad, rocio, pressure, pressureMoreAccurate;

int AnalogA0, Analog60sg;
int Ainread;

bool AccOK;
float ZP1 = 20.51;
float ZP2 = 20.52;
float ZP3 = 20.53;
float ZP4 = 20.54;

bool MLXok;
//float TIR;


float CuentaRxFrecuencia = 0;
float FrecuenciaM = 0;

//float TAmbienteM = 0, TObjetoM = 0, MagnitudM = 0;
//float CuentaRx_tA = 0, CuentaRx_tO = 0, CuentaRxMag = 0;
int nube;

int tiemposer, tocaenviar;
int estado;
unsigned int Numero = 0;
unsigned int NumeroUdp = 0;
unsigned int T1sg = 0;
unsigned int T30sg = 0;
unsigned int TryMqtt = 0;
unsigned int T01sg = 0;
unsigned int T5sg = 0;



boolean modoAP = 0;
int TelnetPort = 23;
boolean EepromOK;

const char* ssid2     = "WLAN_42A7";
const char* password = "12345678";

long now5;
char datestring[30];
  
bool ssidOk;

bool NuevaMedia;
//RtcDateTime ahorita;
int tamano; 

const byte DNS_PORT = 53;

char report[80];
 
int EsperaConectar = 0;
int EsperaEncontrarWifi = 0;
//DNSServer dnsServer;
//WiFiServer webServer(80);
WebServer webServer(80);
//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
WiFiServer server(TelnetPort); //for send sockets to PC TESS program
WiFiClient serverClients[MAX_SRV_CLIENTS];

boolean restoreConfig();
boolean checkConnection();
void startWebServer();
void setupMode();
//void ProcesarTess(unsigned int leidos);
void ProcesarTessFull(unsigned int leidos);
float HzToMag(float HzTSL );
void reconnect();
void CalculaMedias();
//void IniciaSD();
void IniciaRTC();
//void printDateTime(const RtcDateTime& dt);
void printDateTime();
boolean GuardaLog();
//void PrintMAC();
void GestionRed();
//void RefrescaOled();
void LeeBME280();
bool TestBME280();
void EnviaUdp();    
void ConfigDefault();
void RestoreZPs();
void AtiendeSerie();
void Colores();
void CadenaTess();
void CadJReadsV2();
void EnviaUdpV3();


//-----------------------------------------------------------
//-----calculos acelerometro---------------------------------
float roll, pitch, Acimut, Heading, Arect, Decl, HSLgrados;
float pitchM, CuentaRx_pitch, headingM;
float F_roll, F_pitch, F_Acimut, F_heading,  F_Arect, F_Decl, F_HSLgrados;
int mx, my, mz;
//int mxs, Pxs;
int RecAM; //recibidos del acelerometro magnetometro, al llegar mz debe ser 6
typedef struct vector
 {
   float x, y, z;
 } vector;

vector p = {0, 1, 0};  //segun la colocacion del sensor
                      //valores por defecto si no hay fichero
vector m_max = {500, 500, 500};
vector m_min = {-500, -500, -500};
vector m_avg = {0,0,0};
vector MagMax, MagMin;
bool HayManetometro;
vector a_max = {1024, 1024, 1024};
vector a_min = {-1024, -1024, -1024};
vector a_avg = {0,0,0};
vector AceMax, AceMin;


enum calibration_mode {CAL_NONE, CAL_X, CAL_Y, CAL_Z};
void vector_cross(const vector *a, const vector *b, vector *out);
float vector_dot(const vector *a, const vector *b);
void vector_normalize(vector *a);
int get_heading(const vector *a, const vector *m, const vector *p);
void CadJReads(void);

int readADC(void);
int Aina;
//---------------------------------------------------------------
//int BufComp;
bool SilencioWifi;

uint32_t readingsOPT;
//void OPT3001();
//void ReadOPT3001();


//--------------
void IRAM_ATTR handleInterrupt() {
  detachInterrupt(interruptPin);
  portENTER_CRITICAL_ISR(&mux);
  interruptCounter++;
    if ((Tius1 == 0)&&(Tius2==0)) {
      Tius1 = micros();
      attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
    }
    else if (Tius2 == 0 ) {
      Tius2 = micros() -Tius1;
      detachInterrupt(interruptPin);
    }
  portEXIT_CRITICAL_ISR(&mux);
}


void IRAM_ATTR handleInterruptG() {  //Luz Green
  detachInterrupt(interruptPinG);
  portENTER_CRITICAL_ISR(&mux);
  interruptCounterG++;
  if ((Tius1G == 0)&&(Tius2G ==0)) {
      Tius1G = micros();
      attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);
    }
    else if (Tius2G == 0 ) {
      Tius2G = micros() - Tius1G;
      detachInterrupt(interruptPinG);
    }
  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR handleInterruptR() {  //Luz Red
  detachInterrupt(interruptPinR);
  portENTER_CRITICAL_ISR(&mux);
  interruptCounterR++;
  if ((Tius1R == 0)&&(Tius2R ==0)) {
      Tius1R = micros();
      attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);
    }
    else if (Tius2R == 0 ) {
      Tius2R = micros() - Tius1R;
      detachInterrupt(interruptPinR);
    }
  portEXIT_CRITICAL_ISR(&mux);
}


void IRAM_ATTR handleInterruptB() {  //Luz Blue
  detachInterrupt(interruptPinB);
  portENTER_CRITICAL_ISR(&mux);
  interruptCounterB++;
  if ((Tius1B == 0)&&(Tius2B ==0)) {
      Tius1B = micros();
      attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);
    }
    else if (Tius2B == 0 ) {
      Tius2B = micros() - Tius1B;
      detachInterrupt(interruptPinB);
    }
  portEXIT_CRITICAL_ISR(&mux);
}


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup cxaused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

//-----------------------------------------------------------
void setup() {

  Serial.begin(9600);
  delay(10);
  Serial.println("  ");
  Serial.println("-----------------------------------------------");
  Serial.print( "Version: ");
  Serial.println(vernum);
  Serial.print("Compiled ");
  Serial.print(__DATE__);
  Serial.print("  ");
  Serial.println(__TIME__);
  Serial.println("-----------------------------------------------");  

  
 Serial.println(" WDT ON .............");
 //esp_task_wdt_init(segundosWDT, true); //5 segundos. Habilita panic para que esp32 reinicie
/*  esp_task_wdt_config_t twdt_config = {
    .timeout_ms = segundosWDT,          // equivalente a 10 s [web:1]
    .idle_core_mask = (1 << 2) - 1, // todos los cores en ESP32 dual core [web:1]
    .trigger_panic = true        // reinicia en caso de bloqueo [web:1]
 //   .subscribe_idle_task = true   // suscribe tareas idle si procede [web:1][web:5]
  };

  esp_task_wdt_deinit();   // opcional: desactiva WDT previo si lo hubiera [web:1]
  esp_task_wdt_init(&twdt_config); // ← ahora sí: pasas un puntero a config [web:1][web:5]
  vTaskDelay(10 / portTICK_PERIOD_MS);  // Pequeño delay para estabilizar
  esp_task_wdt_add(NULL); //añade thead a WDT
*/
  
  pinMode(HeatPin, OUTPUT);      // sets the digital pin as output
  digitalWrite(HeatPin,LOW);    
  pinMode(DonePin, OUTPUT);      // IO2 LED azul
//   digitalWrite(DonePin,LOW);   // LOW = apagado en D1 mini
//   delay(1000);
  digitalWrite(DonePin,HIGH);    // apagado
  delay(1000);
  digitalWrite(DonePin,LOW);
 
  
  pinMode(interruptPin, INPUT_PULLUP); //conf int canal 1
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  detachInterrupt(interruptPin);

  if (Ncanales > 0) { // !!!!! quitar los detach ?
    pinMode(interruptPinG, INPUT_PULLUP);
     attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);
    detachInterrupt(interruptPinG);
   
    pinMode(interruptPinR, INPUT_PULLUP);
     attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);
    detachInterrupt(interruptPinR);

    pinMode(interruptPinB, INPUT_PULLUP);
     attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);
    detachInterrupt(interruptPinB);
  }
 
  EEPROM.begin(512); 
  
 // Wire.begin(5,4); //Inicia puerto I2C(SCl,SDA):  SCL=PIO5=D1; SDA= PIO4=D2    
 //Wire.begin(4,5); // para TTGO con TFT color
 // Wire.begin(21,22);  //PCBs v2 con D1mini 32. Si se va a usar RTC usar siguiente O cruza cables i2c:
  Wire.begin(22,21);  //pcb v2 con D1mini 32 con RTC y SD. Implica intercambiar Dato y CLK en termopila.
     
  if ( restoreConfig() ) {   //Lee configuracion EEPROM
    Serial.println("Config Found OK ");
    Serial.println("  ");         
    EepromOK = 1;
  }     
  else {
    ConfigDefault();
    EepromOK = 0;
//    GestionRed();         //se configura el wifi
//    ESP8266Client = tname;
    settingMode = true;     
  }       
    
  IniciaRTC();
  
/*
  myHumidity.begin(); //Inicia SHT21
  delay(100);    
  humd21 = myHumidity.readHumidity();  // Prueba si hay sensor SHT21
 
  if ((humd21 < 99) && (humd21 > 0)) { // Humedad dentro de margen
         temp21 = myHumidity.readTemperature();
         HaySHT21 = 1;
         Serial.println("SHT21 found:");
         Serial.print("T amb. ");
         Serial.println(temp21);
         Serial.print("HR ");
         Serial.println(humd21);
  }
  else 
*/ HaySHT21 = 0;
//   Serial.print("Iniciando MLX, ");
  TAmbiente = mlx.readAmbientTempC();
  if ( TAmbiente > 100.0 ) {
    MLXok = 0;
    TAmbiente = 0;
    TObjeto = 0;   
    Serial.println("MLX no encontrada"); 
  }
  else {
    MLXok = 1;        
    Serial.print("MLX OK, Tamb: "); 
    Serial.println(TAmbiente); 
  }
       
  Serial.println("Iniciando BME280");
  TestBME280();

  Serial.println(" Entrada analogica: ");   
  AnalogA0 = readADC();//analogRead(Ain4); //comprueba si hay puente en J11
  Serial.printf("A0: %i Analog", AnalogA0); 

  if (BME280ok == 1)
    Serial.println(" BME280 Found OK");
  else {
    Serial.println(" BME280 NOT FOUND");        
/*    Serial.println(" Working mode:");    
    if(AnalogA0 < 100) { //lectura normal 480
      SilencioWifi = 1; //Puente puesto, Wifi apagada
      Serial.println("Jumper en J11, silencio Wifi. Datos solo por puerto serie");      
    }
    else  {
      SilencioWifi = 0;
      Serial.println("No Ain Jumper found.");      
    }
    */            
  }
 
    
//---borra config y limpia memoria-------------
/*  for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
  }
  EEPROM.commit();
//----------------------------
 */

  Serial.println(" ");
//  WiFi.mode(WIFI_STA);    
//  delay(1);             
//  PrintMAC();   //saca mac y la deja en String macID       

   if (HayRTC) { 
      IniciaSD32(); // solo para versiones de 1 canal. El uso de SD deshabilita el canal Green 
      //Increment boot number and print it every reboot
     ++bootCount;
     Serial.println("Boot number: " + String(bootCount));
     print_wakeup_reason();
     esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
     Serial.println("Setup ESP32 to sleep for  " + String(TIME_TO_SLEEP) + " Seconds");    
   }

  if (SDcardOK) { // Si hay tarjeta SD        
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
//    WiFi.SleepBegin();    //Desactiva completamente la wifi, <10mA
    delay(1);             
    Serial.println("--- Logger Mode -----");  
      //Increment boot number and print it every reboot
//    ++bootCount;
  //  Serial.println("Boot number: " + String(bootCount));
  //  print_wakeup_reason();
  
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for  " + String(TIME_TO_SLEEP) + " Seconds"); 
       
  }
  else if (SilencioWifi) {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
 //   WiFi.forceSleepBegin();    //Desactiva completamente la wifi, <10mA
    delay(1);             
    Serial.println("--- WiFi OFF Serial Mode ------");      
    pinMode(HeatPin, OUTPUT);      //Calentador, pin as output
    digitalWrite(HeatPin,LOW);    // que es desconfigurado por IniciaSD    
  }
  else {   // NO hay SD, se configura la red para funcionamiento normal
    WiFi.mode(WIFI_STA);    
    delay(1);             
   // PrintMAC();   //saca mac y la deja en String macID           
    macID = WiFi.macAddress();
    
 Serial.println(macID);
 /*
 hash = macID;
  hash.replace(":", ""); // Elimina los dos puntos de la dirección MAC
  Serial.println(hash);
  hash = hash.substring(9);
  Serial.println(hash);
  */
 /*  String lastTwoBytes = macID2.substring(9); // Obtiene los dos últimos bytes
  unsigned int lastTwoBytesInt = (int)strtol(lastTwoBytes.c_str(), NULL, 16); // Convierte los dos últimos bytes a entero
 unsigned int lastTwelveBits = lastTwoBytesInt & 0x0FFF; // Obtiene los últimos doce bits
  Serial.print("Last twelve bits of MAC address (hex): 0x");
  Serial.println(lastTwelveBits, HEX);
  String = 
*/   
    pinMode(HeatPin, OUTPUT);      //Calentador, pin as output
    digitalWrite(HeatPin,LOW);    // que es desconfigurado por IniciaSD
    
    Serial.println("--- WIFI Mode -------------------------------------");      
    if ( EepromOK ) {       //Si hay configuracion general
      GestionRed();         //se configura el wifi
      ESP8266Client = tname;
    }
    else {                                        //NO hay configuración
      Serial.println(" AP config mode");
      settingMode = true;     
      setupMode();            // Configura AP 
      Serial.println("-----------------------------------------------");        
    }
//    Udp.begin(localUdpPort);  //para escuchar, en principio no necesario
//    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);     
  } //fin de no hay SD 
 
/*Serial.println(" Inicio OPT3001");
  opt3001.begin();   
  Serial.println(" Lectura OPT3001");
  OPT3001();   
*/  
 /*
  TAmbiente = mlx.readAmbientTempC();
  if ( TAmbiente > 100.0 ) {
    MLXok = 0;
    TAmbiente = 0;
    TObjeto = 0;   
    Serial.println("MLX Fail"); 
  }
  else {
    MLXok = 1;    
    Serial.println("MLX OK"); 
  }
 */
  lastSearch =  millis(); //si esta como AP evita reinicio 
  now5 = millis();
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
  if (Ncanales > 0) {
    attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);
    attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);
    attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);
  }
  milisgAnt = 0;  

 //   digitalWrite(DonePin,LOW); //apaga LED Azul

  if( Ncanales == 0) {
    banda1 = "UVIR750" ;
  }
  else if( Ncanales == 1) {
    banda1 = "UVIR750" ;
    banda2 = "UVIR650" ;
    banda3 = "RGB-R" ;
    banda4 = "RGB-B" ;  
  }
  else if( Ncanales == 2) {
    banda1 = "UVIR750" ;
    banda2 = "Johnson-G" ;
    banda3 = "Johnson-R" ;
    banda4 = "Johnson-B" ;
  } 
    else if( Ncanales == 3) {
    banda1 = "UVIR750" ;
    banda2 = "RGB-G" ;
    banda3 = "RGB-R" ;
    banda4 = "RGB-B" ;
  } 


  digitalWrite(DonePin,HIGH);    // apagado
}
//--------------------------------------------------------------


//int i = 0;
//int last = millis();

//--------------------------------------------------------------
void loop() {
  bool jpreparado = 0;
//  char BufRx[TAMBUFF];
  char msg[100];
	char au[32];
  long now, now2, now3;
  uint8_t i;
  static int con, esperaWifi;
  static bool pio;



//  if (millis() - last >= 2000 && i > 5){
//  if (millis() - last >= 1000 ){
//    last =  millis();
//    i++;
  //  if ( i ==5) {
    //  Serial.println(" Stopping WDT reset, CPU should reboot in 3s  ");
   // }
//  }
//esp_task_wdt_add(NULL); 
//esp_task_wdt_reset();

  if  ( SDcardOK ) { //Si hay SD miro para guardar datos  
      now2 = millis();
      if (( (now2 - now5) >= SegEnvio*500 ) || CuentaRxFrecuencia>= 10) {  //Espera varias lecturas, con 1000 sale 1 minuto
        RecibidoTess = 0;
        if(BME280ok) 
          LeeBME280();  
        CadCsvReads();  //Hace CalculaMedias y crea cadena json 
              
        now5 = millis();
        if ( GuardaLog() ) {  
          Serial.print(" Log Done ");
    
        Serial.print(F_Frecuencia);
        Serial.print("  ");
        Serial.println(F_Magnitud);
          
  //        delay(100);      
        }
        else {
          Serial.println(" Log Fail");
        }
//      digitalWrite(DonePin,LOW);  //Para quitar alimentación con nanotimer
        digitalWrite(DonePin, LOW); // LED Azul ON
        delay(500);
        digitalWrite(DonePin,HIGH); // LED Azul OFF
        Serial.println("Going to sleep now");
        Serial.flush(); 
        esp_deep_sleep_start(); 
        Serial.println("This will never be printed");             
      }     //fin Espera     
      else {
        ;
      }
  }   
  else if (SilencioWifi == 0) {  //------- Funcionamiento NORMAL. No Hay SD ni puente en J11 ----------------
//   client.loop();  
    now2 = millis();  //cada 5 seg se comprueba si hay wifi por si se perdio
//      mqttcon = 0;
      if((settingMode==0)  && (now2 - lastMsg2 > 5000)) {    
        if(BME280ok) 
          LeeBME280();     
        lastMsg2 = now2;            
        if (modoAP == 0){
          if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconectando Wifi");
            if (checkConnection())  { //intenta conexión wifi
              HayWifi = true;
              EsperaConectar = 0;
            }  
            else {
              EsperaConectar++;
              HayWifi = false;
          //    mqttcon = 0;
              if (EsperaConectar > 4) {
                  Serial.println("-- HayWifi = 0, RESET --");
                  delay(100000); //Dispara WatchDog
              }
            }
          } 
          else {
             HayWifi = true;
           //  Serial.println("Recuperada Wifi");
          }
        }
      }
     
      if (settingMode) {
        ;
      }
      else if( (HayWifi) || (modoAP) )  //conectado como STA  o como AP
      {   
        if (server.hasClient()){        //check if there are any new clients telnet
          lastSearch =  millis();       //si esta como AP evita reinicios para buscar wifi      
          for(i = 0; i < MAX_SRV_CLIENTS; i++){
        //find free/disconnected spot
            if (!serverClients[i] || !serverClients[i].connected()){
              if(serverClients[i]) serverClients[i].stop();
                serverClients[i] = server.available();
                Serial.print("New client: ");  
                Serial.println(i);
                continue;
            }
          }
        //no free/disconnected spot so reject
          WiFiClient serverClient = server.available();
          serverClient.stop();
        }      
       
        for(i = 0; i < MAX_SRV_CLIENTS; i++) 
        {  //check clients for data
          if (serverClients[i] && serverClients[i].connected()){
            if(serverClients[i].available()){
            //get data from the telnet client and push it to the UART
              while(serverClients[i].available()) 
                Serial.write(serverClients[i].read());
            }
          }
        }
        if (MLXok) {
           if (( Magnitud > 10 ) && (TAmbiente < 10) || (TAmbiente < 5)){ // Control Heater
                digitalWrite(HeatPin, HIGH );
          }           
          else {
               digitalWrite(HeatPin,LOW);
          }    
        }
            
                //check UART for data
        if( BufCompleto) {        //Si rx mensaje completo tess, 
          BufCompleto = 0;                    
          for( i = 0; i < MAX_SRV_CLIENTS; i++){ //se envia por telnet si clietes
            if (serverClients[i] && serverClients[i].connected()){
              con++;
              if (con >2) {
                con = 0;
                if (mqttcon == 0)
                  CadTess = CadTess + "<WaitBrok>" ; 
              }
           // CadTess = report;            
            serverClients[i].write(jasonLocal.c_str(), jasonLocal.length());
           //   serverClients[i].write(report, strlen( report));
              delay(1);
              lastSearch =  millis(); //si esta como AP evita reinicios para buscar wifi      
            }
          }
         if (HayRTC) {
//            ahorita = Rtc.GetDateTime();
            printDateTime();  //deja fecha y hora en datestring        
          }
          if (T01sg == 5) {      //cada 0.5sg  
            EnviaUdp();           
          }
          else if (T01sg >=10) { //cada 1sg
            T01sg = 0;                 
            EnviaUdp();     
//          if(BufComp++ > 1) {
      //        Serial.println(jasonLocal);  // a serie cada 2 Udp
  //          BufComp = 0;
    //      }  
          }
        } //fin BufCompleto
          
        if (modoAP == false) {        // hay wifi 
          now = millis();
          if (now - lastMsg > 1000) { //cada segundo se comprueba si hay dato valido 
            lastMsg = now;
            T1sg++;    
            if (T1sg == 2)
              rssi = WiFi.RSSI();
            
            RecibidoTess = 1;  //??    

            if (HayRTC) T30sg = 20; //con RTC evita los envios de registro 
                                            
            if ((T30sg == 3) || (T30sg == 8) || (T30sg == 13) ) { //Al inicio, Envio mqtt para auto registrarse
              Serial.println("Mensaje de registro.");
             // rssi = WiFi.RSSI(); 

              if (Ncanales > 0) {                       //Cuatro canales
                  jasonStatus = "{\"name\":\"" + tname +"\""   
                  + ", \"rev\":" + revision                //Revision 3                 
                  + ",\"mac\":\"" + macID +"\"" 
                  + ", \"firmware\":" + "\"" + String( vernum ) + "\""
                  + ",\"date\":\"" + String(__DATE__) +"\""   
                  + ",\"F1\":{\"band\":" +  "\"" + banda1 + "\""
                  + ",\"calib\":"      + String(ZP1) +"}" ; 
               
                jasonStatus = jasonStatus 
                  + ",\"F2\":{\"band\":" + "\"" + banda2 + "\""
                  + ",\"calib\":" + String(ZP2) + "}"           
                  + ",\"F3\":{\"band\":"  + "\"" + banda3 + "\""
                  + ",\"calib\":" + String(ZP3 ) + "}"           
                  + ",\"F4\":{\"band\":"  + "\"" + banda4 + "\""          
                  + ",\"calib\":" + String( ZP4 ) + "}"  ;
                              
                jasonStatus = jasonStatus + "}"  ;                       
              }
              else {                                       //Un canal formato antiguo v2
                jasonStatus = "{\"name\":\"" + tname +"\""
                   + ", \"rev\":2"                        // revision 2 compatible
                   + ", \"firmware\":" + "\"" + String( vernum ) + "\""
                   + ",\"date\":\"" + String(__DATE__) +"\"" 
                   + ", \"mac\":\"" + macID +"\"" 
           //        + ", \"chan\":\"" + tess_channel +"\""
                   + ", \"calib\":" + String( ZP1 )
                   + ", \"offsethz\":" + String( OffsetHz )  
                   + ", \"wdBm\":" + String(rssi)
                   + ", \"hash\":\"" + hash +"\""                                      
                   + "}";              
              } 

              if ( mqttcon == 0) {
                Serial.println("Reconectando MQTT para registro.");
                reconnect(); 
              }             
              if ( mqttcon == 1) {  //si hay conexion mqtt 
                Serial.print("Reg: ");
                Serial.println( T30sg  );                             
                Serial.println(jasonStatus);                 
                client.publish(sttopic.c_str(), jasonStatus.c_str(), false);                      
                 Numero++;
              }
  
                  
            } //Fin cadena de registro
                   
//            if (HayRTC) SegEnvio = 10; // fuerza envio en 10 sg con RTC
            
  		      if(T1sg >= SegEnvio) {  // Envia cada n sg
      	      if ( RecibidoTess) {                      
          	    T1sg = 0;
        	      RecibidoTess = 0;                   	
        	      if (!client.connected())  //mantiene conexión mqtt
                  reconnect();
                                     
                CadJReads();                                                                    
        	      if ( mqttcon == 1) {  //si hay conexion mqtt             
          	      Numero++; 
          	      client.publish(rdtopic.c_str(), jasonReads.c_str(), true);
                  HayWifi = true;  //si ha enviado mqtt seguro que hay wifi
        	      } 
        	      else {
                  if (HayWifi)
          	        Serial.println("Broker not found"); 
        	      } 
      	      }
      	      else {  //no llegan mensajes del modulo TESS     
      		      CadTess = "<WaitTESS>";
      		      BufCompleto = 1;
      		      T1sg = 0;
                Serial.println(CadTess);
              // EnviaUdp(); 
      	      }
    	      
            //--------
              if (HayRTC) {  //SLEEP
                  digitalWrite(DonePin, LOW); // LED Azul ON
                  delay(500);
                  digitalWrite(DonePin,HIGH); // LED Azul OFF
                  Serial.println("Going to sleep now");
                  Serial.flush(); 
                  esp_deep_sleep_start(); 
                  Serial.println("This will never be printed");                            
              }
            //-------
            }  //Fin de if(T1sg >= SegEnvio)
            
            if (T30sg < 15) {  //Se envia 3 veces en el arranque.
              T30sg++;
              T1sg = 0;
            }     
          } //fin bucle de 1 sg
        }   //fin si hay wifi
        else {  //es AP y tratamos de buscar wifi
          now3 = millis();
          if ((now3 - now4) > 5000) {
            now4 = now3;
            Serial.println((now3 - lastSearch) / 1000);
             //-------   
                      
             //-------
          }           
          if (now3 - lastSearch > 300000) {  //si estoy como AP cada 5 min
            lastSearch = now3;
            //setup(); 
            GestionRed();
          }
          else if (now3 - lastSearch == 240000) {   // aviso de buscar wifi cada 5 min
            Serial.println("Searching Wifi in 60sg");
          }  
        } //fin es AP busco Wifi     
      }  //fin si hay Wifi o AP   
               
  } // FIN Funcionamiento NORMAL.  Sin SD  ni puente en J11 
  else if (SilencioWifi) { //------   
      now2 = millis();  
      if( (now2 - lastMsg2) > 1000) {    
        lastMsg2 = now2;            
        if( BufCompleto)        //Si rx mensaje completo tess, 
        {
          BufCompleto = 0;         
          CadJReads();  //Prepara cadena jsonReads 
          Serial.print(jasonReads); 
        } //fin BufCompleto
      }      
  }

  if (  (millis() - milisgAnt) > 1000) {  //Mira lecturas de interrupcion fotometro 1
    wait = 0;
    milisgAnt = millis();
      
 //   esp_task_wdt_reset();  //Reseting WDT......
     
    if (Tius2>0){
        WaitTi = 0;
        if (frec < 100000) {          
          frec = 1000000.0/ Tius2;  //frecuencia medida
          interruptCounter = 0;
          Tius2 = 0;
          Tius1 = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
        }  
        else {
          Tius2 = 0;
          FrecOver = 4; //inicia 4 sg de espera antes de reactivar ints
          Serial.println("Frec Over: ");
        }
     }
     else if (FrecOver == 0) {
        WaitTi++;             
        Serial.print(" Waiting ");  //frecuencia muy baja
        Serial.println(WaitTi);       
        wait = 1;
        if (WaitTi > LimInteg) {    //Por defecto 30sg
          WaitTi = 0;
          Serial.println(" Too low frecuency ");  //frecuencia muy baja
          detachInterrupt(interruptPin);
          frec = 0.0;
          interruptCounter = 0;
          Tius2 = 0;
          Tius1 = 0;        
          attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);        
        }
     }   
            
     if (FrecOver > 0) {             
        FrecOver--;
        if (FrecOver == 0) {
          //frec = 0;
          frec = 90000;
          interruptCounter = 0;
          Tius2 = 0;
          Tius1 = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);          
        }
     }     
   
     Frecuencia = frec;
      if (wait == 0) { 
          FrecuenciaM += Frecuencia;  
         CuentaRxFrecuencia++;     
      }
      
     Magnitud = ZP1 -2.5*log10(frec);
     if (Magnitud < 5) Magnitud = 0;
       else if (Magnitud > 25) Magnitud = 25;
     BufCompleto = 1;
                
 //   if (wait == 0)  //si frecuencia alta, quito int para refrescar
   //   detachInterrupt(interruptPin);
   
/*     if (frec > 0) { //Cuentas originales
       Frecuencia = frec;
       FrecuenciaM += Frecuencia;
       CuentaRxFrecuencia++;      
       Magnitud = ZP1 -2.5*log10(frec);
       BufCompleto = 1;
       if (Magnitud < 5) 
         Magnitud = 0;
       else if (Magnitud > 25) 
         Magnitud = 25;
       MagnitudM += Magnitud;
       CuentaRxMag++;
       frec = 0;
     }
     else     
       Serial.print("No f detect "); 
  */          
 /*    Serial.print(" -> ");
     Serial.print(Frecuencia);
     Serial.print("_");
     Serial.println(Magnitud);
 */     
      if (Ncanales > 0) {
        Colores();
      }
      
//CadenaTess();
float ta;
      if (MLXok){
        ta = mlx.readAmbientTempC();
        if ((ta < 70) && (ta > -70)) {
          TAmbiente = ta;
          TAmbienteM += TAmbiente;
          CuentaRx_tA++;          
        }
        ta = mlx.readObjectTempC();        
        if ((ta < 70) && (ta > -70)) {        
          TObjeto = ta;        
          TObjetoM += TObjeto;   
          CuentaRx_tO++;
        }
// Serial.print("MLX Tamb: "); 
// Serial.println(TAmbiente);         
      }
  
    // if (AccOK )
    
    //    LSM303();
        
     //EnviaBTLE(1);     
  //   DisplayDat();  
  //   delay(10);     
    
 //     if (FrecOver == 0)        //????? ya esta arriba contemplado ¿¿¿
   //     attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, FALLING);
    
      BufCompleto = true; 
 //   Serial.println("BufCompleto");
      RecibidoTess = 1;  //??        
  }  
  
   //-------------------------------------------------------------------------
  long ahora = millis();
  if ((ahora - lastRd) > 100) {  //read serial every 0.1 sg
    T01sg++;
 //   T1sg++;
    T5sg++;
    lastRd = ahora;
    AtiendeSerie();
    if  ( !SDcardOK) {
      webServer.handleClient(); //mantiene la pagina web  
    }
  }     
}  //-------FIN LOOP principal ----------


void Colores(void) {
    //-----------  Luz G  -------------------
    if (Tius2G > 0){  
        WaitTiG = 0;
        if (frecG < 100000) {
          frecG = 1000000.0/ Tius2G;
//          interruptCounterG = 0;
          Tius2G = 0;
          Tius1G = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);
        }  
        else {
          Tius2G = 0;
          FrecOverG = 4; //inicia 4 sg de espera antes de reactivar ints
           Serial.println("Frec Green Over: ");
        }
     }
     else if (FrecOverG == 0) {
       WaitTiG++;             
       Serial.print(" Wait Green ");  //frecuencia muy baja
       Serial.println(WaitTiG);
      // wait = 1;
       if (WaitTiG > LimInteg) {
         WaitTiG = 0;
         Serial.println(" Too low Green frecuency ");  //frecuencia muy baja
         detachInterrupt(interruptPinG);
         frecG = 0.0;
         interruptCounterG = 0;
         Tius2G = 0;
         Tius1G = 0;        
         attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);        
       }
     }     
     if (FrecOverG > 0) {        
        FrecOverG--;
        if (FrecOverG == 0) {
          //frec = 0;
          frecG = 90000;
          interruptCounterG = 0;
          Tius2G = 0;
          Tius1G = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinG), handleInterruptG, FALLING);          
        }
     }  
     frecMG = frecMG + frecG; 
     CounterG++;         
     MagnitudG = ZP2 -2.5*log10(frecG);
     if (MagnitudG < 5) MagnitudG = 0;
       else if (MagnitudG > 25) MagnitudG = 25;

//Serial.printf("Mag GREN:%f\n", MagnitudG);
BufCompleto = 1;
   //-----------  Luz R  -------------------  
    if (Tius2R > 0){  
        WaitTiR = 0;
        if (frecR < 100000) {
          frecR = 1000000.0/ Tius2R;
//          interruptCounterR = 0;
          Tius2R = 0;
          Tius1R = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);
        }  
        else {
          Tius2R = 0;
          FrecOverR = 4; //inicia 4 sg de espera antes de reactivar ints
           Serial.println("Frec Red Over: ");
        }
     }
     else if (FrecOverR == 0) {
       WaitTiR++;             
       Serial.print(" Wait Red ");  //frecuencia muy baja
       Serial.println(WaitTiR);
      // wait = 1;
       if (WaitTiR > LimInteg) {
         WaitTiR = 0;
         Serial.println(" Too low RED frecuency ");  //frecuencia muy baja
         detachInterrupt(interruptPinR);
         frecR = 0.0;
         interruptCounterR = 0;
         Tius2R = 0;
         Tius1R = 0;        
         attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);        
       }
     }     
     if (FrecOverR > 0) {        
        FrecOverR--;
        if (FrecOverR == 0) {
          //frec = 0;
          frecR = 90000;
          interruptCounterR = 0;
          Tius2R = 0;
          Tius1R = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinR), handleInterruptR, FALLING);          
        }
     }           
     frecMR = frecMR + frecR;  
     CounterR++;                 
     MagnitudR = ZP3 -2.5*log10(frecR);
     if (MagnitudR < 5) MagnitudR = 0;
       else if (MagnitudR > 25) MagnitudR = 25;
  //   Serial.printf("Mag RED:%f\n", MagnitudR);
       
   //-----------  Luz B  -------------------  
    if (Tius2B > 0){  
        WaitTiB = 0;
        if (frecB < 100000) {
          frecB = 1000000.0/ Tius2B;
//          interruptCounterB = 0;
          Tius2B = 0;
          Tius1B = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);
        }  
        else {
          Tius2B = 0;
          FrecOverB = 4; //inicia 4 sg de espera antes de reactivar ints
           Serial.println("Frec BLU Over: ");
        }
     }
     else if (FrecOverB == 0) {
       WaitTiB++;             
       Serial.print(" Wait BLU ");  //frecuencia muy baja
       Serial.println(WaitTiB);
      // wait = 1;
       if (WaitTiB > LimInteg) {
         WaitTiB = 0;
         Serial.println(" Too low BLU frecuency ");  //frecuencia muy baja
         detachInterrupt(interruptPinB);
         frecB = 0.0;
         interruptCounterB = 0;
         Tius2B = 0;
         Tius1B = 0;        
         attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);        
       }
     }     
     if (FrecOverB > 0) {        
        FrecOverB--;
        if (FrecOverB == 0) {
          //frec = 0;
          frecB = 90000;
 //         interruptCounterB = 0;
          Tius2B = 0;
          Tius1B = 0;          
          attachInterrupt(digitalPinToInterrupt(interruptPinB), handleInterruptB, FALLING);          
        }
     } 
     frecMB = frecMB + frecB;
     CounterB++;
     MagnitudB = ZP4 -2.5*log10(frecB);
     if (MagnitudB < 5) MagnitudB = 0;
       else if (MagnitudB > 25) MagnitudB = 25;

   //  Serial.printf("Mag BLU:%f\n", MagnitudB);
}

void AtiendeSerie(void) {
  int red;
  float ZPoint;

  while (Serial.available() > 0) {  //Cambio de ZP por usb
     char xx =  Serial.read();
     if (xx == 'C') {
       if (Serial.read() == 'I') {
      //Serial.print(red, HEX);
         red = Serial.parseInt();
         ZPoint = red/100.0;
         if (ZPoint >16 && ZPoint<24)  {
           Serial.print("New CI: ");
           Serial.println(ZPoint);
           cons = ZPoint;        
           for (int i = 0; i < cons.length(); ++i) {
             EEPROM.write(64 + i, cons[i]);
           }
           EEPROM.commit();
           Serial.println("Write EEPROM done!");
           ZP1 = ZPoint;
         }
         else{
           Serial.println("CI invalid value");
           Serial.println("Format: for CI = 20.21 you must enter CI2021. ");
           Serial.print("Actual CI: ");
           Serial.println(ZP1);
         }
       }
       else
         Serial.println("Invalid comand. ");
         // look for the newline. That's the end of your sentence:       
     }
     else if (xx == 'H') {
       pinMode(HeatPin, OUTPUT);      //Calentador, pin as output
        digitalWrite(HeatPin, HIGH );
        delay(2000);
        Serial.println(F("Heat HIG"));
     }
     else if (xx == 'L') {
       pinMode(HeatPin, OUTPUT);      //Calentador, pin as output
        digitalWrite(HeatPin, LOW );
        delay(2000);
        Serial.println(F("Heat LOW"));
     }
     else if (xx == 'M') {
       Serial.println("msg MQTT:");
        Serial.print("frecR:");
        Serial.print(frecR);
          Serial.print("  frecMR: ");
          Serial.print( frecMR);
            Serial.print("  CounterR: "); 
            Serial.print( CounterR);            
              Serial.print("  M/iC: ");
              Serial.println( frecMR / CounterR);
            
       CadJReads();
     }

     else if (xx == '?') {
        // = "TSP" + DirMacL4B + "i" + DirMac + "z" + String(ZP);
           Serial.println(" ");
          Serial.println("-----------------------------------------------");
          Serial.print("Compiled ");
          Serial.print(__DATE__);
          Serial.print("  ");
            Serial.println(__TIME__);             
           Serial.print("MAC: ");
           Serial.println(macID);           
//         Serial.print("TSP SN: ");
  //         Serial.print("TSP");
    //       Serial.println(DirMacL4B);           
           Serial.print("Actual CI: ");
           Serial.println(ZP1);                   
Serial.println("-----------------------------------------------");             
     }
  }
}




/*  
//---borra config y limpia memoria-------------
void EraseEEprom() {
for (int i = 0; i < EepromBuf; ++i) {
        EEPROM.write(i, 0);
  }
  EEPROM.commit();
}
//----------------------------
*/ 

//----------------------------------------------
boolean restoreConfig() {
  char auxi;
  
  Serial.println("Reading EEPROM...");

  ssid = "";
  pass = "";
  tname = "";
  cons = "";
  broker = "";
  Envio = "";
  Puerto = "";
  
//  if ((EEPROM.read(200) != 0) && (EEPROM.read(200) <124)){ //no esta limpia ni caracter raro 
  if ((EEPROM.read(48) != 0) && (EEPROM.read(48) <124)){ //no esta limpia ni caracter raro 
    //for (int i = 0; i < 16; ++i) {  // SSID  16B
    for (int i = 200; i < 232; ++i) {  // SSID  32B      
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)          
        ssid += auxi;
    }
    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    
    for (int i = 16; i < 48; ++i) {  // Password 32B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)          
        pass += auxi;
    }    
    Serial.print("Password: ");
    Serial.println(pass);
    if (pass.length() < 4)
    {
      Serial.println("Config error.");
      return false;
    } 

    for (int i = 48; i < 64; ++i) {  // Nombre 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        tname += auxi;
    }
    Serial.print("Name: ");
    Serial.println(tname);
    if (tname.length() < 5)
    {
      Serial.println("Config error.");
      return false;
    }
    rdtopic = "STARS4ALL/" + tname + "/reading";
    sttopic = "STARS4ALL/register" ;
    
    for (int i = 64; i < 80; ++i) {  // cons 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        cons += auxi;           
    }
    ConstI = cons.toInt();
    if (( ConstI < 0) || ( ConstI > 3))
       ConstI = 0;
     if (ConstI == 1  )
      Serial.println("Filter RGB.");
     else  if (ConstI == 2  )
      Serial.println("Filter Johnson.");
     else  if (ConstI == 0  ) 
      Serial.println("Single Chanel.");
     else 
      Serial.println("Filter unknow.");
     Ncanales = ConstI; 
    
//    for (int i = 80; i < EepromBuf; ++i) {  // Broker 32B
    for (int i = 80; i < 112; ++i) {  // Broker 32B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0) {
        broker += auxi;
        mqtb[i-80] = auxi;
   //   broker = "rich.dia.fi.upm.es"; 
      }
    }
    Serial.print("Broker: ");
    Serial.println(broker);
    if (broker.length() < 2)
    {
      Serial.println("Config error.");
      return false;
    }
    
    
    for (int i = 112; i < 128; ++i) {  // Seg. Envio 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        Envio += auxi;           
    }
    SegEnvio = Envio.toInt();
    if ((SegEnvio < 5) || (SegEnvio > 600)) {
      SegEnvio = 10;
      Envio = "10";
    }
    Serial.print("Seg.: ");
    Serial.println(Envio);

    for (int i = 128; i < 134; ++i) {  // Puerto Telenet 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        Puerto += auxi;           
    }
    TelnetPort = Puerto.toInt();
    if ((TelnetPort < 20) || (TelnetPort > 8080)) {
      TelnetPort = 26;
      Puerto = "26";
    }
    Serial.print("Port.: ");
    Serial.println(TelnetPort);

    RestoreZPs();

      // Offset mHz
    consOffset = "";
    for (int i = 170; i < 186; ++i) {  // cons 16B
      auxi = EEPROM.read(i);
      if (auxi > 124) auxi = 0;
      if (auxi != 0)
        consOffset += auxi;           
    }
    OffsetHz = consOffset.toFloat();
    if (( OffsetHz > 1) || ( OffsetHz  < -1))
       OffsetHz = 0;
    Serial.print("OffsetHz: ");
    Serial.println(OffsetHz);
    
    
    Serial.println("End Readig ...");
        
//    WiFi.begin(ssid.c_str(), pass.c_str());
    return true;
  }
  else {
    Serial.println("Config not found.");
    return false;
  }
}

void RestoreZPs() {   //------------------------CI 4 chanels ------------
 char auxi;
 String auxi2;
   
    for (int i = 134; i < 140; ++i) {  // Max X 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    ZP1 = auxi2.toFloat();
    auxi2 = "";
    if ((ZP1 < 10) || (ZP1 > 25)) {
      ZP1 = 20.5;
    }
    Serial.print("CI1: ");
    Serial.println(ZP1);
//------------------------
    for (int i = 140; i < 146; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    ZP2 = auxi2.toFloat();
    auxi2 = "";
    if ((ZP2 < 10) || (ZP2 > 25)) {
      ZP2 = 20.5;
    }
    Serial.print("CI2: ");
    Serial.println(ZP2);
//------------------------
    for (int i = 146; i < 152; ++i) {  // Max Z 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    ZP3 = auxi2.toFloat();
    auxi2 = "";
    if ((ZP3 < 10) || (ZP3 > 25)) {
      ZP3 = 20.5;
    }
    Serial.print("CI3: ");
    Serial.println(ZP3);
//------------------------------------------
    for (int i = 152; i < 158; ++i) {  // Max X 6 bytes
      auxi = EEPROM.read(i);
      if (auxi != 0)
        auxi2 += auxi;           
    }
    ZP4 = auxi2.toFloat();
    auxi2 = "";
  if ((ZP4 < 10) || (ZP4 > 25)) {
      ZP4 = 20.5;
    }
    Serial.print("CI4: ");
    Serial.println(ZP4);
}


void ConfigDefault(){
  
  Serial.println("Conf not found. Defaul values.");

  ssid = "unknow";
  pass = "1234";
  tname = "unconf";  
  cons = "20.5";
  ConstI = 0;
  broker = "broker.unconfig";
  Envio = "10";
  SegEnvio = 10;
  Puerto = "23";
  TelnetPort = 23;
  ZP1 = 20.50;
  ZP2 = 20.50;
  ZP3 = 20.50;
  ZP4 = 20.50;
   OffsetHz = 0;
      Ncanales =0;

    Serial.print("SSID: ");
    Serial.println(ssid.c_str());
    Serial.print("Password: ");
    Serial.println(pass);
    Serial.print("Name: ");
    Serial.println(tname);
    rdtopic = "STARS4ALL/" + tname + "/reading";
    sttopic = "STARS4ALL/register" ;
    Serial.print("Filter type: ");
    Serial.println(ConstI);

    Serial.print("Offset, nHz: ");
    Serial.println(OffsetHz);    

    Serial.print("Broker: ");
    Serial.println(broker);    
    Serial.print("Seg.: ");
    Serial.println(Envio);
    Serial.print("Port.: ");
    Serial.println(TelnetPort);
}

boolean checkConnection() {
  int count = 0;
 // esp_task_wdt_add(NULL); 
//  esp_task_wdt_reset();  //Reseting WDT......
  Serial.print("Waiting for Wi-Fi connection");
  while ( count < 30 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return (true);
    }
//    esp_task_wdt_reset();  //Reseting WDT......
    delay(500);
    Serial.print(".");
    count++;
  }
  Serial.println("Timed out.");
  return false;      
}

//---------------------------------------------------------------------------
void setupMode() {       //Se llama cuando no hay configuracion.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  delay(100);
  Serial.println("");
  for (int i = 0; i < n; ++i) {
    ssidList += "<option value=\"";
    ssidList += WiFi.SSID(i);
    ssidList += "\">";
    ssidList += WiFi.SSID(i);
    ssidList += "</option>";
  }
  delay(100);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(apSSID);
//  dnsServer.start(53, "*", apIP);
  startWebServer();

  if(HayWifi == 0){
    Serial.print("Starting AP at ");
    Serial.println(WiFi.softAPIP());
  }
  else 
  {
    Serial.print("Starting Web at \"");
    Serial.print(apSSID);   
  }
  Serial.println("\"");
}

String makePage(String title, String contents) {
  String s = "<!DOCTYPE html><html><head>";
  s += "<meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">";
//  s += "<META HTTP-EQUIV=\"Refresh\" Content= \"7\" >";
  s += "<title>";
  s += title;
  s += "</title></head><body>";
  s += contents;
  s += "</body></html>";
  return s;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

//--------Reconnect MQTT--------
void reconnect() {
  unsigned long t1;
 // esp_task_wdt_add(NULL); 
  
  if (broker.length() < 4 ) {
    Serial.print("Broker name too short.");
    return;  //si el nombre del broker es corto no se envía.
  }
      
//  esp_task_wdt_reset();  //Reseting WDT......
//  esp_task_wdt_init(30, true); //Alarga watchdog para que espera mas
//  esp_task_wdt_add(NULL); //añade thead a WDT
  
  Serial.println("Check connection mqtt.");
 
  if(HayWifi == 1 ){  
    if (!client.connected()) {
      t1 = millis();
      Serial.print("Conecting MQTT: ");
  //   Serial.print(mqtt_server); //Conecting MQTT: rich.dia.fi.upm.es   port: 1883
      Serial.print(mqtb);
      Serial.print("-port: ");
      Serial.println(mqtt_port);
    
      // Attempt to connect                  //broker  mosquito, sin pasword
//      if( (broker.indexOf("168") > 0) || (broker.indexOf("mosqui") > 0) ) {   
      if( broker.indexOf("mosqui") > 0) {  
        if (client.connect(ESP8266Client.c_str()) ) {  //como cliente se pone el nombre del sensor
          Serial.print("connected ");
          mqttcon = 1; 
        }         
      }
      else if( broker.indexOf("upm") > 0) {   //broker politecnica
        if (client.connect( ESP8266Client.c_str(), mqtt_user_upm, mqtt_password_upm)) {
          Serial.println("connected upm");
          mqttcon = 1; 
        }         
      }     
      else if( broker.indexOf("ucm") > 0) {  //broker complutense
      //  esp_task_wdt_reset();  //Reseting WDT......
         Serial.println("Conecting mqtt ucm");
//        if (client.connect("ESP8266Client", mqtt_user_ucm, mqtt_password_ucm)) {
        if (client.connect( ESP8266Client.c_str(), mqtt_user_ucm, mqtt_password_ucm)) {
          Serial.println("Connected UCM OK");
          mqttcon = 1; 
        }
        else {
          Serial.println("NO connected MQTT ucm");
          mqttcon = 0; 
        }
      } 
      else if( broker.indexOf("tess") > 0) {   //broker  con tess en la URL
        if (client.connect( ESP8266Client.c_str(), mqtt_user_ucm, mqtt_password_ucm)) {
          Serial.println("connected ucm");
          mqttcon = 1; 
        }
        else {
          Serial.println("NO connected MQTT");
          mqttcon = 0; 
        }                 
      }     
      else if( broker.indexOf("168") > 0) {   //broker  local
        if (client.connect( ESP8266Client.c_str(), mqtt_user_local, mqtt_password_local)) {
          Serial.println("connected to local mosquitto");
          mqttcon = 1; 
        }
        else {
          Serial.println("NO connected MQTT");
          mqttcon = 0; 
        }                  
      }
      else if (client.connect( ESP8266Client.c_str(), mqtt_user_ucm, mqtt_password_ucm)) {
          Serial.println("connected to external broker");
          mqttcon = 1; 
     }
     else {
          Serial.println("NO connected MQTT Broker unreached");
          mqttcon = 0; 
     }              

     if(mqttcon != 1)
     {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" Will try again in seconds");
      // Wait 5 seconds before retrying----> Se espera en loop principal
      // delay(5000);
        mqttcon = 0;
     }
     Serial.print("Reconecting Time ms: ");
     Serial.println( millis() - t1 );     
   }
   else 
   Serial.println("Client connected previusly "); 
  }
  else
   Serial.println("Esperando Wifi");
  //esp_task_wdt_init(segundosWDT, true); //5 segundos. Habilita panic para que esp32 reinicie
//  esp_task_wdt_reset();            // “alimentar” el WDT periódicamente
 // esp_task_wdt_add(NULL); //añade thead a WDT
}


//-----recibe Hz TSL y retorna magnitud visual -------------------
float HzToMag(float HzTSL )
{
  float mv, fHz;
   if ((HzTSL + OffsetHz) > 0)
     mv = ConstI - 2.5*log10(HzTSL + OffsetHz);
   else
     mv = 0;  
     
    return mv;
}
//---------------------------------------------------------------------------


//------------------------------------------------------
//<fm 12345><tA +2312><tO -0312><aX +2312><aY +2312><aZ +2312><mX +2312><mY +2312><mZ +2312>
//{"seq":10308, "name":TESS-Madrid-01, "freq":7384.00, "mag":11.29, "tamb":23.51, "tsky":23.53, "wifidBm":-70}

//-------Analiza cadena recibida----------------------


//------------------------------------------------------------
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}


//---------------------------------------------------
void IniciaSD32(){    
    if(!SD.begin()){
        Serial.println("-----Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        SDcardOK = false;
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    listDir(SD, "/", 0);   //print del listado de ficheros
 //   createDir(SD, "/mydir");
 //   listDir(SD, "/", 0);
/*    removeDir(SD, "/mydir");
    listDir(SD, "/", 2);
    writeFile(SD, "/hello.txt", "Hello ");
    appendFile(SD, "/hello.txt", "World!\n");
    readFile(SD, "/hello.txt");
    deleteFile(SD, "/foo.txt");
    renameFile(SD, "/hello.txt", "/foo.txt");
    readFile(SD, "/foo.txt");
    testFileIO(SD, "/test.txt");
*/    


    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    Serial.println(" ");
    SDcardOK = true;
}



//-----------------------------------------------------
void IniciaSD() {
  File root;
  String cab, fich;
 
  Serial.print("Initializing SD card...");  
  // see if the card is present and can be initialized:
  //if (!SD.begin(15)) {
  if (!SD.begin()) {    
    Serial.println("Card failed, or not present");
    SDcardOK = false;
    // don't do anything more:
    return;
  }
  SDcardOK = true;
  Serial.println("card initialized.");
  
  root = SD.open("/");
  printDirectory(root, 0);

  fich= "/";
  fich = tname;
  fich+= ".txt";
  //myFile = SD.open(fich.c_str(), FILE_WRITE);
 // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  
//  if (myFile) {
  if (SD.exists(fich.c_str()) == false){
   // tamano = myFile.size();    
//    if(myFile.size() < 10) {
       Serial.print("Writing header...");
//        pp = "TESS ID: " & Main.NumSerie & " Gain: " & Round2(Main.ConsIns, 2) & CRLF
//        comienzo = "Date " & TAB & "Time" & TAB & "T IR" & TAB & "T Amb" & TAB & "Mag V " & TAB & " Frecuencia"  & TAB & " Nubes " & TAB & "Latitud " & TAB & "Longitud" & TAB & "Altura" & CRLF
      cab = String("Log ID: ");
      cab+= "\t";      
      cab+= tname;
      cab+= "\tMAC: ";
      cab+= macID;
      cab+= "\tCI: ";
      cab+=  ConstI;
 //     myFile.println(cab);      
      Serial.print(cab);       
      cab= "\nDate \tTime \tLUX \tT Amb \tHR \tPres\n ";
      Serial.print(cab);
      //myFile.println(cab);
      //myFile.close();
       appendFile(SD, fich.c_str(), cab.c_str()) ;
      Serial.println("done.");
  //  }  
   
  }
  else {
    // if the file didn't open, print an error:
    Serial.println(" Fichero encontrado.");
  }
}



boolean GuardaLog()
{
   String dataStr = "";
   String fich;
   String cabecera;
   File root;

   printDateTime();  //deja fecha y hora en datestring 

   dataStr = datestring + jasonReads;

   fich= "/";
   fich+= tname;
   fich+= ".csv";
      
  if(SD.exists(fich.c_str())){
     Serial.println("Fichero encontrado ");
   }
  else {
     cabecera =  "# TESS Photometer. Name: " + tname + "  CI: " + String(ZP1) +  "\r\n# Date Time Seq  Frec  Mag Tsen TIR \r\n"  ;
     Serial.println("Fichero NO encontrado, añadiendo cabecera ");
     appendFile(SD, fich.c_str(),  cabecera.c_str() );
//     appendFile(SD, fich.c_str(),  "# Date Time Seq  Ain Tamb TIR Frec  Mag CI Tsen TIR \r\n" ) ;

     for (int i = 0; i < 10; i++){   //flaseo de creacion de fichero.
        digitalWrite(DonePin, LOW); // LED Azul ON
        delay(500);
        digitalWrite(DonePin,HIGH); // LED Azul OFF
     }   
  } 

  
  //   if(SD.exists(fich.c_str())){
  appendFile(SD, fich.c_str(), dataStr.c_str()) ;
  Serial.print("Dato Guardado en ");
  Serial.println(fich);
  
  return true;
}



//------------------------------------------------------------------
void IniciaRTC() 
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    uint8_t mday;
    uint8_t mon;
    uint16_t year;
    uint8_t wday;

    
    //--------RTC SETUP ------------
    Serial.println(" Iniciando RTC.");    
/*    while (!rtc.begin()) {
        Serial.println(F("RTC not found"));
        delay(3000);        
    }
*/
    if (rtc.begin() ) {
        Serial.println(F("RTC found OK"));        
    }
    else
    {
        Serial.println(F("RTC NOT found"));        
         HayRTC = 0;
         return;
        //exit(0);        
    }

 // Enable RTC clock
    if (!rtc.isRunning()) {
        rtc.clockEnable();
    }

  
    
    // Set square wave out pin
    // SquareWaveDisable, SquareWave1Hz, SquareWave4096Hz, SquareWave8192Hz, SquareWave32768Hz
    rtc.setSquareWave(SquareWaveDisable);
    // Get time from RTC
    if (!rtc.getDateTime(&hour, &min, &sec, &mday, &mon, &year, &wday)) {
        Serial.println(F("Get Datetime failed"));
        HayRTC = 0;
    } else {
        // Print time        
        
        Serial.println(F("RTC OK:  "));
        Serial.print(hour);
        Serial.print(F(":"));
        if (min < 10) {
            Serial.print(F("0"));
        }
        Serial.print(min);
        Serial.print(F(":"));
        if (sec < 10) {
            Serial.print(F("0"));
        }
        Serial.print(sec);
        Serial.print("  ");

//        Serial.print(F(":"));
        Serial.print(mday);

        Serial.print(F(":"));
        Serial.print(mon);

        Serial.print(F(":"));
        Serial.println(year);

        
        HayRTC = 1;
    }
}
/*
void IniciaRTC() 
{
    //--------RTC SETUP ------------
    Serial.println(" Iniciando RTC.");    
    Rtc.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);  //Mira fecha de compilacion para comparar
    printDateTime(compiled);
    Serial.println(" ");
    HayRTC = 0;
    if (!Rtc.IsDateTimeValid()) 
    {
        // Common Cuases:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
    //    Serial.println("RTC lost confidence in the DateTime!");
        // following line sets the RTC to the date & time this sketch was compiled
        // it will also reset the valid flag internally unless the Rtc device is
        // having an issue
        Rtc.SetDateTime(compiled);
    }

    if (!Rtc.GetIsRunning())
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }

    RtcDateTime now = Rtc.GetDateTime();
    
    if (now < compiled) 
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) 
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
         printDateTime(now);
         Serial.println(datestring);
         HayRTC = 1;
    }
    else if (now == compiled)     
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }

    // never assume the Rtc was last configured by you, so
    // just clear them to your needed state
    Rtc.SetSquareWavePin(DS1307SquareWaveOut_Low); 
    
    delay(200);
    now = Rtc.GetDateTime();
    if (!Rtc.IsDateTimeValid()) 
    {
        Serial.println("RTC MISING!");
         HayRTC = 0;
    }
}
*/
//-------------------------------------------------------------------

//-------------------------------------------------------------
#define countof(a) (sizeof(a) / sizeof(a[0]))
/*
void printDateTime(const RtcDateTime& dt)
{
//    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
  //  Serial.print(datestring);
}
*/

void printDateTime()
{
    struct tm dt;
    char buf[32];
char aux[50];
    // Read date/time from RTC
    if (!rtc.read(&dt)) {
        Serial.println(F("Error: Read date/time failed"));
        return;
    }
    
//    snprintf(buf, sizeof(buf), "%d %02d-%02d-%d %d:%02d:%02d",
  //           dt.tm_wday,
    snprintf(buf, sizeof(buf),"%02d/%02d/%d;%d:%02d:%02d;", 
             dt.tm_mday, dt.tm_mon + 1, dt.tm_year + 1900,
             dt.tm_hour, dt.tm_min, dt.tm_sec);
    Serial.println(buf);
    sprintf(datestring, buf);
}
//----------------------------------------------------------------------


void GestionRed()  //Se llama en Setup y cada 5 minutos si AP para buscar wifi
{
   int nn,i;
   String ak; 
   
//   esp_task_wdt_reset();
      Serial.print("STA MAC: ");
      macID = WiFi.macAddress();   
      Serial.println(WiFi.macAddress());    
      hash = macID;
  hash.replace(":", ""); // Elimina los dos puntos de la dirección MAC
  Serial.println(hash);
  hash = hash.substring(9);
  Serial.println(hash);
      Serial.println("--- Searching Wifi -------------------- "); 
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      delay(100);
      nn = WiFi.scanNetworks();
      delay(100);
      Serial.print("Num.Redes: "); 
      Serial.println(nn); 
//    setupMode(); 
      delay(100);
//----
//---   
//Buscamos si en la lista de redes esta la que buscamos    
      if(nn > 0)   {
        for (int i = 0; i < nn; ++i) {    //Crea lista de redes.
          ssidList += "<option value=\"";
          ssidList += WiFi.SSID(i);
          ssidList += "\">";
          ssidList += WiFi.SSID(i);
          ssidList += "</option>";
        }
        delay(100);
        ssidOk = 0;
        for ( i = 0; i < nn; i++) {
          Serial.print(i+1); 
          Serial.print(": "); 
          Serial.print(WiFi.SSID(i)); 
          Serial.print(" ("); 
          Serial.print(WiFi.RSSI(i)); 
          Serial.println(")"); 
          if( WiFi.SSID(i) == ssid ) {
             Serial.println("SIDD presente");
             ssidOk = 1;
          }         
        }
        if( ssidOk ) 
          ;//Serial.println("SIDD presente");
        else     
          Serial.println("SIDD NO presente");      
        delay(10); 
        EsperaEncontrarWifi = 0;         
      }  
      else { //No hay wifi alguna, se hara reinicio
        EsperaEncontrarWifi++;
        if (EsperaEncontrarWifi > 6) { // Aprox cada media hora hara Reinicio
          Serial.println("-- No encuentra Wifis, RESET --");
          delay(100000); //Dispara WatchDog
        } 
      }
//esp_task_wdt_reset();
      if(ssidOk) {                //el Punto Wifi está presente
   //     WiFi.config(IPfija, gateway, subnet);   //-----Para una IP FIJA- pero deja de funcionar UDP----
        WiFi.begin(ssid.c_str(), pass.c_str()); //trata de conectar
        settingMode = false;
        if (checkConnection()) {                    //Si hay conexion wifi 
          HayWifi = 1;   
          Serial.print("Topic MQ: ");
          Serial.println( rdtopic);
          Serial.print("Broker: ");
          Serial.println(broker);
          client.setServer(broker.c_str(), mqtt_port);    
           modoAP = 0;
           startWebServer();                           //crea las paginas 
           delay(100);
        }
        else {  
          HayWifi = false;
          modoAP = 1;          
          settingMode = false;
 //         rdtopic = "STARS4ALL/" + tname + "/reading";
          Serial.println("----- SSID present but not accesible. Going to  AP data mode -----");
   //   setupMode();            // Configura AP
          delay(100);
          WiFi.mode(WIFI_AP);
          modoAP = 1;
          WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
//          WiFi.softAP("TESS");
        ak = "TESS-"+ tname;
        WiFi.softAP(ak.c_str());

          delay(100);
          startWebServer();         
        }
//       esp_task_wdt_reset();
      }
      else {                                // NO hay Conexion con el wifi pero si config
        HayWifi = false;
        settingMode = false;
        
        Serial.println(" AP data mode ");
   //   setupMode();            // Configura AP
        delay(100);
        WiFi.mode(WIFI_AP);
        modoAP = 1;
        WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
        ak = "TESS-"+ tname;
        WiFi.softAP(ak.c_str());
        delay(100);
        startWebServer();
      }     
      server.begin();                             //arranca Telnet
      server.setNoDelay(true);
      Serial.println("Ready for telnet ");
      Serial.println("-----------------------------------------------");   
}


String AjustaD(float fc ) {
char aux[20];
 String raiz;
  if (Frecuencia < 10)
    sprintf(aux,"%05.3f", fc); 
  else if (Frecuencia < 100)
    sprintf(aux,"%5.2f", fc); 
  else   
    sprintf(aux,"%7.1f", fc);     
  raiz = String(aux);
  raiz.trim();
  return(raiz);
}


void EnviaUdp(void) {  //Para 4 canales
  String raiz, nuevaIP, sal;
  String F1,F2,F3,F4;
  unsigned int Ain0;
  
  Ain0 = readADC();
  Ainread++;
  if ( Ain0 > AnalogA0 ) // se queda el valor maximo para UDP
    AnalogA0 = Ain0;
 if ( AnalogA0 > Analog60sg ) // se queda el valor maximo para MQTT, se reinicia en el envío mqtt
    Analog60sg = AnalogA0;

 if (Ncanales == 0){
   CadenaUdpV3();
 //  return;
 }  
 else {  
 
  F1 = AjustaD(Frecuencia);
  F2 = AjustaD(frecG);
  F3 = AjustaD(frecR);
  F4 = AjustaD(frecB);

//  AnalogA0 = readADC();//analogRead(A0); 
  jasonLocal = "{\"udp\":" + String(NumeroUdp) 
               + ",\"rev\":"    + revision
               + ",\"name\":\"" + tname +"\""
               + ",\"wdBm\":"   + String(rssi)   
                + ", \"hash\":\"" + hash +"\""  
               + ",\"ain\":" + String(AnalogA0)                                                   
          
        //       +",\"F1\":{\"freq\":" + String( Frecuencia)       
               +",\"F1\":{\"freq\":" + F1       
               +",\"mag\":"     + String(Magnitud)                    
               +",\"zp\":"      + String(ZP1) +"}" ;  
         if (Ncanales > 0) {
              jasonLocal =  jasonLocal 
               + ",\"F2\":{\"freq\":" + F2
               + ",\"mag\":" + String( MagnitudG )                    
               + ",\"zp\":" + String(ZP2) +"}"  
         
               + ",\"F3\":{\"freq\":" + F3
               + ",\"mag\":" + String(MagnitudR )                    
               + ",\"zp\":" + String(ZP3 ) +"}"  
         
               + ",\"F4\":{\"freq\":" + F4
               + ",\"mag\":" + String(  MagnitudB)                    
               + ",\"zp\":" + String( ZP4 ) +"}"  ;
         }
/*               if( HayManetometro) {
                 jasonLocal = jasonLocal
                 + ", \"alt\":" + String(pitch)     
                 + ", \"azi\":" + String(Heading);                
               }
*/               
               if( MLXok) {
                 jasonLocal = jasonLocal
               + ",\"tamb\":"   + String(TAmbiente) 
               + ",\"tsky\":"   + String(TObjeto);                
               }
                                    
               if (BME280ok) {               
                 jasonLocal =  jasonLocal 
               
                 + ",\"text\":" + String(tempBME) 
                 + ",\"hrel\":" + String(humedad)                
                 + ",\"pres\":" + String(pressure);              
              //     + ", \"RTC\":\"" + datestring  +"\""                          
               }
               jasonLocal = jasonLocal + "}\r\n";     
  }

  if ( Ainread >=5 ) { //reset valor max de 5 lecturas para anemometro
 //   Serial.print(" Wind: ");
   // Serial.println(AnalogA0/11.1);
      Ainread = 0;
      AnalogA0 = 0;
  }  
             
  if (modoAP)    
    Udp.beginPacket("192.168.4.255", remoteUdpPort);
  else {
     raiz =  WiFi.localIP().toString();
     int posi = raiz.lastIndexOf(".");
     nuevaIP = raiz.substring(0,posi + 1);
     nuevaIP = nuevaIP + "255";         
     Udp.beginPacket(nuevaIP.c_str(), remoteUdpPort);          
  }    
//  Udp.write(jasonLocal.c_str());
  Udp.printf(jasonLocal.c_str());
  Udp.endPacket(); 
  NumeroUdp++;   
  Serial.println(jasonLocal.c_str());
}


void CadenaUdpV3(void) {  // Para 1 canal en formato v2 compatible
  String raiz, nuevaIP;

//   AnalogA0 = readADC();
  jasonLocal = "{\"udp\":" + String(NumeroUdp) 
             //  + ", \"rev\":" + rev_udp
               + ", \"rev\":2"              
               + ", \"name\":\"" + tname +"\""
               + ", \"freq\":" + String(Frecuencia)                    
               + ", \"mag\":" + String(Magnitud)                    
               + ", \"tamb\":" + String(TAmbiente) 
               + ", \"tsky\":" + String(TObjeto);
               if( HayManetometro) {
                 jasonLocal = jasonLocal
                 + ", \"alt\":" + String(pitch)     
                 + ", \"azi\":" + String(Heading);                
               }
               jasonLocal = jasonLocal                                   
               + ", \"wdBm\":" + String(rssi)
                + ", \"hash\":\"" + hash +"\""
               + ", \"ain\":" + String(AnalogA0)
               + ", \"ZP\":" + String(ZP1);
               if (BME280ok) {               
                 jasonLocal =  jasonLocal 
                 + ", \"text\":" + String(tempBME) 
                 + ", \"hrel\":" + String(humedad)                
                 + ", \"pres\":" + String(pressure);              
              //     + ", \"RTC\":\"" + datestring  +"\""                          
               }
               jasonLocal = jasonLocal + "}\r\n";     
 /*           
  if (modoAP)    
    Udp.beginPacket("192.168.4.255", remoteUdpPort);
  else {
     raiz =  WiFi.localIP().toString();
     int posi = raiz.lastIndexOf(".");
     nuevaIP = raiz.substring(0,posi + 1);
     nuevaIP = nuevaIP + "255";         
     Udp.beginPacket(nuevaIP.c_str(), remoteUdpPort);          
  }    
  Udp.printf(jasonLocal.c_str());
  Udp.endPacket(); 
  NumeroUdp++;   
  Serial.println(jasonLocal.c_str());
  */
  
}



void vector_cross(const vector *a, const vector *b, vector *out)
{
   out->x = a->y * b->z - a->z * b->y;
   out->y = a->z * b->x - a->x * b->z;
   out->z = a->x * b->y - a->y * b->x;
}

float vector_dot(const vector *a, const vector *b)
{
   return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vector_normalize(vector *a)
{
   float mag = sqrt(vector_dot(a, a));
   if(mag!=0){
     a->x /= mag;
     a->y /= mag;
     a->z /= mag;
   }
   else {
  // TESS->Memo1->Lines->Add("Error Acelerometro/Magnetometro");
     a->x = 0.1;
     a->y = 0.1;
     a->z = 0.1;
   }
}

//--------------------------------------------------------------------
// Returns a heading (in degrees)
// given an acceleration vector a due to gravity, a magnetic vector m, and a facing vector p.
//--------------------------------------------------------------------
int get_heading(const vector *a, const vector *m, const vector *p)
{
  vector E;
  vector N;
  float  k;

 // cross magnetic vector (magnetic north + inclination) with "down" (acceleration vector)
 //  to produce "east"
  vector_cross(m, a, &E);
  vector_normalize(&E);

 // cross "down" with "east" to produce "north" (parallel to the ground)
  vector_cross(a, &E, &N);
  vector_normalize(&N);

  //Calcula Roll
  k = pow(a->y, 2) + pow(a->z, 2);
  if(k != 0)
    ;
  else
    k = 0.0001;
  roll =  atan(a->x/sqrt(k));
  roll = roll*180/3.1416;

  //Calcula Altura
  k = pow(a->x, 2) + pow(a->z, 2);
  if( k!= 0)
    ;
  else
    k = 0.0001;
  pitch =  atan(a->y/sqrt(k));
  pitch = pitch*180/3.1416;

  //Calcula heading
  k = vector_dot(&N, p);
  if (k != 0)
    ;
  else {
  //  TESS->Memo1->Lines->Add("atan2");
    k = 0.00001; //proximo a cero
  }
  Heading = atan2(vector_dot(&E, p), k) * 180.0 / M_PI;
  if (Heading < 0)
     Heading += 360;

  //Presenta lecturas
 // mxs++;
/*  if (Pxs >= 4) {// si llegan mas de 4 mensajes por segundo, se promedian 4 lecturas
  // promedia_heading();
    Serial.print("Pxs ") ;
    Serial.println(Pxs);
  } 
*/  
/*
  TESS->PBroll->Position = (int)(roll);
  TESS->Label17->Caption = "Alabeo: " +  (AnsiString)(RoundTo(roll,-1));
  TESS->PBpitch->Position = (int)(pitch);
  TESS->PBheading->Position = (int)(heading*100/360);
*/

  return Heading;
}

//--------------------------------------------------------------

/*
void OPT3001(){
unsigned int readings = 0;
  
   readings = opt3001.readManufacturerId();  
  Serial.print("Manufacturer ID: "); 
  Serial.println(readings, BIN);

  // get device ID from OPT3001. Default = 0011000000000001
  readings = opt3001.readDeviceId();  
  Serial.print("Device ID: "); 
  Serial.println(readings, BIN);
  
  // read config register from OPT3001. Default = 1100110000010000
  readings = opt3001.readConfigReg();  
  Serial.print("Configuration Register: "); 
  Serial.println(readings, BIN);

  // read Low Limit register from OPT3001. Default = 0000000000000000
  readings = opt3001.readLowLimitReg();  
  Serial.print("Low Limit Register: "); 
  Serial.println(readings, BIN);
  
  // read High Limit register from OPT3001. Default = 1011111111111111
  readings = opt3001.readHighLimitReg();  
  Serial.print("High Limit Register: "); 
  Serial.println(readings, BIN);    
}

void ReadOPT3001(){    
  // Read OPT3001
  //readingsOPT = opt3001.readResult();  
  LUX =  opt3001.readResult();  
  
  Serial.print("LUX Readings = ");
//  Serial.println(readingsOPT);
  Serial.println(LUX);
}
*/

bool TestBME280()
{
  uint8_t chipID;
  static int esperoBME;
  Serial.println("BME280 MOD-1022 weather multi-sensor test.");
//  Serial.println("Embedded Adventures");
  chipID = BME280.readChipId();
  
  // find the chip ID out just for fun
  Serial.print("ChipID_BME = 0x");
  Serial.println(chipID, HEX);
  
  // need to read the NVM compensation parameters
  BME280.readCompensationParams();
  
  // Need to turn on 1x oversampling, default is os_skipped, which means it doesn't measure anything
  BME280.writeOversamplingPressure(os1x);  // 1x over sampling (ie, just one sample)
  BME280.writeOversamplingTemperature(os1x);
  BME280.writeOversamplingHumidity(os1x);
  
  // example of a forced sample.  After taking the measurement the chip goes back to sleep
  BME280.writeMode(smForced);
  esperoBME = 0;
  while (BME280.isMeasuring()) {
    Serial.print(".");
    delay(50);    
    esperoBME++;
    if (esperoBME >25) {      
      BME280ok = 0;
      return(0);      
    }  
    else 
      BME280ok = 1;
  }
  
  Serial.println("Done!");
  
  // read out the data - must do this before calling the getxxxxx routines
  BME280.readMeasurements();
  Serial.print("Temp=");
  Serial.println(BME280.getTemperature());  // must get temp first
  Serial.print("Humidity=");
  Serial.println(BME280.getHumidity());
  Serial.print("Pressure=");
  Serial.println(BME280.getPressure());
  Serial.print("PressureMoreAccurate=");
  Serial.println(BME280.getPressureMoreAccurate());  // use int64 calculcations
  Serial.print("TempMostAccurate=");
  Serial.println(BME280.getTemperatureMostAccurate());  // use double calculations
  Serial.print("HumidityMostAccurate=");
  Serial.println(BME280.getHumidityMostAccurate()); // use double calculations
  Serial.print("PressureMostAccurate=");
  Serial.println(BME280.getPressureMostAccurate()); // use double calculations
  // Example for "indoor navigation"
  // We'll switch into normal mode for regular automatic samples
  
  BME280.writeStandbyTime(tsb_0p5ms);        // tsb = 0.5ms
  BME280.writeFilterCoefficient(fc_16);      // IIR Filter coefficient 16
  BME280.writeOversamplingPressure(os16x);    // pressure x16
  BME280.writeOversamplingTemperature(os2x);  // temperature x2
  BME280.writeOversamplingHumidity(os1x);     // humidity x1
  
  BME280.writeMode(smNormal);
  return(1);
}

void LeeBME280() {
    while (BME280.isMeasuring()) {
//    Serial.println("Measuring...");
  //  delay(50);      
    }
    // read out the data - must do this before calling the getxxxxx routines
    BME280.readMeasurements();
    printCompensatedMeasurements();
}



float calc_dewpoint(float h,float t)
//--------------------------------------------------------------------
// calculates dew point
// input: humidity [%RH], temperature [C]
// output: dew point [C]
{   
  float logEx,dew_point;
  
  logEx = 0.66077 + 7.5*t/(237.3+t) + (log10(h)-2);
  dew_point = (logEx - 0.66077)*237.3/(0.66077+7.5-logEx);
  return dew_point;
}


void printFormattedFloat(float x, uint8_t precision) {
char buffer[10];
  dtostrf(x, 7, precision, buffer);
  Serial.print(buffer);
}


void printCompensatedMeasurements(void) {
//double tempMostAccurate, humidityMostAccurate, pressureMostAccurate;
//char buffer[80];
  tempBME      = BME280.getTemperature();
  tempBME = tempBME -1.0;
  humedad  = BME280.getHumidity();
  pressure  = BME280.getPressure();
  rocio = calc_dewpoint(tempBME, tempBME);
/*
  Serial.print("Roci  ");
  printFormattedFloat(rocio, 2);

  Serial.print("Temperature  ");
  printFormattedFloat(tempBME, 2);
  Serial.println();
  Serial.print("Pressure     ");
  printFormattedFloat(pressure, 2);
  Serial.println(" ");
  Serial.print("Humidity     ");
  printFormattedFloat(humedad, 2);
  Serial.println();
*/

/*  
  pressureMoreAccurate = BME280.getPressureMoreAccurate();  // t_fine already calculated from getTemperaure() above
  
  tempMostAccurate     = BME280.getTemperatureMostAccurate();
  humidityMostAccurate = BME280.getHumidityMostAccurate();
  pressureMostAccurate = BME280.getPressureMostAccurate();
  Serial.println("                Good  Better    Best");
  Serial.print("Temperature  ");
  printFormattedFloat(tempBME, 2);
  Serial.print("         ");
  printFormattedFloat(tempMostAccurate, 2);
  Serial.println();
  
  Serial.print("Humidity     ");
  printFormattedFloat(humedad, 2);
  Serial.print("         ");
  printFormattedFloat(humidityMostAccurate, 2);
  Serial.println();

  Serial.print("Pressure     ");
  printFormattedFloat(pressure, 2);
  Serial.print(" ");
  printFormattedFloat(pressureMoreAccurate, 2);
  Serial.print(" ");
  printFormattedFloat(pressureMostAccurate, 2);
  Serial.println();
  */
}

//---------------
void CadJReads(void){  //Prepara cadena jsonReads para MQTT
  String F1,F2,F3,F4;

  if (Ncanales == 0)
    CadJReadsV2();   //Cadena para 1 canal
  else {  
    CalculaMedias();             
    F1 = AjustaD(F_Frecuencia);
    F2 = AjustaD(F_frecMG);
    F3 = AjustaD(F_frecMR);
    F4 = AjustaD(F_frecMB);  
    
    
    jasonReads = "{\"seq\":" + String(Numero) 
               + ",\"rev\":"    + revision
               + ",\"name\":\"" + tname +"\""
               + ",\"wdBm\":"   + String(rssi)  
               + ",\"hash\":\"" + hash +"\""     
               + ",\"ain\":" + String(Analog60sg)  
                                 
               +",\"F1\":{\"freq\":" +  F1 // String( F_Frecuencia)       
               +",\"mag\":"     + String(F_Magnitud)                    
               +",\"zp\":"      + String(ZP1) +"}";  
         if (Ncanales > 0) {
             jasonReads = jasonReads 
               + ",\"F2\":{\"freq\":" + F2// String( frecR )        
               + ",\"mag\":" + String( MagnitudG )                    
               + ",\"zp\":" + String(ZP2) +"}"  
         
               + ",\"F3\":{\"freq\":" + F3  // String( frecG )        
               + ",\"mag\":" + String(MagnitudR )                    
               + ",\"zp\":" + String(ZP3 ) +"}"  
         
               + ",\"F4\":{\"freq\":" + F4 //String( frecB )        
               + ",\"mag\":" + String(  MagnitudB)                    
               + ",\"zp\":" + String( ZP4 ) +"}"  ;
         }
         if( MLXok) {
              jasonReads = jasonReads
               + ",\"tamb\":"   + String(F_TAmbiente) 
               + ",\"tsky\":"   + String(F_TObjeto);                
         }       
       
         if( HayManetometro) {
           jasonReads = jasonReads
           + ",\"alt\":" + String(pitch)     
           + ",\"azi\":" + String(Heading);                
         }                               
         if (BME280ok) {                
           jasonReads =  jasonReads 
           + ",\"text\":" + String(tempBME) 
           + ",\"hrel\":" + String(humedad)                
           + ",\"pres\":" + String(pressure);                  
         }                                    
         jasonReads =  jasonReads + "}\r\n";     
   }
   Analog60sg = 0; //reset del valor max analogico, anemometro extraido en udp
   Serial.print(jasonReads.c_str());  
}


//---------------
void CadJReadsV2(void){  //Prepara cadena jsonReads para MQTT
  if (HayRTC)
    Numero = bootCount; //envia n rebinicios
  CalculaMedias();             
  jasonReads = "{\"seq\":" + String(Numero) 
//       + ", \"rev\":" + revision
       + ", \"rev\":2"
       + ", \"name\":\"" + tname +"\""
       + ", \"freq\":" + String(F_Frecuencia)                    
       + ", \"mag\":" + String(F_Magnitud)                    
       + ", \"tamb\":" + String(F_TAmbiente) 
       + ", \"tsky\":" + String(F_TObjeto) 
       + ", \"wdBm\":" + String(rssi)
       + ", \"hash\":\"" + hash +"\""
       ;     
       if( HayManetometro) {
         jasonReads = jasonReads
         + ", \"alt\":" + String(pitch)     
         + ", \"azi\":" + String(Heading);                
       }                               
       if (BME280ok) {                
          jasonReads =  jasonReads 
          + ", \"ain\":" + String(Analog60sg)
          + ", \"text\":" + String(tempBME) 
          + ", \"hrel\":" + String(humedad)                
          + ", \"pres\":" + String(pressure);                  
       }                                    
     jasonReads =  jasonReads + "}\r\n"; 
}


//-----------------------------------------------------------------------------------
void CadCsvReads(void){  //Prepara cadena jasonReads csv para SD 
//  AnalogA0 = analogRead(A0); 
  CalculaMedias();       
  jasonReads =  String( bootCount) + ";" +  String( F_Frecuencia)  + ";" 
              + String( F_Magnitud)  + ";" + String( F_TAmbiente) + ";" + String( F_TObjeto)  + "\r\n"; 
   Serial.print("cadcsv:  ");
   Serial.println(jasonReads.c_str());  
}


void CadenaTess(void){ //Crea cadena formato TESS original. Legacy
  float hz;
    hz = frec;
   //<fm 04970><mZ -0000>
   if (hz > 50.0)
//     snprintf(report, sizeof(report), "<fH %00005d><tA %00005d><tO %00005d>", 
//              (int)hz,(int)(TAmbiente*100.0),(int)(F_TObjeto*100.0));
     snprintf(report, sizeof(report), "<fH %00005d><fHR %00005d><fHG %00005d><fHB %00005d><tA %00005d><tO %00005d>", 
              (int)hz, (int)frecR,(int)frecG,(int)frecB,  (int)(TAmbiente*100.0),(int)(F_TObjeto*100.0));

   else 
//     snprintf(report, sizeof(report), "<fm %00005d><tA %00005d><tO %00005d>", 
  //            (int)(hz*1000.0),(int)(TAmbiente*100.0),(int)(F_TObjeto*100.0));
       snprintf(report, sizeof(report), "<fm %00005d> <fmR %00005d><fm %00005d><fm %00005d> <tA %00005d><tO %00005d>", 
              (int)(hz*1000.0), (int)(frecR*1000.0),(int)(frecG*1000.0),(int)(frecB*1000.0), (int)(TAmbiente*100.0),(int)(F_TObjeto*100.0));

    Serial.print(report);
 /*   if (AccOK ) {
      snprintf(report, sizeof(report), "<aX %00005d><aY %00005d><aZ %00005d>", acx,acy,acz);
      Serial.print(report);
      snprintf(report, sizeof(report), "<mX %00005d><mY %00005d><mZ %00005d>", mgx,mgy,mgz);
      Serial.println(report);
    }
    else   
    */    
    Serial.println("<mZ 00000>");
}


int readADC()  //GPIO34 adc canal 6, igual que en TAS y TESS-P  1.57v dan 914
{              //0,455v 186 cuentas en TESS-W4C
/*  adc1_config_width(ADC_WIDTH_BIT_11);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_6); //ADC_ATTEN_DB_11 = 0-1,8v
  Aina = adc1_get_raw(ADC1_CHANNEL_0);
  return Aina;  //GPIO36
*/
//  adc1_config_width(ADC_WIDTH_BIT_12);
//  adc1_config_channel_atten(ADC_CHANNEL_6, ADC_ATTEN_DB_11); //ADC_ATTEN_DB_11 = 0-1,8v
//  Aina = adc1_get_raw(ADC_CHANNEL_6);
//const int pinLDR = 34;      // GPIO34 

analogSetPinAttenuation(34, ADC_11db);
Aina = analogRead(34);
 // return (Aina*1570)/914; //en milivoltios   
  return (Aina*455)/186; //en milivoltios con TESS-W4C divisor 12k 1k2
 //return (Aina); //en milivoltios  
}



bool getBuildTime(const char *str)
{
    int hour;
    int minute;
    int second;

    // Convert build time macro to time
    if (sscanf(str, "%d:%d:%d", &hour, &minute, &second) != 3) {
        return false;
    }

    dt.tm_hour = hour;
    dt.tm_min = minute;
    dt.tm_sec = second;

    return true;
}

bool getBuildDate(const char *str)
{
    const char *monthName[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    uint8_t monthIndex;
    char month[12];
    int dayMonth;
    int year;
    time_t t;

    // Convert build time macro to date
    if (sscanf(str, "%s %d %d", month, &dayMonth, &year) != 3) {
        return false;
    }

    // Convert month string to month number
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(month, monthName[monthIndex]) == 0) {
            break;
        }
    }
    if (monthIndex >= 12) {
        // Not found
        return false;
    }

    // Set date
    dt.tm_mday = dayMonth;
    dt.tm_mon = monthIndex;
    dt.tm_year = year - 1900;

    // Calculate day of the week
    t = mktime(&dt);
    dt.tm_wday = localtime(&t)->tm_wday;

    return true;
}

bool rtcSetDateTime()
{
    // Convert compile date/time to date/time string
    if (!getBuildDate(__DATE__) || !getBuildTime(__TIME__)) {
        Serial.print(F("Build date/time error"));
        return false;
    }

    // Print build date/time
    Serial.print(F("Build date time: "));
    Serial.println(asctime(&dt));

    // Set new date time
    Serial.print(F("Set RTC date time..."));
    if (!rtc.write(&dt)) {
        Serial.println(F("FAILED"));
    } else {
        Serial.println(F("OK"));
    }

    return true;
}


/*
bool getBuildDate(const char *str)
{
    const char *monthName[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    uint8_t monthIndex;
    char month[12];
    int dayMonth;
    int year;
    time_t t;

    // Convert build time macro to date
    if (sscanf(str, "%s %d %d", month, &dayMonth, &year) != 3) {
        return false;
    }

    // Convert month string to month number
    for (monthIndex = 0; monthIndex < 12; monthIndex++) {
        if (strcmp(month, monthName[monthIndex]) == 0) {
            break;
        }
    }
    if (monthIndex >= 12) {
        // Not found
        return false;
    }

    // Set date
    dt.tm_mday = dayMonth;
    dt.tm_mon = monthIndex;
    dt.tm_year = year - 1900;

    // Calculate day of the week
    t = mktime(&dt);
    dt.tm_wday = localtime(&t)->tm_wday;

    return true;
}
*/


//-----------------------------------------------------------------------
void CalculaMedias(void){     //Calculos de medias para enviar por MQTT

  Serial.print("Lecturas promediadas: ");
  Serial.println(CuentaRxFrecuencia);
  //---------------------F1-------------------
  if (CuentaRxFrecuencia > 0) {
               F_Frecuencia = FrecuenciaM/CuentaRxFrecuencia;
               F_Magnitud = ZP1 -2.5*log10(F_Frecuencia);                           
  }             
   else {    
      F_Frecuencia = 0;
      F_Magnitud = 0;
  }    
  FrecuenciaM = 0;
  CuentaRxFrecuencia = 0;   
//-----------------------F2-----------------------
  if (interruptCounterR > 0) {
      F_frecMR =  frecMR / CounterR;
      F_MagnitudR = ZP2 - 2.5*log10(F_frecMR);
      frecMR = 0;
      CounterR = 0;
      interruptCounterR = 0;                 
  }               
//----------------------F3------------------------
  if (interruptCounterG > 0) {
      F_frecMG = frecMG/CounterG;
      F_MagnitudG = ZP3 - 2.5*log10(F_frecMG);
      frecMG = 0;
      CounterG = 0;
      interruptCounterG = 0;                 
  }               
//----------------------F4------------------------  
  if (interruptCounterB > 0) {
      F_frecMB = frecMB/CounterB;
      F_MagnitudB = ZP4 - 2.5*log10(F_frecMB);
      frecMB = 0;
      CounterB = 0;
      interruptCounterB = 0;                 
  }               
//------------------------------------------------
             
  MagnitudM = 0; CuentaRxMag = 0;
  
  if (CuentaRx_tA > 0)  
              F_TAmbiente = TAmbienteM /CuentaRx_tA;
            else F_TAmbiente = 0;
  TAmbienteM = 0; CuentaRx_tA = 0;
  if ( CuentaRx_tO > 0)  
              F_TObjeto = TObjetoM /CuentaRx_tO;             
            else F_TObjeto = 0;
  TObjetoM = 0; CuentaRx_tO = 0; 
}
