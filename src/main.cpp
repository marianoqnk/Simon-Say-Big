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
#define SPEAKER 6
#define BOTONES 4
//#define DEBUG
/*
B♭ (blue, lower right);
C♯ (yellow, lower left);
F (red, upper right).
B♭ (green, upper left, an octave higher than blue);
*/
#define B 466.164
#define C 277.183
#define FA 349.228
#define BUP 932.328
float notas[4] = {C, FA, B, BUP};
enum colores
{
  azul = 1,
  rojo = 2,
  verde = 3,
  naranja = 4
};
const int led[BOTONES] = {2, 3, 4, 5};
int secuencia[INTENTOS];
volatile byte wakeupSource;
bool error = false;
void delayLowPower(period_t tiempo)
{
  if (tiempo == TIEMPO_ON)
    delay(250);
  else
    LowPower.powerDown(tiempo, ADC_OFF, BOD_OFF);
}

void todosLeds(boolean encendidos)
{

  for (int n = 0; n < BOTONES; n++)
    digitalWrite(led[n], encendidos);
}

void enciendeLed(int n)
{
  float nota;
  tone(SPEAKER, notas[n], 250);
  digitalWrite(led[n], LOW);
  delayLowPower(TIEMPO_ON);
  digitalWrite(led[n], HIGH);
}

void ledsInicio()
{

  for (int n = 0; n < INTENTOS; n++)
  {
    secuencia[n] = random(BOTONES); // para eso utiliza randon
#ifdef DEBUG
    Serial.print(secuencia[n]);
#endif
  }
#ifdef DEBUG
  Serial.println();
#endif

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
  for (byte n = BOTONES; n > 0; n--)
  {
    tone(SPEAKER, notas[n], 250);
    delay(250);
  }
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
  for (byte n = 0; n < BOTONES; n++)
  {
    tone(SPEAKER, notas[n], 250);
    delay(250);
  }

  for (byte n = 0; n < 5; n++)
  {
    enciendeLed(0); // azul
    delayLowPower(TIEMPO_OFF);
  }
  for (byte n = 0; n < 7; n++)
  {
    enciendeLed(1); // verde
    delayLowPower(TIEMPO_OFF);
  }
  for (byte n = 0; n < 4; n++)
  {
    enciendeLed(2); // amarillo
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
  tone(SPEAKER, 400, 1000);
  delay(500);
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
    for (byte n = 0; n < 2; n++) //repeat three times
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
