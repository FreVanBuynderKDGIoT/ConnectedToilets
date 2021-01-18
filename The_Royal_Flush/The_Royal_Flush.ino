#include "config.h"
#include "Chrono.h"
#include <ezButton.h>


ezButton btnToilet1(4);
ezButton btnToilet2(5);
ezButton btnToilet3(12);

//This sensors normally go into an analog pin
//Because the huzzah feather only has 1 analog pin, are we connecting these to a digital pin with INPUT_PULLUP
#define dstncPiscine1 A0

int cntToilet1 = 0;
int cntToilet2 = 0;
int cntToilet3 = 0;
int cntPiscine1 = 0;

int statePiscine1 = 0;

int reading;

AdafruitIO_Group *group = io.group("Connected Toilets");
Adafruit_MQTT_Subscribe Toiletrst1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 1");
Adafruit_MQTT_Subscribe Toiletrst2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 2");
Adafruit_MQTT_Subscribe Toiletrst3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 3");

Chrono sendTimer;
Chrono tmPiscine1;
Chrono tmPiscine2;
Chrono tmPiscine3;


void MQTT_connect();

void setup()
{
  Serial.begin(115200);

  // waiting for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");
  Serial.println(F("Adafruit MQTT demo"));

  io.connect();


  // Wait for a connection with the cloud
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());
  btnToilet1.setCountMode(COUNT_FALLING);
  btnToilet2.setCountMode(COUNT_FALLING);
  btnToilet3.setCountMode(COUNT_FALLING);

  mqtt.subscribe(&Toiletrst1);
  mqtt.subscribe(&Toiletrst2);
  mqtt.subscribe(&Toiletrst3);

  group->get();
}

uint32_t x = 0;

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
  statePiscine1 = analogRead(dstncPiscine1);
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

  /*
    if (statePiscine1 >= 900 && tmPiscine1.hasPassed(10000))
    {
      tmPiscine1.stop();
      statePiscine1++;
      Serial.print("sending P1-> ");
      Serial.println(statePiscine1);
      group->set("Piscine 1", cntToilet3);
      group->save();
    }
    else if (statePiscine1 >= 800 && !tmPiscine1.isRunning())
    {
      tmPiscine1.restart();
    }
    /*

  */
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription())) {

    if (subscription == &Toiletrst1) {

      Serial.print(F("Toilet1: "));
      Serial.println((char *)Toiletrst1.lastread);

      if (atoi((char *)Toiletrst1.lastread) <= 0) {
        Serial.println("counter reset");
        btnToilet1.resetCount();
      /*  Serial.print("sending T1 -> ");
        Serial.println(cntToilet1);
        group->set("Toilet 1", 0);
      */ //  group->save();
      }
    }
    if (subscription == &Toiletrst2) {

      Serial.print(F("Toilet2: "));
      Serial.println((char *)Toiletrst2.lastread);

      if (atoi((char *)Toiletrst2.lastread) <= 0) {
        Serial.println("counter reset");
        btnToilet2.resetCount();
      }
    }
    if (subscription == &Toiletrst3) {

      Serial.print(F("Toilet3: "));
      Serial.println((char *)Toiletrst3.lastread);

      if (atoi((char *)Toiletrst3.lastread) <= 0) {
        Serial.println("counter reset");
        btnToilet3.resetCount();

      }
    }
  }
}

/*
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




void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    //delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
