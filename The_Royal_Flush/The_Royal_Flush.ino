#include "config.h"
#include <ezButton.h>

//This sets up the buttons to the pins on the board
ezButton btnToilet1(4);
ezButton btnToilet2(5);
ezButton btnToilet3(14);

#define dstncPiscine1 A0

//Sets all the counters to 0
int cntToilet1 = 0;
int cntToilet2 = 0;
int cntToilet3 = 0;
int cntPiscine1 = 0;

int statePiscine1 = 0;

//We use this to not get stuck in a loop later on
bool sentT1 = true;
bool sentT2 = true;
bool sentT3 = true;
bool sent = true;
bool prsnPiscine1 = false;
bool cntreset = false;

//This makes sure that we are connected to the right group on our dashboard
AdafruitIO_Group *group = io.group("Connected Toilets");
Adafruit_MQTT_Subscribe Toiletrst1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 1");
Adafruit_MQTT_Subscribe Toiletrst2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 2");
Adafruit_MQTT_Subscribe Toiletrst3 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Toilet 3");
Adafruit_MQTT_Subscribe Piscinerst1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/Piscine 1");

//This make shure that the millis for the picine works
long previousMillis = 0;
long interval = 10000;


void MQTT_connect();

void setup()
{
  //This starts the serial monitor
  Serial.begin(115200);

  pinMode(dstncPiscine1, INPUT);
  // waiting for serial monitor to open
  while (! Serial);

  Serial.print("Connecting to Adafruit IO");
  Serial.println(F("Adafruit MQTT demo"));
  //This connects us to the cloud
  io.connect();


  // Wait for a connection with the cloud
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  //We use count_falling to set that the button is pressed when the button is pushed down
  btnToilet1.setCountMode(COUNT_FALLING);
  btnToilet2.setCountMode(COUNT_FALLING);
  btnToilet3.setCountMode(COUNT_FALLING);

  mqtt.subscribe(&Toiletrst1);
  mqtt.subscribe(&Toiletrst2);
  mqtt.subscribe(&Toiletrst3);
  mqtt.subscribe(&Piscinerst1);

  group->set("Toilet 1", cntToilet1);
  group->set("Toilet 2", cntToilet2);
  group->set("Toilet 3", cntToilet3);
  group->set("Piscine 1", cntPiscine1);
  group->save();

  group->get();
}

uint32_t x = 0;

void loop()
{
  // this line is possible in the if statements, we will try if this works
  io.run(); // keeps the client connected

  //Doing this allows us to use the is.Pressed function
  btnToilet1.loop();
  btnToilet2.loop();
  btnToilet3.loop();

  //Sets the counter in a variable
  cntToilet1 = btnToilet1.getCount();
  cntToilet2 = btnToilet2.getCount();
  cntToilet3 = btnToilet3.getCount();

  statePiscine1 = analogRead(dstncPiscine1);

  //Here we check if the buttons are pressed
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

  statePiscine1 = analogRead(dstncPiscine1);
  unsigned long currentMillis = millis();


  if ((statePiscine1) == (1024)) {
    Serial.print("state");
    Serial.println(statePiscine1);
  }
  //This will detect when there is someone standing in front of the sensor for at least 10 seconds
  if ((currentMillis - previousMillis > interval) && (statePiscine1 == 1024)) {
    previousMillis = currentMillis;
    prsnPiscine1 = true;
    Serial.print("prsn");
  }

  //this will count if a person leaves and has been standing in front of the sensor for at least 10 seconds 
  if ((statePiscine1 < 1024) && (prsnPiscine1 == true)) {
    cntPiscine1++;
    Serial.print("count");
    Serial.println(cntPiscine1);
    group->set("Piscine 1", cntPiscine1);
    group->save();
    prsnPiscine1 = false;
    sent = false;
  }
  //This will reset the counter if there hasn't been a person in front of the sensor for at least 10 seconds.
  if ((currentMillis - previousMillis > interval) && (statePiscine1 < 1024)) {
    previousMillis = currentMillis;
    prsnPiscine1 = false;
    Serial.print("rst count");
  }


  MQTT_connect();    //This is a protocol we use to connect

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription())) {

    if (subscription == &Toiletrst1) {

      Serial.print(F("Toilet1: "));
      Serial.println((char *)Toiletrst1.lastread);

      if (atoi((char *)Toiletrst1.lastread) <= 0 && sentT1 == false) {
        Serial.println("counter reset");
        btnToilet1.resetCount();
        group->set("Toilet 1", 0);    //Sets the value ready for sending to the cloud
        group->save();    //Sends the value to the cloud
        sentT1 = true;    //This prevents an infinite loop to begin
      }
    }
    if (subscription == &Toiletrst2) {

      Serial.print(F("Toilet2: "));
      Serial.println((char *)Toiletrst2.lastread);

      if (atoi((char *)Toiletrst2.lastread) <= 0 && sentT2 == false) {
        Serial.println("counter reset");
        btnToilet2.resetCount();
        group->set("Toilet 2", 0);    //Sets the value ready for sending to the cloud
        group->save();    //Sends the value to the cloud
        sentT2 = true;     //This prevents an infinite loop to begin
      }
    }
    if (subscription == &Toiletrst3) {

      Serial.print(F("Toilet3: "));
      Serial.println((char *)Toiletrst3.lastread);

      if (atoi((char *)Toiletrst3.lastread) <= 0 && sentT3 == false) {
        Serial.println("counter reset");
        btnToilet3.resetCount();
        group->set("Toilet 3", 0);    //Sets the value ready for sending to the cloud
        group->save();    //Sends the value to the cloud
        sentT3 = true;     //This prevents an infinite loop to begin
      }
    }
    if (subscription == &Piscinerst1) {

      Serial.print(F("Piscine1: "));
      Serial.println((char *)Piscinerst1.lastread);

      if (atoi((char *)Piscinerst1.lastread) <= 0 && sent == false) {
        Serial.println("counter reset");
        cntPiscine1 = 0;
        group->set("Piscine 1", 0);   //Sets the value ready for sending to the cloud
        group->save();    //Sends the value to the cloud
        sent = true;     //This prevents an infinite loop to begin
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
