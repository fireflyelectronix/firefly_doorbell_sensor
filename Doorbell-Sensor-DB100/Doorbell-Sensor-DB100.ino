#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>         //MQTT library
#include "src/ConfigPortal.h"     //all the config portal code moved here to keep the main sketch clean. Based on WiFiManager
#include "src/Blinker.h"          //Library to help with managing the LED blink. Uses millis() instead of delay()


const int buttonPin = 0;              //the button for resetting wifi credentials
int buttonState;                      //the current state of the button input
int lastButtonState = HIGH;           //the previouse reading from the button input
unsigned long lastDebounceTime = 0;   // the last time the output pin was toggled
unsigned long debounceDelay = 50;     // the debounce time; increase if the output flickers

Blinker led1(4, 500, 500);            //create a new blink pattern for the led on pin 4. On for 500ms off for 500ms


WiFiClient wifiClient;
PubSubClient client(wifiClient);

  //function used to reconnect the mqtt client. called in loop.
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create the clientID using the ESP8266 Chip ID
    String clientId = "FireFlyClient-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(mqtt_topic, "ring");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 3 seconds");
      // Wait 3 seconds before retrying
      delay(3000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  configPortal();
  client.setServer(mqtt_server, atoi(mqtt_port));
}

void loop() {
  // put your main code here, to run repeatedly:
  yield();

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //read the state of the button
  int buttonReading = digitalRead(buttonPin);

  if (buttonReading != lastButtonState) {
    lastDebounceTime = millis();        //set the last time a button was pressed to now
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonReading != buttonState) {
      buttonState = buttonReading;
    }
    if (buttonState == LOW) {
      unsigned long currentMillis = millis();
      if ((currentMillis - lastDebounceTime >= 2000)) {
        for (int i = 1; i <= 3; i++) {    //Blink the LED 3 times
          digitalWrite(4, HIGH);
          delay(300);
          digitalWrite(4, LOW);
          delay(300);
        }
        Serial.println("Erasing WiFi Credentials");
        WiFi.disconnect(true);
        delay(200);
        ESP.reset();
      }
    }
  }
  lastButtonState = buttonReading;    //save the button reading for the next time through the loop
}
