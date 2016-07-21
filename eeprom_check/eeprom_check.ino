#include <EEPROM.h>
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  byte fonzie = 5;
  EEPROM.put(2,fonzie);
  Serial.println(EEPROM.read(2));
}

void loop() {
  // put your main code here, to run repeatedly:

}
