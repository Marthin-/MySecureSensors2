#include <SPI.h>
#include <SD.h>
#include <EEPROM.h>
#include <Wire.h>

#ifndef MysecureSensors_h
#define MysecureSensors_h
#ifdef MY_GATEWAY_FEATURE
#define GATEWAY //comment/uncomment to select between gateway and node mode
#endif
#define READ_CONFIG //in case you want to know what your  secured network is made of ;)
#define DEBUG //more verbose
#define TRANSMISSION //for debug purpose
#ifdef GATEWAY
  #include "../MySensors/drivers/AES/AES.h"
#endif

//for data reading
#define NODE_ID_ADDR 0
#define IV_ADDR 1
#define KEY_ADDR 9
#define DESC_ADDR 41
#define DATA_SIZE 141

typedef struct {
	byte nodeId;
	unsigned long long int IV;
	byte AES_256[32];
	byte description[100];
} nodeData;

void SdReadNodeData(byte nodeId, nodeData nd);

void initiateSecureSensors();
void initiateGateway();
void initiateNode();
byte requestNodeID();
void requestDescription(byte* buff);
boolean nodeIdAvailable(File file, byte ID);
void sendCryptData();
void buildData(byte* myData);
void writeSD(byte* data);
void readSecureConfig();
void ArduinoSendNodeId();
void ArduinoSendDescription();
void ArduinoReceiveAes(int howMany);
void sendNodeId();
void sendDescription();
void receiveAes();

#endif
