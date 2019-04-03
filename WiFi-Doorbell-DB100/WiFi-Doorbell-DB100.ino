#include <FS.h>                   //SPIFFS File System to store files
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <ESP8266HTTPClient.h>    //library for HTTP client
#include <PubSubClient.h>         //MQTT library
#include "src/ConfigPortal.h"     //all the config portal code moved here to keep the main sketch clean. Based on WiFiManager

ADC_MODE (ADC_VCC);  //set the ADC mode of the ESP8266 to read the voltage level

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
const long client_interval = 2000; //only run the http and mqtt clients every 2 seconds

unsigned long loop_timer = 0;
const long loop_interval = 8000; //run loop for 8 seconds then go to sleep

bool ifttt_sent = false;
bool mqtt_sent = false;

WiFiClient wifiClient;
HTTPClient http;
PubSubClient client(wifiClient);


void sendMQTT() {

  if (!client.connected()) {
    client.setServer(mqtt_server, atoi(mqtt_port)); //setup the mqtt target server and port
  }

  float vcc = ((float)ESP.getVcc())/1024;
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& JSONvoltage = jsonBuffer.createObject();
  JSONvoltage["batt"] = vcc;
  JSONvoltage.printTo(Serial);
  char JSONmessageBuffer[100];
  JSONvoltage.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  char batt_topic[100];
  const char *battery = "/battery";
  strcpy(batt_topic, mqtt_topic);
  strcat(batt_topic, battery);
  Serial.println(batt_topic);

  Serial.print("MQTT: Attempting MQTT connection...");
    // Create the clientID using the ESP8266 Chip ID
  String clientId = "FireFlyClient-";
  clientId += String(random(0xffff), HEX);
  // Attempt to connect
  if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
    Serial.println("MQTT: Connected");
    // Once connected, publish an announcement...
    client.publish(batt_topic, JSONmessageBuffer, true);
    Serial.println("MQTT: Battery Level Sent");
    client.publish(mqtt_topic, "ON");
    Serial.println("MQTT: Status Sent");
    mqtt_sent = true;
  } else {
    Serial.println("MQTT: Connection failed, rc=");
    Serial.print(client.state());
    }
}

void sendIFTTT() {

  float vcc = ((float)ESP.getVcc())/1024;
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& JSONvoltage = jsonBuffer.createObject();
  JSONvoltage["value1"] = vcc;
  JSONvoltage.printTo(Serial);
  char JSONmessageBuffer[100];
  JSONvoltage.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

  http.begin(wifiClient, "http://maker.ifttt.com/trigger/doorbell/with/key/" + String(ifttt_key));
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(JSONmessageBuffer);
  String payload = http.getString();

  Serial.println(httpCode);
  Serial.println(payload);

  http.end();
  ifttt_sent = true; //todo - need logic to get the http response before setting this flag.
  Serial.println("IFTTT: Trigger Sent");

}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(pin_s1, INPUT_PULLUP);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  loadConfigFile();
}

void loop() {

  loop_timer = millis();

  while(millis() - loop_timer < loop_interval) {

    if (millis() - client_timer >= client_interval) { //Only run this periodically based on client_interval
      client_timer = millis(); //reset the clock starting point
      if (WiFi.status() == WL_CONNECTED) { //Only run the following once we are connected to wifi
        if (strlen(mqtt_topic) != 0) { //check if we have a value stored for mqtt setting
          if (mqtt_sent == false) { //if we haven't published to mqtt, then publish to mqtt server
            sendMQTT();
          }
        }

        if (strlen(ifttt_key) != 0) { //check to see if there a value for the http address
          if (ifttt_sent == false) { //if we haven't sent an i, then send one
            sendIFTTT();
          }
        }

      }
    }

    //client.loop(); may not be needed since we do not need to subcribe to incomming messages.

    s1buttonState(); //check the state of the button

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
  delay(100); //the module likes to have a delay after sending deepsleep.

}

void s1buttonState(){
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
