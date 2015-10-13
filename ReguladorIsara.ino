/**  usando EEPROM 24LC512  **/   
/**  usando ESP8266 WIFI    **/
#define miversion 1
#define misubver 0
#define VERMEM 1
#define ARDUINO 103

#define ES        // idioma EN:english,  ES: español
#define LOG 0
#define WEBDUINO_SERIAL_DEBUGGING 0
#define WEBDUINO_FAIL_MESSAGE "<h1>FATAL ERROR</h1>"
#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
#include "EEPROM.h"
#include "EEPROMAnything.h"
#include "LiquidCrystal.h"
#include "Wire.h"
#include "DS1307RTC.h"
#include "Time.h"
#include "Base64.h"
#include "OneWire.h"
#include "Mudbus.h"
#include "DallasTemperature.h"
#include "commontexts.h"
#include "idiomas.h"
#include <avr/wdt.h>re
#include <Keypad.h>

volatile int ninter=0;

#define inact 255          // valor para programación, significa inactivo
#define despMail 251       // revalor para enviar mails, de 90 a 92
#define delayACKMail 2000   // espera a respuesta de servidor smtp despuÃ©s de cada comando
/* NO USAR LOS PINES: 53, 52, 51, 50, 10,4   Ethernet shield  */
#define spiMISO 50
#define spiMOSI 51
#define spiSCK 52
#define ethSEL 10
#define sdSEL 4
#define rstFab 42    //  Pin para reset fábrica

/* 49, 47, 45, 35, 33, 31, 29   LCD  */
#define lcdRS 49
#define lcdRW 47
#define lcdEN 45
//#define lcdEN2 18    // sólo para Powertip PC4004, se quita una Ent. digital
#define lcdD0 35
#define lcdD1 33
#define lcdD2 31
#define lcdD3 29
// LiquidCrystal lcd(lcdRS,lcdRW,lcdEN,lcdD0,lcdD1,lcdD2,lcdD3); 

#define owPin1 43    // pin para 1-wire, puerto 1

#define lTAB 8       // en 8 bytes se guardan variables de 64 bits
#define nPINES 54    // varía según la placa, 54 para Arduino MEGA
#define nANALO 16    // número máximo de entradas analÃ³gicas
#define nDIG 34      // número máximo de entradas/salidas digitales
#define nPRG 32      // número de programaciones
#define nsalporesc 86  // número de salidas por escena
#define nPRGsem 5    // programas a los que se pueden asociar los programas semanales
#define nPRGeve 5    // programas a los que se pueden asociar los programas de eventos
#define LEN5 5
#define LEN20 20
#define LEN40 40
#define LEN80 80
#define maxTemp 20     // máximo de sondas 1-wire
#define baseAna 80

// definición posiciones EEPROM
// hueco desde 0 hasta 50
#define dirEEmodo 50       // 1 byte, modo de presentación web, 0:Avanzado, 1:Básico, 2:Local, 3:Domótica,4:Riego
#define dirEEevenTemp 51   // 40
#define dirEEprectemp 91   // 1 byte, precisión sondas ds18b20: 9,10,11,12
#define dirEEsdHay 99      // 1
#define dirEEmac 100       // 6 MAC
#define dirEEip 106        // 4 IP
#define dirEEmask 110      // 4 Mask
#define dirEEgw 114        // Gateway
#define dirEEdns 118       // DNS
#define dirEEwebport 122   // 2 Port web
#define dirEEpass 124      // 20 password
#define dirEEactprg 144    // 5 bytes guarda que programas están activos
#define dirEEuser 149      // 20 usuario
// hueco desde 173 hasta 182  // 14 bytes
#define dirEEcodigoAM 170    // 8 bytes, código antimoroso 8 dígitos
#define dirEEarranquesAM 180  // 2 bytes, número de arranques anti moroso
#define dirEEAMactivo 182    // 1 byte dispositivo Anti Moros activado 
#define dirEEESTADO 183    // estado (parado o no)
#define dirEEusepass 184   // 1 activar password
#define dirEEshowTemp 185  // 3 bytes, 20 temperaturas (cabrían 24=8x3)
#define dirEEfecval 188     // 8
#define dirEEpmactVar 196    // 4 bytes
#define dirEEnumIA 200     // 1   
#define dirEEnumID 201     // 1
#define dirEEnumOD 202     // 1
#define dirEEshown 203     // 1 mostrar número de pin entre parÃ©ntesis
#define dirEEperact 204    // 1 período de actualizaciÃ³n en segundos de pantalla 
#define dirEEestado 351     // 32 bytes, se guarda el estado de 64 pines locales y 192 remotos
// hueco desde 384 hasta 385 (2 bytes)
#define dirEEdescpinD 410   // descripción de pines locales 34=680
#define dirEEdescpinA 1090   // descripción de pines locales 16x20=320
#define dirEEfecdia 1470     // 32
// hueco desde 1502 hasta 1505  4 bytes
//hueco desde 1746 hasta 1809  // 
#define dirEEevenvalD 1810   // 64 bytes
#define dirEEbprgtipo 1882   // 4 bytes, tipo señal salida en prg semana
#define dirEEbeveacttipo 1886   // 4 bytes, tipo señal activadora en prg evento
#define dirEEbevesaltipo 1890   // 4 bytes, tipo señal salida en prg evento
#define dirEEtipoLCD 1894       // 1 byte
#define dirEEtipoED 1895        // 16 bytes tipo de entrada digital
// hueco desde 1911 hasta 1958, 49 bytes
#define dirEEfecmin 1959    // 32  fecha, minuto   
#define dirEEevenact 1991   // 32 señales activadoras del evento
#define dirEEevensal 2023   // 32 bytes, pin sobre el que se actÃºa en eventos
#define dirEEfecsal 2055    // 32
#define dirEEbfectipo 2087   // 4 bytes, tipo señal salida en prg fecha
// hueco desde 2091 hasta 2092, 2 bytes
#define dirEEevenniv 2093   // 8 bytes, se guarda bevenniv, nivel de activaciÃ³n del evento
// hueco desde 2105 hasta 2108, 4 bytes
#define dirEEredNodo 2109   // 120, red a la que pertenece cada nodo
#define dirEEdescTemp 2229  // 400, 20x20 descripción de las sondas 1-wire
#define dirEEbshowpin 2629  // 8 bytes, Panel Principal locales
// hueco desde 2637 hasta 2639
#define dirEEbactsem 2664      // 4 bytes programa semanal activo
#define dirEEbacteve 2668      // 4 bytes programa eventos activo
#define dirEEbactfec 2672      // 4 bytes programa fechas activo
#define dirEEbshowEsc 2676       // 1 bytes
// hueco desde 2677 hasta 2682 (6 bytes)
#define dirEEprgsal 2683     // 32   
#define dirEEfecano 2715     // 32
#define dirEEprgval 2747     // 64  
#define dirEEprgdia 2811     // 32 
#define dirEEprghor 2843     // 32
#define dirEEprgmin 2875     // 32
#define dirEEprgcon 2907     // 32
#define dirEEprgconv 2939    // 64 bytes
#define dirEEprgcomp 3003    // 32
#define dirEEfechor 3355     // 32
#define dirEEmailACT 3387    // 1 activar envío de correo
#define dirEEmailSVR 3388    // 40 servidor de correo
#define dirEEmailPER 3428    // 1 período de envío de correo en minutos
#define dirEEmailPORT 3429   // 2 puerto del servidor de correo
#define dirEEmailUSER 3431   // 40 usuario correo
#define dirEEmailPASS 3471   // 40 password correo
#define dirEEmailSENDER 3511 // 40 remitente correo
#define dirEEmailDEST 3551    // 120: 3 x 40, destinatarios correo
#define dirEEevencomp 3671   // 32 comparador eventos analógicos
#define dirEEcontadores 3703  // 32 8 contadores de 4 bytes(long) entradas digitales
#define dirEEfecmes 3735     // 32
#define dirEEcode433Off 3767  // 4 x 30=120 byter
// hueco desde 3887 hasta 4066, 180 bytes.
// hueco desde 4067 hasta 4095, 28 bytes
#define dirEEfactorA 7776      // 16 x 4 = 64 bytes, factor conversion analógicas locales
#define dirEEdescEsc 7840      // 5 x 20 = 100, descripciones escenas
#define dirEEesc 7940          // 5 escenas x 11 = 55 bytes, escenas 
#define dirEEPRGsem 7999    // 20 bytes
#define dirEEPRGeve 8019    // 20 bytes
#define dirEEdyndnsHost 8039   // 40 bytes
#define dirEEdyndnsUser 8079   // 40 bytes
#define dirEEdyndnsPass 8119   // 40 bytes
// hueco desdespe 8119 hasta 8191, 73 bytes
#define dirEEevenvalA 10000  // 128, 32x4, valor de activación para analógicas
// hueco desde 10128 hasta 10255, 128 bytes  // Reservados ampliación evenval 
#define dirEEevenhis 10256   // 128  32x4, valor de histéresis de activación del evento
// hueco desde 10384 hasta 10511, 128 bytes  // Reservados ampliación evenhis 
#define dirEEdescPrg 10512      // 5 x 20 = 100, descripciones escenas
// hueco desde 10612 hasta 10711, 128 bytes  // Reservados ampliación evenhis 
#define dirEEoffsetA 11892      // 16 x 4 = 64 bytes, offset conversion analógicas locales
#define dirEEsumatA 12706      // 20 x 1 = 20 bytes, sumatorio conversion analógicas locales
#define dirEEisEstacion 13000
#define dirEEisT1 13002
#define dirEEisT2 13004
#define dirEEisT3 13006
#define dirEEisT4 13008
#define dirEEisC 13010
#define dirEEisCC 13012
#define dirEEisCCC 13014
#define dirEEisACS 13016
#define dirEEisPIS 13018
#define dirEEisRES 13020
#define dirEEisCCC 13022
#define dirEEisCr1 13024
#define dirEEisCr2 13026
#define dirEEisAoC 13028
#define dirEEisAoF 13030
#define dirEEisHYC 13032
#define dirEEisHYF 13034
#define dirEEisHYA 13036
#define dirEEisHYP 13038
#define dirEEisHYr 13040
#define dirEEisbPC 13042
#define dirEEisAH2 13044
#define dirEEisAH4 13046
#define dirEEisAb4 13048
#define dirEEistLP 13050
#define dirEEisECO 13052
#define dirEEisTOF 13054
#define dirEEisTON 13056
#define dirEEisPro 13058
#define dirEEisTin 13060
#define dirEEisTfi 13062
#define dirEEisNbh 13064
#define dirEEisTL2 13066
#define dirEEisAPP 13068
#define dirEEisFR  13070

// hueco desde 12746  hasta final

byte modo=0;
/********* IMPORTANTE  *****************/
//const byte valorpin[2] = {0,1};    // directo 0=LOW,  1=HIGH    placa  placa Pozuelo
const byte valorpin[2] = {1,0};    // 2  inverso 1=LOW,  0=HIGH    placa Ramón (Gar
/********* IMPORTANTE  *****************/
const int buffLEN = 128;   // 2 tamaño buffer para webserver.processConnection

int minRam = 8000;        // 2

byte showN   =  1;    // 1 indica si se muestra el número de pin en la lista de entradas/salidas
int peract  =  60;     // 2 período de actualización automática de la página principal 
byte debugserial = 0;

byte usepass =  0;    // 1, usar password:1,  sin password:0
char user1[LEN20];    // 20 bytes
char pass1[LEN20];    // 20 bytes
tmElements_t tm;       // estructura para RTC
tmElements_t tmInicio; // estructura para RTC

const byte freepines[]={        // 54  pin libre=0; pin reservado=1
          0,0,0,0,1, 0,0,0,0,0,  1,0,0,1,0, 0,0,0,0,1,  // pines  0 a  19
          1,1,0,0,0, 0,0,0,0,1,  0,1,0,1,0, 1,0,0,0,0,  // pines 20 a 39
          0,0,1,1,1, 1,1,1,1,1,  1,1,1,1};              // pines 40 a 53
const byte tabpin[34]={0,1,2,3,5,6,7,8,9,11,12,14,15,16,17,18,19,22,23,  // tabla de pines útiles
                       24,25,26,27,28,30,32,34,36,37,38,39,40,41,42};        
  byte EEmac[6];                // 6  100-105   dirección MAC
  byte EEip[4];                 // 4   106-109   dirección IP
  byte EEgw[4];                 // 4   110-113   puerta de enlace 
  byte EEmask[4];               // 4   114-117   máscara de subred
  byte EEdns[4];                // 4   118-121   servidor DNS
  unsigned int EEwebPort=82;    // 2   122-123   puerto servidor web 
  
  byte numIA = 16;              // 1   200       número de entradas anlÃ³gicas mÃ¡ximo 16
  byte numID = 16;              // 1    201       número de entradas digitales
  byte numOD = 22;              // 1    202       número de E/S utilizadas, la suma de las 3 anteriores mÃ¡ximo 54
  byte numPines;                // 1    número total de pines usados, se calcula y no se guarda
  byte bshowpin[lTAB];          // 8    bytes,   0:visible, 1:oculto, en Panel Principal
  byte bshowEsc;                // 1 byte, por bits uno para cadaescena 
  byte bshowTemp[3];            // 3    bytes,   0:visible, 1:oculto, en Panel Principal
  byte bprgtipo[4];             // tipo de salida en prg semanal: local/remota
  byte beveacttipo[4];          // tipo de activadora en prg evento: local/remota
  byte bevesaltipo[4];          // tipo de salida en prg evento: local/remota
  byte bfectipo[4];             // tipo de salida en prg fecha: local/remota
  byte bactsem[4];              // programa semanal activo, 4 bytes = 32 programas
  byte bacteve[4];              // programa eventos activo, 4 bytes = 32 programas
  byte bactfec[4];              // programa fechas activo, 4 bytes = 32 programas
  byte evenact[nPRG];           // 32   1 por cada evento, señal que activa el evento
  byte bevenENABLE[2][lTAB];    // 16   2x8 bytes, define si el evento ha activado ya algo
  byte evencomp[nPRG];          // 32   32 bytes, comparador: =, <>, >=, .... 
  byte evenvalD[nPRG];          // 32   32 bytes, valor de activación del evento para digitales
  byte evensal[nPRG];           // 68   define la señal sobre la que se actua
  byte bevenniv[lTAB];          // 8    8 bytes, define nivel a activar. 

  byte evenTemp[nPRG];          // 32   define la señal sobre la que se actua
  long evenvalA[nPRG];          // 128  32x4, valor de activación del evento para analógicas
  long evenhis[nPRG];           // 128  32x4, valor de histéresis de activación del evento
  byte bPRGsem[nPRGsem][4];     // 20 bytes, para guardar 5x32 bits de asociación PRG-PRGsem
  byte bPRGeve[nPRGeve][4];     // 20 bytes, para guardar 5x32 bits de asociación PRG-PRGeve
  byte actPrg[5];               // 5 bytes guarda que programas están activos: 0/1 
  byte tipoED[16];              // tipo de la entrada digital: 0=ON/OFF, 1=DHT11
  
  // variables para programación SEMANAL
  byte bprgval[lTAB];         // 8 bytes, valor a poner en cada programación, on/off
  byte prgsal[nPRG];          // 32 bytes, salida a actuar en cada programación
  byte prgdia[nPRG];          // 32 bytes, días de la semana, bits 0 a 6
  byte prghor[nPRG];          // 32 bytes, hora
  byte prgmin[nPRG];          // 32 bytes, minuto
  byte prgcon[nPRG];          // 32 bytes, señal para activación condicional
  byte prgcomp[nPRG];         // 32 bytes, tipo de comparación
//  int prgconv[nPRG];          // 64 bytes, valor de la señal para activación condicional

  // variables para programación por FECHAS
  byte bfecval[lTAB];         // 8 bytes, valor a poner en cada programación, on/off
  byte fecsal[nPRG];          // 32 bytes, salida a actuar en cada programación por fechas
  byte fecano[nPRG];          // 32 bytes, año menos 2000 para poder usar un byte
  byte fecmes[nPRG];          // 32 bytes, mes
  byte fecdia[nPRG];          // 32 bytes, día del mes 
  byte fechor[nPRG];          // 32 bytes, hora
  byte fecmin[nPRG];          // 32 bytes, minuto

  byte nTemp1=0;                 // número temperaturas 1-wire, puerto 1
  byte nTemp2=0;                 // número temperaturas 1-wire, puerto 2
  int nodact, nodnue, canact, cannue;
  uint16_t Nodoraiz=0;
  byte addr1Wire[maxTemp][8];      // ver de eliminar
  unsigned long contadores[8];
  byte prectemp=12;
  int contador=0; 
  int velocidad=0; 

  long mact1;
  long mact10;
  long mact60;
  long mactVar;
  long pmactVar=60;
  int opcode=99;
  
  float factorA[nANALO];  // 16x4 factor conversión analógicas locales
  float offsetA[nANALO];   // 16x4 offset conversión analógicas locales
  byte bsumatA[2];  
  byte tipoLCD=0;
  boolean tareason=false;
  
  //File milog;
  char namelogEve[]="eventos.log";  // tipo 2  
  char namelogAcc[]="acciones.log";  // tipo 1, 5 y 6    
  char namelogAna[]="analog.log";  // tipo 3
  char namelogSys[]="system.log";  // tipo 4    
  char namelogIP[]="ipaccess.log";  // tipo 7
  char namelogTem[]="temperat.log";  // 
 // file backup
  char namefilebackup[]={"backup.bin"};
  char namefile[13];
  byte resetfab=0;
  
// DEFINICIONES ISARA //////////////////
char codigoAM[9]={"12341234"};
int maxarranquesAM=150;
int arranquesAM=0;
boolean AMactivo=false;
// PARÁMETROS
byte isEstacion=1;   // 1 invierno, 2 verano
int isT1=1;          // -20º a 20º
int isT2=15;         // -20º a 20º
int isT3=33;         // 20º a 60º
int isT4=28;         // 20º a 60º
int isC=1;           // 1 a 15 minutos
int isCC=5;          // 1 a 15 minutos
int isCCC=5;         // 0 a 15 minutos
int isFR=12;         // 5º a 20º consigna fríoa
int isACS=55;        // 40º a 65º consigna ACS
int isPIS=28;        // 25º a 35º consigna piscina
int isRES=15;        // -10º a 25º
int isCr1=10;        // 0 a 15 minutos
int isCr2=5;         // 1 a 24 horas
int isAoC=33;        // 25º a 45º
int isAoF=12;        // 5º a 25º
int isHYC=2;         // 1º a 10º
int isHYF=2;         // 1º a 10º
int isHYA=10;        // 1º a 10º
int isHYP=1;         // 1º a 10º
int isHYr=1;         // 1º a 10º
int isbPC=4;        // 0 a 10 
int isAH2=65;        // 40º a 75º
int isAH4=55;        // -15º a 65º
int isAb4=5;         // -15º a 45º
int istLP=55;        // 35º a 45º
boolean isECO=false; // modo ECO on/off
int isTOF=1;         // 1 a 24 horas
int isTON=15;        // 1 a 15 minutos
boolean isPro=false; // calentamiento suelo
int isTin=10;        // 10º a 20º temperatura inicial retorno
int isTfi=30;        // 25º a 40º temperatura final retorno
int isNbh=240;       // 120 a 480 horas
int isTL2=65;        // 0º a 75º límite máximo impulsión
boolean isAPP=false; // activar apoyo 
// ENTRADAS
float isST1=20.0;         // sonda 1 Retorno calefacción
float isST2=23.0;         // sonda 1 Ida calefacción
float isST3=10.0;         // sonda 1 Exterior
float isST4=0.0;         // sonda 1 Sonda auxiliar
float isST5=10.0;         // sonda 1 Retorno captador
float isST6=7.0;         // sonda 1 Ida captador
// identificadores
#define tON 0
#define tAcs 1
#define tFri 2
#define tPis 3
#define tTer 4
#define tPta 5
#define tPtb 6
#define tFx1 7
#define tFx2 8
#define tFx3 9
#define tMCal 11
#define tmCalh 12
#define tMFri 13
#define tmFrih 14
#define tAh2 15
#define tAh4 16
#define tAb4 17
//Pines SOLO CALOR
//#define pON 2
//int pAcs=5;
//int pFri=3;
//#define pPis 7
//#define pTer 8
//#define pPta 9
//#define pPtb 11
//#define pFx1 12
//#define pFx2 14
//#define pFx3 15
//Pines REVERSIBLE
#define pON 2
int pAcs=3;
int pFri=5;
#define pPis 7
#define pTer 8
#define pPta 9
#define pPtb 11
#define pFx1 12
#define pFx2 14
#define pFx3 15
//CONTACTOS ENTRADA
#define maxEve 18
byte isEve[maxEve][2]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//byte isvAct[maxEve]={0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
byte isvAct[maxEve]={0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1};
// Contacto filtración piscina
int isModo=0;   // 0: Calefacción,t 1:Piscina, 2:Frio, 3:ACS, 4:alarma
int isModoAnt=0;   // 0: Calefacción, 1:Piscina, 2:Frio, 3:ACS, 4:alarma

//Teclado
const byte rows = 1; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {{'1','2','3','4'}};
byte rowPins[rows] = {38}; //connect to the row pinouts of the keypad
byte colPins[cols] = {34, 32, 37, 36}; //connect to the column pinouts of the º
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
char key;
int estadoLCD=0;  // reposo mostrando datos normales
// EVENTOS BC

// SALIDAS
boolean isRBCCal=false;  // bomba circulación calefacción    // relé 1
boolean isRBCCap=false;  // bomba circulación captador       // relé 2
boolean isRCOMP=false;   // Compresor                        // relé 3
boolean isRACS=false;    // ACS                              // relé 4
boolean isRV3V=false;    // Válvula de 3 vías ACS            // relé 5
boolean isRRes=false;    // Resistencia                      // relé 6
boolean isRV4V=false;    // Válvula 4 vías (frío)            // relé 7
boolean isRALARM=false;   // piloto alarma                    // relé 8
// VALORES AUXILIARES
int isCodAlarma=0;
float isCONSCalor=30;         // consigna calor
long isSegIniReg=0;         // segundos que lleva desde el arranque del regulador
long isSegIniBCal=0;        // segundos que lleva desde el arranque bomba calefacción
long isSegIniBCap=0;        // segundos que lleva desde el arranque bomba captador
long isSegIniComp=0;        // segundos que lleva desde el arranque compresor
long isSegFinComp=0;        // segundos que lleva desde la parada compresor
long isSegIniAlarmaTemp=0;  // segundos desde que se produjo alarma temporizada
long isSegIniReg0=0;         // segundos que lleva desde el arranque del regulador
long isSegIniBCal0=0;        // segundos que lleva desde el arranque bomba calefacción
long isSegIniBCap0=0;        // segundos que lleva desde el arranque bomba captador
long isSegIniComp0=0;        // segundos que lleva desde el arranque compresor
long isSegFinComp0=0;        // segundos que lleva desde la parada compresor
long isSegIniAlarmaTemp0=0;  // segundos desde que se produjo alarma temporizada
//ESTADOS
const int isPARADO=0;         // Parada, desconectado
const int isSTANDBY=1;        // Parado, sin demanda de nada
const int isFASE1=2;          // Esperando arranque bomba calefacción
const int isFASE2=3;          // Esperando arranque bomba captador
const int isFASE3=4;          // Esperando arranque compresor
const int isRUNNING=5;        // Funcionando normalmente: compresor y bombas ON
const int isALARMA=9;         // hay alarma
const int isINDEF=99;         // indefinido
int ESTADO=isPARADO;          // Parada en el inicio (se podría guardar el estao en EEPROM para reiniciar después corte luz)
int ESTAnt=isPARADO;
int EVENTO=0;
float VCOMP=0;
float CONSIGNA=0;
float HIST=0;

Mudbus Mb;

#define PREFIX ""
WebServer webserver(PREFIX, EEwebPort);
LiquidCrystal lcd(lcdRS,lcdRW,lcdEN,lcdD0,lcdD1,lcdD2,lcdD3); 
EthernetClient EthClient;

//////  tratamiento de bits /////////////////////
const byte tab[8] = {1,2,4,8,16,32,64,128};    // 8

byte mailACT = 0;          //  1
byte mailPER = 5;          //  2
char mailSVR[LEN40] = "";  //  40
int mailPORT = 25;         //  2
char mailUSER[LEN40] = ""; //  40
char mailPASS[LEN40] = "";   //  40
char mailSENDER[LEN40] = ""; //  40
char mailDEST[3][LEN40] = {"","",""};   //  120
byte dyndnsServ=0;
char dyndnsHost[LEN40]="";
char dyndnsUser[LEN40]="";
char dyndnsPass[LEN40]="";

void strcatP(char *dest, const prog_uchar *orig)
  {
  char c;
  while((c = pgm_read_byte(orig++))) dest[strlen(dest)]=c;
  }

// DEFINICIONES ISARA //////////////////

