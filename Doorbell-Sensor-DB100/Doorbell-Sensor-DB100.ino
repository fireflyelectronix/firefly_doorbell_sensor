#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>         //MQTT library
#include "src/ConfigPortal.h"     //all the config portal code moved here to keep the main sketch clean. Based on WiFiManager

//varialbles for switch 1 (s1)
int state_s1 = 0;
int prev_state_s1 = 0;
int pin_s1 = 0;
int val_s1 = 0;
unsigned long t_s1 = 0;
unsigned long t_0_s1 = 0;
#define bounce_delay_s1 10
#define hold_delay_s1 2000 //Hold the button for 2 seconds to enter config portal

unsigned long client_timer = 0;
const long client_interval = 2000; //check if mqtt client is connected every 2 seconds. reconnect if not connected

unsigned long loop_timer = 0;
const long loop_interval = 8000; //run loop for 8 seconds then go to sleep

WiFiClient wifiClient;
PubSubClient client(wifiClient);

  //function used to reconnect the mqtt client. called in loop.
void reconnect() {
  Serial.print("Attempting MQTT connection...");
    // Create the clientID using the ESP8266 Chip ID
  String clientId = "FireFlyClient-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("MQTT connected");
    // Once connected, publish an announcement...
    client.publish(mqtt_topic, "ring");
  } else {
    Serial.println("MQTT Connection failed, rc=");
    Serial.print(client.state());
    }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pin_s1, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  loadConfigFile();
  client.setServer(mqtt_server, atoi(mqtt_port));
}

void loop() {

  loop_timer = millis();

  while(millis() - loop_timer < loop_interval) {

    if (millis() - client_timer >= client_interval) {
      client_timer = millis();
      if (!client.connected()) {
        reconnect();
      }
    }

    client.loop();

    statemachine_s1();

    if (state_s1 == 5) {
      //nothing here yet for short press. maybe for reset
    }

    if (state_s1 == 6) {//if there is a long press, open config portal
      configPortal();
      loop_timer = millis(); //reset the loop timer once we return from the config portal
    }
    yield();
  }

  Serial.println("Going to sleep");
  ESP.deepSleep(0);

}   //end of the void loop function

void statemachine_s1(){
  val_s1 = digitalRead(pin_s1);
  prev_state_s1 = state_s1;

  switch (state_s1) {
    case 0: //Reset the state
      state_s1 = 1;
    break;

    case 1: //waiting to see if the switch goes LOW
      if (val_s1 == LOW) {state_s1 = 2;}
    break;

    case 2: //the switch has sensed a LOW. Start the clock
      t_0_s1 = millis();
      state_s1 = 3;
    break;

    case 3: //debounce the switch
      t_s1 = millis();
      if (t_s1 - t_0_s1 > bounce_delay_s1) {state_s1 = 4;}
      if (val_s1 == HIGH) {state_s1 = 0;}
    break;

    case 4: //check how the button is pressed short vs long press
      t_s1 = millis();
      if (val_s1 == HIGH) {state_s1 = 5;}
      if (t_s1 - t_0_s1 > hold_delay_s1) {state_s1 = 6;}
    break;

    case 5: //short press state then reset the state to 0
      state_s1 = 0;
    break;

    case 6: //long press state then go to wait state
      state_s1 = 7;
    break;

    case 7: //wait for switch to be released and reset state
      if (val_s1 == HIGH) {state_s1 = 0;}
    break;
  }

}
