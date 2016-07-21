#include <Arduino.h>
#include <EEPROM.h>

#include <Wire.h>

#define MY_RADIO_NRF24

#define GATEWAY

#include <MySensors.h>
#include <MyConfig.h>


AES aes;
unsigned long long int IV;
byte AES_256_key[32];


void setup() {
  // put your setup code here, to run once:
  Serial.begin(MY_BAUD_RATE);
  Serial.println("============= MySensors =============");
  Serial.println("======= Secured Device Pairing ======");
  #ifdef GATEWAY
  Serial.println("======= Gateway mode: engaged =======");
  Serial.print("Generating Initialization Vector...");
  IV=aes.generate_IV();
  Serial.println("[OK]");
  Serial.print("Generating AES-256 key.............");
  aes.generate_main_key(AES_256_key);
  Serial.println("[OK]");
  #else
  Serial.println("======== Node mode: engaged =========");
  #endif

  Serial.println("=====================================");
}

void loop() {
  // put your main code here, to run repeatedly:

}