void leerDatosBC()
  {
  EEPROM_readAnything (dirEEisEstacion, isEstacion);
  EEPROM_readAnything (dirEEisT1, isT1);
  EEPROM_readAnything (dirEEisT2, isT2);
  EEPROM_readAnything (dirEEisT3, isT3);
  EEPROM_readAnything (dirEEisT4, isT4);
  EEPROM_readAnything (dirEEisC, isC);
  EEPROM_readAnything (dirEEisCC, isCC);
  EEPROM_readAnything (dirEEisCCC, isCCC);
  EEPROM_readAnything (dirEEisACS, isACS);
  EEPROM_readAnything (dirEEisPIS, isPIS);
  EEPROM_readAnything (dirEEisRES, isRES);
  EEPROM_readAnything (dirEEisCCC, isCCC);
  EEPROM_readAnything (dirEEisCr1, isCr1);
  EEPROM_readAnything (dirEEisCr2, isCr2);
  EEPROM_readAnything (dirEEisAoC, isAoC);
  EEPROM_readAnything (dirEEisAoF, isAoF);
  EEPROM_readAnything (dirEEisHYC, isHYC);
  EEPROM_readAnything (dirEEisHYF, isHYF);
  EEPROM_readAnything (dirEEisHYA, isHYA);
  EEPROM_readAnything (dirEEisHYP, isHYP);
  EEPROM_readAnything (dirEEisHYr, isHYr);
  EEPROM_readAnything (dirEEisbPC, isbPC);
  EEPROM_readAnything (dirEEisAH2, isAH2);
  EEPROM_readAnything (dirEEisAH4, isAH4);
  EEPROM_readAnything (dirEEisAb4, isAb4);
  EEPROM_readAnything (dirEEistLP, istLP);
  EEPROM_readAnything (dirEEisECO, isECO);
  EEPROM_readAnything (dirEEisTOF, isTOF);
  EEPROM_readAnything (dirEEisTON, isTON);
  EEPROM_readAnything (dirEEisPro, isPro);
  EEPROM_readAnything (dirEEisTin, isTin);
  EEPROM_readAnything (dirEEisTfi, isTfi);
  EEPROM_readAnything (dirEEisNbh, isNbh);
  EEPROM_readAnything (dirEEisTL2, isTL2);
  EEPROM_readAnything (dirEEisAPP, isAPP);
  EEPROM_readAnything (dirEEisFR, isFR);
  
  }
void guardarDatosBC()
  {
  EEPROM_writeAnything (dirEEisEstacion, isEstacion);
  EEPROM_writeAnything (dirEEisT1, isT1);
  EEPROM_writeAnything (dirEEisT2, isT2);
  EEPROM_writeAnything (dirEEisT3, isT3);
  EEPROM_writeAnything (dirEEisT4, isT4);
  EEPROM_writeAnything (dirEEisC, isC);
  EEPROM_writeAnything (dirEEisCC, isCC);
  EEPROM_writeAnything (dirEEisCCC, isCCC);
  EEPROM_writeAnything (dirEEisACS, isACS);
  EEPROM_writeAnything (dirEEisPIS, isPIS);
  EEPROM_writeAnything (dirEEisRES, isRES);
  EEPROM_writeAnything (dirEEisCCC, isCCC);
  EEPROM_writeAnything (dirEEisCr1, isCr1);
  EEPROM_writeAnything (dirEEisCr2, isCr2);
  EEPROM_writeAnything (dirEEisAoC, isAoC);
  EEPROM_writeAnything (dirEEisAoF, isAoF);
  EEPROM_writeAnything (dirEEisHYC, isHYC);
  EEPROM_writeAnything (dirEEisHYF, isHYF);
  EEPROM_writeAnything (dirEEisHYA, isHYA);
  EEPROM_writeAnything (dirEEisHYP, isHYP);
  EEPROM_writeAnything (dirEEisHYr, isHYr);
  EEPROM_writeAnything (dirEEisbPC, isbPC);
  EEPROM_writeAnything (dirEEisAH2, isAH2);
  EEPROM_writeAnything (dirEEisAH4, isAH4);
  EEPROM_writeAnything (dirEEisAb4, isAb4);
  EEPROM_writeAnything (dirEEistLP, istLP);
  EEPROM_writeAnything (dirEEisECO, isECO);
  EEPROM_writeAnything (dirEEisTOF, isTOF);
  EEPROM_writeAnything (dirEEisTON, isTON);
  EEPROM_writeAnything (dirEEisPro, isPro);
  EEPROM_writeAnything (dirEEisTin, isTin);
  EEPROM_writeAnything (dirEEisTfi, isTfi);
  EEPROM_writeAnything (dirEEisNbh, isNbh);
  EEPROM_writeAnything (dirEEisTL2, isTL2);
  EEPROM_writeAnything (dirEEisAPP, isAPP);
  EEPROM_writeAnything (dirEEisFR, isFR);
  RTC.write(tm);
  }
  
int BuscaModo()
  {
  return 1;
  }

void cambiaestadoOnOff(byte numT, char *texto)
  {
   if (isEve[numT][0]==0) 
     isEve[numT][0]=1; 
   else 
     isEve[numT][0]=0; 
  }

void leeEntradas()
  {
//  isEve[tON][0]=(digitalRead(pON)==isvAct[tON]);
//  isEve[tON][0]=1;
  isEve[tAcs][0]=(digitalRead(pAcs)==isvAct[tAcs]);
  isEve[tFri][0]=(digitalRead(pFri)==isvAct[tFri]);
  isEve[tPis][0]=(digitalRead(pPis)==isvAct[tPis]);
  isEve[tTer][0]=(digitalRead(pTer)==isvAct[tTer]);
  isEve[tPta][0]=(digitalRead(pPta)==isvAct[tPta]);
  isEve[tPtb][0]=(digitalRead(pPtb)==isvAct[tPtb]);
  isEve[tFx1][0]=(digitalRead(pFx1)==isvAct[tFx1]);
  isEve[tFx2][0]=(digitalRead(pFx2)==isvAct[tFx2]);
//  isEve[tFx3][0]=(digitalRead(pFx3)==isvAct[tFx3]);
  isEve[tFx3][0]=0;
  }
    
void asignaEstados()
  {
  if (isModo==2)    // frio
    {
    if (isST2<=isFR)  // se ha bajado de la consigna de frío
      isEve[tMFri][0]=1;
    else
      isEve[tMFri][0]=0;
    if (isST2>isFR+isHYF)  // se ha subido de la consigna de frío + HYF
      isEve[tmFrih][0]=1;
    else
      isEve[tmFrih][0]=0;
    }
  else            // calor, piscina, acs
    {
    if (VCOMP>=CONSIGNA)  // se ha superadola consigna de calor
      isEve[tMCal][0]=1;
    else
      isEve[tMCal][0]=0;
    if (VCOMP<CONSIGNA-HIST)  // se ha bajado de la consigna de calor - histéresis
      isEve[tmCalh][0]=1;
    else
      isEve[tmCalh][0]=0;
    }
  if (isST2>=isAH2) isEve[15][0]=1; else isEve[15][0]=0;
  if (isST2>=isAH4) isEve[16][0]=1; else isEve[16][0]=0;
  if (isST5>=isAb4) isEve[17][0]=1; else isEve[17][0]=0;
  }
  
void calculaConsigna()
  {
  if (isModo!=2)    // no es modo frío
    {
    if (isModo==3)    // ACS
      {
      VCOMP=isST2;    //impulsión
      CONSIGNA=isACS;
      HIST=isHYA;
      }
    else
      if (isModo==1)    // piscina
        {
        VCOMP=isST1;
        CONSIGNA=isPIS;
        HIST=isHYP;
        }
      else    // modo=1 calefacción
        {
        VCOMP=isST1;
        HIST=isHYC;
        if (isST3 < isT1)
          CONSIGNA=isT3;
        else
          if (isST3 > isT2)
            CONSIGNA=isT4;
          else
            CONSIGNA=float(isT3) - float((float(isT3-isT4)*float(isST3-isT1))/float(isT2-isT1));   
        }
    }
  }
  
void buscaAlarma()      // comprueba si hay aarmas. Se mira por el estado no por el cambio de estado
  {
  isCodAlarma=0;
  if (isEve[tPta][0]==isvAct[tPta]) isCodAlarma=1;
  if (isEve[tPtb][0]==isvAct[tPtb]) isCodAlarma=2;
  if (isST2>=isAH2) isCodAlarma=3;
  if (isST2>=isAH4) isCodAlarma=4;
  if (isST5<=isAb4) isCodAlarma=5;
//  if (isEve[tFx1][0]==isvAct[tFx1]) isCodAlarma=6;
//  if (isEve[tFx2][0]==isvAct[tFx2]) isCodAlarma=7;
//  if (isEve[tFx3][0]==isvAct[tFx3]) isCodAlarma=8;
  }
  
void buscaEvento()    // se buscan cambios de estado
  {
  
  for (int i=0; i<maxEve; i++)
    {
    if (isEve[i][0]==1)    // si estado actual es 1
      {
      if (isEve[i][1]==0)  // y estado anterior 0
        {
        EVENTO=100+i;        // de 0 a 1 (hacia adelante)
        Serial.println(EVENTO);
        return;
        }
      }
    else                   // si estado actual es 0
      if (isEve[i][1]==1)  // y estado anterior 1
        {
        EVENTO=200+i;      // de 1 a 0 (hacia atrás)
        return;
        }
    }
  }

void asignaModo()
  {
  isModo=0;
//  if (isEve[tAcs][0]==isvAct[tAcs]) {isModo=3; return; }    // Acs
//  if (isEve[tPis][0]==isvAct[tPis]) {isModo=1; return; }    // Piscina
//  if (isEve[tFri][0]==isvAct[tFri]) {isModo=2; return; }    // Frío
  if (isEve[tAcs][0]==1) {isModo=3; return; }    // Acs
  if (isEve[tPis][0]==1) {isModo=1; return; }    // Piscina
  if (isEve[tFri][0]==1) {isModo=2; return; }    // Frío
  }
  
void activaEstado (int estadonew)
  {
  ESTADO=estadonew;
  EVENTO=0;
  if (ESTADO==isPARADO)
    {
    isRBCCal=false;    pinVAL(22,0);
    isRBCCap=false;    pinVAL(23,0);
    isRCOMP=false;     pinVAL(24,0);
    isRACS=false;      pinVAL(25,0);
    isRV3V=false;      pinVAL(26,0);
    isRRes=false;      pinVAL(27,0);
    isRV4V=false;      pinVAL(28,0);
    isRALARM=false;    pinVAL(30,0);
    isSegIniReg0=millis()/1000;    // reinicia tiempo desde arranque regulador
    isSegIniBCal0=millis()/1000;   // reinicia tiempo desde arranque bomba circulación
    isSegIniBCap0=millis()/1000;   // reinicia tiempo desde arranque bomba captador
    isSegIniComp0=millis()/1000;   // reinicia tiempo desde arranque compresor    
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    isSegIniBCal=millis()/1000-isSegIniBCal0;    // actualiza tiempo desde arranque bomba calefacción
    isSegIniBCap=millis()/1000-isSegIniBCap0;    // actualiza tiempo desde arranque bomba captador
    isSegIniComp=millis()/1000-isSegIniComp0;    // actualiza tiempo desde arranque compresor
    }
  if (ESTADO==isFASE1)
    {
    isRBCCal=false;      pinVAL(22,1);
    isRBCCap=false;      pinVAL(23,0);
    isRCOMP=false;       pinVAL(24,0);
    isRACS=(isModo==3);  pinVAL(25,isRACS);
    isRV3V=isRACS;       pinVAL(26,isRV3V);
    isRRes=false;        pinVAL(27,0);
    isRV4V=(isModo==2);  pinVAL(28,isRV4V);
    isRALARM=false;      pinVAL(30,0);
    isSegIniReg0=millis()/1000;    // reiniciar tiempo desde arranque regulador
    isSegIniBCal0=millis()/1000;    
    isSegIniBCap0=millis()/1000;    
    isSegIniComp0=millis()/1000;    
    }
  if (ESTADO==isFASE2)
    {
    isRBCCal=true;       pinVAL(22,1);  
    isRBCCap=false;      pinVAL(23,0);
    isRCOMP=false;       pinVAL(24,0);
    isRACS=(isModo==3);  pinVAL(25,isRACS);
    isRV3V=isRACS;       pinVAL(26,isRV3V);
    isRRes=false;        pinVAL(27,0);
    isRV4V=(isModo==2);  pinVAL(28,isRV4V);
    isRALARM=false;      pinVAL(30,0);
    isSegIniBCal0=millis()/1000;    
    isSegIniBCap0=millis()/1000;    
    isSegIniComp0=millis()/1000;    
    }
  if (ESTADO==isFASE3)
    {
    isRBCCal=true;       pinVAL(22,1);
    isRBCCap=true;       pinVAL(23,1);
    isRCOMP=false;       pinVAL(24,0);
    isRACS=(isModo==3);  pinVAL(25,isRACS);
    isRV3V=isRACS;       pinVAL(26,isRV3V);
    isRRes=false;        pinVAL(27,0);
    isRV4V=(isModo==2);  pinVAL(28,isRV4V);
    isRALARM=false;      pinVAL(30,0);
    isSegIniBCap0=millis()/1000;    
    isSegIniComp0=millis()/1000;    
    }
  if (ESTADO==isRUNNING)
    {
    isRBCCal=true;       pinVAL(22,1);
    isRBCCap=true;       pinVAL(23,1);
    isRCOMP=true;        pinVAL(24,1);
    isRACS=(isModo==3);  pinVAL(25,isRACS);
    isRV3V=isRACS;       pinVAL(26,isRV3V);
    isRRes=false;        pinVAL(27,0);
    isRV4V=(isModo==2);  pinVAL(28,isRV4V);
    isRALARM=false;      pinVAL(30,0);
    }
  if (ESTADO==isALARMA)
    {
    isRBCCal=false;     pinVAL(22,0);
    isRBCCap=false;     pinVAL(23,0);
    isRCOMP=false;      pinVAL(24,0);
    isRACS=false;       pinVAL(25,0);
    isRV3V=false;       pinVAL(26,0);
    isRRes=false;       pinVAL(27,0);
    isRV4V=false;       pinVAL(28,0);
    isRALARM=false;     pinVAL(30,1);
    isSegIniReg0=millis()/1000;    // empieza a contar tiempo desde arranque regulador
    isSegIniBCal0=millis()/1000;    // empieza a contar tiempo desde arranque bomba calefacción
    isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador
    isSegIniComp0=millis()/1000;    // empieza a contar tiempo desde arranque compresor 
    }
  }

void debugevento()
  {  Serial.print("Procesa->");  Serial.print("  Estado:"); Serial.print(ESTADO);  Serial.print("  Evento:"); Serial.println(EVENTO);  }    
  
boolean trataEvento(int evento, int estado)
  {
  if (EVENTO==evento)  
    {
    debugevento();
    activaEstado(estado); 
    return true; 
    }
  return false;
  }
  
void quitarBloqueo()
  {
  if (AMactivo)
    {
    if (arranquesAM >= maxarranquesAM)    // se alcanzó el límite
      {
      lcd.clear();
      printLCD(avisoAM1);
      lcd.setCursor(0,1); printLCD(avisoAM2);
      lcd.setCursor(0,2); printLCD(avisoAM3);
      lcd.setCursor(0,3); printLCD(avisoAM4);
      boolean repeat;
      do {
          key = keypad.getKey();
          repeat=(key!='4');
         } while (repeat);
      int i=0;
      Serial.println("Pulsada tecla 4");
      lcd.setCursor(0,3); printLCD(codigo);
      boolean cambiar=false;
      while (!cambiar)
        {
        key = keypad.getKey();
        if (key>0)
          {
          Serial.print("  i:"); Serial.print(i); 
          Serial.print("  key:"); Serial.print(key); 
          Serial.print("  codigoAM[i]:"); Serial.println(codigoAM[i]);
          if (key==codigoAM[i])
            {
            lcd.print(key);
            i++;
            }
          else
            {
            i=0;
            lcd.setCursor(7,3);printLCD(lcdblancos); lcd.setCursor(7,3);
            }
          cambiar=(i==8);
          }
        }
      if (cambiar)
        {
        AMactivo=false;
        EEPROM_writeAnything (dirEEAMactivo, AMactivo); 
        lcd.clear();
        }
      }
    }
  }
  
void confBloqueo()
  {
  lcd.clear();  
  if (AMactivo)
    printLCD(desactivarbloqueo);
  else
    printLCD(activarbloqueo);
  lcd.setCursor(0,3); printLCD(codigo);
  lcd.setCursor(7,3);
  boolean cambiar=false;
  int i=0;
  while (!cambiar)
    {
    key = keypad.getKey();
    if (key>0)
      {
      Serial.print("  i:"); Serial.print(i); 
      Serial.print("  key:"); Serial.print(key); 
      Serial.print("  codigoAM[i]:"); Serial.println(codigoAM[i]);
      if (key==codigoAM[i])
        {
        lcd.print(key);
        i++;
        }
      else
        {
        i=0;
        lcd.setCursor(7,3);printLCD(lcd10blancos); lcd.setCursor(7,3);
        }
      cambiar=(i==8);
      }
    }
  if (cambiar)
    {
    if (AMactivo)
      AMactivo=false;
    else
      AMactivo=true;
    EEPROM_writeAnything (dirEEAMactivo, AMactivo); 
    arranquesAM=0;
    EEPROM_writeAnything (dirEEarranquesAM, arranquesAM); 
    lcd.clear();
    }
  }
  
void procesaIsara2()
  {
  for (int i=0; i<maxEve; i++) isEve[i][1]=isEve[i][0];   // guarda valores actuales en anteriores
  leeEntradas();
  asignaModo();  
  calculaConsigna();
  asignaEstados();   // compara valores de temperatura y asigna si es mayor o menor   
  buscaEvento();     // busca si ha habido algún cambio de estado: evento
  buscaAlarma();     // busca si hay alguna condición de alarma
  if (isCodAlarma>0) 
    {
    if (ESTADO!=isPARADO)
      {
        
      activaEstado(isALARMA);
      return;
      }
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isPARADO)
    {
    if (EVENTO==100) {ESTADO=isFASE1; return;  }
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isFASE1)
    {
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    if (isSegIniReg == isC*60)    // cambia estado temporización
      {
      if ((isEstacion==1) || (isModo!=0))
        {
        isSegIniBCal0=millis()/1000;    // empieza a contar tiempo desde arranque bomba circulación
        activaEstado(isFASE2);  
        return;
        }
      }
    if (trataEvento(105,isALARMA)) return;   // Alarma Presostato alta
    if (trataEvento(106,isALARMA)) return;   // Alarma Presostato baja
    if (trataEvento(107,isALARMA)) return;   // Alarma Fluxostato 1
    if (trataEvento(108,isALARMA)) return;   // Alarma Fluxostato 2
    if (trataEvento(109,isALARMA)) return;   // Alarma Fluxostato 3
    if (trataEvento(115,isALARMA)) return;   // aparece AH2
    if (trataEvento(116,isALARMA)) return;   // aparece AH4
    if (trataEvento(117,isALARMA)) return;   // aparece Ab4
    if (trataEvento(200,isPARADO)) return;   // Encendido a OFF
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isFASE2)
    {
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    isSegIniBCal=millis()/1000-isSegIniBCal0;    // actualiza tiempo desde arranque bomba calefacción
    if (isModo==0)  // Calor 
      {
      if (isEve[tTer][0]==1)    // termostato on
        {
        if (isEve[tmCalh][0]==isvAct[tmCalh])  // temperatura menor que consigna
          {
          if (isSegIniBCal >= isCC*60)    // cambia estado temporización
            {
            isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador
            activaEstado(isFASE3);  
            return;
            }
          }
         }
       }
    if (isModo==1)  // Piscina 
      if (isEve[tmCalh][0]==isvAct[tmCalh])  // temperatura menor que consigna
        if (isSegIniBCal >= isCC*60)    // cambia estado temporización
          {
          isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador
          activaEstado(isFASE3);  
          return;
          }
    if (isModo==2)  // Frio 
      if (isEve[tTer][0]==1)    // termostato on
        if (isEve[tmFrih][0]==isvAct[tmFrih])  // temperatura mayor que consigna
          if (isSegIniBCal >= isCC*60)    // cambia estado temporización
            {
            isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador
            activaEstado(isFASE3);  
            return;
            }
    if (isModo==3)  // ACS
      if (isEve[tmCalh][0]==isvAct[tmCalh])  // temperatura menor que consigna
        if (isSegIniBCal >= isCC*60)    // cambia estado temporización
          {
          isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador
          activaEstado(isFASE3);  
          return;
          }
    if (trataEvento(101,isFASE1)) return;   // Cambio a ACS 
    if (trataEvento(102,isFASE1)) return;   // Cambio a Frio 
    if (trataEvento(103,isFASE1)) return;   // Cambio a Piscina 
    if (trataEvento(104,isFASE3)) return;   // Termostato On
    if (trataEvento(105,isALARMA)) return;   // Alarma Presostato alta
    if (trataEvento(106,isALARMA)) return;   // Alarma Presostato baja
    if (trataEvento(107,isALARMA)) return;   // Alarma Fluxostato 1
    if (trataEvento(108,isALARMA)) return;   // Alarma Fluxostato 2
    if (trataEvento(109,isALARMA)) return;   // Alarma Fluxostato 3
    if (trataEvento(115,isALARMA)) return;   // aparece AH2
    if (trataEvento(116,isALARMA)) return;   // aparece AH4
    if (trataEvento(117,isALARMA)) return;   // aparece Ab4
    if (trataEvento(200,isPARADO)) return;   // Cambio a Parado 
    if (trataEvento(201,isFASE1)) return;    // Cambio de ACS 
    if (trataEvento(202,isFASE1)) return;    // Cambio de Frio 
    if (trataEvento(203,isFASE1)) return;    // Cambio de Piscina 
    return;
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isFASE3)
    {
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    isSegIniBCal=millis()/1000-isSegIniBCal0;    // actualiza tiempo desde arranque bomba calefacción
    isSegIniBCap=millis()/1000-isSegIniBCap0;    // actualiza tiempo desde arranque bomba calefacción
    if (isSegIniBCap >= isCCC*60)    // cambia estado temporización
      {
      isSegIniBCap0=millis()/1000;    // empieza a contar tiempo desde arranque bomba captador

      arranquesAM++;
      EEPROM_writeAnything (dirEEarranquesAM, arranquesAM); 
      quitarBloqueo();
      activaEstado(isRUNNING);  
      return;
      }
    if (trataEvento(101,isFASE1)) return;   // Cambio a ACS 
    if (trataEvento(102,isFASE1)) return;   // Cambio a Frio 
    if (trataEvento(103,isFASE1)) return;   // Cambio a Piscina 
    if (trataEvento(105,isALARMA)) return;   // Alarma Presostato alta
    if (trataEvento(106,isALARMA)) return;   // Alarma Presostato baja
    if (trataEvento(107,isALARMA)) return;   // Alarma Fluxostato 1
    if (trataEvento(108,isALARMA)) return;   // Alarma Fluxostato 2
    if (trataEvento(109,isALARMA)) return;   // Alarma Fluxostato 3
    if (trataEvento(111,isFASE2)) return;   // VCOMP >= CONSIGNA  (calor)
    if (trataEvento(113,isFASE2)) return;   // VCOMP <= CONSIGNA  (Frío)
    if (trataEvento(115,isALARMA)) return;   // aparece AH2
    if (trataEvento(116,isALARMA)) return;   // aparece AH4
    if (trataEvento(117,isALARMA)) return;   // aparece Ab4
    if (trataEvento(200,isPARADO)) return;   // Cambio a Parado 
    if (trataEvento(201,isFASE1)) return;   // Cambio de ACS 
    if (trataEvento(202,isFASE1)) return;   // Cambio de Frio 
    if (trataEvento(203,isFASE1)) return;   // Cambio de Piscina 
    if (isModo!=3) if (trataEvento(204,isFASE2)) return;   // Termostato Off, salvo en ACS
    return;
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isRUNNING)
    {
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    isSegIniBCal=millis()/1000-isSegIniBCal0;    // actualiza tiempo desde arranque bomba calefacción
    isSegIniBCap=millis()/1000-isSegIniBCap0;    // actualiza tiempo desde arranque bomba captador
    isSegIniComp=millis()/1000-isSegIniComp0;    // actualiza tiempo desde arranque compresor
    if (trataEvento(101,isFASE1)) return;   // Cambio a ACS 
    if (trataEvento(102,isFASE1)) return;   // Cambio a Frio 
    if (trataEvento(103,isFASE1)) return;   // Cambio a Piscina 
    if (trataEvento(105,isALARMA)) return;   // Alarma Presostato alta
    if (trataEvento(106,isALARMA)) return;   // Alarma Presostato baja
    if (trataEvento(107,isALARMA)) return;   // Alarma Fluxostato 1
    if (trataEvento(108,isALARMA)) return;   // Alarma Fluxostato 2
    if (trataEvento(109,isALARMA)) return;   // Alarma Fluxostato 3
    if (trataEvento(111,isFASE2)) return;   // VCOMP >= CONSIGNA  (calor)
    if (trataEvento(113,isFASE2)) return;   // VCOMP <= CONSIGNA  (Frío)
    if (trataEvento(115,isALARMA)) return;   // aparece AH2
    if (trataEvento(116,isALARMA)) return;   // aparece AH4
    if (trataEvento(117,isALARMA)) return;   // aparece Ab4
    if (trataEvento(200,isPARADO)) return;   // Cambio a Parado 
    if (trataEvento(201,isFASE1)) return;   // Cambio de ACS 
    if (trataEvento(202,isFASE1)) return;   // Cambio de Frio 
    if (trataEvento(203,isFASE1)) return;   // Cambio de Piscina 
    if (isModo!=3) if (trataEvento(204,isFASE2)) return;   // Termostato Off, salvo en ACS
    return;
    }
  ////////////////////////////////////////////////////////////////////    
  if (ESTADO==isALARMA)
    {
    isSegIniReg=millis()/1000-isSegIniReg0;    // actualiza tiempo desde arranque regulador
    if (trataEvento(200,isPARADO)) return;   // Cambio a Parado 
    int newEstado=0;
    if (isEve[tON][0]==1) newEstado=isFASE1; else newEstado=isPARADO;
    if (trataEvento(205,newEstado)) return;   // Presostato alta OK  
    if (trataEvento(206,newEstado)) return;   // Presostato baja OK  
    if (trataEvento(207,newEstado)) return;   // Fluxostato 1 OK  
    if (trataEvento(208,newEstado)) return;   // Fluxostato 2 OK  
    if (trataEvento(209,newEstado)) return;   // Fluxostato 3 OK  
    if (trataEvento(215,newEstado)) return;   // Temperatura AH2 OK  
    if (trataEvento(216,newEstado)) return;   // Temperatura AH4 OK  
    if (trataEvento(217,newEstado)) return;   // Temperatura Ab4 OK  
    return;
    }
  }
  
