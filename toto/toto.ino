#define MY_DEBUG
#include <MySensors.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(MY_BAUD_RATE);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("toto");
  delay(1000);
}
