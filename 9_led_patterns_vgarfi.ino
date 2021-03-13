---------------------------------------\
*/
#include <LiquidCrystal.h>
#include <TimerOne.h>

//----------DEFINES DE PINES----------\\

#define PIN_RS         7
#define PIN_EN         8
#define PIN_D4         9
#define PIN_D5         10
#define PIN_D6         11
#define PIN_D7         12

#define PIN_BTN_UP     28
#define PIN_BTN_DOWN   30
#define PIN_BTN_1      50
#define PIN_BTN_2      51

#define PIN_LED1       21
#define PIN_LED2       22
#define PIN_LED3       23
#define PIN_LED4       5
#define PIN_LED5       4
#define PIN_LED6       3
#define PIN_LED7       2
#define PIN_LED8       14
#define PIN_LED9       15
#define PIN_LED10      16
#define PIN_LED11      6
#define PIN_LED12      17
#define PIN_LED13      18
////////////////////////////////////////////

//---DEFINES Y VARIABLES DE LOS BOTONES---\\

#define ESTADO_BOTON_ESPERA         0
#define ESTADO_BOTON_CONFIRMACION   1
#define ESTADO_BOTON_LIBERACION     2

#define ESTADO_BOTON_INICIAL    ESTADO_BOTON_ESPERA
#define MS_ANTIRREB 25  // Espera (en milisegundos) para el antirrebote

#define CANT_BOTONES                4

#define BTN_UP                      0
#define BTN_DOWN                    1
#define BTN_1                       2
#define BTN_2                       3

int estadoBoton[CANT_BOTONES];    // Estados de las maquinas de estados

char flagBoton[CANT_BOTONES];   // Banderas que indican el estado actual de los botones. Los uso en la maq de estados
char pinBoton[CANT_BOTONES];    // Array de pines donde estan conectados los botones. Los uso en la maq de estados

int msBoton[CANT_BOTONES];      // Contadores de milisegundos para los antirrebotes de los botones
int ultimoBtnApretado;          // Variable que almacena cual fue el último botón presionado
////////////////////////////////////////////

//-DEFINES Y VARIABLES DE LA PANTALLA LCD-\\

#define CANT_COL          16
#define CANT_FIL          2

#define PRIM_COL          0
#define PRIM_FIL          0
#define SEG_COL           1

#define OPCION_1          "Asc. simple "
#define OPCION_2          "Asc. descen."
#define OPCION_3          "Asc. apagado"
#define OPCION_4          "Expl. cn-rst"
#define OPCION_5          "Expl. sn-rst"
#define OPCION_6          "Pumba pumba "
#define OPCION_7          "Ambulancia  "
#define OPCION_8          "Pinguipingui"
#define OPCION_9          "Asc p asc a"

#define MSJ_BIENVENIDA1   "Hola! Elija el"
#define MSJ_BIENVENIDA2   "patron que desee"
#define MSJ_FINAL         "Usted ha elegido"

#define MSJ_DELAY                   3000

#define ESTADO_LCD_INICIAL          0
#define ESTADO_LCD_PRINT            1
#define ESTADO_LCD_UP               2
#define ESTADO_LCD_DOWN             3
#define ESTADO_LCD_FINAL            4

#define PRIMERA_FILA                1
#define ULTIMA_FILA                 8

String primeraOpcion = OPCION_1; // Variables que contienen las opciones impresas en pantalla
String segundaOpcion = OPCION_2;

int estadoLCD = ESTADO_LCD_INICIAL;

int msMensaje = 0;
bool contarMsMensaje = true;     // Bloqueador que permite dejar de contar los milisegundos del mensaje de bienvenida

int filaOpcion = 1;              // Variable que indica que opcion se encuentra en la primer fila

int calcularFilas (int num_fila, int up_or_down);


LiquidCrystal lcd(PIN_RS, PIN_EN, PIN_D4, PIN_D5, PIN_D6, PIN_D7);
/////////////////////////////////////////////

//-----DEFINES Y VARIABLES DE LOS LEDS-----\\

#define MS_ESPERA_LEDS       80

#define IDA            0
#define VUELTA         1

int ms = 0;               // Variable que contabiliza el tiempo para el encendido y apgado de los LEDs
int numeroPatronElegido;  // Variable que almacena cual fue el patrón que el usuario desea ejecutar
int estado_patron = 1;    // Variables utilizada en los patrones para indicar en que etapa del mismo se encuentra