//
byte getbit(byte mibyte, byte pin)
 {if ((mibyte & tab[(pin % 8)]) > 0) 
    return 1; 
  else 
    return 0;  }

void setbit(byte *mibyte, byte pin, byte value)  { 
  if (value == 0)
    *mibyte = *mibyte & (255 ^ tab[(pin % 8)]);  
  else
    *mibyte = *mibyte | tab[(pin % 8)]; }
 
byte getbit8(byte tabla[], byte pin)
 {if ((tabla[pin/8] & tab[(pin % 8)]) > 0) 
    return 1; 
  else 
    return 0;  }

void setbit8(byte tabla[], byte pin, byte value)  { 
  if (value == 0)
    tabla[pin/8] = tabla[pin/8] & (255 ^ tab[(pin % 8)]);  
  else
    tabla[pin/8] = tabla[pin/8] | tab[(pin % 8)]; }
    
byte EEread(unsigned int dir)  {
  return readEEPROM(EEPROM2dir, dir); }   // está en EEPROMAnything  }

void EEwrite(int dir, byte valor)  {
   writeEEPROM(EEPROM2dir,dir,valor);  }  // está en EEPROMAnything  

void printPiP(WebServer &server, const prog_uchar *texto1,int num,const prog_uchar *texto2)
  {
    server.printP(texto1);
    server.print(num);
    server.printP(texto2);
  }

void printPlP(WebServer &server, const prog_uchar *texto1,long num,const prog_uchar *texto2)
  {
    server.printP(texto1);
    server.print(num);
    server.printP(texto2);
  }

void printPcP(WebServer &server, const prog_uchar *texto1,char car,const prog_uchar *texto2)
  {
    server.printP(texto1);
    server.print(car);
    server.printP(texto2);
  }

void printP(WebServer &server, const prog_uchar *texto1)
  {
    server.printP(texto1); 
  }

void printP(WebServer &server, const prog_uchar *texto1, const prog_uchar *texto2)
  {
    server.printP(texto1); server.printP(texto2);
  }

void printP(WebServer &server, const prog_uchar *texto1, const prog_uchar *texto2, const prog_uchar *texto3)
  {
    server.printP(texto1); server.printP(texto2); server.printP(texto3);
  }

void printP(WebServer &server, const prog_uchar *texto1, const prog_uchar *texto2, 
                                const prog_uchar *texto3, const prog_uchar *texto4)
  {
    printP(server, texto1, texto2); printP(server, texto3, texto4);
  }

void printP(WebServer &server, const prog_uchar *texto1,  const prog_uchar *texto2, 
                                const prog_uchar *texto3,  const prog_uchar *texto4,
                                const prog_uchar *texto5)
  {
    printP(server, texto1, texto2, texto3);
    printP(server, texto4, texto5);
  }

void printP(WebServer &server, const prog_uchar *texto1,  const prog_uchar *texto2, 
                                const prog_uchar *texto3,  const prog_uchar *texto4,
                                const prog_uchar *texto5,  const prog_uchar *texto6)
  {
    printP(server, texto1, texto2, texto3);
    printP(server, texto4, texto5, texto6);
  }
  
void printP(WebServer &server, const prog_uchar *texto1,  const prog_uchar *texto2, 
                                const prog_uchar *texto3,  const prog_uchar *texto4,
                                const prog_uchar *texto5,  const prog_uchar *texto6,
                                const prog_uchar *texto7)
  {
    printP(server, texto1, texto2, texto3);
    printP(server, texto4, texto5, texto6, texto7);
  }
  
void printP(WebServer &server, const prog_uchar *texto1,  const prog_uchar *texto2, 
                                const prog_uchar *texto3,  const prog_uchar *texto4,
                                const prog_uchar *texto5,  const prog_uchar *texto6,
                                const prog_uchar *texto7,  const prog_uchar *texto8)
  {
    printP(server, texto1, texto2, texto3, texto4);
    printP(server, texto5, texto6, texto7, texto8);
  }

void printS(const prog_uchar *str)
  {
  char c;
  while((c = pgm_read_byte(str++)))
    Serial.write(c);
  }

void printlnS(const prog_uchar *str)
  {
  char c;
  while((c = pgm_read_byte(str++)))
    Serial.write(c);
  Serial.println();
  }

void serialmem(char texto[])
  {
  printS(libre); Serial.print(mifreeRam()); 
  }
    
void printTDTR(WebServer &server, byte posicion, boolean trb, boolean tdb)
  {
    if (posicion==0)    // inicio
      {
      if (trb) printP(server,tr);
      if (tdb) printP(server,td);
      }
    else
      {
      if (tdb) printP(server,td_f);
      if (trb) printP(server,tr_f);
      }
  }
  
void printTD(WebServer &server, char *texto, boolean tr, boolean td)
  {
  printTDTR(server, 0, tr, td);
  server.print(texto);
  printTDTR(server, 1, tr, td);
  }
 
void printTDP(WebServer &server, const prog_uchar *texto, boolean tr, boolean td)
  {
  printTDTR(server, 0, tr, td);
  server.printP(texto);
  printTDTR(server, 1, tr, td);
  }
 
void printIP(WebServer &server, int valor, byte base, const prog_uchar *texto)
  {
    if (base==10) server.print(valor,DEC);
    else if (base==8) server.print(valor,OCT);
      else if (base==16) server.print(valor,HEX);
        else if (base==2) server.print(valor,BIN);
          else server.print(valor);
    server.printP(texto);
  }
  
void printLP(WebServer &server, long valor, byte base, const prog_uchar *texto)
  {
    if (base==10) server.print(valor,DEC);
    else if (base==8) server.print(valor,OCT);
      else if (base==16) server.print(valor,HEX);
        else if (base==2) server.print(valor,BIN);
          else server.print(valor);
    server.printP(texto);
  }
  
