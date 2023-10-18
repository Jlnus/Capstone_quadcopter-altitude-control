/*************************************
ESP8266 TCP connection
WiFI Passthrough Mode
**************************************/

#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); // RX, TX

void setup() {
    Serial.begin(115200);
    mySerial.begin(115200);

    delay(5000);                      // delay boot-up time
    sendData("AT+CWMODE=3\r\n", 100); // configure as access point (working mode: AP+STA)
    sendData("AT+CWJAP=\"WiFI SSID\",\"WiFI password\"\r\n", 2000); // connect WiFi
    sendData("AT+CIPMUX=0\r\n", 100); // configure for single connections
    sendData("AT+CIPSTART=\"TCP\",\"IP address\",1234\r\n", 500); // connect server [type, ip, port]
    sendData("AT+CIPMODE=1\r\n", 100);                            // set to passthrough mode
    sendData("AT+CIPSEND\r\n", 500);                              // start sending message
}

void loop() {
    if (Serial.available()) {
        mySerial.write(Serial.read());
    }
    if (mySerial.available()) {
        Serial.write(mySerial.read());
    }
}

void sendData(String command, const int timeout) {
    mySerial.print(command);
    delay(timeout);
}