bool modo;
int estado_p3 = 1;        // Utilizado en el patron 3, ya que no todo el patrón se repite completamente
bool bloq_p3 = false;     // Bloqueador que impide repetir un segmento del código en el patrón 3

int intervalo = 1;        // Nos sirve para saber si se debe avanzar a una proxima etapa dentro de un patrón o a la anterior

void patron_1 (void);
void patron_2 (void);
void patron_3 (void);
void patron_4 (void);
void patron_5 (void);
void patron_6(void);
void patron_7 (void);
void patron_8(void);
void patron_9(void);

void apagarLEDs (void);
void apagarOPrenderLEDs(int pin_PIN_LED1, int pin_PIN_LED2, bool estado);
void apagarYPrenderLEDs (int pin_PIN_LED1, int pin_PIN_LED2, int pin_PIN_LED3, int pin_PIN_LED4);
/////////////////////////////////////////////////////

//-----MAQUINAS DE ESTADOS DE TODO EL PROGRAMA-----\\

void FSM_Antirrebote(int boton);
void maquinaPantalla_LCD (void);
void maquinaLEDs (int opcion_elegida);
/////////////////////////////////////////////////////

void setup() 
{
  // put your setup code here, to run once:
  Timer1.initialize(1000);
  Timer1.attachInterrupt(isr_timer);
  
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_LED2, OUTPUT);
  pinMode(PIN_LED3, OUTPUT);
  pinMode(PIN_LED4, OUTPUT);
  pinMode(PIN_LED5, OUTPUT);
  pinMode(PIN_LED6, OUTPUT);
  pinMode(PIN_LED7, OUTPUT);
  pinMode(PIN_LED8, OUTPUT);
  pinMode(PIN_LED9, OUTPUT);
  pinMode(PIN_LED10, OUTPUT);
  pinMode(PIN_LED11, OUTPUT);
  pinMode(PIN_LED12, OUTPUT);
  pinMode(PIN_LED13, OUTPUT);
  
  pinMode(PIN_BTN_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);
  pinMode(PIN_BTN_1, INPUT_PULLUP);
  pinMode(PIN_BTN_2, INPUT_PULLUP);
  
  pinBoton[BTN_UP] = PIN_BTN_UP;
  pinBoton[BTN_DOWN] = PIN_BTN_DOWN;
  pinBoton[BTN_1] = PIN_BTN_1;
  pinBoton[BTN_2] = PIN_BTN_2;

  estadoBoton[BTN_UP] = ESTADO_BOTON_INICIAL;
  estadoBoton[BTN_DOWN] = ESTADO_BOTON_INICIAL;
  estadoBoton[BTN_1] = ESTADO_BOTON_INICIAL;
  estadoBoton[BTN_2] = ESTADO_BOTON_INICIAL;
  
  flagBoton[BTN_UP] = 0;
  flagBoton[BTN_DOWN] = 0;
  flagBoton[BTN_1] = 0;
  flagBoton[BTN_2] = 0;

  msBoton[BTN_UP] = 0;
  msBoton[BTN_DOWN] = 0;
  msBoton[BTN_1] = 0;
  msBoton[BTN_2] = 0;

  
  lcd.begin(CANT_COL, CANT_FIL);
  apagarLEDs();                     // Empezamos con los LEDs apagados
}

void loop() // en el loop lo úncio que hacemos es llamar a las maquinas de estados
{
  FSM_Antirrebote(BTN_UP);
  FSM_Antirrebote(BTN_DOWN);
  FSM_Antirrebote(BTN_1);
  FSM_Antirrebote(BTN_2);
  maquinaPantalla_LCD();
  maquinaLEDs(numeroPatronElegido);
}

