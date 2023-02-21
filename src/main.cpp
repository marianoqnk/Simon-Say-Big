#include <Arduino.h>
// Arduino minipro
#include <LowPower.h>
#define TIME_INTERMITENTE SLEEP_500MS
#define VECES_INTERMITENTE 3
#define TIEMPO_ON SLEEP_250MS
#define TIEMPO_OFF SLEEP_500MS
#define TIEMPO_PULSAR 1000
#define LAT "40 19.732"
#define LONG "03 43.295"
#define INTENTOS 10
#define BOTONES 4
#define DEBUG
enum colores
{
  azul = 1,
  rojo = 2,
  verde = 3,
  naranja = 4
};
const int led[BOTONES] = {1, 2, 3, 4};
int secuencia[INTENTOS];
volatile byte wakeupSource;
bool error = false;
void delayLowPower(period_t tiempo)
{
  LowPower.powerDown(tiempo, ADC_OFF, BOD_OFF);
}

void todosLeds(boolean encendidos)
{

  for (int n = 0; n < BOTONES; n++)
    digitalWrite(led[n], encendidos);
}

void enciendeLed(int n)
{

  digitalWrite(led[n], LOW);
  delayLowPower(TIEMPO_ON);
  digitalWrite(led[n], HIGH);
}

void ledsInicio()
{

  for (int n = 0; n < INTENTOS; n++)
  {
    secuencia[n] = random(3); // para eso utiliza randon
    Serial.print(secuencia[n]);
  }
  Serial.println();

  for (int k = 0; k < VECES_INTERMITENTE; k++)
  {
    for (int n = 0; n < BOTONES; n++)
      digitalWrite(led[n], !(n % 2));
    delayLowPower(TIME_INTERMITENTE);
    for (int n = 0; n < BOTONES; n++)
      digitalWrite(led[n], (n % 2));
    delayLowPower(TIME_INTERMITENTE);
  }
  todosLeds(HIGH);
  delayLowPower(TIEMPO_ON);
}
void wakeUp(void)
{
  wakeupSource = 1;
}
boolean leePulsador(int cual)
{
  int tiempo = TIEMPO_PULSAR / 10;
  for (int k = 0; k < tiempo; k++)
    for (int n = 0; n < BOTONES; n++)
    {
      if (!digitalRead(led[n]))
      {
        enciendeLed(n);
        if (n == cual)
          return true;
        else
          return false;
      }
      delay(10);
    }
  return false;
}


void ledsError()
{

  for (int k = 0; k < VECES_INTERMITENTE; k++)
  {
    todosLeds(LOW);
    delayLowPower(TIEMPO_ON);
    todosLeds(HIGH);
    delayLowPower(TIEMPO_OFF);
  }
}

void lucesFinal()
{
  // azul,rojo,verde

  for (byte n = 0; n < 5; n++)
  {
    enciendeLed(0); // azul
    delayLowPower(TIEMPO_OFF);
  }
  for (byte n = 0; n < 7; n++)
  {
    enciendeLed(1); // rojo
    delayLowPower(TIEMPO_OFF);
  }
  for (byte n = 0; n < 4; n++)
  {
    enciendeLed(2); // Verde
    delayLowPower(TIEMPO_OFF);
  }
  delayLowPower(SLEEP_2S);
}

void setup()
{
#ifdef DEBUG
  Serial.begin(115000);
#endif
  randomSeed(analogRead(A7)); // inicia el array con los numeros aleatorios
  for (int n = 0; n < BOTONES; n++)
  {
    pinMode(led[n], OUTPUT);
  }
}

void loop()
{
  error = false;
  ledsInicio();
  for (int k = 0; k < INTENTOS; k++)
  {

    for (int n = 0; n < k; n++)
    { // muestra secuencia
      enciendeLed(secuencia[n]);
      delayLowPower(TIEMPO_ON);
    }
    for (int n = 0; n < k; n++)
    {
      if (!leePulsador(secuencia[n]))
      {
        ledsError();
        error = true;
        break;
      }
    }
    if (error)
      break;
    todosLeds(LOW);
    delayLowPower(TIEMPO_ON);
    todosLeds(HIGH);
    delayLowPower(TIEMPO_OFF);
  }
  if (!error)
  {
    for (byte n = 0; n < 3; n++)
      lucesFinal();
    for (int n = 0; n < BOTONES; n++)
      pinMode(led[n], INPUT);
  }
  attachInterrupt(digitalPinToInterrupt(2), wakeUp, HIGH);
  attachInterrupt(digitalPinToInterrupt(3), wakeUp, HIGH);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(2));
  detachInterrupt(digitalPinToInterrupt(3));
  for (int n = 0; n < BOTONES; n++)
    pinMode(led[n], OUTPUT);
}





/*void escribeCadena(char *cadena)
{
  int n = 0;
  int k[8];
  int c;
  boolean punto = false;
  while (cadena[n] != 0)
  {
    c = cadena[n];
    if (c == 32)c = 12; else c -= 48;
    if (cadena[n + 1] == '.') {
      punto = true;
      n++;
    } else punto = false ;

    n++;
  }
}
*/
