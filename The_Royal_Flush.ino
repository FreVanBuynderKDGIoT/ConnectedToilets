#include "config.h"
#include "Chrono.h"
#include <ezButton.h>

ezButton btnToilet1(4); 
ezButton btnToilet2(5); 
ezButton btnToilet3(2);

//This sensors normally go into an analog pin
//Because the huzzah feather only has 1 analog pin, are we connecting these to a digital pin with INPUT_PULLUP
#define dstncPiscine1 4
#define dstncPiscine2 5
#define dstncPiscine3 2

int cntToilet1 = 0;
int cntToilet2 = 0;
int cntToilet3 = 0;
int cntPiscine1 = 0;
int cntPiscine2 = 0;
int cntPiscine3 = 0;

int reading;

AdafruitIO_Group *group = io.group("Connected Toilets");


Chrono sendTimer;
Chrono tmPiscine1;
Chrono tmPiscine2;
Chrono tmPiscine3;


void setup()
{
  Serial.begin(115200);

  // waiting for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");

  io.connect();

  //group->onMessage("Toilet 1", handleToilet1);
  //group->onMessage("Toilet 2", handleToilet2);
  //group->onMessage("Toilet 3", handleToilet3);

  // Wait for a connection with the cloud
  while(io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
  btnToilet1.setCountMode(COUNT_FALLING);
  btnToilet2.setCountMode(COUNT_FALLING);
  btnToilet3.setCountMode(COUNT_FALLING);
  
  group->get();
}

void loop() 
{
  // this line is possible in the if statements, we will try if this works
  io.run(); // keeps the client connected

  btnToilet1.loop();
  btnToilet2.loop();
  btnToilet3.loop();
  
  cntToilet1 = btnToilet1.getCount();
  cntToilet2 = btnToilet2.getCount();
  cntToilet3 = btnToilet3.getCount();

  //normally on digital pins we use digitalRead(), we use analogRead() because we don't have to change the code later on
  //statePiscine1 = analogRead(dstncPiscine1);
  //statePiscine2 = analogRead(dstncPiscine2);
  //statePiscine3 = analogRead(dstncPiscine3);


  if (btnToilet1.isPressed())
  { 
    Serial.print("sending T1 -> ");
    Serial.println(cntToilet1);
    group->set("Toilet 1", cntToilet1);
    group->save();
  }
  if (btnToilet2.isPressed())
  {
    Serial.print("sending T2 -> ");
    Serial.println(cntToilet2);
    group->set("Toilet 2", cntToilet2);
    group->save();
  }
  if (btnToilet3.isPressed())
  {
    Serial.print("sending T3 -> ");
    Serial.println(cntToilet3);
    group->set("Toilet 3", cntToilet3);
    group->save();
  }
}
/*
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
*/
void handleToilet1(AdafruitIO_Data *data)
{
  reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  cntToilet1 = reading;
}

void handleToilet2(AdafruitIO_Data *data)
{
  reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  cntToilet2 = reading;
}

void handleToilet3(AdafruitIO_Data *data)
{
  reading = data->toInt();
  Serial.print("received <- ");
  Serial.println(reading);
  cntToilet3 = reading;
}
/*
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
*/