void maquinaPantalla_LCD (void)
{
  int upOrDown;
  
  switch (estadoLCD)
  {
    case ESTADO_LCD_INICIAL:
                          lcd.setCursor(PRIM_COL, PRIM_FIL);
                          lcd.print(MSJ_BIENVENIDA1);
                          lcd.setCursor(PRIM_COL, 1);
                          lcd.print(MSJ_BIENVENIDA2);

                          if (msMensaje >= MSJ_DELAY)
                          {
                            msMensaje = 0;
                            contarMsMensaje = false;
                            lcd.clear();
                            estadoLCD = ESTADO_LCD_PRINT;
                          }
    break;
    
    case ESTADO_LCD_PRINT:
                          lcd.setCursor(PRIM_COL, PRIM_FIL);
                          lcd.print("(" + String(filaOpcion) + ")" + primeraOpcion);
                          lcd.setCursor(PRIM_COL, 1);
                          lcd.print("(" + String(filaOpcion+1) + ")" + segundaOpcion);

                          if (flagBoton[BTN_1] == 1)           // Boton para elegir la primera opcion presionado
                          {
                            flagBoton[BTN_1] = 0;
                            lcd.clear();
                            ultimoBtnApretado = BTN_1;
                            numeroPatronElegido = filaOpcion; // Almacenamos el numero de patron elegido
                            ms = 0;
                            estadoLCD = ESTADO_LCD_FINAL;
                          }

                          if (flagBoton[BTN_2] == 1)              // Boton para elegir la segunda opcion presionado
                          {
                            flagBoton[BTN_2] = 0;
                            lcd.clear();
                            ultimoBtnApretado = BTN_2;
                            numeroPatronElegido = filaOpcion + 1; // Almacenamos el numero de patron elegido (+1 porque es el siguiente al primero)
                            ms = 0;
                            estadoLCD = ESTADO_LCD_FINAL;
                          }

                          if (flagBoton[BTN_UP] == 1)             // Boton para scrollear para arriba presionado
                          {
                            flagBoton[BTN_UP] = 0;
                            estadoLCD = ESTADO_LCD_UP;
                          }

                          if (flagBoton[BTN_DOWN] == 1)           // Boton para scrollear para abajo presionado
                          {
                            flagBoton[BTN_DOWN] = 0;
                            estadoLCD = ESTADO_LCD_DOWN;
                          }
    break;
    
    
    case ESTADO_LCD_UP:
                        
                        if (filaOpcion != PRIMERA_FILA)                         // Solo podemos scrollear para arriba si la primera opcion mostrada no es la opcion N°1
                        {
                          upOrDown = -1;                                        // Indicamos que se debe traer una posición adelante a las opciones
                          filaOpcion = calcularFilas(filaOpcion, upOrDown);     // Calculamos que numero de patrón debe ser mostrado en la primera fila
                        }

                        estadoLCD = ESTADO_LCD_PRINT;
    break;

    case ESTADO_LCD_DOWN:
                        if (filaOpcion != ULTIMA_FILA)                          // Solo podemos scrollear para abajo si la última opcion mostrada no es la opcion N°9
                        {
                          upOrDown = 1;                                         // Indicamos que se debe llevar una posición atras a las opciones
                          filaOpcion = calcularFilas(filaOpcion, upOrDown);     // Calculamos que numero de patrón debe ser mostrado en la primera fila
                        }

                        estadoLCD = ESTADO_LCD_PRINT;
    break;

    case ESTADO_LCD_FINAL:
                          lcd.setCursor(PRIM_COL, PRIM_FIL);
                          lcd.print(MSJ_FINAL);
                          lcd.setCursor(PRIM_COL, PRIM_FIL+1);
                          
                          if (ultimoBtnApretado == BTN_1)
                          {
                             lcd.print(primeraOpcion);
                          }

                          if (ultimoBtnApretado == BTN_2)
                          {
                            lcd.print(segundaOpcion);
                          }
                            
                          if (ultimoBtnApretado == BTN_1) // Esperamos a que vuelvan a presionar el boton
                          {
                            if (flagBoton[BTN_1] == 1)
                            {
                              flagBoton[BTN_1] = 0;
                              
                              ultimoBtnApretado = 0;
                              estado_patron = 1;            // Hacemos esto para que todos los patrones comiencen desde el principio
                              numeroPatronElegido = 0;
                              ms = 0;
                              apagarLEDs();
                              
                              estadoLCD = ESTADO_LCD_PRINT;
                            }     
                          }

                         if (ultimoBtnApretado == BTN_2) // Esperamos a que vuelvan a presionar el boton
                         {  
                            if (flagBoton[BTN_2] == 1)
                            {
                              flagBoton[BTN_2] = 0;
                              
                              ultimoBtnApretado = 0;
                              numeroPatronElegido = 0;
                              estado_patron = 1;
                              ms = 0;
                              apagarLEDs();
                              
                              estadoLCD = ESTADO_LCD_PRINT;
                            }
                         }               
    break;
  }
}

