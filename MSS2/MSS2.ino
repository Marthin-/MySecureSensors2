#include <MySecureSensors.h>

#define MY_GATEWAY_SERIAL
#define MY_RADIO_NRF24
#define MY_RF24_ENABLE_ENCRYPTION
#define MY_SECURE_SENSORS
#include <MySensors.h>
void setup() {
  // put your setup code here, to run once:
  initiateSecureSensors();
}

void loop() {
  // put your main code here, to run repeatedly:

}
