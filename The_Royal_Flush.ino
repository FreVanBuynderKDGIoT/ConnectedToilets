#include "config.h"
#include "Chrono.h"

#define btnToilet1 1
#define btnToilet2 2
#define btnToilet3 3

//This sensors normally go into an analog pin
//Because the huzzah feather only has 1 analog pin, are we connecting these to a digital pin with INPUT_PULLUP
#define dstncPiscine1 4
#define dstncPiscine2 5
#define dstncPiscine3 6

bool stateToilet1;
bool stateToilet2;
bool stateToilet3;
bool statePiscine1;
bool statePiscine2;
bool statePiscine3;

int cntToilet1 = 0;
int cntToilet2 = 0;
int cntToilet3 = 0;
int cntPiscine1 = 0;
int cntPiscine2 = 0;
int cntPiscine3 = 0;

AdafruitIO_Feed *toilet1 = io.feed("Toilet 1");
AdafruitIO_Feed *toilet2 = io.feed("Toilet 2");
AdafruitIO_Feed *toilet3 = io.feed("Toilet 3");
AdafruitIO_Feed *piscine1 = io.feed("Piscine 1");
AdafruitIO_Feed *piscine2 = io.feed("Piscine 2");
AdafruitIO_Feed *piscine3 = io.feed("Piscine 3");

Chrono tmPiscine1;
Chrono tmPiscine2;
Chrono tmPiscine3;


void setup()
{
  Serial.begin(9600);

  // waiting for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");

  io.connect();

  // Wait for a connection with the cloud
  while(io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  pinMode(btnToilet1, INPUT_PULLUP);
  pinMode(btnToilet2, INPUT_PULLUP);
  pinMode(btnToilet3, INPUT_PULLUP);

  // This pins are INPUT_PULLUP but must be INPUT if on an analog pin
  pinMode(dstncPiscine1, INPUT_PULLUP);
  pinMode(dstncPiscine2, INPUT_PULLUP);
  pinMode(dstncPiscine3, INPUT_PULLUP);

  toilet1->onMessage(handleToilet1);
  toilet2->onMessage(handleToilet2);
  toilet3->onMessage(handleToilet3);
  piscine1->onMessage(handlePiscine1);
  piscine2->onMessage(handlePiscine2);
  piscine3->onMessage(handlePiscine3);
}

void loop() 
{
  // this line is possible in the if statements, we will try if this works
  io.run(); // keeps the client connected

  stateToilet1 = digitalRead(btnToilet1);
  stateToilet2 = digitalRead(btnToilet2);
  stateToilet3 = digitalRead(btnToilet3);

  //normally on digital pins we use digitalRead(), we use analogRead() because we don't have to change the code later on
  statePiscine1 = analogRead(dstncPiscine1);
  statePiscine2 = analogRead(dstncPiscine2);
  statePiscine3 = analogRead(dstncPiscine3);

  if (stateToilet1 == true)
  {
    cntToilet1++;
    
    Serial.print("sending -> ");
    Serial.println(cntToilet1);
    toilet1->save(cntToilet1);
  }
  if (stateToilet2 == true)
  {
    cntToilet2++;
    
    Serial.print("sending -> ");
    Serial.println(cntToilet2);
    toilet2->save(cntToilet2);
  }
  if (stateToilet3 == true)
  {
    cntToilet3++;
    
    Serial.print("sending -> ");
    Serial.println(cntToilet3);
    toilet3->save(cntToilet3);
  }
  
  if (statePiscine1 >= 800 && tmPiscine1.hasPassed(10000))
  {
    tmPiscine1.stop();
    statePiscine1++;
    Serial.print("sending -> ");
    Serial.println(statePiscine1);
    piscine1->save(statePiscine1);
  }
  else if (statePiscine1 >= 800 && !tmPiscine1.isRunning())
  {
    tmPiscine1.restart();
  }
  
  if (statePiscine2 >= 800 && tmPiscine2.hasPassed(10000))
  {
    tmPiscine2.stop();
    statePiscine2++;
    Serial.print("sending -> ");
    Serial.println(statePiscine2);
    piscine2->save(statePiscine2);
  }
  else if (statePiscine2 >= 800 && !tmPiscine2.isRunning())
  {
    tmPiscine2.restart();
  }
  
  if (statePiscine3 >= 800 && tmPiscine3.hasPassed(10000))
  {
    tmPiscine3.stop();
    statePiscine3++;
    Serial.print("sending -> ");
    Serial.println(statePiscine3);
    piscine3->save(statePiscine3);
  }
  else if (statePiscine3 >= 800 && !tmPiscine3.isRunning())
  {
    tmPiscine3.restart();
  }
}

void handleToilet1(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntToilet1, reading);
}

void handleToilet2(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntToilet2, reading);
}

void handleToilet3(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntToilet3, reading);
}

void handlePiscine1(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntPiscine1, reading);
}

void handlePiscine2(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntPiscine2, reading);
}

void handlePiscine3(AdafruitIO_Data *data)
{
  int reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  analogWrite(cntPiscine3, reading);
}
