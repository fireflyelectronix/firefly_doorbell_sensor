# WiFi Doorbell 
This is the source code for the WiFi Doorbell from Firefly Electronix. Visit our website for more information. https://www.fireflyelectronix.com/product/wifidoorbell


Software Roadmap
- [ ] Support for OTA (Over-the-air) Updates
- [ ] Support for TLS/SSL Secure Client

### Steps required to setup Arduino IDE

1. Download and install Arduino IDE (tested with version 1.8.8)

1.  Add ESP8266 to File -> Preferences -> Additional Boards Manager URLs: http://arduino.esp8266.com/stable/package_esp8266com_index.json

1.  Install ESP8266 Board Tools -> Board: -> Boards Manager... -> Search "ESP8266" -> click Install

1.  Install Libraries Tools -> Manage Libraries -> Search -> Select Version -> click Install
    1. Install ArduinoJson Library (tested with version 5.13.4)
    1. Install PubSubClient Library (tested with version 2.7.0)

### Programming the WiFi Doorbell

1. To program the WiFi Doorbell, press and hold SW1 prior to installing the battery. This puts the ESP8266 module into UART Download mode. 

1. The 4 pin header J2 is used for progamming. 
    1. Pin 1 - VCC
    1. Pin 2 - GND
    1. Pin 3 - TX
    1. Pin 4 - RX
    
![](https://github.com/fireflyelectronix/wifidoorbell/blob/master/images/j2-pins.JPG)