void printcampoC(WebServer &server, int numpar, char *valactual, byte tam, boolean visible, boolean td, boolean pass)
  {
  printTDTR(server, 0, false, td);
  if (visible)
    if (pass)
      server.printP(Form_input_pass_start);
    else
      server.printP(Form_input_text_start);
  else
    server.printP(Form_input_text_start_invis);
  printIP(server, numpar, 10, Form_input_value);
  server.print(valactual);
  server.printP(Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoL(WebServer &server, int numpar, long valactual, byte tam, boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Form_input_text_start);
  printIP(server, numpar, 10, Form_input_value);
  printLP(server, valactual, 10, Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoChr(WebServer &server, int numpar, int valactual, byte tam, boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Form_input_text_start);
  printIP(server, numpar, 10, Form_input_value);
  server.write(valactual);
  printP(server, Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoI(WebServer &server, int numpar, int valactual, byte tam, boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Form_input_text_start);
  printIP(server, numpar, 10, Form_input_value);
  printIP(server, valactual, 10, Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoF(WebServer &server, int numpar, float valactual, byte tam, byte deci, boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Form_input_text_start);
  printIP(server, numpar, 10, Form_input_value);
  server.print(valactual,deci);
  server.printP(Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoH(WebServer &server, byte numpar, int valactual, byte tam,boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Form_input_text_start);
  
  printIP(server, numpar, 10, Form_input_value);
  printIP(server, valactual, 16, Max_length);
  printIP(server, tam, 10, size_i);
  printIP(server, tam, 10, Form_input_end);
  printTDTR(server, 1, false, td);
  }
  
void printcampoSiNo(WebServer &server, int numpar, int valactual, const prog_uchar *uno, const prog_uchar *cero, boolean visible,boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(visible ? Select_name:Select_name_invis);
  server.print(numpar); server.printP(barramayor);
  server.printP(optionvalue1);
  if (valactual == 1) server.printP(selected);
  printP(server, cierre, uno, option_f);
  server.printP(optionvalue0);
  if (valactual == 0) server.printP(selected); 
  printP(server, cierre, cero, option_f);
  server.printP(Select_f);  
  printTDTR(server, 1, false, td);
  }         
  
void printcampoCB(WebServer &server, int numpar, int valactual, int pri, int ult, boolean visible, boolean td)
  {
  printTDTR(server, 0, false, td);
  server.printP(Select_name);
  server.print(numpar);   server.printP(barramayor);
  for (int j=pri; j<=ult; j++)  {
    server.printP(optionvalue); server.print(j); server.printP(barraatras);
    if (j == valactual) server.printP(selected);
    server.printP(cierre);
    server.print(j);     
    server.printP(option_f);  }
  server.printP(Select_f);  
  printTDTR(server, 1, false, td);
  }         
  
void printparCBP(WebServer &server, const prog_uchar *titulo, int numpar, int valactual, int pri, int ult, boolean visible, boolean td)
  {
  printP(server,tr);                   
  printTDP(server,titulo,false,true);
  printcampoCB(server,numpar,valactual,pri,ult,visible,td); 
  printP(server,tr_f);
  }

void printparC(WebServer &server, char *titulo, int numpar, char valactual[], byte tam, boolean pass)
  {
  printTDTR(server, 0, true, true);
  server.print(titulo);
  server.printP(td_f);  
  printcampoC(server, numpar, valactual, tam, true, true, pass);
  printTDTR(server, 1, true, false);
  }
  
void printparCP(WebServer &server, const prog_uchar *titulo, int numpar, char valactual[], byte tam, boolean pass)
  {
  printTDTR(server, 0, true, true);
  printP(server, titulo, td_f);
  printcampoC(server, numpar, valactual, tam, true, true, pass);
  printTDTR(server, 1, true, false);
  }
  
void printparI(WebServer &server, char *titulo, int numpar, int valactual, byte tam)
  {
  printTDTR(server, 0, true, true);
  server.print(titulo);
  server.printP(td_f);
  printcampoI(server, numpar, valactual, tam, true);
  printTDTR(server, 1, true, false);
  }  
  
void printparIP(WebServer &server, const prog_uchar *titulo, int numpar, int valactual, byte tam)
  {
  printTDTR(server, 0, true, true);
  printP(server, titulo, td_f);
  printcampoI(server, numpar, valactual, tam, true);
  printTDTR(server, 1, true, false);
  }  
  
void printparLP(WebServer &server, const prog_uchar *titulo, int numpar, long valactual, byte tam)
  {
  printTDTR(server, 0, true, true);
  printP(server, titulo, td_f);
  printcampoL(server, numpar, valactual, tam, true);
  printTDTR(server, 1, true, false);
  }  
  
void printColspan(WebServer &server, int ancho, boolean cerrado)
  {
  server.printP(colspan);
  server.print(ancho); 
  if (cerrado) server.printP(barramayor);
  }

void lineaH(WebServer &server, byte ancho)
  {
  printTDTR(server, 0, true, false);
  printColspan(server,ancho,true);
  server.printP(hr);
  printTDTR(server, 1, true, true);
  }

void printEE(WebServer &server, int dir, byte len)
  {
  int i=0;
  char mitext[80]; mitext[0]='\0';
  boolean repeat;
  do {
    mitext[i] = EEread(dir+i);
    repeat = (mitext[i] != '\0');
    i = i++;
    } while (repeat);
  server.print(mitext);
  }
  
void printEEmail(EthernetClient &client, int dir, byte len)
  {
  int i=0;
  char mitext[20]; mitext[0]='\0';
  boolean repeat;
  do {
    mitext[i] = EEread(dir+i);
    repeat = (mitext[i] != '\0');
    i = i++;
    } while (repeat);
  client.print(mitext);
  }
  
//******************************************************************************************************

void printEth(const prog_uchar *str)
  {
  char c;
  while((c = pgm_read_byte(str++)))
    EthClient.write(c);
  }

//Función que envia el email
byte sendEmail(byte pinx, byte even, byte tipo)
{
  boolean debugmail=true;
   char user64[40];
   char pass64[40];
    if (mailACT == 0) return(0);
    if (!EthClient.connect(mailSVR,mailPORT)) return 0; 
    if(!eRcv()) return 0;
    
    printEth(mailHELO);  
    if(!eRcv()) return 0; 
    
    printEth(mailAUTH);    //Le decimos que queremos autenticarnos
    if(!eRcv()) return 0;
    byte largo=strlen(mailUSER);
    base64_encode(user64,mailUSER,largo);

    EthClient.println(user64);   //Enviamos el usuario en base64
    if(!eRcv()) return 0;
    
    largo=strlen(mailPASS);
    base64_encode(pass64,mailPASS,largo);
    EthClient.println(pass64);   //Enviamos el password en base64    
    if(!eRcv()) return 0;
    
    printEth(mailFROM); 
    printEth(menor); EthClient.print(mailSENDER); printEth(mayor);     //especificamos el sender
    if (!eRcv()) return 0;

    printEth(mailRCPT); printEth(menor); 
    if (tipo==0)
      {
      EthClient.print(mailDEST[evensal[pinx]-despMail]);      //especificamos el destinatario
      }
    else
      EthClient.print(mailDEST[evenTemp[pinx]-despMail]);     //especificamos el destinatario
    printEth(mayor); 
    if(!eRcv()) return 0;
    
    printEth(mailDATA);   //indicamos que vamos a empezar a escribir el correo

    printEth(mailFROM); EthClient.print(mailSENDER); printEth(mayor); 
    printEth(mailTO); 
    if (tipo==0)
      EthClient.println(mailDEST[evensal[pinx]-despMail]);
    else
      EthClient.println(mailDEST[evenTemp[pinx]-despMail]);
    printEth(mailSubject); printEth(mailALARMA);
    if (tm.Day < 10)  EthClient.print(0); EthClient.print (tm.Day);      printEth(barra);
    if (tm.Month < 9) EthClient.print(0); EthClient.print (tm.Month+1);  printEth(barra);
    EthClient.print (tmYearToCalendar(tm.Year)); printEth(b);
    if (tm.Hour < 10) EthClient.print(0);   EthClient.print (tm.Hour);   printEth(dospuntos);
    if (tm.Minute < 10) EthClient.print(0); EthClient.print (tm.Minute); printEth(dospuntos);
    if (tm.Second < 10) EthClient.print(0); EthClient.print (tm.Second); printEth(b);
    if (tipo==0)
      printEEmail(EthClient, dirEEdescpinD+(evenact[pinx]*LEN20), LEN20);
    else
      printEEmail(EthClient, dirEEdescTemp+(evenact[pinx]*LEN20), LEN20);
    printEth(b); 
    if (tipo==0) 
      if (evenvalD[pinx-numIA] == 1) printEth(ON); else printEth(OFF);
    else
      {
      EthClient.print(float(Mb.R[pinx])/100); 
      printEth(blancoC); 
      }
    EthClient.println("");   
    if (tm.Day < 10)  EthClient.print(0); EthClient.print (tm.Day);     printEth(barra);
    if (tm.Month < 9) EthClient.print(0); EthClient.print (tm.Month+1); printEth(barra);
    EthClient.print (tmYearToCalendar(tm.Year)); printEth(b);
    if (tm.Hour < 10)   EthClient.print(0); EthClient.print (tm.Hour);   
    printEth(dospuntos);
    if (tm.Minute < 10) EthClient.print(0); EthClient.print (tm.Minute); 
    printEth(dospuntos);
    if (tm.Second < 10) EthClient.print(0);  EthClient.println (tm.Second);
    if (tipo==0) 
      {
      printEEmail(EthClient, dirEEdescpinD+(evenact[pinx]*LEN20), LEN20);
      printEth(mailHaPasadoa); 
      if (evenvalD[pinx] == 1) printEth(ON); else printEth(OFF);
      }
    else
      {
      printEth(mailsonda); 
      printEEmail(EthClient, dirEEdescTemp+(evenact[pinx]*LEN20), LEN20);
      printEth(mailhasuperado); 
      EthClient.print(float(evenvalA[pinx])/100);
      printEth(mailvaloract); 
      EthClient.print(float(Mb.R[pinx])/100);
      printEth(blancoC);
      }
    EthClient.println(""); 
    printEth(mailelconuco);
    EthClient.println(); 
    EthClient.println(""); 
    printEth(punto);        //Cuando enviamos un punto indica el fin del mensaje
    if(!eRcv()) return 0;
    
    printEth(mailQUIT);
    EthClient.println();   // Cerramos la conexion telnet
    if(!eRcv()) return 0;
    EthClient.flush();
    EthClient.stop();      //Paramos el cliente
    return 1; 
}

//

byte eRcv()
{
  byte respCode;
  byte thisByte;
  while(!EthClient.available()) delay(delayACKMail);
  respCode = EthClient.peek();
  while(EthClient.available())
    thisByte = EthClient.read();    
  if(respCode >= '4')
    {
    efail();
    return 0;  
    }  
  return 1;
}
//

void efail()
{
  byte thisByte = 0;
  printEth(mailQuitrin);
  while(!EthClient.available()) delay(1);
  while(EthClient.available())
    thisByte = EthClient.read();    
  EthClient.stop();
}

void printparentesis(WebServer &server, char letra, int numero, byte base)
  {
  server.printP(paren_i);
  server.print(letra); 
  if (base==10) server.print(numero);
  else
    if (base==8) server.print(numero,OCT);
  server.printP(paren_f2b);
  }

void printcorchete(WebServer &server, int numero, byte base)
  {
  server.printP(corchete_i);
  if (base==10) server.print(numero);
  else
    if (base==8) server.print(numero,OCT);
  server.printP(corchete_f);
  }

void printDayName(WebServer &server, byte dia)
  {
    if (dia==0) server.printP(domingo);
    else if (dia==1) server.printP(lunes);
      else if (dia==2) server.printP(martes);
        else if (dia==3) server.printP(miercoles);
          else if (dia==4) server.printP(jueves);
            else if (dia==5) server.printP(viernes);
              else if (dia==6) server.printP(sabado);
  }

void printMes(WebServer &server, byte mes)
  {
    if (mes==0) server.printP(enero);
      else if (mes==1) server.printP(febrero);
        else if (mes==2) server.printP(marzo);
          else if (mes==3) server.printP(abril);
            else if (mes==4) server.printP(mayo);
              else if (mes==5) server.printP(junio);
                else if (mes==6) server.printP(julio);
                  else if (mes==7) server.printP(agosto);
                    else if (mes==8) server.printP(septiembre);
                      else if (mes==9) server.printP(octubre);
                        else if (mes==10) server.printP(noviembre);
                          else if (mes==11) server.printP(diciembre);
  }
  
boolean autOK(WebServer &server)
 {
    char codeori[41]="";
    char code64[41]="";
    strcpy(codeori,user1);
    strcat(codeori,":");
    strcat(codeori,pass1);
    int largo=strlen(codeori);  
    base64_encode(code64,codeori,largo);
    if ((usepass) && (!server.checkCredentials(code64)))
      return false;
    else
      return true;
 }
 
void printparOnOff(WebServer &server,const prog_uchar *texto,int codigo)
  {
  printP(server,tr,td,texto);
  printP(server,td_f,td);
  printP(server,hrefon);
  server.print(codigo);
  printP(server,ona_f,td_f,td,hrefoff);
  server.print(codigo);
  printP(server,offa_f,td_f,tr_f);
  }
  
//******************************************************************************************************  
void outputPins(WebServer &server, WebServer::ConnectionType type)
{ 
//    
 
  if (!autOK(server))  {server.httpUnauthorized(); return; }
      
  if (type == WebServer::HEAD) return;
  server.httpSuccess();

  printP(server,htmlHead_i0, htmlHead_ix, htmlHead_i1);
  printP(server,seguimiento);
  if (peract != 0)   {
    server.printP(htmlRefresh_i);
    printIP(server, peract,10,comillascierre);
    }
  printP(server, head_f, body_i,tablamenu, trcolor1_i);
  printP(server, opc01,  opc19, opc18, opc13, tr_f, table_f,brn);
  printP(server,tablapanel);

//     if (tabpin[i]==pON) {val=isEve[tON][0];    printP(server,contactoonoff);}

  printP(server,tr,td,encendido);
  printP(server,td_f);
  if (isEve[tON][0]==1)
    printP(server,th);
  else
    printP(server,td);
  printP(server,hrefon);
  server.print(101);
  printP(server,ona_f,td_f,td,hrefoff);
  server.print(101);
  printP(server,offa_f,td_f,tr_f);

  printP(server,tr,td,Modo,td_f,td);    // modo actual
  if (isModo==0) printP(server,tcalor);
  if (isModo==2) printP(server,tfrio);
  if (isModo==3) printP(server,tacs);
  if (isModo==1) printP(server,tpiscina);
  printP(server,td_f,tr_f);
    printP(server,tr,td,estado,td_f,td);
    if (ESTADO==isPARADO) printP(server,Parado);
    if (ESTADO==isFASE1) printP(server,fase1);
    if (ESTADO==isFASE2) printP(server,fase2);
    if (ESTADO==isFASE3) printP(server,fase3);
    if (ESTADO==isRUNNING) printP(server,running);
    if (ESTADO==isALARMA) 
      {
      printP(server,alarma);
      printP(server,b);
      char buff[6]="";
      server.print(itoa(isCodAlarma,buff,10));
      }
    printP(server,td_f,tr_f);
  
// TEMPERATURAS LOCALES
  byte val;
  for (int i=0; i<6; ++i)
    {
    printTDTR(server, 0, true, true);
    if (showN == 1) 
     {
      printparentesis(server,'s',i,10);
      for(int j = 0; j < 8; j++) {
        server.printP(b);
        server.print(addr1Wire[i][j], HEX);
        }
     }
    server.printP(b);
    if (i==0) printP(server,retcalefaccion);
    if (i==1) printP(server,idacalefaccion);
    if (i==2) printP(server,sondaexterior);
    if (i==3) printP(server,sondaauxiliar);
    if (i==4) printP(server,retcaptador);
    if (i==5) printP(server,idacaptador);
    printTDTR(server, 1, false, true);
    printColspan(server,2,true);
    
    if (i==0) server.print(isST1);
    if (i==1) server.print(isST2);
    if (i==2) server.print(isST3);
    if (i==3) server.print(isST4);
    if (i==4) server.print(isST5);
    if (i==5) server.print(isST6);
//    server.print(float(Mb.R[i])/100);
    
    printP(server, celsius);     
    printTDTR(server, 1, true, true);
    }
  printP(server, tr, tdcolspan3);   
  if (estadoLCD==0)
   if (RTC.read(tm)) 
    { 
    printDayName(server, tm.Wday);
    server.printP(comablanco);      
    if (tm.Day < 10) server.printP(cero); server.print (tm.Day); server.printP(barra);
    printMes(server,tm.Month);
    server.printP(barra);
    server.print (tmYearToCalendar(tm.Year)); server.printP(blancos4);
    if (tm.Hour < 10) server.printP(cero); server.print (tm.Hour); server.printP(dospuntos);
    if (tm.Minute < 10) server.printP(cero); server.print (tm.Minute); server.printP(dospuntos);
    if (tm.Second < 10) server.printP(cero); server.print (tm.Second);
    }
  else    
    if (RTC.chipPresent())      
      printP(server,rtcparado);
    else 
      printP(server,rtcnodisponible);
  printP(server,td_f,tr_f,table_f, body_f, html_f);
}

void estadobcHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{ 
//    
  if (!autOK(server))  {server.httpUnauthorized(); return; }
      
  if (type == WebServer::HEAD) return;
  server.httpSuccess();

  printP(server,htmlHead_i0, htmlHead_ix, htmlHead_i1);
  printP(server,seguimiento);
  if (peract != 0)   {
    server.printP(htmlRefresh_i);
    printIP(server, peract,10,comillascierre);
    }
  printP(server, head_f, body_i,tablamenu, trcolor1_i);
  printP(server, opc11, opc09, opc18, opc13, tr_f, table_f,brn);
  
  printP(server,tablapanel);

    printP(server,tr,td,estado,td_f,td);
    if (ESTADO==isPARADO) printP(server,Parado);
    if (ESTADO==isFASE1) printP(server,fase1);
    if (ESTADO==isFASE2) printP(server,fase2);
    if (ESTADO==isFASE3) printP(server,fase3);
    if (ESTADO==isRUNNING) printP(server,running);
    if (ESTADO==isALARMA) 
      {
      printP(server,alarma);
      printP(server,b);
      char buff[6]="";
      server.print(itoa(isCodAlarma,buff,10));
      }
    printP(server,td_f,tr_f);

    printP(server,tr,td,Modo,td_f,td);
    if (isModo==0) printP(server,tcalor);
    if (isModo==2) printP(server,tfrio);
    if (isModo==3) printP(server,tacs);
    if (isModo==1) printP(server,tpiscina);
    printP(server,td_f,tr_f);

// TEMPERATURAS LOCALES
  byte val;
  for (int i=0; i<6; ++i)
    {
    printTDTR(server, 0, true, true);
    if (showN == 1) 
     {
      printparentesis(server,'s',i,10);
      for(int j = 0; j < 8; j++) {
        server.printP(b);
        server.print(addr1Wire[i][j], HEX);
        }
     }
    server.printP(b);
    if (i==0) printP(server,retcalefaccion);
    if (i==1) printP(server,idacalefaccion);
    if (i==2) printP(server,sondaexterior);
    if (i==3) printP(server,sondaauxiliar);
    if (i==4) printP(server,retcaptador);
    if (i==5) printP(server,idacaptador);
    printTDTR(server, 1, false, true);
    printColspan(server,2,true);
    
    if (i==0) server.print(isST1);
    if (i==1) server.print(isST2);
    if (i==2) server.print(isST3);
    if (i==3) server.print(isST4);
    if (i==4) server.print(isST5);
    if (i==5) server.print(isST6);
//    server.print(float(Mb.R[i])/100);
    
    printP(server, celsius);     
    printTDTR(server, 1, true, true);
    }
  
// ENTRADAS ANALÓGICAS
//  for (byte i=0; i<16; ++i)    
//    if (getbit8(bshowpin,i+41) == 1)
//     {  
//      printTDTR(server, 0, true, true);
////      if (showN == 1) printparentesis(server,'a',tabpin[i],10);
//      if (showN == 1) printparentesis(server,'a',i,10);
//      printEE (server, dirEEdescpinA+(i*LEN20), LEN20);
//      printTDTR(server, 1, false, true);
//      printColspan(server,2,true);
//      server.print(float(Mb.R[i+baseAna])*factorA[i]); 
//      printP(server, b);
//      printTDTR(server, 1, true, true);
//     }

// ENTRADAS DIGITALES
    printP(server,tr,td,entradasdigitales,td_f,tr_f);
    for (byte i = 0; i<13; ++i)
      if (getbit8(bshowpin,i) == 1)
        { 
        printP(server,tr,td);
        if (showN == 1) printparentesis(server,'d',tabpin[i],10);
        if (tabpin[i]==pON) {val=isEve[tON][0];    printP(server,contactoonoff);}
        if (tabpin[i]==pTer) {val = isEve[tTer][0]; printP(server,termostato);}
        if (tabpin[i]==pAcs) {val = isEve[tAcs][0]; printP(server,contactoacs);}
        if (tabpin[i]==pFri) {val = isEve[tFri][0]; printP(server,contactofrio);}
        if (tabpin[i]==pPis) {val = isEve[tPis][0]; printP(server,contactopiscina);}
        if (tabpin[i]==pPta) {val = isEve[tPta][0]; printP(server,presostatoalta);}
        if (tabpin[i]==pPtb) {val = isEve[tPtb][0]; printP(server,presostatobaja);}
        if (tabpin[i]==pFx1) {val = isEve[tFx1][0]; printP(server,fluxostatocal);}
        if (tabpin[i]==pFx2) {val = isEve[tFx2][0]; printP(server,fluxostatocap);}
        if (tabpin[i]==pFx3) {val = isEve[tFx3][0]; printP(server,fluxostatocf);}
        printP(server, td_f);
        printP(server,(val==0) ? td : th, (val==0) ? OFF : ON, (val==0) ? td_f : th_f);
//        if (i<=7)  {  // contadores
//          printP(server,td);
//          printPiP(server,resetcontp,i,barramayor);
//          server.print(contadores[i]);
//          printP(server,a_f,td_f);     }
       }

// SALIDAS DIGITALES
  printP(server,tr,td,reles,td_f,tr_f);
  if (numOD > 0)   
    {
    for (byte i = 17; i < nDIG-1; ++i)
      if (getbit8(bshowpin,i) == 1)
       { 
       if (tabpin[i]<=30)
         {
          char buf[3];
          itoa(tabpin[i],buf,10);
          printP(server,tr, td);
          if (showN == 1) printparentesis(server,'d',tabpin[i],10);
          if (tabpin[i]==22) {val = isRBCCal; printP(server,rbombacal);}
          if (tabpin[i]==23) {val = isRBCCap; printP(server,rbombacap);}
          if (tabpin[i]==24) {val = isRCOMP; printP(server,rcompresor);}
          if (tabpin[i]==25) {val = isRACS; printP(server,racs);}
          if (tabpin[i]==26) {val = isRV3V; printP(server,rv3v);}
          if (tabpin[i]==27) {val = isRRes; printP(server,rresistencia);}
          if (tabpin[i]==28) {val = isRV4V; printP(server,rv4v);}
          if (tabpin[i]==30) {val = isRALARM; printP(server,ralarma);}
          printP(server,(val==0) ? td:th, (val==0) ? OFF:ON, (val==0) ? td_f:th_f);
          }
        }
      }
// VALORES AUXILIARES
    printP(server,tr,td,valoresinternos,td_f,tr_f);
    for (byte i = 0; i < 9; ++i)
       { 
        char buf[3];
        itoa(tabpin[i],buf,10);
        printP(server,tr, td);
        if (i==0) {printP(server,consignacalor,td_f,td);server.print(CONSIGNA);}
        if (i==1) {printP(server,consignafrio,td_f,td);server.print(isFR);}
        if (i==2) {printP(server,consignapiscina,td_f,td);server.print(isPIS);}
        if (i==3) {printP(server,segdesdeinireg,td_f,td);server.print(isSegIniReg);}
        if (i==4) {printP(server,segdesdeinibcal,td_f,td);server.print(isSegIniBCal);}
        if (i==5) {printP(server,segdesdeinibcap,td_f,td);server.print(isSegIniBCap);}
        if (i==6) {printP(server,segdesdeinicomp,td_f,td);server.print(isSegIniComp);}
        if (i==7) {printP(server,segdesdefincomp,td_f,td);server.print(isSegFinComp);}
        if (i==8) {printP(server,segdesalarmatemp,td_f,td);server.print(isSegIniAlarmaTemp);}
        printP(server,td_f,tr_f);
        }

  printP(server, tr, tdcolspan3);   

  if (estadoLCD==0)
   if (RTC.read(tm)) 
    { 
    printDayName(server, tm.Wday);
    server.printP(comablanco);      
    if (tm.Day < 10) server.printP(cero); server.print (tm.Day); server.printP(barra);
    printMes(server,tm.Month);
    server.printP(barra);
    server.print (tmYearToCalendar(tm.Year)); server.printP(blancos4);
    if (tm.Hour < 10) server.printP(cero); server.print (tm.Hour); server.printP(dospuntos);
    if (tm.Minute < 10) server.printP(cero); server.print (tm.Minute); server.printP(dospuntos);
    if (tm.Second < 10) server.printP(cero); server.print (tm.Second);
    }
  else    
    if (RTC.chipPresent())      
      printP(server,rtcparado);
    else 
      printP(server,rtcnodisponible);
  printP(server,td_f,tr_f, trcolor1_i,tdcolspan3, letrav, b);
  server.print(miversion);
  printP(server,punto);
  server.print(misubver);
  printP(server, b,elconuco,b);
  server.print(mifreeRam());
  printP(server,barra);
  server.print(velocidad);
  printP(server, td_f, tr_f,table_f, body_f, html_f);
}

void writeMenu(WebServer &server, byte opcprin, byte opcsec)
  {
  server.printP(htmlHead_i0);
  server.printP(htmlHead_ix);
  server.printP(htmlHead_i1);
  printP(server,head_f,body_i,tablamenu,trcolor1_i);    // formato menú
  if (opcprin==1) printP(server,opc01); else printP(server,opc11);  // PANEL    
  if (opcprin==3) printP(server,opc03); else printP(server,opc13);  // CONFIGURACIÓN
  if (opcprin==2) printP(server,opc02); else printP(server,opc12);  // PROGRAMACIÓN
  if (opcprin==4) printP(server,opc04); else printP(server,opc14);  // AVANZADO
  if (opcprin==8) printP(server,opc08); else printP(server,opc18);  // BOMBA CALOR
  if (opcprin==9) printP(server,opc09); else printP(server,opc19);  // estado BOMBA CALOR
  printP(server,tr_f,table_f);
  
  printP(server,tablamenu,trcolor1_i);    // formato menú
  if (opcprin==1)    // PANEL
    {
    }
  if (opcprin==2)    // PROGRAMACIÓN
    {
    if (opcsec==5) printP(server,opc025); else printP(server,opc125);  // Semanal
    if (opcsec==1) printP(server,opc021); else printP(server,opc121);  // Semanal
    if (opcsec==2) 
      if (modo==5)    // modo clima
        printP(server,opc022clima); 
      else
        printP(server,opc022); 
    else 
       if (modo==5)    // modo clima
         printP(server,opc122clima); 
       else
          printP(server,opc122);  // Eventos
    if (opcsec==3) printP(server,opc023); else printP(server,opc123);  // Fechas
    if (opcsec==4) printP(server,opc024); else printP(server,opc124);  // Escenas
    }
  if (opcprin==8)    // BOMBA CALOR
    {
    if (opcsec==1) printP(server,opc081); else printP(server,opc181);  // Nivel 1
    if (opcsec==2) printP(server,opc082); else printP(server,opc182);  // Nivel 2
    if (opcsec==3) printP(server,opc083); else printP(server,opc183);  // Nivel 3
    }
  if (opcprin==3)    // CONFIGURACIÓN
    {
    if (opcsec==0) printP(server,opc030); else printP(server,opc130);  // Dispositivo
    if (opcsec==1) printP(server,opc031); else printP(server,opc131);  // Sondas Temp.
    if (opcsec==7) printP(server,opc037); else printP(server,opc137);  // E/S Analog
    if (opcsec==8) if (modo==5) printP(server,opc038clima); else printP(server,opc038); 
    else  if (modo==5) printP(server,opc138clima); else printP(server,opc138); 
    if (opcsec==9) printP(server,opc039); else printP(server,opc139);  // Sal. Digitales
    if (opcsec==3) printP(server,opc033); else printP(server,opc133);  // Red  
    if (opcsec==5) printP(server,opc035); else printP(server,opc135);  // Seguridad  
    if (opcsec==6) printP(server,opc036); else printP(server,opc136);  // Fecha/Hora
    }
  if (modo==0)  // Avanzado
    if (opcprin==4)    // AVANZADO
      {
      if (opcsec==1) printP(server,opc041); else printP(server,opc141);  // logs
      if (opcsec==2) printP(server,opc042); else printP(server,opc142);  // files
      if (opcsec==5) printP(server,opc045); else printP(server,opc145);  // logs
      if (opcsec==6) printP(server,opc046); else printP(server,opc146);  // files
      if (opcsec==3) printP(server,opc043); else printP(server,opc143);  // show EEPROM
      if (opcsec==4) printP(server,opc044); else printP(server,opc144);  // reinicia EEPROM
      }
   printP(server,tr_f,table_f);
  }

void setupPrgHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp = 2;    // número de parámetros por fila

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    int indice,resto,mival;
    memset(actPrg,0,sizeof(actPrg));
    do   
      {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      indice = param_number / mp;
      resto = param_number % mp;
      mival = atoi(value);
      if (resto==0) 
        for (int j=0; j<LEN20;j++) 
          EEwrite(dirEEdescPrg+(indice*LEN20)+j, value[j]);
      if (resto==1) actPrg[indice]=mival;
      } while (repeat);

    EEPROM_writeAnything(dirEEactprg, actPrg);
    server.httpSeeOther(PREFIX "/sprg.html"); 
    return;
    }
  
  server.httpSuccess();
  writeMenu(server, 2, 5);
  printP(server,brn,Form_action,setupprg,Form_post,Form_input_send);
  printP(server,tablaconf,trcolor1_i,td_if);
  printTDP(server, descripcion, false, true);
  printTDP(server, activo, false, true);
  printP(server,tr_f);
  char buf[3];
  for (int i=0;i<5;i++) 
    {
      int mpi=mp*i;
      printP(server,trcolor1_i);
      printP(server,td,programa,b);
      server.print(i+1);                   
      printP(server,td_f);
      
      if (actPrg[i]==1) printP(server,th); else printP(server,td);
      printP(server,Form_input_text_start);      
      server.print(mpi);    // número de parámetro                  
      server.printP(Form_input_value);        
      printEE (server, dirEEdescPrg+(i*LEN20), LEN20);
      printPiP(server,Max_length,LEN20-1,size_i);
      server.print(LEN20-1);
      printP(server,Form_input_end);
      if (actPrg[i]==1) printP(server,th_f); else printP(server,td_f);
      
      miCheckBox(server, "", (actPrg[i]==1), itoa(mpi+1,buf,10), "1", (actPrg[i]==1),true);
    }
  server.printP(tr_f);
  server.printP(table_f);
  
  //print the send button
  printP(server,Form_input_send,Form_f,body_f,html_f);  
}

void setupEveHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp = 15;    // número de parámetros por fila
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    boolean enANA;
    int indice; int resto;
    for (int i=0; i<nPRGeve;i++) for (int j=0; j<4;j++) bPRGeve[i][j]=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      indice =(param_number / mp);
      resto= (param_number % mp);
      int mival = atoi(value);
      if (param_number<999)    
          {      
//          if (resto==0) setbit8(bacteve,indice,mival);
          if (resto==1) setbit8(beveacttipo,indice,mival);  // Tipo activadora: local o remota
          if (resto==2) evenact[indice] = mival;            // señal activadora 
          if (resto==3) evenvalD[indice] = mival;           // valor activador si digital
          if (resto==4) evencomp[indice]=mival;             // tipo comparador: meno-igual o mayor-igual 
          if (resto==5) evenvalA[indice] = atof(value)*100; // valor consigna 
          if (resto==6) evenhis[indice] = atof(value)*100;  // histéresis  
          if (resto==7) setbit8(bevesaltipo,indice,mival);  // Tipo salida: local o remota
          if (resto==8) evensal[indice] = mival;            // señal a activar
          if (resto==9) setbit8(bevenniv,indice, mival);    // valor que debe tomar la señal de salida
          if (resto>=10) {setbit8(bPRGeve[resto-10],indice,mival);  }   // asociacion PRG
          }
      } while (repeat);
    guardarDatosEventos();
    server.httpSeeOther(PREFIX "/sv.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server, 2,2);
  printP(server, brn,Form_action, Form_eve, Form_post, Form_input_send);
  
  printP(server,tablaconf);
  server.printP(trcolor1_i);
  printP(server,td,evento,td_f,tdcolspan5,asociaraprograma,tdcolspan2,disparador,td_f);

  printTDP(server, dig, false, true);
  server.printP(tdcolspan3);
  printTDP(server, ana, false, false);
  server.printP(td_f);
  server.printP(tdcolspan3);
  printTDP(server, b, false, false);
  server.printP(td_f);
  server.printP(tr_f);

  server.printP(trcolor1_i);
  printTDP(server, n, false, true);
  printTDP(server, uno, false, true);
  printTDP(server, dos, false, true);
  printTDP(server, tres, false, true);
  printTDP(server, cuatro, false, true);
  printTDP(server, cinco, false, true);
  printTDP(server, lor, false, true);
  printTDP(server, senal, false, true);
  printTDP(server, onoff, false, true);
  printTDP(server, compa, false, true);
  printTDP(server, valor, false, true);
  printTDP(server, hist, false, true);
  printTDP(server, lor, false, true);
  printTDP(server, senalsalida, false, true);
  printTDP(server, onoff, false, true);
  server.printP(tr_f);
// 
/////////////////////////////////////////////////////////
  char buf[3];
  for (byte i=0;i<nPRG;i++)     
    {
    boolean colorea = ((getbit8(bPRGeve[0],i)==1) || (getbit8(bPRGeve[1],i)==1) ||
                       (getbit8(bPRGeve[2],i)==1) || (getbit8(bPRGeve[3],i)==1) ||
                       (getbit8(bPRGeve[4],i)==1));
    boolean actlocal= (getbit8(beveacttipo,i)==0);
    boolean actdigital= (evenact[i] <= 32) || ((evenact[i] >= 71) && (evenact[i] <= 170));
    boolean sallocal= (getbit8(bevesaltipo,i)==0);

    int  mpi = mp * i;
    int indice = (i*12)+420;  // parámetros del 420/426 en adelante
    printP(server,tr);   

    if (colorea) printP(server,th); else printP(server,td);
    server.print(i+1);
    if (colorea) printP(server,th_f); else printP(server,td_f);
    
    for (byte j=0;j<5;j++)
      miCheckBox(server, "",(getbit8(bPRGeve[j],i)==1),itoa(mpi+10+j,buf,10),"1",getbit8(bPRGeve[j],i)==1,true);

    if (colorea)
      {
      if (colorea) printP(server,th); else printP(server,td);
      if (modo==2)
        printP(server,Local);
      else
        printcampoSiNo(server, mpi+1, getbit8(beveacttipo,i), Local,Local, true, false);    // valor local/remota
      if (colorea) printP(server,th_f); else printP(server,td_f);

      if (colorea) printP(server,th); else printP(server,td);
      server.printP(Select_name);
      server.print(mpi+2);   server.printP(barramayor);
      if (actlocal) // señal activadora es local
        {
        for (byte j=0; j<16; j++)  {  // añade entradas digitales locales
          printPiP(server, optionvalue, j, barraatras);
          if (j == evenact[i]) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          }
        for (byte j=17; j<33; j++)  {  // añade salidas digitales locales
          printPiP(server, optionvalue, j, barraatras);
          if (j == evenact[i]) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          }
        for (byte j=0; j<nANALO; j++)  {  // añade entradas analógicas locales
          printPiP(server, optionvalue, j+33, barraatras);
          if (j+33 == evenact[i]) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j+33, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinA+(j*LEN20), LEN20);
          }
        for (byte j=0; j<maxTemp; j++)   // añade temperaturas locales
          if (getbit8(bshowTemp,j) == 1)        {             
            printPiP(server, optionvalue, j+171, barraatras);
            if (j+171 == evenact[i]) server.printP(selected);
            if (showN == 1) printPiP(server, mayorparen, j+171, parenguion); else server.printP(cierre);
            printEE (server, dirEEdescTemp+(j*LEN20), LEN20);
            }
        for (byte j=0; j<1; j++)   // añade precio kwh
          if (getbit8(bshowTemp,j) == 1)        {             
            printPiP(server, optionvalue, j+254, barraatras);
            if (j+254 == evenact[i]) server.printP(selected);
            if (showN == 1) printPiP(server, mayorparen, j+254, parenguion); else server.printP(cierre);
            printP(server,preciokwh);
            }
          }
      printP(server,  Select_f, td_f);
      if (actdigital)    // la señal activadora es digital
        {
        printP(server,th);   // valor ON/OFF que dispara
        printcampoSiNo(server, mpi+3, evenvalD[i], on, off,true,false);
        server.printP(th_f); 
        printP(server,th,th_f,th,th_f,th,th_f);
        }
      else
        {
        printP(server,th,th_f);
        printP(server,th);   // tipo comparación 
        server.printP(Select_name);
        server.print(mpi+4);   server.printP(barramayor);
        for (byte j=0; j<2; j++)  {  // añade salidas posibles
          printPiP(server, optionvalue, j, barraatras);
          if (evencomp[i]==j) server.printP(selected);
          server.printP(cierre);
          server.printP((j==0)?mayoroigual:menoroigual);
          server.printP(option_f);  
          }
        printP(server,  Select_f,th_f);
        printP(server,th);   // valor analÃ³gico que dispara 
        printcampoF(server,mpi+5,float(evenvalA[i])/100, 5,2, false);
        printP(server,th_f); 
        printP(server,th);    // valor histÃ©resis que dispara 
        printcampoF(server,mpi+6,float(evenhis[i])/100, 5,2, false);
        printP(server,th_f); 
        }
      if (colorea) printP(server,th_f); else printP(server,td_f);
      printP(server,th);
      if (modo==2)
        printP(server,Local);
      else
        printcampoSiNo(server, mpi+7, getbit8(bevesaltipo,i), Local,Local, true, false);    // valor local/remota
      printP(server,th_f,th);
      server.printP(Select_name);
      server.print(mpi+8);   server.printP(barramayor);  // señal de salida
      if (sallocal)    // salida a activar es local
        {
        for (byte j=17; j<33; j++)   {
          printPiP(server,optionvalue,j,barraatras);
          if (evensal[i] == j) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          }  
        for (byte m=0; m<3; m++)  {  // añade eventos correo
          server.printP(optionvalue); server.print(m+despMail); server.printP(barraatras);
          if (m+despMail == evensal[i]) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, m+despMail, parenguion); else server.printP(cierre);
          server.print(mailDEST[m]); }
        }
      printP(server, option_f, Select_f,th_f);
      
      server.printP(th);    // Nivel de la salida
      printcampoSiNo(server, mpi+9, getbit8(bevenniv,i), on,off, true, false);
      server.printP(th_f);    // Nivel de la salida
      }
    }
  
    
  server.printP(table_f);
  printP(server, Form_input_send,Form_f,body_f,html_f);
}

void setupClimaHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp = 15;    // número de parámetros por fila
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    boolean enANA;
    int indice; int resto;
    for (int i=0; i<nPRGeve;i++) for (int j=0; j<4;j++) bPRGeve[i][j]=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      indice =(param_number / mp);
      resto= (param_number % mp);
      int mival = atoi(value);
      if (param_number<999)    
          {      
//          if (resto==0) setbit8(bacteve,indice,mival);
          if (resto==1) setbit8(beveacttipo,indice,mival);  // Tipo activadora: local o remota
          if (resto==2) evenact[indice] = mival;            // señal activadora 
          if (resto==3) evenvalD[indice] = mival;           // valor activador si digital
          if (resto==4) evencomp[indice]=mival;             // tipo comparador: meno-igual o mayor-igual 
          if (resto==5) evenvalA[indice] = atof(value)*100; // valor consigna 
          if (resto==6) evenhis[indice] = atof(value)*100;  // histéresis  
          if (resto==7) setbit8(bevesaltipo,indice,mival);  // Tipo salida: local o remota
          if (resto==8) evensal[indice] = mival;            // señal a activar
          if (resto==9) setbit8(bevenniv,indice, mival);    // valor que debe tomar la señal de salida
          if (resto>=10) {setbit8(bPRGeve[resto-10],indice,mival);}   // asociacion PRG
          }
      } while (repeat);
    guardarDatosEventos();
    server.httpSeeOther(PREFIX "/sclima.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server, 2,2);
  printP(server, brn,Form_action, Form_clima, Form_post, Form_input_send);
  
  printP(server,tablaconf);

  server.printP(trcolor1_i);
  printTDP(server, n, false, true);
  printTDP(server, letraa, false, true);
  printTDP(server, circuito, false, true);
  printTDP(server, onoff, false, true);
  printTDP(server, termostato, false, true);
  printTDP(server, onoff, false, true);
  server.printP(tr_f);
// 
/////////////////////////////////////////////////////////
  char buf[3];
  for (byte i=0;i<nPRG;i++)     
    {
    boolean colorea = ((getbit8(bPRGeve[0],i)==1));
    boolean actlocal= (getbit8(beveacttipo,i)==0);
    boolean actdigital= (evenact[i] <= 32) || ((evenact[i] >= 71) && (evenact[i] <= 170));
    boolean sallocal= (getbit8(bevesaltipo,i)==0);

    int  mpi = mp * i;
    int indice = (i*12)+420;  // parámetros del 420/426 en adelante
    printP(server,tr);   

    if (colorea) printP(server,th); else printP(server,td);
    server.print(i+1);
    if (colorea) printP(server,th_f); else printP(server,td_f);
      
  //    for (byte j=0;j<5;j++)
    for (byte j=0;j<1;j++)
      miCheckBox(server, "",(getbit8(bPRGeve[j],i)==1),itoa(mpi+10+j,buf,10),"1",getbit8(bPRGeve[j],i)==1,true);

    if (colorea)
      {
        printP(server,th);
        server.printP(Select_name);
        server.print(mpi+8);   server.printP(barramayor);  // señal de salida
        if (sallocal)    // salida a activar es local
          {
          for (byte j=17; j<33; j++)   {
            printPiP(server,optionvalue,j,barraatras);
            if (evensal[i] == j) server.printP(selected);
            if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
            printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
            }  
          }
        printP(server, option_f, Select_f,th_f);
        
        server.printP(th);    // Nivel de la salida
        printcampoSiNo(server, mpi+9, getbit8(bevenniv,i), on,off, true, false);
        server.printP(th_f);    // Nivel de la salida
      }
  
    if (colorea)
      {
      printP(server,th); 
      server.printP(Select_name);
      server.print(mpi+2);   server.printP(barramayor);
      if (actlocal) // señal activadora es local
        {
        for (byte j=0; j<16; j++)  {  // añade entradas digitales locales
          printPiP(server, optionvalue, j, barraatras);
          if (j == evenact[i]) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          }
        }
      printP(server,  Select_f, td_f);
      if (actdigital)    // la señal activadora es digital
        {
        printP(server,th);   // valor ON/OFF que dispara
        printcampoSiNo(server, mpi+3, evenvalD[i], on, off,true,false);
        server.printP(th_f); 
//        printP(server,th,th_f,th,th_f,th,th_f);
        }
      printP(server,th_f);
      }
    }
  
    
  server.printP(table_f);
  printP(server, Form_input_send,Form_f,body_f,html_f);
}

void pinVAL(int n, byte value)
  { 
    if (valorpin[digitalRead(n)] != value)
      {
       digitalWrite(n, valorpin[value]);
       setbit8(Mb.C8, n, value);
       EEwrite(dirEEestado+(n/8), Mb.C8[n/8]);    // el byte modificado
      }
  }

void pinVALcond(int n, byte value)
  { 
    if (digitalRead(n) != valorpin[value])
      {
       digitalWrite(n, valorpin[value]);
       setbit8(Mb.C8, n, value);
       EEwrite(dirEEestado+(n/8), Mb.C8[n/8]);    // el byte modificado
      }
  }
  
void onCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  int auxi = atoi(url_tail);
  if (auxi < nPINES)
    {
    pinVAL(auxi,1);
    }
  if (auxi==101)
    {
    isEve[tON][0]=1; 
    EEPROM_writeAnything (dirEEESTADO, isEve[tON][0]);  
    activaEstado(isFASE1); 
    showlcd(); 
    }
  server.httpSeeOther(PREFIX "/");
}

void offCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  int auxi = atoi(url_tail);
  if (auxi < nPINES)
    {
    pinVAL(auxi,0);
    }
  if (auxi==101)
    {
    isEve[tON][0]=0; 
    EEPROM_writeAnything (dirEEESTADO, isEve[tON][0]);  
    activaEstado(isPARADO); 
    showlcd(); 
    }
  server.httpSeeOther(PREFIX "/");
}

boolean enviado=true;

void indexHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
  { 
//  Serial.print("index:");  Serial.println(url_tail);
  outputPins(server, type);  
  }

void leerDatosDevice()   {
  for (byte i=0; i<LEN20; i++) user1[i] = EEread(dirEEuser+i);
  for (byte i=0; i<LEN20; i++) pass1[i] = EEread(dirEEpass+i);
  
  EEPROM_readAnything (dirEEmodo, modo);
  EEPROM_readAnything (dirEEprectemp, prectemp);
  EEPROM_readAnything (dirEEnumIA, numIA);
  EEPROM_readAnything (dirEEnumID, numID);
  EEPROM_readAnything (dirEEnumOD, numOD);
  EEPROM_readAnything (dirEEshown, showN);
  EEPROM_readAnything (dirEEperact, peract);
  EEPROM_readAnything (dirEEusepass, usepass);
  EEPROM_readAnything (dirEEESTADO, isEve[tON][0]);
//  for (int i=0; i<8; i++)
//    Mb.C8[i]=EEread(dirEEestado+i);    // los 8 primeros bytes de Mb.C8=bestado
  EEPROM_readAnything (dirEEbshowpin, bshowpin);
  EEPROM_readAnything (dirEEtipoED, tipoED);
  EEPROM_readAnything (dirEEshowTemp, bshowTemp);
  EEPROM_readAnything (dirEEshowTemp, bshowTemp);
  EEPROM_readAnything (dirEEshowTemp, bshowTemp);
//  EEPROM_readAnything (dirEEfactorA, factorA);

  if (numIA > nANALO) numIA = nANALO;
  if (numIA+numID > nPINES) numID = nPINES - numIA;
  if (numIA+numID+numOD > nPINES) numOD = nPINES - numIA - numID;
  numPines = numIA+numID+numOD;
  EEPROM_readAnything(dirEEactprg, actPrg);
  EEPROM_readAnything(dirEEpmactVar, pmactVar);
  EEPROM_readAnything(dirEEtipoLCD, tipoLCD);
  EEPROM_readAnything(dirEEbshowEsc, bshowEsc);
  }
//
void leerDatosRed()    {
    EEPROM_readAnything (dirEEmac, EEmac);
    EEPROM_readAnything (dirEEip, EEip);
    EEPROM_readAnything (dirEEmask, EEmask);
    EEPROM_readAnything (dirEEgw, EEgw);
    EEPROM_readAnything (dirEEdns, EEdns);
    EEPROM_readAnything (dirEEwebport, EEwebPort);
    EEPROM_readAnything (dirEEmailPER, mailPER);  
    EEPROM_readAnything (dirEEmailACT, mailACT);  
    EEPROM_readAnything (dirEEmailSVR, mailSVR);  
    EEPROM_readAnything (dirEEmailPORT, mailPORT);  
    EEPROM_readAnything (dirEEmailUSER, mailUSER);  
    EEPROM_readAnything (dirEEmailPASS, mailPASS);  
    EEPROM_readAnything (dirEEmailSENDER, mailSENDER);  
    EEPROM_readAnything (dirEEmailDEST, mailDEST);  
    EEPROM_readAnything (dirEEdyndnsHost, dyndnsHost);  
    EEPROM_readAnything (dirEEdyndnsUser, dyndnsUser);  
    EEPROM_readAnything (dirEEdyndnsPass, dyndnsPass);  
  }
  
void guardarDatosRed()    {
    EEPROM_writeAnything (dirEEmac, EEmac);
    EEPROM_writeAnything (dirEEip, EEip);
    EEPROM_writeAnything (dirEEmask, EEmask);
    EEPROM_writeAnything (dirEEgw, EEgw);
    EEPROM_writeAnything (dirEEdns, EEdns);
    EEPROM_writeAnything (dirEEwebport, EEwebPort);  
    EEPROM_writeAnything (dirEEmailPER, mailPER);  
    EEPROM_writeAnything (dirEEmailACT, mailACT);  
    EEPROM_writeAnything (dirEEmailSVR, mailSVR);  
    EEPROM_writeAnything (dirEEmailPORT, mailPORT);  
    EEPROM_writeAnything (dirEEmailUSER, mailUSER);  
    EEPROM_writeAnything (dirEEmailPASS, mailPASS);  
    EEPROM_writeAnything (dirEEmailSENDER, mailSENDER);  
    EEPROM_writeAnything (dirEEmailDEST, mailDEST);  
    EEPROM_writeAnything (dirEEdyndnsHost, dyndnsHost);  
    EEPROM_writeAnything (dirEEdyndnsUser, dyndnsUser);  
    EEPROM_writeAnything (dirEEdyndnsPass, dyndnsPass);  
  }
    
void leerDatosEventos()  {
  EEPROM_readAnything (dirEEevenact, evenact);
  EEPROM_readAnything (dirEEevencomp, evencomp);
  EEPROM_readAnything (dirEEevenvalA, evenvalA);
  EEPROM_readAnything (dirEEevenvalD, evenvalD);
  EEPROM_readAnything (dirEEevensal, evensal);
  EEPROM_readAnything (dirEEevenniv, bevenniv);
  EEPROM_readAnything (dirEEevenhis, evenhis);   
  EEPROM_readAnything (dirEEevenTemp, evenTemp);  
  EEPROM_readAnything (dirEEbeveacttipo, beveacttipo);  
  EEPROM_readAnything (dirEEbevesaltipo, bevesaltipo);  
  EEPROM_readAnything (dirEEbacteve, bacteve);
  EEPROM_readAnything (dirEEPRGeve, bPRGeve);
  }

void guardarDatosEventos()  {
  EEPROM_writeAnything (dirEEevenact, evenact);
  EEPROM_writeAnything (dirEEevencomp, evencomp);
  EEPROM_writeAnything (dirEEevenvalA, evenvalA);
  EEPROM_writeAnything (dirEEevenvalD, evenvalD);
  EEPROM_writeAnything (dirEEevensal, evensal);
  EEPROM_writeAnything (dirEEevenhis, evenhis);
  EEPROM_writeAnything (dirEEevenniv, bevenniv);  
  EEPROM_writeAnything (dirEEevenTemp, evenTemp);  
  EEPROM_writeAnything (dirEEbeveacttipo, beveacttipo);  
  EEPROM_writeAnything (dirEEbevesaltipo, bevesaltipo);  
  EEPROM_writeAnything (dirEEbacteve, bacteve);
  EEPROM_writeAnything (dirEEPRGeve, bPRGeve);
  }
    
void guardarDatosFechas()  {
  EEPROM_writeAnything (dirEEfecsal, fecsal);
  EEPROM_writeAnything (dirEEfecval, bfecval);
  EEPROM_writeAnything (dirEEfecdia, fecdia);
  EEPROM_writeAnything (dirEEfecmes, fecmes);
  EEPROM_writeAnything (dirEEfecano, fecano);
  EEPROM_writeAnything (dirEEfechor, fechor);
  EEPROM_writeAnything (dirEEfecmin, fecmin);    
  EEPROM_writeAnything (dirEEbfectipo, bfectipo);    
  EEPROM_writeAnything (dirEEbactfec, bactfec);
}  
  
void leerDatosFechas()  {
  EEPROM_readAnything (dirEEfecsal, fecsal);
  EEPROM_readAnything (dirEEfecdia, fecdia);
  EEPROM_readAnything (dirEEfecmes, fecmes);
  EEPROM_readAnything (dirEEfecano, fecano);
  EEPROM_readAnything (dirEEfechor, fechor);
  EEPROM_readAnything (dirEEfecmin, fecmin);    
  EEPROM_readAnything (dirEEbfectipo, bfectipo);    
  EEPROM_readAnything (dirEEbactfec, bactfec);
}  

void leerDatosSemanal()  {
  EEPROM_readAnything (dirEEprgsal, prgsal);
  EEPROM_readAnything (dirEEprgval, bprgval);
  EEPROM_readAnything (dirEEprgdia, prgdia);
  EEPROM_readAnything (dirEEprghor, prghor);
  EEPROM_readAnything (dirEEprgmin, prgmin);
  EEPROM_readAnything (dirEEprgcon, prgcon);
//  EEPROM_readAnything (dirEEprgconv, prgconv);
  EEPROM_readAnything (dirEEprgcomp, prgcomp);
  EEPROM_readAnything (dirEEbprgtipo, bprgtipo);
  EEPROM_readAnything (dirEEbactsem, bactsem);
  EEPROM_readAnything (dirEEPRGsem, bPRGsem);
  }

void guardarDatosSemanal()  {
  EEPROM_writeAnything (dirEEprgsal, prgsal);
  EEPROM_writeAnything (dirEEprgval, bprgval);
  EEPROM_writeAnything (dirEEprgdia, prgdia);
  EEPROM_writeAnything (dirEEprghor, prghor);
  EEPROM_writeAnything (dirEEprgmin, prgmin);
  EEPROM_writeAnything (dirEEprgcon, prgcon);
//  EEPROM_writeAnything (dirEEprgconv, prgconv);
  EEPROM_writeAnything (dirEEprgcomp, prgcomp);
  EEPROM_writeAnything (dirEEbprgtipo, bprgtipo);
  EEPROM_writeAnything (dirEEbactsem, bactsem);
  EEPROM_writeAnything (dirEEPRGsem, bPRGsem);
  }
  
void leerEstado() {
  for (int i=0; i<8; i++)
    Mb.C8[i+32]=EEread(dirEEestado+i);    // los 8 primeros bytes de Mb.C8=bestado
  EEPROM_readAnything (dirEEcontadores, contadores);
  }

void miCheckBox (WebServer &server, char *label, boolean colorea, char *name, char *valor, boolean checked, boolean contd)
  {
    if (contd) printP(server,(colorea?th:td));
    server.checkBox(name,valor,label,checked);
    if (contd) printP(server,(colorea?th_f:td_f));
  }

/*********************************************************************************************/
void setupNetHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN40];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    mailACT=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN40);
      if (!repeat) break;
      param_number = atoi(name);        
        if (param_number >=0 && param_number <=5) EEmac[param_number]=strtol(value,NULL,16);       
        if (param_number >=6 && param_number <=9) EEip[param_number-6]=atoi(value);       
        if (param_number >=10 && param_number <=13) EEmask[param_number-10]=atoi(value);
        if (param_number >=14 && param_number <=17) EEgw[param_number-14]=atoi(value); 
        if (param_number >=18 && param_number <=21) EEdns[param_number-18]=atoi(value);
        if (param_number == 22) EEwebPort=atoi(value); 

        if (param_number == 23) mailACT=atoi(value); 
        if (param_number == 24) mailPER=atoi(value); 
        if (param_number == 25) for (byte i=0; i<LEN40; i++) mailSVR[i] = value[i];    
        if (param_number == 26) mailPORT = atoi(value); 
        if (param_number == 27) for (byte i=0; i<LEN40; i++) mailUSER[i] = value[i];        
        if (param_number == 28) for (byte i=0; i<LEN40; i++) mailPASS[i] = value[i];        
        if (param_number == 29) for (byte i=0; i<LEN40; i++) mailSENDER[i] = value[i];        
        if (param_number == 30) for (byte i=0; i<LEN40; i++) mailDEST[0][i] = value[i];        
        if (param_number == 31) for (byte i=0; i<LEN40; i++) mailDEST[1][i] = value[i];        
        if (param_number == 32) for (byte i=0; i<LEN40; i++) mailDEST[2][i] = value[i];        
        if (param_number == 34) for (byte i=0; i<LEN40; i++) dyndnsHost[i] = value[i];        
        if (param_number == 35) for (byte i=0; i<LEN40; i++) dyndnsUser[i] = value[i];        
        if (param_number == 36) for (byte i=0; i<LEN40; i++) dyndnsPass[i] = value[i];        

      } while (repeat);
    guardarDatosRed();  //  Guardar datos red en EEPROM
    server.httpSeeOther(PREFIX "/sNe.html"); return;
    }

  server.httpSuccess();
  writeMenu(server,3,3);
  printP(server,brn,Form_action,netSetup,Form_post,Form_input_send,tablanormal);
  
  server.printP(tr); 
  printTDP(server, MAC, false,true);
  server.printP(td);
  for (byte i=0;i<6;i++) printcampoH(server, i, EEmac[i], 2,false);
  printTDTR(server, 1, true, true);

  // print the current IP
  server.printP(tr);  
  printTDP(server, DIRIP, false,true);
  server.printP(td);    
  for (byte i=0; i<4; i++) printcampoI(server, i+6, EEip[i], 3,false);
  printTDTR(server, 1, true, true);
  
  // print the current SUBNET
  server.printP(tr);  
  printTDP(server, SUBNET,false,true);
  server.printP(td); 
  for (byte i=0;i<4;i++) printcampoI(server, i+10, EEmask[i], 3,false);
  printTDTR(server, 1, true, true);
  
  // print the current GATEWAY
  server.printP(tr);  
  printTDP(server, GATEWAY,false,true);
  server.printP(td); 
  for (byte i=0;i<4;i++) printcampoI(server, i+14, EEgw[i], 3,false);
  printTDTR(server, 1, true, true);
  
  // print the current DNS-SERVER
  server.printP(tr);  
  printTDP(server, DNS_SERVER,false,true);
  server.printP(td); 
  for (byte i=0;i<4;i++) printcampoI(server, i+18, EEdns[i], 3,false);
  printTDTR(server, 1, true, true);

  printparIP(server,WEB_PORT, 22, EEwebPort, 5);
  lineaH(server, 2);
  server.printP(tr);  
  printTDP(server, actenviomail, false, true);
  miCheckBox(server,"",mailACT==1,"23","1",mailACT==1,true);
  server.printP(tr_f); 
  
  printparIP(server, smtpPER, 24, mailPER, 2);
  printparCP(server, smtpSVR, 25, mailSVR, 40,false);
  printparIP(server, smtpPORT, 26, mailPORT, 5);
  printparCP(server, smtpUSER, 27, mailUSER, 40,false);
  printparCP(server, smtpPASS, 28, mailPASS, 40,true);
  printparCP(server, smtpREMI, 29, mailSENDER, 40,false);
  printparCP(server, smtpDEST1, 30, mailDEST[0], 40,false);
  printparCP(server, smtpDEST2, 31, mailDEST[1], 40,false);
  printparCP(server, smtpDEST3, 32, mailDEST[2], 40,false);
  lineaH(server, 2);
 
  printP(server,tr,td,dyndnsserv,td_f,td);
  printPiP(server,Select_name,33,barramayor);  // número de parámetro
  for (byte j=0; j<2; j++)  
    {
    printPiP(server,optionvalue,j,barraatras);
    if (dyndnsServ == j) server.printP(selected);
    server.printP(cierre);
    if (j==0) server.printP(noip);
    if (j==1) server.printP(dyndns);
    server.printP(option_f);
    }  
  printP(server,td_f,tr);             // modo de funcionamiento
//  printparCP(server, dyndnsserv, 33, dyndnsServ, 40,false);
  printparCP(server, dyndnshost, 34, dyndnsHost, 40,false);
  printparCP(server, dyndnsuser, 35, dyndnsUser, 40,false);
  printparCP(server, dyndnspass, 36, dyndnsPass, 40,false);

  printP(server, table_f,Form_input_send,Form_f,body_f); 
  server.printP(html_f);
}

void setupDevHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    showN=0;
    debugserial=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      byte mival = atoi(value);
      if (param_number == 0) showN=mival;  
      if (param_number == 1) peract=mival;
      if (param_number == 2) debugserial=mival;
      if (param_number == 3) prectemp=mival;
      if (param_number == 5) modo = mival; 
      if (param_number == 6) pmactVar = atol(value); 
      if (param_number == 7) tipoLCD = mival; 
      } while (repeat);
      
    //// guardar
    if (numIA > nANALO) numIA = nANALO;
    if (numIA+numID > nPINES) numID = nPINES - numIA;
    if (numIA+numID+numOD > nPINES) numOD = nPINES - numIA - numID;
    numPines = numIA+numID+numOD;

    EEPROM_writeAnything (dirEEnumIA, numIA);        
    EEPROM_writeAnything (dirEEnumID, numID);
    EEPROM_writeAnything (dirEEnumOD, numOD);
    EEPROM_writeAnything (dirEEshown, showN);
    EEPROM_writeAnything (dirEEperact, peract);
    EEPROM_writeAnything (dirEEprectemp, prectemp);
    EEPROM_writeAnything (dirEEmodo, modo);
    EEPROM_writeAnything (dirEEpmactVar, pmactVar);
    EEPROM_writeAnything (dirEEtipoLCD, tipoLCD);
    if (modo==2)
      {
      for (int i=0; i<4; i++) {bfectipo[i]=0;  }
        guardarDatosFechas();
      for (int i=0; i<4; i++) {
        bactsem[i]=0;
        bacteve[i]=0;}
        guardarDatosEventos();
      for (int i=0; i<4; i++) {bprgtipo[i]=0;  }
        guardarDatosSemanal();
      }
    
    server.httpSeeOther(PREFIX "/sd.html"); 
    }
  
  server.httpSuccess();
  writeMenu(server,3,0);
  printP(server,brn,Form_action,setupDev,Form_post,tablanormal);

  printP(server,tr,td);             // modo de funcionamiento
  printP(server,Modo);
  printP(server,td_f,td);
  printPiP(server,Select_name,5,barramayor);  // número de parámetro
  for (byte j=0; j<6; j++)  
    {
    printPiP(server,optionvalue,j,barraatras);
    if (modo == j) server.printP(selected);
    server.printP(cierre);
    if (j==0) server.printP(avanzado);
    if (j==1) server.printP(basico);
    if (j==2) server.printP(local);
    if (j==3) server.printP(domotica);
    if (j==4) server.printP(riego);
    if (j==5) server.printP(climatizacion);
    server.printP(option_f);
    }  
  printP(server,td_f,tr);             // modo de funcionamiento

  printP(server,tr,td,sN,td_f);
  if (showN==1) printP(server,th); else printP(server,td);
  server.checkBox("0","1","",(showN==1));
  if (showN==1) printP(server,th_f); else printP(server,td_f);
  printP(server,tr_f);
  
    
  printparIP(server, periodoact, 1, peract, 5);
  
  printP(server,tr,td,debugserie,td_f);
  if (debugserial==1) printP(server,th); else printP(server,td);
  server.checkBox("2","1","",(debugserial==1));
  if (debugserial==1) printP(server,th_f); else printP(server,td_f);
  printP(server, tr_f);
  
  printP(server,tr);
  printTDP(server,precisionsondas,false,true);
  printcampoCB(server,3,prectemp,9,12,true,true); 
  printP(server,tr_f);

  printparLP(server, tmactVar, 6, pmactVar, 5);
  
  printP(server,table_f,Form_input_send,Form_f,body_f,html_f);
  
}

P(pageesp8266) = "esp8266.html";
P(comando)="Comando";
P(GetConfig)="GET Config";
P(SetConfig)="SET Config";
P(RESET)="Reset";
P(GetCIPMode)="Get CIP mode";
P(GetCIPStatus)="Get CIP Status";
P(CipStart)="CIP Start";
P(GetCWMode)="Get CW mode";
P(GetCWLif)="Get CWLIF";
P(SetCWMode)="Set CW mode";
P(GetCWSAP)="Get CWSAP (AP parm.)";
P(SetCWSAP)="Set CWSAP (AP parm.)";
P(GetAP)="Get AP List";
P(GetIP)="Get IP";
P(GetMUX)="Get MUX";
P(JoinAP)="Join AP";
P(QuitAP)="Quit AP";
P(GetVer)="Get Version";
P(cwmode)="CW MODE";  
P(cwmode1)="STA";  
P(cwmode2)="AP";  
P(cwmode3)="STA+AP";  
P(wifiSSid)="Connect to: SSID";
P(wifiPass)="Connect to: PASS";
P(mySSid)="AP SSID";
P(myPass)="AP PASS";
P(CipClose)="CIP Close";
P(GetCIOBaud)="Get CIO Baud";
P(tcpserver)="TCP server";
P(tcpport)="TCP port";
P(GetHTTP)="Get HTTP";
P(OpenServer)="Open Server";
P(CloseServer)="Close Server";
P(sendgjson)="Send Get /json";
P(send1a10)="Send 1234567890";
P(noencriptyon)="No encryption";   
P(wep)="WEP";   
P(wpapsk)="WPA_PSK";   
P(wpa2psk)="WPA2_PSK";   
P(wpawpa2psk)="WPA_WPA2_PSK";   
P(lineadoble)="============";

void ShowLogCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  server.httpSuccess();
  writeMenu(server,4,1);
 
  printP(server, brn, tablanormal);
  
  printP(server, tr);
  printP(server, td,sistema,td_f);
  printP(server, versistema, delsistema);
  printP(server,tr_f);

  printP(server, tr);
  printP(server, td,acciones,td_f);
  printP(server, veracciones, delacciones);
  printP(server,tr_f);

  printP(server, tr);
  printP(server, td,Ent_temp,td_f);
  printP(server, verEnt_temp, delEnt_temp);
  printP(server,tr_f);

  printP(server, tr);
  printP(server, td,Ent_ana, td_f);
  printP(server, verEnt_ana, delEnt_ana);
  printP(server,tr_f);

  printP(server, tr);
  printP(server, td,eventos,td_f);
  printP(server, vereventos, deleventos);
  printP(server,tr_f);

  printP(server,table_f,body_f,html_f);
}

byte actdescr=0;

void (*resetFunc)(void)=0;

void resetHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int auxpar=0;
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      byte mival = atoi(value);
      if (param_number == 0) auxpar=1; 
      if (param_number == 1) resetfab=1; 
      } while (repeat);
      
    //// reset nodo 
    if (auxpar==1)
      if (resetfab==1)
        iniciavalores();          // reset fabrica  
      else
        resetFunc();          // reset simple  
    server.httpSeeOther(PREFIX "/rst.html"); 
    return;
    }
  
  server.httpSuccess();
  writeMenu(server,4,4);
  printP(server,brn,Form_action,setupSys,Form_post);
  printP(server,tablanormal,trcolor1_i);
  printColspan(server,3,true);
  printP(server,h3_i,resetsystem,h3_f);
  printTDTR(server, 1, true, true);

  printP(server,td,resetsystem,td_f);
  printP(server,td);
  server.checkBox("0","1","",(auxpar==1));
  printP(server,td_f,td,td_f,tr);

  resetfab=0;
  printP(server,tr,td,resetfabrica,td_f);
  printP(server,td);
  server.checkBox("1","1","",(resetfab==1));
  printP(server,td_f,td);
  printP(server,avisoresetfab);
  printP(server,td_f,tr_f);

  printP(server,table_f);
  printP(server,Form_input_exec,Form_f,body_f); 
  server.printP(html_f);
}

void setupSondHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    for (byte i=0; i<3;i++) bshowTemp[i]=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      int indice1 = ((param_number)/3); 
      int indice2 = ((param_number+1)/3); 
      int resto = param_number % 3;
      byte mival = atoi(value);
      if (param_number >= nDIG*3)        
        if (param_number<200)   // TEMPERATURAS 1-WIRE LOCALES
          {
          if (resto == 1)        // descripción
            for (byte i=0; i<LEN20; i++)  
              EEwrite(dirEEdescTemp+((indice2-nDIG)*LEN20)+i, value[i]);
          if (resto == 2) setbit8(bshowTemp, indice1-nDIG, mival);  // Mostrar si/no   
          }
      } while (repeat);
      
    //// guardar
    EEPROM_writeAnything (dirEEshowTemp, bshowTemp);
    server.httpSeeOther(PREFIX "/st.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server,3,1);
  printP(server,brn,Form_action,setupSond,Form_post,Form_input_send,tablaconf);
  
  printP(server,trcolor1_i);
  printColspan(server,2,true);
  server.printP(sondastemperatura); 
  printP(server,td_f);
  printColspan(server,2,true);
  printP(server,b);
  printTDTR(server, 1, true, true);
  printP(server,trcolor1_i,td_if);
  printTDP(server, descripcion, false, true);
  printTDP(server, ver, false, true);
  printP(server,tr_f); 

  char buf[5];
// TEMPERATURAS 1-WIRE LOCALES
  for (byte i=0; i<maxTemp; i++) 
    {
    printP(server,tr);
    if ((getbit8(bshowTemp,i)==1)) printP(server,th); else printP(server,td);
    server.printP(paren_i);
    printPiP(server,letras,i,paren_f1b); 
    printP(server,b,sondatemp,b);
    server.print(i+1); 
    printP(server,td_f);
    
    if ((getbit8(bshowTemp,i)==1)) printP(server,th); else printP(server,td);
    printP(server,Form_input_text_start);      
    server.print(((i+nDIG)*3)+1);                   
    server.printP(Form_input_value);        
//    int dir=i;
    printEE (server, dirEEdescTemp+(i*LEN20), LEN20);
    printPiP(server,Max_length,LEN20-1,size_i);
    server.print(LEN20-1);
    printP(server,Form_input_end,td_f);
    
    if (getbit8(bshowTemp,i)==1) printP(server,th); else printP(server,td);
    server.checkBox(itoa(((i+nDIG)*3)+2,buf,10),"1","",(getbit8(bshowTemp,i)==1));
    if (getbit8(bshowTemp,i)==1) printP(server,th_f); else printP(server,td_f);

    printP(server,tr_f);
    }
  server.printP(table_f);
  printP(server,Form_input_send,Form_f,body_f,html_f);
}

void setupEAnagHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp=5;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    for (int i=0; i<nANALO;i++) setbit8(bshowpin,i+41,0);
    for (int i=0; i<2;i++) setbit8(bsumatA,i,0);
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      int indice = param_number / mp;
      int resto = param_number % mp;
      byte mival = atoi(value);
      if (resto == 0)        // descripción
        for (byte i=0; i<LEN20; i++)  
          EEwrite(dirEEdescpinA+(indice*LEN20)+i, value[i]);
      if (resto == 1) setbit8(bshowpin, indice+41, mival);   // Mostrar si/no panel   
      if (resto == 2) factorA[indice] = atof(value); 
      if (resto == 3) offsetA[indice] = atof(value); 
      if (resto == 4) setbit8(bsumatA, indice, mival); 
      } while (repeat);
      
    //// guardar
    EEPROM_writeAnything (dirEEbshowpin, bshowpin);
    EEPROM_writeAnything (dirEEfactorA, factorA);
    EEPROM_writeAnything (dirEEoffsetA, offsetA);
    EEPROM_writeAnything (dirEEsumatA, bsumatA);
    server.httpSeeOther(PREFIX "/sea.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server,3,7);
  printP(server,brn,Form_action,setupEAnag,Form_post,Form_input_send,tablaconf);
  
  char buf[5];
    
// E/S LOCALES ANALÓGICAS
  printP(server,trcolor1_i);
  printColspan(server,8,true);
  server.printP(eanalogicas); printP(server,td_f,tr_f);
  
  printP(server,trcolor1_i,td_if); 
  printTDP(server, descripcion, false, true);
  printTDP(server, ver, false, true);
  printTDP(server, factor, false, true);
  printTDP(server, offset, false, true);
  printTDP(server, sumat, false, true);
  server.printP(tr_f);  
  for (byte i=0; i<nANALO; i++)  
    {
    int mpi=mp*i;
    printP(server, tr);
    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    server.printP(paren_i);
    printPiP(server,letraa,i,paren_f1b); 
    printP(server,b,entana,b);
    server.print(i+1); 
    printP(server,b);
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);
    
    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    printP(server,Form_input_text_start);      
    server.print(mpi);                   
    server.printP(Form_input_value);        
    int dir=i;
    printEE (server, dirEEdescpinA+(dir*LEN20), LEN20);
    printPiP(server,Max_length,LEN20-1,size_i);
    server.print(LEN20-1);
    printP(server,Form_input_end);
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);

    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    server.checkBox(itoa(mpi+1,buf,10),"1","",(getbit8(bshowpin,i+41)==1));
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);
    
    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    printcampoF(server,mpi+2, factorA[i], 10,5, false);
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);
    
    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    printcampoF(server,mpi+3, offsetA[i], 10,5, false);
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);
    
    if (getbit8(bshowpin,i+41)==1) printP(server,th); else printP(server,td);
    server.checkBox(itoa(mpi+4,buf,10),"1","",(getbit8(bsumatA,i)==1));
    if (getbit8(bshowpin,i+41)==1) printP(server,th_f); else printP(server,td_f);
    
    printTDTR(server, 1, true, false);
    
    }
  printP(server,table_f,Form_input_send,Form_f,body_f,html_f);
}

void setupEdigHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp=3;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    for (int i=0;i<16;i++)
      setbit8(bshowpin,i,0);
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      int indice1 = ((param_number)/mp); 
      int indice2 = ((param_number+1)/mp); 
      int resto = param_number % mp;
      byte mival = atoi(value);
      if (param_number < nDIG*mp)        
        {  
        if (resto == 0)        // descripción
          for (byte i=0; i<LEN20; i++) 
            EEwrite(dirEEdescpinD+(indice2*LEN20)+i, value[i]);
            
        if (resto == 1) tipoED[indice1]=mival;                 // Mostrar si/no panel 
        if (resto == 2) setbit8(bshowpin, indice1, mival);     // Mostrar si/no panel 
        }
      } while (repeat);
      
    //// guardar
//    EEPROM_writeAnything (dirEEshowTemp, bshowTemp);
    EEPROM_writeAnything (dirEEtipoED, tipoED);
    EEPROM_writeAnything (dirEEbshowpin, bshowpin);
    server.httpSeeOther(PREFIX "/sed.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server,3,8);
  printP(server,brn,Form_action,setupEDig,Form_post,Form_input_send,tablaconf);

  char buf[5];
    
// E. LOCALES DIGITALES
  printP(server,trcolor1_i);
  printColspan(server,4,true);
  if (modo==5) printP(server,termostatos); else printP(server,edigitales);
  printTDTR(server, 1, true, true);
  
  printP(server,trcolor1_i,td_if); 
  printTDP(server, descripcion, false, true);
  printTDP(server, tipo, false, true);
  printTDP(server, ver, false, true);
  printP(server,tr_f);  
  for (byte i=0; i<16; i++)  
    {
    int mpi=mp*i;
    printP(server,tr);
    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    server.printP(paren_i);
    printPiP(server,letrad,tabpin[i],paren_f1b); 
    server.printP((i<17)?entdig:saldig);
    printPiP(server,b,(i<17)?i+1:i-17+1,b);
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);

    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    printP(server,Form_input_text_start);      
    server.print((mpi));  // número parámetro
    server.printP(Form_input_value);        
    int dir=i;
    printEE (server, dirEEdescpinD+(dir*LEN20), LEN20);
    printPiP(server,Max_length,LEN20-1,size_i);
    server.print(LEN20-1);
    printP(server,Form_input_end);
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);

    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    printPiP(server,Select_name,mpi+1,barramayor);
    for (byte j=0; j<4; j++)  
      {
      printPiP(server,optionvalue,j,barraatras);
      if (tipoED[i]==j) server.printP(selected);
      if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
      if (j==0) server.printP(ONOFF);
      if (j==1) server.printP(dht11);
      if (j==2) server.printP(dht21);
      if (j==3) server.printP(dht22);
      server.printP(option_f);
      }  
    printP(server,Select_f);
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);

    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    server.checkBox(itoa(mpi+2,buf,10),"1","",(getbit8(bshowpin,i)==1));
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);
    printTDTR(server, 1, true, false);
    }
  printP(server,table_f,Form_input_send ,Form_f,body_f,html_f);
}

void setupSdigHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    for (int i=17;i<33;i++)
      setbit8(bshowpin,i,0);
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);  
      int indice1 = ((param_number)/3); 
      int indice2 = ((param_number+1)/3); 
      int resto = param_number % 3;
      byte mival = atoi(value);
      if (param_number < nDIG*3)        {  // E/S LOCALES
        if (resto == 1)        // descripción
          for (byte i=0; i<LEN20; i++)  
            EEwrite(dirEEdescpinD+(indice2*LEN20)+i, value[i]);
        if (resto == 2) setbit8(bshowpin, indice1, mival);  // Mostrar si/no panel   
        }
      } while (repeat);
      
    //// guardar
//    EEPROM_writeAnything (dirEEshowTemp, bshowTemp);
    EEPROM_writeAnything (dirEEbshowpin, bshowpin);
    server.httpSeeOther(PREFIX "/ssd.html"); return;
    }
  
  server.httpSuccess();
  writeMenu(server,3,9);
  printP(server,brn,Form_action,setupSDig,Form_post,Form_input_send,tablaconf);
  
  char buf[5];
  printP(server,trcolor1_i);
  printColspan(server,2,true);
  printP(server,sdigitales);
  printP(server,td,b);
  printTDTR(server, 1, true, true);
  
  printP(server,trcolor1_i,td_if); 
  printTDP(server, descripcion, false, true);
  printTDP(server, ver, false, true);
  server.printP(tr_f);  
// S LOCALES DIGITALES
  for (byte i=17; i<33; i++)  
    {
    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    server.printP(paren_i);
    printPiP(server,letrad,tabpin[i],paren_f1b); 
    server.printP((i<17)?entdig:saldig);
    printPiP(server,b,(i<17)?i+1:i-17+1,b);
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);

    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    printP(server,Form_input_text_start);      
    server.print((i*3)+1);  // nÃºmero parÃ¡metro
    server.printP(Form_input_value);        
    int dir=i;
    printEE (server, dirEEdescpinD+(dir*LEN20), LEN20);
    printPiP(server,Max_length,LEN20-1,size_i);
    server.print(LEN20-1);
    printP(server,Form_input_end);
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);

//    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
//    server.radioButton (itoa((i*3)+2,buf,10),"1","", (getbit8(bshowpin,i) ==1));  // número parámetro
//    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);
//    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
//    server.radioButton (itoa((i*3)+2,buf,10),"0","", (getbit8(bshowpin,i) ==0));
//    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);
//    
    if (getbit8(bshowpin,i)==1) printP(server,th); else printP(server,td);
    server.checkBox(itoa((i*3)+2,buf,10),"1","",(getbit8(bshowpin,i)==1));
    if (getbit8(bshowpin,i)==1) printP(server,th_f); else printP(server,td_f);
    
    printTDTR(server, 1, true, false);
    }
  printP(server,table_f,Form_input_send,Form_f,body_f,html_f);
}

void setupAvaHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  if (type == WebServer::HEAD) return;
  server.httpSuccess();
  writeMenu(server,4,0);
  printP(server,brn,body_f,html_f);
}

void setupSegHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  char pass1temp[LEN20]="";    // 20 bytes
  char pass2temp[LEN20]="";    // 20 bytes
  int param_number = 0;
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    usepass=0;
    opcode=0;
    do   
     {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);        
      if (param_number == 0) usepass=atoi(value);  
      if (param_number == 1) for (byte i=0; i<LEN20; i++) pass1temp[i] = value[i];      
      if (param_number == 2) for (byte i=0; i<LEN20; i++) pass2temp[i] = value[i];      
      if (param_number == 3) for (byte i=0; i<LEN20; i++) user1[i] = value[i];      
      } while (repeat);

    if (usepass)    // contraseña activa
      {
       if (strcmp(pass1temp,pass2temp) == 0)    // si coinciden ambas password se almacena
         {
          for (byte i=0; i<LEN20; i++) 
            {
              opcode=1;
              pass1[i]=pass1temp[i];
              EEwrite(dirEEuser+i, user1[i]); 
              EEwrite(dirEEpass+i, pass1temp[i]); 
            }
         }
       else
        {
         opcode=2;
         usepass=0;  // no se guarda y se desactiva contraseña
         }
      }
    else// contraseña NO activa
      if (strcmp(pass1temp,pass1)!= 0)    // si no se da la contraseña correcta, no se desactiva
        {
        opcode=3;
        usepass=1;
        }
    EEPROM_writeAnything (dirEEusepass, usepass);
    server.httpSeeOther(PREFIX "/ss.html"); return;
    }

/////////////////////
  server.httpSuccess();
  writeMenu(server,3,5);
  if (opcode!=99)
    {
    printP(server,brn);
    if (opcode==0) printP(server, operacionrealizada,brn);
    if (opcode==1) printP(server, contrasenaguardada,brn);
    if (opcode==2) printP(server, contrasenanocoincide,brn);
    if (opcode==3) printP(server, contrasenaincorrecta,brn);
    opcode=99;
    }
  printP(server,brn,Form_action,Form_seg,Form_post,tablaconf,tr); 
  printTDP(server, activarcontrasena, false,true);

  if (usepass==1) printP(server,th); else printP(server,td);
  server.checkBox("0","1","",(usepass==1));
  if (usepass==1) printP(server,th_f); else printP(server,td_f);
//  printparCP(server, contrasena, 1, pass1, 20,true);
  printparCP(server, usuario, 3, user1, 20,false);
  printparCP(server, contrasena, 1, "", 20,true);
  printparCP(server, confcontrasena, 2, "", 20,true);
  printP(server,table_f,Form_input_send,Form_f,body_f,html_f);
}

void setupRelHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
//
  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)
    {
    bool repeat;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
        param_number = atoi(name);   
        int mival = atoi(value);   
        if (param_number==0) tm.Wday = mival;  
        if (param_number==1) tm.Day = mival;   
        if (param_number==2) tm.Month = mival;   
        if (param_number==3) tm.Year = CalendarYrToTm(mival);  
        if (param_number==4) tm.Hour = mival;   
        if (param_number==5) tm.Minute = mival;  
        if (param_number==6) tm.Second = mival;      
      } while (repeat);
    if (RTC.write(tm)) delay(100);
    server.httpSeeOther(PREFIX "/srl.html"); return;
    }
//
  server.httpSuccess();
  writeMenu(server,3,6);
  printP(server,brn,Form_action,Form_rel,Form_post,tablaconf,tr);
  printTDP(server,diasemana,false,true);
  printColspan(server,3,true);
  printPiP(server,Select_name,0,barramayor);
  for (byte j=0; j<7; j++)  {
    printPiP(server,optionvalue,j,barraatras);
    if (j == tm.Wday) server.printP(selected);
    server.printP(cierre); 
    printDayName(server, j); 
    server.printP(option_f);  
    }
  server.printP(Select_f);  
  printTDTR(server, 1, true, false);   

  server.printP(tr); 
  printTDP(server,diamesano,false,true);
  printcampoCB(server, 1, tm.Day, 1, 31,true,true);
  printP(server,td);
  printPiP(server,Select_name,2,barramayor);
  for (byte j=0; j<12; j++)  {
    printPiP(server,optionvalue,j,barraatras);
    if (j == tm.Month) server.printP(selected);
    server.printP(cierre); 
    printMes(server,j);
    server.printP(option_f);  }
  printP(server,Select_f,td_f);   
  printcampoCB(server, 3, tmYearToCalendar(tm.Year), 2010, 2025,true,true);
  printP(server,tr_f,tr);
  printTDP(server, horaminseg,false,true);
  printcampoCB(server, 4, tm.Hour, 0, 23,true,true);
  printcampoCB(server, 5, tm.Minute, 0, 59,true,true);
  printcampoCB(server, 6, tm.Second, 0, 59,true,true);
  printP(server,tr_f,table_f); 
  
  //print the send button
  printP(server,Form_input_send,Form_f,body_f,html_f);
}

void printLCD(const prog_uchar *str)
  {
  char c;
   while((c = pgm_read_byte(str++)))
     lcd.write(c);
  }

void iniciavalores()
  {
  lcd.clear();  printLCD(inic);
  for (int i=0;i<16000;i++) 
    {
    EEwrite(i,0);  // borra EEPROM
    if ((i%1000)==0)
      Serial.println(i); 
    }
  
  printLCD(uno);
  modo=0;
    EEPROM_writeAnything (dirEEmodo, modo);  
  for (byte i=0; i<nPRG; i++) evenTemp[i]=inact;
    EEPROM_writeAnything (dirEEevenTemp, evenTemp);  
  EEmac[0]=170;EEmac[1]=171;EEmac[2]=172;EEmac[3]=173;EEmac[4]=174;EEmac[5]=133;
    EEPROM_writeAnything (dirEEmac, EEmac);
  EEip[0]=192;EEip[1]=168;EEip[2]=1;EEip[3]=178;
    EEPROM_writeAnything (dirEEip, EEip);
  EEmask[0]=255;EEmask[1]=255;EEmask[2]=255;EEmask[3]=0;
    EEPROM_writeAnything (dirEEmask, EEmask);
  EEgw[0]=192;EEgw[1]=168;EEgw[2]=1;EEgw[3]=1;
    EEPROM_writeAnything (dirEEgw, EEgw);
  EEdns[0]=8;EEdns[1]=8;EEdns[2]=8;EEdns[3]=8;
    EEPROM_writeAnything (dirEEdns, EEdns);
  EEwebPort=82; 
    EEPROM_writeAnything (dirEEwebport, EEwebPort);  
  for (byte i=0; i<20; i++) pass1[i]=0;    // pasword
    for (byte i=0; i<LEN20; i++) EEwrite(dirEEpass+i, pass1[i]); 
  usepass = 0;    // activar password NO
    EEPROM_writeAnything (dirEEusepass, usepass);
  for (int i=0;i<3; i++) bshowTemp[i]=0;
    EEPROM_writeAnything (dirEEshowTemp, bshowTemp);
  for (byte i=0; i<lTAB; i++) bfecval[i]=0;      // Valor a poner la salida en programación fechas
    EEPROM_writeAnything (dirEEfecval, bfecval);
  numIA = 16;     // número de entradas analÃ³gicas
    EEPROM_writeAnything (dirEEnumIA, numIA);
  numID = 16;     // número de entradas digitales
    EEPROM_writeAnything (dirEEnumID, numID);
  numOD = 16;     // número de salidas digitales
    EEPROM_writeAnything (dirEEnumOD, numOD);
  showN = 1;      // mostrar número de pin
    EEPROM_writeAnything (dirEEshown, showN);
  peract = 15;     // período de actualización automática de la página principal
    EEPROM_writeAnything (dirEEperact, peract);
  for (byte i=0; i<32; i++) Mb.C8[i]=0;          // estado de E/S locales y remotas
    EEPROM_writeAnything (dirEEestado, Mb.C8);
  printLCD(dos);
  for (int i=0; i<16; i++)  // descripciones de E digitales
    for (int j=0; j<20; j++) 
      EEwrite(dirEEdescpinD+(i*LEN20+j), 0);
  for (int i=17; i<34; i++)  // descripciones de S digitales
    for (int j=0; j<20; j++) 
      EEwrite(dirEEdescpinD+(i*LEN20+j), 0);
  printLCD(tres);
  for (int i=0; i<16; i++) {    // descripciones de E/S analógicas
    EEwrite(dirEEdescpinA+(i*LEN20), byte('E'));
    EEwrite(dirEEdescpinA+(i*LEN20)+1, byte('N'));
    EEwrite(dirEEdescpinA+(i*LEN20)+2, byte('T'));
    EEwrite(dirEEdescpinA+(i*LEN20)+3, byte(' '));
    EEwrite(dirEEdescpinA+(i*LEN20)+4, byte('A'));
    EEwrite(dirEEdescpinA+(i*LEN20)+5, byte('N'));
    EEwrite(dirEEdescpinA+(i*LEN20)+6, byte('A'));
    EEwrite(dirEEdescpinA+(i*LEN20)+7, byte(' '));
    if (i<9) 
      EEwrite(dirEEdescpinA+(i*LEN20)+8, char(i+49));
    else
      {
      EEwrite(dirEEdescpinA+(i*LEN20)+8, '1');
      EEwrite(dirEEdescpinA+(i*LEN20)+9, char(i+39));
      }
    }
  for (byte i=0; i<nPRG; i++) fecdia[i]=0;       // Día en programación fechas
    EEPROM_writeAnything (dirEEfecdia, fecdia);
  for (byte i=0; i<nPRG; i++) evenvalD[i]=0;  // todos los valores a 0
    EEPROM_writeAnything (dirEEevenvalD, evenvalD);
  for (int i=0; i<4; i++) {bprgtipo[i]=0;  }
    guardarDatosSemanal();
  for (int i=0; i<4; i++) {
    beveacttipo[i]=0;
    bevesaltipo[i]=0; }
  printLCD(cuatro);
  guardarDatosEventos();
  for (byte i=0; i<nPRG; i++) fecmin[i]=0;     // minuto
    EEPROM_writeAnything (dirEEfecmin, fecmin);     
  for (int i=0; i<nPRG; i++) evenact[i]=inact;
    EEPROM_writeAnything (dirEEevenact, evenact);
  for (byte i=0; i<nPRG; i++) evensal[i]=0;    // seÃ±al sobre la que se actÃºa en eventos
    EEPROM_writeAnything (dirEEevensal, evensal);
  for (byte i=0; i<nPRG; i++) fecsal[i]=inact;       // Salida a activar en programaciÃ³n fechas
    EEPROM_writeAnything (dirEEfecsal, fecsal);
  for (int i=0; i<4; i++) {bfectipo[i]=0;  }
    guardarDatosFechas();
  for (byte i=0; i<lTAB; i++) bevenniv[i]=0;    // nivel al que se activa la seÃ±al sobre la que se actÃºa en eventos
    EEPROM_writeAnything (dirEEevenniv, bevenniv);  
  printLCD(cinco);
  for (int i=0; i<20; i++) 
    {    // descripciones de sondas temperatura
    for (int j=0; i<20; i++) 
      EEwrite(dirEEdescpinD+(i*LEN20+j), 0);
    }
  for (int i=0; i<8; i++) bshowpin[i]=0;
    EEPROM_writeAnything (dirEEbshowpin, bshowpin);
  for (int i=0; i<16; i++) tipoED[i]=0;
    EEPROM_writeAnything (dirEEtipoED, tipoED);

  for (int i=0; i<4; i++) {
    bactsem[i]=0;
    bacteve[i]=0;}
    guardarDatosEventos();
  for (int i=0; i<4; i++) {bactfec[i]=0;}
    guardarDatosFechas();
  for (byte i=0; i<nPRG; i++) prgsal[i] = inact;    // todos desactivadas
    EEPROM_writeAnything (dirEEprgsal, prgsal);
  for (byte i=0; i<nPRG; i++) fecano[i]=13;      // Año en programación fechas
    EEPROM_writeAnything (dirEEfecano, fecano);
  for (byte i=0; i<lTAB; i++) bprgval[i] = 0;     // todos desactivadas a Off
    EEPROM_writeAnything (dirEEprgval, bprgval);
  for (byte i=0; i<nPRG; i++) prgdia[i] = 0;     // todos desactivadas 
    EEPROM_writeAnything (dirEEprgdia, prgdia);
  for (byte i=0; i<nPRG; i++) prghor[i] = 0;     // hora 0 
    EEPROM_writeAnything (dirEEprghor, prghor);
  for (byte i=0; i<nPRG; i++) prgmin[i] = 0;     // minuto 0 
    EEPROM_writeAnything (dirEEprgmin, prgmin);
  for (byte i=0; i<nPRG; i++) prgcon[i] = inact;    // todos desactivadas  
    EEPROM_writeAnything (dirEEprgcon, prgcon);
//  for (byte i=0; i<nPRG; i++) prgconv[i] = 0.0;   // todos a 0  
//    EEPROM_writeAnything (dirEEprgconv, prgconv);
  for (byte i=0; i<nPRG; i++) prgcomp[i] = 0;   // todos a 0  
    EEPROM_writeAnything (dirEEprgcomp, prgcomp);
  for (byte i=0; i<nPRG; i++) fechor[i]=0;       // Hora en programación fechas
    EEPROM_writeAnything (dirEEfechor, fechor);
   mailACT=0;                                    // mail desactivado
    EEPROM_writeAnything (dirEEmailACT, mailACT);  
  for (byte i=0; i<40; i++) mailSVR[i]=0;        // servidor smtp
    EEPROM_writeAnything (dirEEmailSVR, mailSVR);  
  mailPER=60;                                    // período entre mails
    EEPROM_writeAnything (dirEEmailPER, mailPER);  
  mailPORT=25;                                   // puerto servidor smtp
    EEPROM_writeAnything (dirEEmailPORT, mailPORT);  
  for (byte i=0; i<40; i++) mailUSER[i]=0;       // usuario smtp
    EEPROM_writeAnything (dirEEmailUSER, mailUSER);  
  for (byte i=0; i<40; i++) mailPASS[i]=0;       // password smtp
    EEPROM_writeAnything (dirEEmailPASS, mailPASS);  
  for (byte i=0; i<40; i++) mailSENDER[i]=0;     // remitente smtp
    EEPROM_writeAnything (dirEEmailSENDER, mailSENDER); 
  memset(mailDEST,0,sizeof(mailDEST)); 
    EEPROM_writeAnything (dirEEmailDEST, mailDEST);  
  for (byte i=0; i<nPRG; i++) evencomp[i]=0;     // comparador analógicas
    EEPROM_writeAnything (dirEEevencomp, evencomp);
  for (byte i=0; i<lTAB; i++) contadores[i]=0;   // contadores Entradas digitales 0 a 7 3
    EEPROM_writeAnything (dirEEcontadores, contadores);
  for (byte i=0; i<nPRG; i++) fecmes[i]=0;       // Mes en programación fechas
    EEPROM_writeAnything (dirEEfecmes, fecmes);
  printLCD(seis);
  for (byte i=0; i<nANALO; i++) factorA[i]=1.0;  
    EEPROM_writeAnything (dirEEfactorA, factorA);    
    
  for (int i=0; i<5; i++) {    // descripciones de escenas
    char buff[6]; memset(buff,0,sizeof(buff));
    char auxchr[LEN20]; memset(auxchr,0,sizeof(auxchr));
    strcatP(auxchr, escena);
    strcat(auxchr,itoa(i,buff,10));
    EEPROM_writeAnything (dirEEdescEsc+(i*LEN20), auxchr);    
    }
  
//  for (int i=0; i<5; i++) {    // descripciones de escenas
//    int pos=dirEEdescEsc+(i*LEN20);
//    EEwrite(pos, byte('E'));
//    EEwrite(pos+1, byte('s'));
//    EEwrite(pos+2, byte('c'));
//    EEwrite(pos+3, byte('e'));
//    EEwrite(pos+4, byte('n'));
//    EEwrite(pos+5, byte('a'));
//    EEwrite(pos+6, byte(' '));
//    EEwrite(pos+7, char(i+49));
//    EEwrite(pos+8, 0);
//    }
    
  for (int i=0; i<5; i++) {    // descripciones de programas
    int pos=dirEEdescPrg+(i*LEN20);
    EEwrite(pos, byte('P'));
    EEwrite(pos+1, byte('r'));
    EEwrite(pos+2, byte('o'));
    EEwrite(pos+3, byte('g'));
    EEwrite(pos+4, byte('r'));
    EEwrite(pos+5, byte('a'));
    EEwrite(pos+6, byte('m'));
    EEwrite(pos+7, byte('a'));
    EEwrite(pos+8, byte(' '));
    EEwrite(pos+9, char(i+49));
    EEwrite(pos+10, 0);
    }
    
//xxxxxxxxxxxxxxxxxxxx    
  for (byte i=0; i<nPRG; i++) evenvalA[i]=0.0;  // todos los valores a 0
    EEPROM_writeAnything (dirEEevenvalA, evenvalA);
  for (byte i=0; i<nPRG; i++) evenhis[i]=0;  // todos los valores a 0
    EEPROM_writeAnything (dirEEevenhis, evenhis);
  }
  