void apagarLEDs (void)              // funcion que apaga todos los LEDs
{
  digitalWrite (PIN_LED1, LOW);
  digitalWrite (PIN_LED2, LOW);
  digitalWrite (PIN_LED3, LOW);
  digitalWrite (PIN_LED4, LOW);
  digitalWrite (PIN_LED5, LOW);
  digitalWrite (PIN_LED6, LOW);
  digitalWrite (PIN_LED7, LOW);
  digitalWrite (PIN_LED8, LOW);
  digitalWrite (PIN_LED9, LOW);
  digitalWrite (PIN_LED10, LOW);
  digitalWrite (PIN_LED11, LOW);
  digitalWrite (PIN_LED12, LOW);
  digitalWrite (PIN_LED13, LOW);
}




int calcularFilas (int num_fila, int up_or_down) // Devuelve el numero de patrón que debe ser mostrado en la primer fila
{
  num_fila = num_fila + up_or_down;

  switch(num_fila)
  {
    case 1:
            primeraOpcion = OPCION_1;
            segundaOpcion = OPCION_2;
    break;

    case 2:
            primeraOpcion = OPCION_2;
            segundaOpcion = OPCION_3;
    break;

    case 3:
            primeraOpcion = OPCION_3;
            segundaOpcion = OPCION_4;
    break;

    case 4:
            primeraOpcion = OPCION_4;
            segundaOpcion = OPCION_5;
    break;

    case 5:
            primeraOpcion = OPCION_5;
            segundaOpcion = OPCION_6;
    break;

    case 6:
            primeraOpcion = OPCION_6;
            segundaOpcion = OPCION_7;
    break;

    case 7:
            primeraOpcion = OPCION_7;
            segundaOpcion = OPCION_8;
    break;

    case 8:
            primeraOpcion = OPCION_8;
            segundaOpcion = OPCION_9;
    break;
  }  
  
  return num_fila;
}


void FSM_Antirrebote(int boton)     // Maquina de estados para poder diferenciar entre una pulsación real (humana) y un rebote
{
    char estadoPin;
    
    switch(estadoBoton[boton])
    {
      case ESTADO_BOTON_ESPERA:
                              estadoPin = digitalRead(pinBoton[boton]);
                              
                              // Si se da la condicion de cambio de estado
                              if( estadoPin == 0)
                              {
                                msBoton[boton] = 0;
                                estadoBoton[boton] = ESTADO_BOTON_CONFIRMACION;
                              }
      break;
          
      case ESTADO_BOTON_CONFIRMACION:
                                    estadoPin = digitalRead(pinBoton[boton]);
                                    
                                    // Si se da la condicion, se considera el boton como presionado
                                    if( estadoPin == 0 && msBoton[boton] >= MS_ANTIRREB)
                                    {
                                      estadoBoton[boton] = ESTADO_BOTON_LIBERACION;
                                    }
                        
                                    // Si se da la condicion, se considera ruido
                                    if( estadoPin == 1 && msBoton[boton] < MS_ANTIRREB)
                                    {
                                      estadoBoton[boton] = ESTADO_BOTON_ESPERA;
                                    }
      break;
          
      case ESTADO_BOTON_LIBERACION:
                                  estadoPin = digitalRead(pinBoton[boton]);
                                  
                                  // Si se da la condicion, se considera el boton como presionado
                                  if(estadoPin == 1)
                                  {
                                    flagBoton[boton] = 1;
                                    Serial.println("Boton " + String(boton+1) + " presionado!");
                                    estadoBoton[boton] = ESTADO_BOTON_ESPERA;
                                  }
      break;
          
      default:
          // Si entra aca, hay un error en la variable estado_FSM, o algo mal asignado en ella
          Serial.println("Error.");
          estadoBoton[boton] = ESTADO_BOTON_INICIAL;
          break;
    }
}

void maquinaLEDs (int opcion_elegida) // Maquina de estados que selecciona que patron debe ser ejecutado dependiendo de lo que el usuario haya elegido
{
  switch (opcion_elegida)
  {
    case 1:
                  patron_1();
    break;

    case 2:
                  patron_2();
    break;

    case 3:       
                  patron_3();
    break;

    case 4:
                  patron_4();
    break;

    case 5:
                  patron_5();
    break;

    case 6:
                  patron_6();
    break;

    case 7:
                  patron_7();
    break;

    case 8:
                  patron_8();
    break;

    case 9:
                  patron_9();
    break;
  }
}

