#include "config.h"
#include "Chrono.h"
#include <ezButton.h>


ezButton btnToilet1(4);
ezButton btnToilet2(5);
ezButton btnToilet3(14);

#define dstncPiscine1 17

int cntToilet1 = 0;
int cntToilet2 = 0;
int cntToilet3 = 0;
int cntPiscine1 = 0;

int statePiscine1 = 0;

bool sentT1 = true;
bool sentT2 = true;
bool sentT3 = true;
bool sent = true;

AdafruitIO_Group *group = io.group("Connected Toilets");
Adafruit_MQTT_Subscribe Toiletrst1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 1");
Adafruit_MQTT_Subscribe Toiletrst2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 2");
Adafruit_MQTT_Subscribe Toiletrst3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 3");
Adafruit_MQTT_Subscribe Piscinerst1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Piscine 1");

Chrono sendTimer;
Chrono tmPiscine1;

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

  group->set("Toilet 1", cntToilet1);
  group->set("Toilet 2", cntToilet2);
  group->set("Toilet 3", cntToilet3);
  group->save();

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

  if (btnToilet1.isPressed())
  {
    Serial.print("sending T1 -> ");
    Serial.println(cntToilet1);
    group->set("Toilet 1", cntToilet1);
    group->save();
    sentT1 = false;
  }

  if (btnToilet2.isPressed())
  {
    Serial.print("sending T2 -> ");
    Serial.println(cntToilet2);
    group->set("Toilet 2", cntToilet2);
    group->save();
    sentT2 = false;
  }

  if (btnToilet3.isPressed())
  {
    Serial.print("sending T3 -> ");
    Serial.println(cntToilet3);
    group->set("Toilet 3", cntToilet3);
    group->save();
    sentT3 = false;
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
      sent = false;
    }

    else if (statePiscine1 >= 800 && !tmPiscine1.isRunning())
    {
      tmPiscine1.restart();
    }
  */
  MQTT_connect();

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription())) {

    if (subscription == &Toiletrst1) {

      Serial.print(F("Toilet1: "));
      Serial.println((char *)Toiletrst1.lastread);

      if (atoi((char *)Toiletrst1.lastread) <= 0 && sentT1 == false) {
        Serial.println("counter reset");
        btnToilet1.resetCount();
        group->set("Toilet 3", 0);
        group->save();
        sentT1 = true;
      }
    }
    if (subscription == &Toiletrst2) {

      Serial.print(F("Toilet2: "));
      Serial.println((char *)Toiletrst2.lastread);

      if (atoi((char *)Toiletrst2.lastread) <= 0 && sentT2 == false) {
        Serial.println("counter reset");
        btnToilet2.resetCount();
        group->set("Toilet 3", 0);
        group->save();
        sentT2 = true;
      }
    }
    if (subscription == &Toiletrst3) {

      Serial.print(F("Toilet3: "));
      Serial.println((char *)Toiletrst3.lastread);

      if (atoi((char *)Toiletrst3.lastread) <= 0 && sentT3 == false) {
        Serial.println("counter reset");
        btnToilet3.resetCount();
        group->set("Toilet 3", 0);
        group->save();
        sentT3 = true;
      }
    }
    if (subscription == &Piscinerst1) {

      Serial.print(F("piscine 1: "));
      Serial.println((char *)Piscinerst1.lastread);

      if (atoi((char *)Piscinerst1.lastread) <= 0 && sent == false) {
        Serial.println("counter reset");
        cntPiscine1 = 0;
        group->set("Piscine 1", 0);
        group->save();
        sent = true;
      }
    }
  }
}
void MQTT_connect()
{
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