void iniciavaloresIP()
  {
  lcd.clear();  
  printLCD(iniciando);
  printLCD(blancoIP);
  EEmac[0]=170;EEmac[1]=171;EEmac[2]=172;EEmac[3]=173;EEmac[4]=174;EEmac[5]=175;
  EEip[0]=192;EEip[1]=168;EEip[2]=1;EEip[3]=178;
  EEmask[0]=255;EEmask[1]=255;EEmask[2]=255;EEmask[3]=0;
  EEgw[0]=192;EEgw[1]=168;EEgw[2]=1;EEgw[3]=1;
  EEdns[0]=8;EEdns[1]=8;EEdns[2]=8;EEdns[3]=8;
  EEwebPort=82; 

  printLCD(uno);
  guardarDatosRed();
  
  }
  
void resetcontador(WebServer &server, byte n, boolean panel)
{
  contadores[n]=0;
  EEPROM_writeAnything(dirEEcontadores+(4*n), contadores[n]);
  server.httpSeeOther(PREFIX "/"); 
}

void systemHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{ 
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  URLPARAM_RESULT rc;
  char name[LEN5];
  char value[LEN20];
  boolean params_present = false;
  
////////////////////////////////////////  

  if (type == WebServer::HEAD) return;
  // check for parameters
  if (strlen(url_tail)) 
    {
    while (strlen(url_tail)) {
      rc = server.nextURLparam(&url_tail, name, LEN5, value, LEN20);
      if (rc != URLPARAM_EOS) {
        params_present=true;
        if (strcmp(name, "act") == 0)    // actuación a realizar
          {
          if (strcmp (value, "r0") == 0) { resetcontador(server,0,false); return; }
          if (strcmp (value, "r1") == 0) { resetcontador(server,1,false); return; }
          if (strcmp (value, "r2") == 0) { resetcontador(server,2,false); return; }
          if (strcmp (value, "r3") == 0) { resetcontador(server,3,false); return; }
          if (strcmp (value, "r4") == 0) { resetcontador(server,4,false); return; }
          if (strcmp (value, "r5") == 0) { resetcontador(server,5,false); return; }
          if (strcmp (value, "r6") == 0) { resetcontador(server,6,false); return; }
          if (strcmp (value, "r7") == 0) { resetcontador(server,7,false); return; }
          if (strcmp (value, "rp0") == 0) { resetcontador(server,0,false); return; }
          if (strcmp (value, "rp1") == 0) { resetcontador(server,1,false); return; }
          if (strcmp (value, "rp2") == 0) { resetcontador(server,2,false); return; }
          if (strcmp (value, "rp3") == 0) { resetcontador(server,3,false); return; }
          if (strcmp (value, "rp4") == 0) { resetcontador(server,4,false); return; }
          if (strcmp (value, "rp5") == 0) { resetcontador(server,5,false); return; }
          if (strcmp (value, "rp6") == 0) { resetcontador(server,6,false); return; }
          if (strcmp (value, "rp7") == 0) { resetcontador(server,7,false); return; }
          if (strcmp (value, "rsteex") == 0)  // reinicia EEPROM
            { 
            server.httpSuccess();
            writeMenu(server,4,4);
            iniciavalores(); 
            printP(server,br,operacionrealizada);
            printP(server,body_f,html_f);  
            }
          if (strcmp (value, "shee") == 0)  // show EEPROM
            {
            server.httpSuccess();
            writeMenu(server,4,3);
            printP(server,brn,tablamenu,trcolor1_i);
            printColspan(server,17,true);
            printP(server,h3_i,volcadoeeprom,h3_f);
            printTDTR(server, 1, true, true); 
            printP(server,table_f,tablaconf,tralignright,th,Bytes,th_f); 
            for (byte i=0; i<=15; i++) printPiP(server,th,i,th_f);     
            server.printP(tr_f);
            for (int i=0; i<4096; i++)   {
              server.printP(tralignright);
              printPiP(server,td,i*16,guion); 
              unsigned int auxi=i*16+15;
              server.print (auxi); 
              server.printP(td_f); 
              for (int j=0; j<16; j++)     // DECIMAL
                printPiP(server,td,EEread((i*16)+j),td_f);  
              server.printP(tr_f);
              }
            server.printP(table_f);
            printP(server,body_f,html_f);  
            }
          if (strcmp (value, "backup") == 0)  // guarda EEPROM en fichero SD
            { 
            server.httpSuccess();
            writeMenu(server,4,5);
            printP(server,espere,brn);
            printP(server,body_f,html_f);  
            }
          if (strcmp (value, "restore") == 0)  // recupera EEPROM de fichero SD
            { 
            server.httpSuccess();
            writeMenu(server,4,6);
            printP(server,espere,brn);
            printP(server,body_f,html_f);  
            }
         }
        }
      }
    }
}

void errorHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{ server.httpFail();
  if (type == WebServer::HEAD) return;
  printP(server,Http400,body_f,html_f);  }

void setupSemHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  URLPARAM_RESULT rc;
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp = 18;    // número de parámetros por fila

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    int indice,resto,mival;
    for (int j=0; j<5;j++) for (int i=0;i<4;i++) bPRGsem[j][i]=0;
    for (int i=0; i<nPRG; i++) 
      {
        prgdia[i]=0;
        setbit8(bactsem,i,0);
      }
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
      param_number = atoi(name);     
      indice = param_number / mp;
      resto = param_number % mp;
      mival = atoi(value);
      if (resto==0) {setbit8(bactsem,indice,mival);  }    // Activo: sí/no
      if (resto==1) {setbit8(bprgtipo,indice,mival);  }    // Tipo: local/remoto
      if (resto==2) {prgsal[indice] = mival;  }            // Número de salida
      if (resto==3) {setbit8(bprgval, indice, mival);  }   // valor de la salida
      if (resto==4) {setbit(&prgdia[indice], 0, mival);  }  // domingo
      if (resto==5) {setbit(&prgdia[indice], 1, mival);  }  // lunes
      if (resto==6) {setbit(&prgdia[indice], 2, mival);  }
      if (resto==7) {setbit(&prgdia[indice], 3, mival);  }
      if (resto==8) {setbit(&prgdia[indice], 4, mival);  }
      if (resto==9) {setbit(&prgdia[indice], 5, mival);  }  // viernes
      if (resto==10) {setbit(&prgdia[indice], 6, mival);  }  // sábado
      if (resto==11) {prghor[indice] = mival;  }           // Hora
      if (resto==12) {prgmin[indice] = mival;  }          // Minuto
      if (resto>=13) {setbit8(bPRGsem[resto-13],indice,mival);  }   // asociacion PRG
      } 
    while (repeat);

    guardarDatosSemanal();
    server.httpSeeOther(PREFIX "/sse.html"); return;  }
  
  server.httpSuccess();
  writeMenu(server,2,1);
  printP(server,brn,Form_action,setupsem,Form_post,Form_input_send);
  printP(server,tablaconf,trcolor1_i);
  printP(server,td);
  printP(server,tdcolspan5,asociaraprograma); 
  printP(server,tdcolspan3);
  printP(server,salida,td_f,tdcolspan7,dias);
  printP(server,tdcolspan2,hora); 
  printP(server,tr_f);
  
  printP(server,trcolor1_i);
  printTDP(server, programa, false, true);
  printTDP(server, uno, false, true);
  printTDP(server, dos, false, true);
  printTDP(server, tres, false, true);
  printTDP(server, cuatro, false, true);
  printTDP(server, cinco, false, true);
  printTDP(server, lor, false, true);
  printTDP(server, nombre, false, true);
  printTDP(server, valor, false, true);
  printTDP(server, D, false, true);
  printTDP(server, L, false, true);
  printTDP(server, M, false, true);
  printTDP(server, X, false, true);
  printTDP(server, J, false, true);
  printTDP(server, V, false, true);
  printTDP(server, S, false, true);
  printTDP(server, hora, false, true);
  printTDP(server, minuto, false, true);
  server.printP(tr_f);
  char buf[5];
  for (int i=0; i<nPRG; i++) 
    {
    int mpi = mp*i;
    printP(server,tr); 
    boolean colorea=((getbit8(bPRGsem[0],i)==1) || 
                     (getbit8(bPRGsem[1],i)==1) || (getbit8(bPRGsem[2],i)==1) || 
                     (getbit8(bPRGsem[3],i)==1) || (getbit8(bPRGsem[4],i)==1));
    
    printP(server,(colorea?th:td));
    server.print(i+1);
    printP(server,(colorea?th_f:td_f));
    
    for (byte j=0;j<5;j++)
      miCheckBox(server,"",getbit8(bPRGsem[j],i),itoa(mpi+13+j,buf,10),"1",(getbit8(bPRGsem[j],i)==1),true);
      
    if (colorea)
      {
      printP(server,(colorea?th:td));
      if (modo==2)
        printP(server,Local);
      else
        printcampoSiNo(server, mpi+1, getbit8(bprgtipo,i),Local,Local, true, false);    // valor local/remota
      printP(server,(colorea?th_f:td_f));

      printP(server,(colorea?th:td));
      printPiP(server,Select_name,mpi+2,barramayor);
      if (getbit8(bprgtipo,i)==0)    // locales
        {
        for (byte j=17; j<33; j++)   {
          printPiP(server,optionvalue,j,barraatras);
          if (prgsal[i] == j) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          server.printP(option_f);  }  
        }
      printP(server,Select_f,td_f,(colorea?th_f:td_f));
      
      printP(server,(colorea?th:td));
      printcampoSiNo(server, mpi+3, getbit8(bprgval,i), on,off, true, false);    // valor on/off
      printP(server,(colorea?th_f:td_f));
      for (byte j=0; j<7;j++) {    // días de la semana
        if (getbit(prgdia[i],j)==1) printP(server,th); else printP(server,td);
        printPiP(server,inputcheckbox,mpi+4+j,value1);
        if (getbit(prgdia[i],j) == 1) server.printP(checked); 
        printP(server,cierre,br);
        if (getbit(prgdia[i],j)==1) printP(server,th_f); else printP(server,td_f);
       }
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+11, prghor[i], 0, 23,false,false);  // hora
      if (colorea) printP(server,th_f); else printP(server,td_f);
      
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+12, prgmin[i], 0, 59,false,false);  // minuto
      if (colorea) printP(server,th_f); else printP(server,td_f);
      
      }
    server.printP(tr_f);
    }
  server.printP(table_f);
  
  printP(server,Form_input_send,Form_f,body_f,html_f);  
}

void setupFecHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN20];
  int param_number = 0;
  int mp = 9;    // número de parámetros por fila

  if (type == WebServer::HEAD) return;
  
  if (type == WebServer::POST)   {
    bool repeat;
    int indice,resto,mival;
    for (int i=0;i<4;i++) bactfec[i]=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN20);
      if (!repeat) break;
        param_number = atoi(name);     
        indice = param_number / mp;
        resto = param_number % mp;
        mival = atoi(value);
        if (resto==0) setbit8(bactfec,indice,mival);    // programa activo si/no
        if (resto==1) setbit8(bfectipo,indice,mival);   // tipo de salida local/remota
        if (resto==2) fecsal[indice] = mival;           // Número de salida
        if (resto==3) setbit8(bfecval, indice, mival);  // valor de la salida
        if (resto==4) fecdia[indice] = mival;           // día
        if (resto==5) fecmes[indice] = mival;           // mes
        if (resto==6) fecano[indice] = mival-2000;      // año
        if (resto==7) fechor[indice] = mival;           // Hora
        if (resto==8) fecmin[indice] = mival;           // minuto
      } while (repeat);

    guardarDatosFechas();
    server.httpSeeOther(PREFIX "/sf.html"); 
      }
  
  server.httpSuccess();
  writeMenu(server,2,3);
  printP(server,brn,Form_action,setupfec,Form_post,Form_input_send);

  printP(server,tablaconf,trcolor1_i);
  printP(server,tdcolspan2,tdcolspan3,salida);
  printP(server,td_f,tdcolspan5,fechahora,td_f);
  
  printP(server,tr_f,trcolor1_i);
  printTDP(server, programa, false, true);
  printTDP(server, Act, false, true);
  printTDP(server, lor, false, true);
  printTDP(server, nombre, false, true);
  printTDP(server, valor, false, true);
  printTDP(server, dia, false, true);
  printTDP(server, mes, false, true);
  printTDP(server, ano, false, true);
  printTDP(server, hora, false, true);
  printTDP(server, minuto, false, true);
  server.printP(tr_f);
  char buf[3];
  for (byte i=0;i<nPRG;i++)      
    {
    int mpi = mp * i;
    boolean colorea=(getbit8(bactfec,i)==1);
    printP(server,tr);
    
    if (colorea) printP(server,th); else printP(server,td);
    server.print(i+1);
    if (colorea) printP(server,th_f); else printP(server,td_f);

    miCheckBox(server,"",colorea, itoa(mpi,buf,10), "1", getbit8(bactfec,i)==1,true);
    if (colorea)
      {  
      if (colorea) printP(server,th); else printP(server,td);
      if (modo==2)
        printP(server,Local);
      else
        printcampoSiNo(server, mpi+1, getbit8(bfectipo,i), Local,Local, true, false);    // valor local/remota
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printPiP(server,Select_name,mpi+2,barramayor);
      if (getbit8(bfectipo,i)==0)    // locales
        {
        for (byte j=17; j<33; j++)   {
          printPiP(server,optionvalue,j,barraatras);
          if (fecsal[i] == j) server.printP(selected);
          if (showN == 1) printPiP(server, mayorparen, j, parenguion); else server.printP(cierre);
          printEE (server, dirEEdescpinD+(j*LEN20), LEN20);
          server.printP(option_f);  }  
        }
      printP(server,Select_f);
      if (colorea) printP(server,th_f); else printP(server,td_f);
  
      if (colorea) printP(server,th); else printP(server,td);
      printcampoSiNo(server, mpi+3, getbit8(bfecval,i), on,off, true, false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+4, fecdia[i], 1, 31,false,false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+5, fecmes[i], 1, 12,false,false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+6, fecano[i]+2000, 2013, 2050,false,false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+7, fechor[i], 0, 23,false,false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      if (colorea) printP(server,th); else printP(server,td);
      printcampoCB(server, mpi+8, fecmin[i], 0, 59,false,false);
      if (colorea) printP(server,th_f); else printP(server,td_f);
      }
    server.printP(tr_f);
    }
  server.printP(table_f);
  
  //print the send button
  printP(server,Form_input_send,Form_f,body_f,html_f);  
}


void jsonnamesCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  if (type == WebServer::POST)  {
    server.httpFail();
    return;  }
  server.httpSuccess("application/json");
  if (type == WebServer::HEAD) return;

  server.printP(llave_i);
  for (byte i = 0; i < 16; i++)    // entradas digitales locales
    {
      byte val = digitalRead(tabpin[i]);
      printPiP(server,barradi,i,letran);
      printP(server,dospuntos,barraatras);
      printEE (server, dirEEdescpinD+(i*LEN20), LEN20);
      printP(server,barraatras,coma);          
    }
  for (byte i = 17; i < nDIG-1; i++)    // salidas digitales locales
    {
      printPiP(server,barrads,i-17,letran);
      printP(server,dospuntos,barraatras);
      printEE (server, dirEEdescpinD+(i*LEN20), LEN20);
      printP(server,barraatras,coma);          
    }
  for (byte i = 0; i < 16 ; i++)    // entradas analógicas locales
    {
      printPiP(server,barraa,i,dospuntos);
      server.print(float(Mb.R[i+baseAna])*factorA[i]);
      server.printP(coma);}
  for (byte i = 0; i < 20 ; i++)    // temperaturas locales
    {
      printPiP(server,barras,i,letran);
      printP(server,dospuntos,barraatras);
      printEE (server, dirEEdescTemp+(i*LEN20), LEN20);
      printP(server,barraatras,coma);  }        
  server.printP(llave_f);
}

int minutosentre(tmElements_t tm, tmElements_t tmI)
  {long actual;
   long inicio; 
   actual = (tmYearToCalendar(tm.Year)*525600) + (tm.Month*44640) + (tm.Hour*60) + tm.Minute;
   inicio = (tmYearToCalendar(tmI.Year)*525600) + (tmI.Month*44640) + (tmI.Hour*60) + tmI.Minute;
   return actual - inicio;
  }
 
byte inverso(byte valor)
  {if (valor==0) return(1); else return(0);}
  
void horalcd(boolean exacto)
  {
  lcd.setCursor(0,0);
  if (RTC.read(tm)) 
    { 
    if (tm.Day < 10) printLCD(cero); lcd.print (tm.Day); printLCD(barra);
    if (tm.Month < 9) lcd.print(0); lcd.print (tm.Month+1); printLCD(barra);
    lcd.print (tmYearToCalendar(tm.Year));  printLCD(b);
    if (tm.Hour < 10) lcd.print(0); lcd.print (tm.Hour); 
    printLCD(dospuntos);
    if (tm.Minute < 10) printLCD(cero); lcd.print (tm.Minute); 
    if ((!exacto) || (tm.Second == 0))
      {
      printLCD(dospuntos);
      if (tm.Second < 10) lcd.print(0); 
      lcd.print (tm.Second);
      }
    }
  else    
    {
    if (RTC.chipPresent())  
      printLCD(rtcparado);    
    else 
      printLCD(rtcnodisponible);
    }
  }
  
/*********************************************************************************************/
void hpsetup1HTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN40];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    mailACT=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN40);
      if (!repeat) break;
      param_number = atoi(name);        
      if (param_number==0) isEstacion=atoi(value)+1;
      if (param_number==1) isT1=atoi(value);
      if (param_number==2) isT2=atoi(value);
      if (param_number==3) isT3=atoi(value);
      if (param_number==4) isT4=atoi(value);
      if (param_number==5) isC=atoi(value);
      if (param_number==6) isCC=atoi(value);
      if (param_number==7) isCCC=atoi(value);
      if (param_number==8) isACS=atoi(value);
      if (param_number==9) isFR=atoi(value);
      if (param_number==10) isPIS=atoi(value);
      if (param_number==11) isRES=atoi(value);
      if (param_number==12) isCr1=atoi(value);
      if (param_number==13) isCr2=atoi(value);
      if (param_number==14) isAoC=atoi(value);
      if (param_number==15) isAoF=atoi(value);

      } while (repeat);
    guardarDatosBC();  //  Guardar datos BC en EEPROM
    server.httpSeeOther(PREFIX "/hpsetup1.html"); return;
    }

  server.httpSuccess();
  writeMenu(server,8,0);
  printP(server,brn,Form_action,hpsetup1html,Form_post,Form_input_send,tablanormal);
  
  printP(server,tr,td,estacion,td_f,td);    // mod INV/VER
  printPiP(server,Select_name,0,barramayor);  // número de parámetro: 0
  for (byte j=0; j<2; j++)  
    {
    printPiP(server,optionvalue,j,barraatras);
    if (isEstacion == j+1) server.printP(selected);
    server.printP(cierre);
    if (j==0) server.printP(invierno);
    if (j==1) server.printP(verano);
    server.printP(option_f);
    }  
  printP(server,td_f,tr);             // modo de funcionamiento
  printparCBP(server, t1, 1, isT1, -10, 10, true, true);    //T1
  printparCBP(server, t2, 2, isT2, -5, 15, true, true);    //T2
  printparCBP(server, t3, 3, isT3, 20, 60, true, true);    //T3
  printparCBP(server, t4, 4, isT4, 20, 60, true, true);    //T4
  printparCBP(server, c, 5, isC, 1, 15, true, true);    //C
  printparCBP(server, cc, 6, isCC, 1, 15, true, true);    //CC
  printparCBP(server, ccc, 7, isCCC, 1, 15, true, true);    //CCC
  printparCBP(server, acs, 8, isACS, 40, 60, true, true);    //ACS
  printparCBP(server, frio, 9, isFR, 5, 25, true, true);    //Frio
  printparCBP(server, pis, 10, isPIS, 25, 35, true, true);    //PIS
  printparCBP(server, res, 11, isRES, -5, 15, true, true);    //RES
  printparCBP(server, cr1, 12, isCr1, 0, 15, true, true);    //Cr1
  printparCBP(server, cr2, 13, isCr2, 1, 24, true, true);    //Cr2
  printparCBP(server, aoc, 14, isAoC, 25, 45, true, true);    //AoC
  printparCBP(server, aof, 15, isAoF, 5, 25, true, true);    //AoF
  
  printP(server, table_f,Form_input_send,Form_f,body_f); 
  server.printP(html_f);
}

/*********************************************************************************************/
void hpsetup2HTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN40];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
    bool repeat;
    mailACT=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN40);
      if (!repeat) break;
      param_number = atoi(name);
      if (param_number==0) isHYC=atoi(value);       
      if (param_number==1) isHYF=atoi(value);       
      if (param_number==2) isHYA=atoi(value);       
      if (param_number==3) isHYP=atoi(value);       
      if (param_number==4) isHYr=atoi(value);       
      if (param_number==5) isbPC=atoi(value);       
      if (param_number==6) isAH2=atoi(value);       
      if (param_number==7) isAH4=atoi(value);       
      if (param_number==8) isAb4=atoi(value);       
      if (param_number==9) istLP=atoi(value);       
      } while (repeat);
    guardarDatosBC();  //  Guardar datos BC en EEPROM
    server.httpSeeOther(PREFIX "/hpsetup2.html"); return;
    }

  server.httpSuccess();
  writeMenu(server,8,0);
  printP(server,brn,Form_action,hpsetup2html,Form_post,Form_input_send,tablanormal);
  printparCBP(server, histcalefaccion, 0, isHYC, 1, 10, true, true);    //HYC
  printparCBP(server, histfrio, 1, isHYF, 1, 10, true, true);    //HYF
  printparCBP(server, histacs, 2, isHYA, 1, 10, true, true);    //HYA
  printparCBP(server, histpiscina, 3, isHYP, 1, 10, true, true);    //HYP
  printparCBP(server, histresistencia, 4, isHYr, 1, 10, true, true);    //HYr
  printparCBP(server, bandaprop, 5, isbPC, 0, 10, true, true);    // bPC
  printparCBP(server, limiteah2, 6, isAH2, 40, 75, true, true);    //AH2
  printparCBP(server, limiteah4, 7, isAH4, 40, 65, true, true);    //AH4
  printparCBP(server, protcapafre, 8, isAb4, -15, 45, true, true);    //Ab4
  printparCBP(server, limitepiscina, 9, istLP, 35, 45, true, true);    //Piscina
  printP(server, table_f,Form_input_send,Form_f,body_f); 
  server.printP(html_f);
}

/*********************************************************************************************/
void hpsetup3HTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
  char name[LEN5];
  char value[LEN40];
  int param_number = 0;

  if (type == WebServer::HEAD) return;
  if (type == WebServer::POST)   {
     isECO=0;
    isPro=0;
    isAPP=0;
    bool repeat;
    mailACT=0;
    do   {
      repeat = server.readPOSTparam(name, LEN5, value, LEN40);
      if (!repeat) break;
      param_number = atoi(name);        
      if (param_number==0) isECO=(atoi(value)==1);       
      if (param_number==1) isTOF=atoi(value);       
      if (param_number==2) isTON=atoi(value);       
      if (param_number==3) isPro=(atoi(value)==1);
      if (param_number==4) isTin=atoi(value);       
      if (param_number==5) isTfi=atoi(value);       
      if (param_number==7) isAPP=atoi(value);       
      } while (repeat);
    guardarDatosBC();  //  Guardar datos BC en EEPROM
    server.httpSeeOther(PREFIX "/hpsetup3.html"); return;
    }

  server.httpSuccess();
  writeMenu(server,8,0);
  printP(server,brn,Form_action,hpsetup3html,Form_post,Form_input_send,tablanormal);
  
  printP(server,tr);
  printTDP(server,modoeco,false,true);
  printP(server,td);