//--------PATRON 1----------\\

void patron_1 (void)
{
  switch (estado_patron)
  {
    case 1:
        digitalWrite(PIN_LED1, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 2:
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 3:
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 4:
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 5:
        digitalWrite(PIN_LED4, LOW);
        digitalWrite(PIN_LED5, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 6:
        digitalWrite(PIN_LED5, LOW);
        digitalWrite(PIN_LED6, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 7:
        digitalWrite(PIN_LED6, LOW);
        digitalWrite(PIN_LED7, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 8:
        digitalWrite(PIN_LED7, LOW);
        digitalWrite(PIN_LED8, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 9:
        digitalWrite(PIN_LED8, LOW);
        digitalWrite(PIN_LED9, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 10:
        digitalWrite(PIN_LED9, LOW);
        digitalWrite(PIN_LED10, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 11:
        digitalWrite(PIN_LED10, LOW);
        digitalWrite(PIN_LED11, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 12:
        digitalWrite(PIN_LED11, LOW);
        digitalWrite(PIN_LED12, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 13:
        digitalWrite(PIN_LED12, LOW);
        digitalWrite(PIN_LED13, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 14:
        digitalWrite(PIN_LED13, LOW);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = 1;
        }
    break;
  }
}

//--------PATRON 2----------\\

void patron_2 (void)
{
  switch (estado_patron)
  {
    case 1:
        digitalWrite(PIN_LED1, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 2:
        digitalWrite(PIN_LED1, LOW);
        digitalWrite(PIN_LED2, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 3:
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED3, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 4:
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED4, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 5:
        digitalWrite(PIN_LED4, LOW);
        digitalWrite(PIN_LED5, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 6:
        digitalWrite(PIN_LED5, LOW);
        digitalWrite(PIN_LED6, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 7:
        digitalWrite(PIN_LED6, LOW);
        digitalWrite(PIN_LED7, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 8:
        digitalWrite(PIN_LED7, LOW);
        digitalWrite(PIN_LED8, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 9:
        digitalWrite(PIN_LED8, LOW);
        digitalWrite(PIN_LED9, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 10:
        digitalWrite(PIN_LED9, LOW);
        digitalWrite(PIN_LED10, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 11:
        digitalWrite(PIN_LED10, LOW);
        digitalWrite(PIN_LED11, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 12:
        digitalWrite(PIN_LED11, LOW);
        digitalWrite(PIN_LED12, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 13:
        digitalWrite(PIN_LED12, LOW);
        digitalWrite(PIN_LED13, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 14:
        digitalWrite(PIN_LED13, LOW);
        digitalWrite(PIN_LED12, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 15:
        digitalWrite(PIN_LED12, LOW);
        digitalWrite(PIN_LED11, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 16:
        digitalWrite(PIN_LED11, LOW);
        digitalWrite(PIN_LED10, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 17:
        digitalWrite(PIN_LED10, LOW);
        digitalWrite(PIN_LED9, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 18:
        digitalWrite(PIN_LED9, LOW);
        digitalWrite(PIN_LED8, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 19:
        digitalWrite(PIN_LED8, LOW);
        digitalWrite(PIN_LED7, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 20:
        digitalWrite(PIN_LED7, LOW);
        digitalWrite(PIN_LED6, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 21:
        digitalWrite(PIN_LED6, LOW);
        digitalWrite(PIN_LED5, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 22:
        digitalWrite(PIN_LED5, LOW);
        digitalWrite(PIN_LED4, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 23:
        digitalWrite(PIN_LED4, LOW);
        digitalWrite(PIN_LED3, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 24:
        digitalWrite(PIN_LED3, LOW);
        digitalWrite(PIN_LED2, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 25:
        digitalWrite(PIN_LED2, LOW);
        digitalWrite(PIN_LED1, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 26:
        digitalWrite(PIN_LED1, LOW);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = 1;
        }
    break;  
  }
}

//--------PATRON 3----------\\

void patron_3 (void)
{
  switch (estado_patron)
  {
    case 1: 

            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED13, HIGH);
              digitalWrite (PIN_LED1, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED1, HIGH);
            }

            
    break;

    case 2:

            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED1, HIGH);
              digitalWrite (PIN_LED2, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED2, HIGH);
            }
  
    break;

    case 3:

            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED2, HIGH);
              digitalWrite (PIN_LED3, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED3, HIGH);
            }

    break;

    case 4:
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED3, HIGH);
              digitalWrite (PIN_LED4, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED4, HIGH);
            }
    break;

    case 5:
            
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED4, HIGH);
              digitalWrite (PIN_LED5, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED5, HIGH);
            }
    break;

    case 6:
            
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED5, HIGH);
              digitalWrite (PIN_LED6, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED6, HIGH);
            }
    break;

    case 7:
    
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED6, HIGH);
              digitalWrite (PIN_LED7, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED7, HIGH);
            }

    break;

    case 8:
    
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED7, HIGH);
              digitalWrite (PIN_LED8, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED8, HIGH);
            }

    break;

    case 9:
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED8, HIGH);
              digitalWrite (PIN_LED9, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED9, HIGH);
            }  
    break;

    case 10:
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED9, HIGH);
              digitalWrite (PIN_LED10, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED10, HIGH);
            }
            
    break;

    case 11:
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED10, HIGH);
              digitalWrite (PIN_LED11, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED11, HIGH);
            }
    break;

    case 12:
            
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED11, HIGH);
              digitalWrite (PIN_LED12, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED12, HIGH);
            }
    break;

    case 13:
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
            
            if (bloq_p3 == true)
            {
              digitalWrite (PIN_LED12, HIGH);
              digitalWrite (PIN_LED13, LOW);
            }
            
            else
            {
              digitalWrite (PIN_LED13, HIGH);
            }              
    break;

    case 14:
            bloq_p3 = true;
            ms = 0;
            estado_patron = 1;
    break;
  }
}

//--------PATRON 4----------\\

void patron_4 (void)
{
  switch (estado_patron)
  {
    case 1:
            digitalWrite (PIN_LED7, HIGH); // se mantiene siempre encendido
            intervalo = 1;
            modo = IDA;

            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 2:
            apagarOPrenderLEDs(PIN_LED6, PIN_LED8, modo); 
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 3:
            apagarOPrenderLEDs(PIN_LED5, PIN_LED9, modo);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 4:
            apagarOPrenderLEDs(PIN_LED4, PIN_LED10, modo);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 5:
            apagarOPrenderLEDs(PIN_LED3, PIN_LED11, modo);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
           
    break;

    case 6:
            apagarOPrenderLEDs(PIN_LED2, PIN_LED12, modo);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 7:
            apagarOPrenderLEDs(PIN_LED1, PIN_LED13, modo);
            
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + intervalo;
            }
    break;

    case 8: 
            intervalo = -1;
            modo = VUELTA;
            estado_patron = estado_patron + intervalo;
            ms = 0;
    break;
  }
  
}
//--------PATRON 5----------\\

void patron_5 (void)
{
  switch (estado_patron)
  {
    case 1:
        digitalWrite (PIN_LED7, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 2:
        digitalWrite (PIN_LED7, LOW);
        digitalWrite (PIN_LED6, HIGH);
        digitalWrite (PIN_LED8, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 3:
        apagarYPrenderLEDs(PIN_LED5,PIN_LED9,PIN_LED6,PIN_LED8);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 4:
        apagarYPrenderLEDs(PIN_LED4,PIN_LED10,PIN_LED5,PIN_LED9);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 5:
        apagarYPrenderLEDs(PIN_LED3,PIN_LED11,PIN_LED4,PIN_LED10);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 6:
        apagarYPrenderLEDs(PIN_LED2,PIN_LED12,PIN_LED3,PIN_LED11);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
    
    case 7:
        apagarYPrenderLEDs(PIN_LED1,PIN_LED13,PIN_LED2,PIN_LED12);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;
     
    case 8:
        apagarYPrenderLEDs(PIN_LED2,PIN_LED12,PIN_LED1,PIN_LED13);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break; 
     
    case 9:
        apagarYPrenderLEDs(PIN_LED3,PIN_LED11,PIN_LED2,PIN_LED12);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break; 
     
    case 10:
        apagarYPrenderLEDs(PIN_LED4,PIN_LED10,PIN_LED3,PIN_LED11);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break; 
     
    case 11:
        apagarYPrenderLEDs(PIN_LED5,PIN_LED9,PIN_LED4,PIN_LED10);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break; 
    
    case 12:
        apagarYPrenderLEDs(PIN_LED6,PIN_LED8,PIN_LED5,PIN_LED9);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 13:
        digitalWrite(PIN_LED6, LOW);
        digitalWrite(PIN_LED8, LOW);
        digitalWrite(PIN_LED7, HIGH);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = estado_patron + 1;
        }
    break;

    case 14:
        digitalWrite(PIN_LED7, LOW);
        if (ms >= MS_ESPERA_LEDS)
        {
          ms = 0;
          estado_patron = 1;
        }
    break;
  }
}

//--------PATRON 6----------\\

void patron_6()
{
  switch (estado_patron)
  {
    case 1:
            digitalWrite (PIN_LED1, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 2:
            digitalWrite (PIN_LED13, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 3:
            digitalWrite (PIN_LED2, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 4:
            digitalWrite (PIN_LED12, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 5:
            digitalWrite (PIN_LED3, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 6:
            digitalWrite (PIN_LED11, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 7:
            digitalWrite (PIN_LED4, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 8:
            digitalWrite (PIN_LED10, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 9:
            digitalWrite (PIN_LED5, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 10:
            digitalWrite (PIN_LED9, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 11:
            digitalWrite (PIN_LED6, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 12:
            digitalWrite (PIN_LED8, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 13:
            digitalWrite (PIN_LED7, HIGH);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 14:
            digitalWrite (PIN_LED7, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 15:
            digitalWrite (PIN_LED8, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 16:
            digitalWrite (PIN_LED6, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 17:
            digitalWrite (PIN_LED9, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 18:
            digitalWrite (PIN_LED5, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 19:
            digitalWrite (PIN_LED10, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 20:
            digitalWrite (PIN_LED4, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 21:
            digitalWrite (PIN_LED11, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 22:
            digitalWrite (PIN_LED3, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 23:
            digitalWrite (PIN_LED12, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 24:
            digitalWrite (PIN_LED2, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 25:
            digitalWrite (PIN_LED13, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 26:
            digitalWrite (PIN_LED1, LOW);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = 1;
            }
    break;
  }
}


//--------PATRON 7----------\\

void patron_7 (void)
{
  switch (estado_patron)
  {
    case 1:
          digitalWrite (PIN_LED7, HIGH); // se mantiene siempre encendido
          digitalWrite (PIN_LED5, LOW);
          digitalWrite (PIN_LED9, LOW);
          apagarYPrenderLEDs(PIN_LED1, PIN_LED10, PIN_LED2, PIN_LED11);
          if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;


    case 2:
            apagarYPrenderLEDs(PIN_LED2, PIN_LED11, PIN_LED1, PIN_LED10);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 3:
            digitalWrite (PIN_LED6, HIGH);
            digitalWrite (PIN_LED8, HIGH);
            apagarYPrenderLEDs(PIN_LED3, PIN_LED12, PIN_LED2, PIN_LED11);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }  
    break;

    case 4:
            apagarYPrenderLEDs(PIN_LED4, PIN_LED13, PIN_LED3, PIN_LED12);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 5:
            digitalWrite (PIN_LED6, LOW);
            digitalWrite (PIN_LED8, LOW);
            digitalWrite (PIN_LED5, HIGH);
            digitalWrite (PIN_LED9, HIGH);

            apagarYPrenderLEDs(PIN_LED3, PIN_LED12, PIN_LED4, PIN_LED13);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = estado_patron + 1;
            }
    break;

    case 6:
            apagarYPrenderLEDs(PIN_LED2, PIN_LED11, PIN_LED3, PIN_LED12);
            if (ms >= MS_ESPERA_LEDS)
            {
              ms = 0;
              estado_patron = 1;
            }
    break;
  }
}


//--------PATRON 8----------\\

void patron_8(void)
{
  switch (estado_patron)
  {
    case 1:
    digitalWrite (PIN_LED1, HIGH);
    digitalWrite (PIN_LED5, HIGH);
    digitalWrite (PIN_LED10, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 2:
    digitalWrite (PIN_LED6, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 3:
    digitalWrite (PIN_LED2, HIGH);
    digitalWrite (PIN_LED11, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 4:
    digitalWrite (PIN_LED7, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 5:
    digitalWrite (PIN_LED3, HIGH);
    digitalWrite (PIN_LED12, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 6:
    digitalWrite (PIN_LED8, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 7:
    digitalWrite (PIN_LED4, HIGH);
    digitalWrite (PIN_LED13, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 8:
    digitalWrite (PIN_LED9, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 9:
    digitalWrite (PIN_LED4, LOW);
    digitalWrite (PIN_LED9, LOW);
    digitalWrite (PIN_LED13, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 10:
    digitalWrite (PIN_LED8, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 11:
    digitalWrite (PIN_LED3, LOW);
    digitalWrite (PIN_LED12, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 12:
    digitalWrite (PIN_LED7, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;
    
    case 13:
    digitalWrite (PIN_LED2, LOW);
    digitalWrite (PIN_LED11, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 14:
    digitalWrite (PIN_LED6, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 15:
    digitalWrite (PIN_LED1, LOW);
    digitalWrite (PIN_LED10, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 16:
    digitalWrite (PIN_LED5, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = 1;
    }
    break;
  }
}


//--------PATRON 9----------\\

void patron_9(void)
{
  switch (estado_patron)
  {
    case 1:
    digitalWrite (PIN_LED1, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 2:
    digitalWrite (PIN_LED2, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 3:
    digitalWrite (PIN_LED3, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 4:
    digitalWrite (PIN_LED4, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 5:
    digitalWrite (PIN_LED5, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 6:
    digitalWrite (PIN_LED6, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 7:
    digitalWrite (PIN_LED7, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 8:
    digitalWrite (PIN_LED8, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 9:
    digitalWrite (PIN_LED9, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 10:
    digitalWrite (PIN_LED10, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 11:
    digitalWrite (PIN_LED11, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 12:
    digitalWrite (PIN_LED12, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 13:
    digitalWrite (PIN_LED13, HIGH);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 14:
    digitalWrite (PIN_LED13, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 15:
    digitalWrite (PIN_LED12, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 16:
    digitalWrite (PIN_LED11, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 17:
    digitalWrite (PIN_LED10, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 18:
    digitalWrite (PIN_LED9, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 19:
    digitalWrite (PIN_LED8, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 20:
    digitalWrite (PIN_LED7, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 21:
    digitalWrite (PIN_LED6, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 22:
    digitalWrite (PIN_LED5, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 23:
    digitalWrite (PIN_LED4, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 24:
    digitalWrite (PIN_LED3, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 25:
    digitalWrite (PIN_LED2, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = estado_patron + 1;
    }
    break;

    case 26:
    digitalWrite (PIN_LED1, LOW);
    if (ms >= MS_ESPERA_LEDS)
    {
      ms = 0;
      estado_patron = 1;
    }
    break;
  }
}


//--------RESTO DE FUNCIONES---------\\

void apagarOPrenderLEDs(int pin_PIN_LED1, int pin_PIN_LED2, bool estado)
{
  if (estado == IDA)
  {
    digitalWrite (pin_PIN_LED1, HIGH);
    digitalWrite (pin_PIN_LED2, HIGH);
  }

  if (estado == VUELTA)
  {
    digitalWrite (pin_PIN_LED1, LOW);
    digitalWrite (pin_PIN_LED2, LOW);
  }
}

void apagarYPrenderLEDs (int pin_PIN_LED1, int pin_PIN_LED2, int pin_PIN_LED3, int pin_PIN_LED4)
{
  digitalWrite (pin_PIN_LED1, HIGH);
  digitalWrite (pin_PIN_LED2, HIGH);
  
  digitalWrite (pin_PIN_LED3, LOW);
  digitalWrite (pin_PIN_LED4, LOW);
}

void isr_timer()
{
  ms = ms + 1;

  msBoton[BTN_UP] = msBoton[BTN_UP] + 1;
  msBoton[BTN_DOWN] = msBoton[BTN_DOWN] + 1;
  msBoton[BTN_1] = msBoton[BTN_1] + 1;
  msBoton[BTN_2] = msBoton[BTN_2] + 1;

  if (contarMsMensaje == true)
  {
    msMensaje = msMensaje + 1;
  }
}