//void miCheckBox (WebServer &server, char *label, boolean colorea, char *name, char *valor, boolean checked, boolean contd)
  miCheckBox(server, "", isECO==1, "0", "1", isECO, false);
  printP(server,td_f);  printP(server,tr_f);
  
  printparCBP(server, tiempooff, 1, isTOF, 1, 24, true, true);    //TOF
  printparCBP(server, tiempoon, 2, isTON, 1, 15, true, true);    //TON
  printP(server,tr);
  printTDP(server,secadomortero,false,true);
  printP(server,td);
  miCheckBox(server, "", isPro==1, "3", "1", isPro, false);
  printP(server,td_f);  printP(server,tr_f);
  printparCBP(server, tempini, 4, isTin, 10, 20, true, true);    //Tin
  printparCBP(server, tempfin, 5, isTfi, 25, 40, true, true);    //Tfi
  printparCBP(server, nbh, 6, isNbh, 180, 240, true, true);    //Nbh
  printP(server,tr);
  printTDP(server,apoyo,false,true);
  printP(server,td);
  miCheckBox(server, "", isAPP==1, "7", "1", isAPP, false);
  printP(server,td_f);  printP(server,tr_f);
  printP(server, table_f,Form_input_send,Form_f,body_f); 
  server.printP(html_f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int mifreeRam()
  {int auxmem;
  auxmem=freeRam();
  if (auxmem < minRam) minRam = auxmem;
  return auxmem;
    
  }
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

OneWire ds1(owPin1);  // on pin 43 (a 4.7K resistor is necessary)
DallasTemperature sensors1(&ds1);

void leevaloresOW()
  { 
  sensors1.requestTemperatures();
  for (int i=0; i<nTemp1; i++)
    {
    Mb.R[i]=sensors1.getTempC(addr1Wire[i])*100;
    }
  if ((Mb.R[0]/100 <= 50) && (Mb.R[0]/100 >= 10)) isST1=Mb.R[0]/100;
  if ((Mb.R[1]/100 <= 50) && (Mb.R[1]/100 >= 10)) isST2=Mb.R[1]/100;
  if ((Mb.R[2]/100 <= 50) && (Mb.R[2]/100 >= 10)) isST3=Mb.R[2]/100;
  if ((Mb.R[3]/100 <= 50) && (Mb.R[3]/100 >= 10)) isST4=Mb.R[3]/100;
  if ((Mb.R[4]/100 <= 50) && (Mb.R[4]/100 >= 10)) isST5=Mb.R[4]/100;
  if ((Mb.R[5]/100 <= 50) && (Mb.R[5]/100 >= 10)) isST6=Mb.R[5]/100;
  }

void webclientHTML(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (!autOK(server))  {server.httpUnauthorized(); return; }
}

void testEEPROM()
  {
    boolean ok=true;
    printS(ttestEEPROM);
    for (int i=0; i<16;i++)  // escribe todo ceros
      {
        writeEEPROM(EEPROM2dir,i,0);
        ok=(ok && (readEEPROM(EEPROM2dir,i)==0));
      }
    for (int i=0; i<16;i++)  // escribe valores
       {
        writeEEPROM(EEPROM2dir,i,i);
        ok=(ok && (readEEPROM(EEPROM2dir,i)==i));
      }
    printlnS(ok?blancoOK:blancoERROR);
  }

void leervaloresAna()
  {
    for(int i=0; i<16;i++)      
      Mb.R[i+baseAna]=analogRead(i+54);
  }

void actualizasegunModbus()    // actualiza salidas seg´`un estado Modbus
  {
    for (int i=17; i<41; i++) // Salidas digitales localesº
      pinVAL(tabpin[i], getbit8(Mb.C8, tabpin[i]));
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  
  wdt_disable();   // DESACTIVAR WDT
  SPI.begin(); 
  lcd.begin (20,4); 
//  if (tipoLCD==2) lcd.begin (40,4); 
  Wire.begin(); 
  byte xx=1;
  lcd.print (xx);  leerDatosRed(); xx++;
  lcd.print (xx);  leerDatosDevice(); xx++;
  lcd.print (xx);  leerDatosBC(); xx++;
  numPines = numIA+numID+numOD;
  for (byte i = 0; i < 18; ++i)
    if (freepines[i] == 0) {
      pinMode(i, INPUT);  
      digitalWrite(i, INPUT_PULLUP);  
      }
  for (byte i = 22; i < 54; ++i)
    if (freepines[i] == 0) {
      pinMode(i, OUTPUT);  
      digitalWrite(i, valorpin[getbit8(Mb.C8,i)]);  
      }
   pinMode(rstFab,INPUT);  digitalWrite(rstFab,HIGH);    // reset fábrica
   Serial.begin(115200);
  
  //////////////////////////////////////////////////////////
  if (digitalRead(rstFab)==0)
    {
    printS(espere);
    iniciavalores();
    }
  //////////////////////////////////////////////////////////
//
  lcd.print (xx);  leerDatosEventos(); xx++;
  lcd.print (xx);  leerDatosSemanal();  xx++;
  lcd.print (xx);  leerDatosFechas();  xx++;
  lcd.print (xx);  leerEstado();  xx++;
  
  webserver.m_server.chport(EEwebPort);
  lcd.print (xx);  Ethernet.begin(EEmac, EEip);  xx++;  
  lcd.print (xx);  webserver.begin(); 

//  for (byte i=0; i<8; i++) { bevenENABLE[0][i] = 255; bevenENABLE[1][i] = 255; }
  memset(bevenENABLE,255,sizeof(bevenENABLE));

  webserver.setDefaultCommand(&indexHTML);
  // Máximo 10 comandos. Para ampliar modificar variable m_commands[] en webserver.h
  webserver.addCommand("index.html", &indexHTML);
  webserver.addCommand("sd.html", &setupDevHTML);
  webserver.addCommand("st.html", &setupSondHTML);
  webserver.addCommand("sea.html", &setupEAnagHTML);
  webserver.addCommand("sed.html", &setupEdigHTML);
  webserver.addCommand("ssd.html", &setupSdigHTML);
  webserver.addCommand("sse.html", &setupSemHTML);
  webserver.addCommand("sa.html", &setupAvaHTML);
  webserver.addCommand("sv.html", &setupEveHTML);
  webserver.addCommand("sclima.html", &setupClimaHTML);
  webserver.addCommand("sNe.html", &setupNetHTML);
  webserver.addCommand("ss.html", &setupSegHTML);
  webserver.addCommand("srl.html", &setupRelHTML);
  webserver.addCommand("rst.html", &resetHTML);
  webserver.addCommand("sf.html", &setupFecHTML);
  webserver.addCommand("sprg.html", &setupPrgHTML);
  webserver.addCommand("sy.html", &systemHTML);
  webserver.addCommand("wc.html", &webclientHTML);
  webserver.addCommand("slogs.html",&ShowLogCmd );   // muestra ficheros log
  webserver.addCommand("on",&onCmd);         // pone a ON salida digital  sintaxis: /on?nn
  webserver.addCommand("off",&offCmd);       // pone a OFF salida digital  sintaxis: /off?nn   
  webserver.addCommand("jsonnames",&jsonnamesCmd);
  webserver.addCommand("hpsetup1.html", &hpsetup1HTML);
  webserver.addCommand("hpsetup2.html", &hpsetup2HTML);
  webserver.addCommand("hpsetup3.html", &hpsetup3HTML);
  webserver.addCommand("estadobc.html", &estadobcHTML);

// 
  lcd.clear();
  printLCD(ip);
  for (byte i=0; i<3;i++) {lcd.print(EEip[i]); printLCD(punto);} lcd.print (EEip[3]);
  printLCD(dospuntos); lcd.print(EEwebPort); 
  EEPROM_readAnything (dirEEAMactivo, AMactivo); 
  EEPROM_readAnything (dirEEarranquesAM, arranquesAM); 
  lcd.setCursor(0,2);
  if (AMactivo) 
    printLCD(bloqueoactivo);
  else
    printLCD(bloqueoinactivo);
  delay(2000);
  RTC.read(tmInicio);
  horalcd(false);
//
  sensors1.begin();
  nTemp1=sensors1.getDeviceCount();
//  sensors2.begin();
//  nTemp2=sensors2.getDeviceCount();
//
    printS(conucoweb); Serial.print(miversion);
    printS(punto); Serial.println(misubver);
    serialmem(" "); Serial.println();
    
    printS(lanip);
    for (byte i=0; i<4;i++) { Serial.print(EEip[i]); if (i<3) printS(punto); }
    printS(dospuntos);
    Serial.print(EEwebPort);
    printS(blancogw);
    for (byte i=0; i<4;i++) { Serial.print(EEgw[i]); if (i<3) printS(punto); }
    Serial.println();

    printS(blanco1wp1); Serial.print(nTemp1, DEC);
    printS(blancoSondas);
    printS(blancomodo);
    if (sensors1.isParasitePowerMode()) printS(parasite); else printS(power);
    Serial.println();
    for (int i=0; i<nTemp1; i++)   {
      if (sensors1.getAddress(addr1Wire[i], i))     
        {
        sensors1.setResolution(prectemp);
        printS(blancos4);
        for (uint8_t j = 0; j < 8; j++)     
          {
          if (addr1Wire[i][j] < 16) printS(cero);
          Serial.print(addr1Wire[i][j], HEX);   
          }
        Serial.println();
        }
      }
    leevaloresOW();

  EEPROM_readAnything (dirEEfactorA, factorA);
  EEPROM_readAnything (dirEEoffsetA, offsetA);
  EEPROM_readAnything (dirEEoffsetA, bsumatA);
  mact1 =millis();
  mact10 =millis();
  mact60 =millis();
  mactVar =millis();
  testEEPROM();
  if (isEve[tON][0]==1)
    ESTADO=isFASE1;
  quitarBloqueo();
  Serial.print("Arranques:"); Serial.print(arranquesAM);
  Serial.print("/"); Serial.println(maxarranquesAM);
  printlnS(ready); 
}

const int maxestadoLCD=41;

void printparlcd(const prog_uchar *str1,const prog_uchar *str2, int valor,boolean traducir)
    {
    lcd.setCursor(0,0);
    printLCD(parametro);
    lcd.setCursor(10,0); lcd.print(estadoLCD);
    lcd.setCursor(0,1); printLCD(str1);
    lcd.setCursor(8,2); printLCD(str2); printLCD(igual);
    lcd.setCursor(13                                                                                                                                                                                  ,2); 
    if (traducir)
      if (valor==0)
        printLCD(OFF);
      else
        printLCD(ON);
    else
      lcd.print(valor);
    printLCD(b);
    }

int diaaux, mesaux,anoaux,horaaux,minaux;

void showlcd()
  {
  if (estadoLCD==0)
    {
    horalcd(false);
    if (ESTADO==isPARADO)
      {
      lcd.setCursor(0,1); for (int i=0;i<20;i++) lcd.write(' ');
      lcd.setCursor(0,2); for (int i=0;i<20;i++) lcd.write(' ');
      lcd.setCursor(0,3); for (int i=0;i<20;i++) lcd.write(' ');
      lcd.setCursor(0,3); printLCD(apagado); 
      return;
      }
    lcd.setCursor(0,1);
      if (isModo==0) printLCD(lcdcal);
      if (isModo==1) printLCD(lcdpis);
      if (isModo==2) printLCD(lcdfri);
      if (isModo==3) printLCD(lcdacs);
    char buff[5];
    lcd.setCursor(4,1); printLCD(isRBCCal?lcdbc1:lcdblanco);
    lcd.setCursor(8,1); printLCD(isRBCCap?lcdbc2:lcdblanco);
    lcd.setCursor(12,1); printLCD(isRCOMP?lcdcomp:lcdblanco);
    lcd.setCursor(17,1); printLCD((isEve[tTer][0]==1)?lcdter:lcdblanco); 
    lcd.setCursor(19,1); printLCD((AMactivo)?letrab:b);
    lcd.setCursor(0,2);  lcd.print(isST2,1); printLCD(b);
    lcd.setCursor(5,2); lcd.print(isST1,1); printLCD(b); 
    lcd.setCursor(10,2); lcd.print(isST5,1); printLCD(b); 
    lcd.setCursor(15,2); lcd.print(isST6,1); printLCD(b); 
    lcd.setCursor(0,3); printLCD(lcdext); 
    lcd.setCursor(4,3); lcd.print(isST3,1); printLCD(b);
    lcd.setCursor(9,3); lcd.print(isST4,1); printLCD(b);
    lcd.setCursor(14,3); if (isCodAlarma!=0) {printLCD(lcdala); lcd.write(itoa(isCodAlarma,buff,10));} else {printLCD(lcdblancos); }
    }
  if (estadoLCD==1) {lcd.clear(); printLCD(aviso1); 
                      lcd.setCursor(0,1);printLCD(aviso2); 
                      lcd.setCursor(0,2);printLCD(aviso3); 
                      lcd.setCursor(0,3);printLCD(aviso4);  }  // avisos
  if (estadoLCD==2) printparlcd(lcdestacion,lcdsta,isEstacion,false); // estación
  if (estadoLCD==3) printparlcd(lcddia,lcddia,tm.Day,false);  // dia
  if (estadoLCD==4) printparlcd(lcdmes,lcdmes,tm.Month+1,false);  // mes
  if (estadoLCD==5) printparlcd(lcdano,lcdano,tm.Year+1970,false);  // año
  if (estadoLCD==6) printparlcd(lcdhora,lcdhora,tm.Hour,false);  // hora
  if (estadoLCD==7) printparlcd(lcdmin,lcdmin,tm.Minute,false);  // minuto
  if (estadoLCD==8) printparlcd(lcdtt1,lcdt1,isT1,false);  // T1
  if (estadoLCD==9) printparlcd(lcdtt2,lcdt2,isT2,false);  // T2
  if (estadoLCD==10) printparlcd(lcdtt3,lcdt3,isT3,false);  // T3
  if (estadoLCD==11) printparlcd(lcdtt4,lcdt4,isT4,false);  // T4
  if (estadoLCD==12) printparlcd(lcdtc,lcdc,isC,false);  // c
  if (estadoLCD==13) printparlcd(lcdtcc,lcdcc,isCC,false);  // cc
  if (estadoLCD==14) printparlcd(lcdtccc,lcdccc,isCCC,false);  // ccc
  if (estadoLCD==15) printparlcd(lcdcacs,lcdacs,isACS,false);  // ACS
  if (estadoLCD==16) printparlcd(lcdfrio,lcdfro,isFR,false);  // Frio
  if (estadoLCD==17) printparlcd(lcdcpis,lcdpis,isPIS,false);  // Pis
  if (estadoLCD==18) printparlcd(lcdcres,lcdres,isRES,false);  // res
  if (estadoLCD==19) printparlcd(lcdccr1,lcdcr1,isCr1,false);  // cr1
  if (estadoLCD==20) printparlcd(lcdccr2,lcdcr2,isCr2,false);  // cr2
  if (estadoLCD==21) printparlcd(lcdcaoc,lcdaoc,isAoC,false);  // AoC
  if (estadoLCD==22) printparlcd(lcdcaof,lcdaof,isAoF,false);  // AoF
  if (estadoLCD==23) printparlcd(lcdhistcalefaccion,lcdhyc,isHYC,false);  // HYC
  if (estadoLCD==24) printparlcd(lcdhistfrio,lcdhyf,isHYF,false);  // HYF
  if (estadoLCD==25) printparlcd(lcdhistacs,lcdhya,isHYA,false);  // HYA
  if (estadoLCD==26) printparlcd(lcdhistpiscina,lcdhyp,isHYP,false);  // HYP
  if (estadoLCD==27) printparlcd(lcdhistresistencia,lcdhyr,isHYr,false);  // HYR
  if (estadoLCD==28) printparlcd(lcdbandaprop,lcdbpc,isbPC,false);  // bPC
  if (estadoLCD==29) printparlcd(lcdlimiteah2,lcdah2,isAH2,false);  // AH2
  if (estadoLCD==30) printparlcd(lcdlimiteah4,lcdah4,isAH4,false);  // AH4
  if (estadoLCD==31) printparlcd(lcdprotcapafre,lcdab4,isAb4,false);  // Ab4
  if (estadoLCD==32) printparlcd(lcdlimitepiscina,lcdtlp,istLP,false);  // tLP
  if (estadoLCD==33) printparlcd(lcdmodoeco,lcdeco,isECO,true);  // ECO
  if (estadoLCD==34) printparlcd(lcdtiempooff,lcdtof,isTOF,false);  // tOff
  if (estadoLCD==35) printparlcd(lcdtiempoon,lcdton,isTON,false);  // tOn
  if (estadoLCD==36) printparlcd(lcdsecadomortero,lcdpro,isPro,true);  // Secado Mortero
  if (estadoLCD==37) printparlcd(lcdtempini,lcdtin,isTin,false);  // Temp.inicial
  if (estadoLCD==38) printparlcd(lcdtempfin,lcdtfi,isTfi,false);  // Temp. final
  if (estadoLCD==39) printparlcd(lcdNbh,lcdnbh,isNbh,false);  // horas
  if (estadoLCD==40) printparlcd(lcdapoyo,lcdapp,isAPP,true);  // usar Apoyo
  int auxnada=0;
  if (estadoLCD==41) {lcd.clear(); printLCD(noolvide1); lcd.setCursor(0,2);printLCD(noolvide2); }  // minuto
  }
  
void procesaTecla(char tecla)
  {
  if (tecla=='1') {estadoLCD++; lcd.clear(); if (estadoLCD>maxestadoLCD) estadoLCD=0; showlcd();}
  if (tecla=='2') // bajar
    {
    if (estadoLCD==0) 
      {
      isEve[tON][0]=0; 
      EEPROM_writeAnything (dirEEESTADO, isEve[tON][0]);  
      activaEstado(isPARADO); 
      showlcd(); 
      }
    if (estadoLCD==2) {isEstacion--; if (isEstacion<1) isEstacion=2; showlcd(); }
    if (estadoLCD==3) {if (tm.Day > 0) tm.Day--; showlcd(); }
    if (estadoLCD==4) {if (tm.Month > 0) tm.Month--; showlcd(); }
    if (estadoLCD==5) {if (tm.Year > 45) tm.Year--; showlcd(); }
    if (estadoLCD==6) {if (tm.Hour > 0) tm.Hour--; showlcd(); }
    if (estadoLCD==7) {if (tm.Minute > 0) tm.Minute--; showlcd(); }
    if (estadoLCD==8) {if (isT1 > -10) isT1--; showlcd(); }
    if (estadoLCD==9) {if (isT2 > -5) isT2--; showlcd(); }
    if (estadoLCD==10) {if (isT3 > 20) isT3--; showlcd(); }
    if (estadoLCD==11) {if (isT4 > 20) isT4--; showlcd(); }
    if (estadoLCD==12) {if (isC > 1) isC--; showlcd(); }
    if (estadoLCD==13) {if (isCC > 1) isCC--; showlcd(); }
    if (estadoLCD==14) {if (isCCC > 1) isCCC--; showlcd(); }
    if (estadoLCD==15) {if (isACS > 40) isACS--; showlcd(); }
    if (estadoLCD==16) {if (isFR > 5) isFR--; showlcd(); }
    if (estadoLCD==17) {if (isPIS > 25) isPIS--; showlcd(); }
    if (estadoLCD==18) {if (isRES > -5) isRES--; showlcd(); }
    if (estadoLCD==19) {if (isCr1 > 0) isCr1--; showlcd(); }
    if (estadoLCD==20) {if (isCr2 > 1) isCr2--; showlcd(); }
    if (estadoLCD==21) {if (isAoC > 25) isAoC--; showlcd(); }
    if (estadoLCD==22) {if (isAoF > 5) isAoF--; showlcd(); }
    if (estadoLCD==23) {if (isHYC > 1) isHYC--; showlcd(); }
    if (estadoLCD==24) {if (isHYF > 1) isHYF--; showlcd(); }
    if (estadoLCD==25) {if (isHYA > 1) isHYA--; showlcd(); }
    if (estadoLCD==26) {if (isHYP > 1) isHYP--; showlcd(); }
    if (estadoLCD==27) {if (isHYr > 1) isHYr--; showlcd(); }
    if (estadoLCD==28) {if (isbPC > 0) isbPC--; showlcd(); }
    if (estadoLCD==29) {if (isAH2 > 40) isAH2--; showlcd(); }
    if (estadoLCD==30) {if (isAH4 > 40) isAH4--; showlcd(); }
    if (estadoLCD==31) {if (isAb4 > -15) isAb4--; showlcd(); }
    if (estadoLCD==32) {if (istLP > 35) istLP--; showlcd(); }
    if (estadoLCD==33) {if (isECO > 0) isECO--; showlcd(); }
    if (estadoLCD==34) {if (isTOF > 1) isTOF--; showlcd(); }
    if (estadoLCD==35) {if (isTON > 1) isTON--; showlcd(); }
    if (estadoLCD==36) {if (isPro > 0) isPro--; showlcd(); }
    if (estadoLCD==37) {if (isTin > 10) isTin--; showlcd(); }
    if (estadoLCD==38) {if (isTfi > 25) isTfi--; showlcd(); }
    if (estadoLCD==39) {if (isNbh > 180) isNbh--; showlcd(); }
    if (estadoLCD==40) {if (isAPP > 0) isAPP--; showlcd(); }
    }
    
  if (tecla=='3') // subir
    {
    if (estadoLCD==0) 
      {
      isEve[tON][0]=1; 
      EEPROM_writeAnything (dirEEESTADO, isEve[tON][0]);  
      ESTADO=isFASE1;
      showlcd(); 
      }
    if (estadoLCD==2) {isEstacion++; if (isEstacion>2) isEstacion=1;showlcd();  }
    if (estadoLCD==3) {if (tm.Day < 31) tm.Day++; showlcd(); }
    if (estadoLCD==4) {if (tm.Month < 11) tm.Month++; showlcd(); }
    if (estadoLCD==5) {if (tm.Year < 55) tm.Year++; showlcd(); }
    if (estadoLCD==6) {if (tm.Hour < 23) tm.Hour++; showlcd(); }
    if (estadoLCD==7) {if (tm.Minute < 59) tm.Minute++; showlcd(); }
    if (estadoLCD==8) {if (isT1 < 10) isT1++; showlcd(); }
    if (estadoLCD==9) {if (isT2 < 15) isT2++; showlcd(); }
    if (estadoLCD==10) {if (isT3 < 60) isT3++; showlcd(); }
    if (estadoLCD==11) {if (isT4 < 60) isT4++; showlcd(); }
    if (estadoLCD==12) {if (isC < 15) isC++; showlcd(); }
    if (estadoLCD==13) {if (isCC < 15) isCC++; showlcd(); }
    if (estadoLCD==14) {if (isCCC < 15) isCCC++; showlcd(); }
    if (estadoLCD==15) {if (isACS < 60) isACS++; showlcd(); }
    if (estadoLCD==16) {if (isFR < 25) isFR++; showlcd(); }
    if (estadoLCD==17) {if (isPIS < 35) isPIS++; showlcd(); }
    if (estadoLCD==18) {if (isRES < 15) isRES++; showlcd(); }
    if (estadoLCD==19) {if (isCr1 < 15) isCr1++; showlcd(); }
    if (estadoLCD==20) {if (isCr2 < 24) isCr2++; showlcd(); }
    if (estadoLCD==21) {if (isAoC < 45) isAoC++; showlcd(); }
    if (estadoLCD==22) {if (isAoF < 25) isAoF++; showlcd(); }
    if (estadoLCD==23) {if (isHYC < 10) isHYC++; showlcd(); }
    if (estadoLCD==24) {if (isHYF < 10) isHYF++; showlcd(); }
    if (estadoLCD==25) {if (isHYA < 10) isHYA++; showlcd(); }
    if (estadoLCD==26) {if (isHYP < 10) isHYP++; showlcd(); }
    if (estadoLCD==27) {if (isHYr < 10) isHYr++; showlcd(); }
    if (estadoLCD==28) {if (isbPC < 10) isbPC++; showlcd(); }
    if (estadoLCD==29) {if (isAH2 < 75) isAH2++; showlcd(); }
    if (estadoLCD==30) {if (isAH4 < 65) isAH4++; showlcd(); }
    if (estadoLCD==31) {if (isAb4 < 45) isAb4++; showlcd(); }
    if (estadoLCD==32) {if (istLP < 45) istLP++; showlcd(); }
    if (estadoLCD==33) {if (isECO < 1) isECO++; showlcd(); }
    if (estadoLCD==34) {if (isTOF < 24) isTOF++; showlcd(); }
    if (estadoLCD==35) {if (isTON < 15) isTON++; showlcd(); }
    if (estadoLCD==36) {if (isPro < 1) isPro++; showlcd(); }
    if (estadoLCD==37) {if (isTin < 10) isTin++; showlcd(); }
    if (estadoLCD==38) {if (isTfi < 25) isTfi++; showlcd(); }
    if (estadoLCD==39) {if (isNbh < 240) isNbh++; showlcd(); }
    if (estadoLCD==40) {if (isAPP < 1) isAPP++; showlcd(); }
    }
    
  if (tecla=='4') // intro
    {
    if (estadoLCD==0) 
      {
      confBloqueo();
      showlcd(); 
      }
    else
      {
      lcd.clear();
      printLCD(guardando);
      guardarDatosBC();
      delay(1000);
      lcd.clear();
      }
    }
  }

void loop()    // el bucle se ejecuta unas 75 veces/segundo
{
  key = keypad.getKey();
  if (key) procesaTecla(key);
  procesaIsara2();
  char buff[buffLEN];
  int len = buffLEN;  
  contador++;
  if ((millis() > (mact1 + 1000)))    // tareas que se hacen cada segundo
    {
      mact1=millis();
      velocidad=contador;
      contador=0;
      if (estadoLCD==0) horalcd(false);
      showlcd();
//      leervaloresAna();
    }
  if ((millis() > (mact10 + 10000)))    // tareas que se hacen cada 10 segundos
    {
        mact10=millis();
        velocidad=contador/10;
        contador=0;
  //      actualizasegunModbus();
        leevaloresOW();  
    }
//  if ((millis() > (mact60 + 60000)))    // tareas que se hacen cada 60 segundos
//    {
//     mact60=millis();
//    }
//  if ((millis() > (mactVar + (pmactVar*1000))))    // tareas que se hacen cada pmactVar segundos
//    {
//      mactVar=millis();
////        if (mactVar>0) logTemp();
//    }
////  
////    for (byte i=0; i<8;i++) //  actualiza contadores
////      {     
////       if (Mb.C8[i+32]!==digitalRead(tabpin[i]))
////         {
////         if (digitalRead(tabpin[i])==1)
////           {
////           contadores[i]++;
////           EEPROM_writeAnything(dirEEcontadores+(4*i), contadores[i]);
////           }
////         Mb.C8[i+32]=digitalRead(tabpin[i]);
////         }
////      }
//
//    Mb.Run();    // actualizar Modbus
    webserver.processConnection(buff, &len);
}
/////////////// end loop ////////////////////////////inicia
