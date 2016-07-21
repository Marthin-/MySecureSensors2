#include "MysecureSensors.h"

#ifdef GATEWAY
AES aes;
#endif
unsigned long long int IV;
byte buff[40];
byte nodeID;
File myFile;
byte AES_256_key[32];
byte buff_8[8];
byte descr[100];
byte myData[DATA_SIZE];

void initiateSecureSensors(){
#ifdef DEBUG
	Serial.println("============= MySensors =============");
	Serial.println("======= Secured Device Pairing ======");
#endif
	SD.begin();
#ifdef GATEWAY
	initiateGateway();
#endif
#ifndef GATEWAY
	initiateNode();
#endif
}


#ifdef GATEWAY
/*****************************************************************
 * GATEWAY SIDE : ASK FOR NODE ID & DESCRIPTION, GENERATE AES KEY *
 * AND IV, SEND THROUGH I2C, ADDS NODE TO FILE STORED INTO SD CARD*
 *****************************************************************/

//////////////////////////////////////////////////////////////////
/********************** main gateway code ***********************/
//////////////////////////////////////////////////////////////////
void initiateGateway(){
	Serial.println("======= Gateway mode: engaged =======");
	Wire.begin();
	//Ask node for data so that it can be added
	nodeID=101;
#ifdef TRANSMISSION
	nodeID=requestNodeID();
	requestDescription(descr);
#endif
#ifndef TRANSMISSION
	nodeID=252;
	byte fonzie[41]= "Adding dummy sensor to test gateway side"; //prevents buffer overflow
	for (int i=0;i<41;i++){
		descr[i]=fonzie[i];
	}
#endif
	Serial.print("Adding node number ");
	Serial.print(nodeID);
	Serial.println(" to secured MySensors");

	//Check configuration on SD card to avoid redundancy
	Serial.println("Checking duplication");
	if(nodeIdAvailable(myFile,nodeID)!=true){
		Serial.println("Error adding sensor : Node ID already taken");
		return;
	}
	Serial.println("No duplication found.");

	//Generate AES key & IV
	Serial.println("Generating Initialization Vector");
	IV=aes.generate_IV();
	Serial.println("[OK]");
#ifdef DEBUG
	Serial.println("Generated IV :");
	Serial.write((byte*)IV,8);
#endif
	Serial.println("\nGenerating AES-256 key");
	aes.generate_main_key(AES_256_key);
	Serial.println("[OK]");
#ifdef DEBUG
	Serial.println("Generated key : ");
	Serial.write(AES_256_key,32);
	Serial.println("");
#endif

#ifdef TRANSMISSION
	//Send IV & key through I2C
	sendCryptData();
#endif

	//build data to be written on SD card
	buildData(myData);

	//store data on SD card
	Serial.println("Storing data on SD card");
	writeSD(myData);
	Serial.println("[OK]");

#ifdef READ_CONFIG
	//Read sensors base on SD card
	readSecureConfig();
#endif
	//finished initialization
	Serial.println("=====================================");
	Serial.println("You can now upload your MySensors \ncode and enjoy your secure network ;)");
}

//////////////////////////////////////////////////////////////////
/**************** request node ID through I2C *******************/
//////////////////////////////////////////////////////////////////
byte requestNodeID(){
#ifdef DEBUG
	Serial.println("Asking node for nodeID...");
#endif
	byte nodeID;
	Wire.requestFrom(8,1);
	while(Wire.available()){
		nodeID=Wire.read();
	}
	return nodeID;
}

/////////////////////////////////////////////////////////////////
/******************** request node information *****************/
/////////////////////////////////////////////////////////////////
void requestDescription(byte* buff){
#ifdef DEBUG
	Serial.println("Asking node for description...");
#endif
	byte cpt=0;
	Wire.requestFrom(8,100);
	while(Wire.available()){
		buff[cpt]=Wire.read();
		cpt++;
	}
}

////////////////////////////////////////////////////////////////
/*********** check SD card if node ID is available ************/
////////////////////////////////////////////////////////////////
boolean nodeIdAvailable(File file,byte ID){
	file=SD.open("SENSBDD.TXT");
	if(!file){
#ifdef DEBUG
		Serial.println("Could not open SENSBDD.TXT for checking redundancy. Please check SD and retry");
#endif
		return false;
	}
	unsigned int count=NODE_ID_ADDR;
	myFile.seek(0);
	for(unsigned int count=0;count<file.size();count+=DATA_SIZE){ //check every nodeID in file
#ifdef DEBUG
		Serial.println("reading byte n°");
		Serial.print(count);
		Serial.println("");
#endif
		if(!file.available()){
			break;
		}
		file.seek(count);
		if((char)file.read()==(char)nodeID){
			return false;
		}
	}
	file.close();
	return true;
}

#ifdef TRANSMISSION
/////////////////////////////////////////////////////////////////
/*********** send informations to node though I2C **************/
/////////////////////////////////////////////////////////////////
void sendCryptData(){
	Serial.println("Sending IV & AES key through I2C");
	Wire.beginTransmission(8);
	memcpy(buff,(byte*)&IV,8);
	memcpy(buff+8,AES_256_key,32);
#ifdef DEBUG
	Serial.println("Sending IV & AES key through I2C");
	Serial.print("Sent buffer : ");
	Serial.write(buff,40);
#endif
	Wire.write(buff,40);
	Wire.endTransmission();
	Serial.println("\n[OK]");
}
#endif

//////////////////////////////////////////////////////////////////
/************** build data to be written on SD card *************/
//////////////////////////////////////////////////////////////////
void buildData(byte* data){
	memcpy(data+NODE_ID_ADDR,(byte*)&nodeID,1); //little trick to write nodeID correctly
	memcpy(data+IV_ADDR,(byte*)IV,8);
	memcpy(data+KEY_ADDR,AES_256_key,32);
	memcpy(data+DESC_ADDR,descr,100);
#ifdef DEBUG
	Serial.print("Data written into SD card : ");
	Serial.write(data,141);
	Serial.println("");
#endif
}

/////////////////////////////////////////////////////////////////
/******************** write data to SD card ********************/
/////////////////////////////////////////////////////////////////
void writeSD(byte* data){
	myFile = SD.open("SENSBDD.TXT",FILE_WRITE);
	if (myFile) {
		myFile.seek(myFile.size());
		myFile.write(myData,141);
#ifdef DEBUG
		Serial.println("Data written to SD card : ");
		Serial.write(data,141);
	}
	else{
		Serial.println("error opening MySecureSensors file");
	}
	myFile.close();
}

#ifdef READ_CONFIG
//////////////////////////////////////////////////////////////////
/*********** read secure sensor configuration on SD card ********/
//////////////////////////////////////////////////////////////////

void readSecureConfig(){
	myFile=SD.open("SENSBDD.TXT");
	Serial.println("reading SENSBDD.TXT :");
	byte temp[32];
	byte temp2[8];
	byte temp3[100];
	Serial.println("Node ID|    IV    |               AES key             |           info");
	while(myFile.available()){
		Serial.print("    ");
		Serial.print(myFile.read());
		Serial.print("  | ");
		for(int i=0;i<8;i++){
			temp2[i]=myFile.read();
		}
		Serial.write(temp2,8);
		Serial.print(" | ");
		for(int i=0;i<32;i++){
			temp[i]=myFile.read();
		}
		Serial.write(temp,32);
		Serial.print(" | ");
		for(int i=0;i<100;i++){
			temp3[i]=myFile.read();
		}
		Serial.println((char*)temp3);
	}
	myFile.close();
}
#endif

#endif

#endif


#ifndef GATEWAY
/*********************************************************************
 * NODE SIDE : ASK OR GET NODE ID, ASK USER FOR SENSOR DESCRIPTION    *
 * SEND DATA TO GATEWAY, RECEIVE AES KEY AND IV AND STORE INTO EEPROM *
 *********************************************************************/

void initiateNode(){
	Serial.println("======== Node mode: engaged =========");
#ifdef TRANSMISSION
	Wire.begin(8);
	sendNodeId();
	sendDescription();
	receiveAes();
#endif
	Serial.println("=====================================");
	Serial.println("You can now upload your MySensors \ncode and enjoy your secure network ;)");
}

//////////////////////////////////////////////////////////////////////
/****** get node ID from user or EEPROM and send it to gateway ******/
//////////////////////////////////////////////////////////////////////
void sendNodeId(){
	Serial.println("You may enter node ID manually (between 000 and 255, e.g. 001,212,054...");
#ifdef DEBUG
	Serial.println("Use \\n as line terminator");
#endif
	Serial.setTimeout(20000);
	byte nodeID_byte[3];
	if((Serial.readBytesUntil('\n',nodeID_byte,3))<1){
		EEPROM.get(2,nodeID);
	}
	else{
		nodeID=((nodeID_byte[0]-48)*100+(nodeID_byte[1]-48)*10+nodeID_byte[2]-48);
	}
	if(nodeID==0){
		nodeID=255;
	}
#ifdef DEBUG
	Serial.print("Sending (parent) node ID to gateway : ");
	Serial.println(nodeID);
#endif
	Wire.onRequest(ArduinoSendNodeId);
#ifdef DEBUG
	Serial.println("[OK]");
#endif
}
///////////////////////////////////////////////////////////////////////
/********* get description from user and send it to gateway **********/
///////////////////////////////////////////////////////////////////////
void sendDescription(){
	Serial.setTimeout(30000);
	Serial.println("Enter short device description");
	Serial.readBytesUntil('\n',descr,100);
	Serial.print("Sending description to gateway...\n");
	Wire.onRequest(ArduinoSendDescription);
	Serial.println("[OK]");
}

///////////////////////////////////////////////////////////////////////
/******* receive AES key & IV from gateway, store into EEPROM ********/
///////////////////////////////////////////////////////////////////////
void receiveAes(){
	Wire.onReceive(ArduinoReceiveAes);
	for(int i=0;i<8;i++){
		buff_8[i]=buff[i];
	}
	for(int i=8;i<40;i++){
		AES_256_key[i-8]=buff[i];
	}
	EEPROM.put(396,AES_256_key);  //396 = EEPROM_RF_ENCRYPTION_AES_KEY_ADDRESS
	EEPROM.put(429,buff_8);       //429 = EEPROM_RF_ENCRYPTION_AES_IV_ADDRESS
	Serial.print("Received and stored (not human-readable) AES 256 bit key :\n\"");
	Serial.write(AES_256_key,32);
	Serial.println("\nReceived and stored (not human-readable) AES initialization vector :\n\"");
	Serial.write(buff_8,8);
	Serial.println("\n[OK]\n");
}

///////////////////////////////////////////////////////////////////////
/************** callback function for SendDescription, ***************/
/***************** sendNodeID & receiveAes functions *****************/
///////////////////////////////////////////////////////////////////////
void ArduinoSendDescription(){
	Wire.write(descr,100);
}

void ArduinoSendNodeId(){
	Wire.write(nodeID);
}

void ArduinoReceiveAes(int howMany){
	byte i=0;
	/* store received byte array in buff (reduce, recycle, reuse ;) */
	while(Wire.available()){
		buff[i]=Wire.read(); //buff = global
		i++;
	}
}

void SdReadNodeData(byte nodeId, nodeData nd){
	SD.begin();
	file=SD.open("SENSBDD.TXT");
	if(!file){
#ifdef DEBUG
		Serial.println("Could not open SENSBDD.TXT. Please check SD and retry");
#endif
		return false;
	}
	unsigned int count=NODE_ID_ADDR;
	myFile.seek(0);
	for(unsigned int count=0;count<file.size();count+=DATA_SIZE){ //check every nodeID in file
#ifdef DEBUG
		Serial.println("reading byte n°");
		Serial.print(count);
		Serial.println("");
#endif
		if(!file.available()){
			break;
		}
		file.seek(count);
		if((char)file.read()==(char)nodeID){
			nd.nodeID=nodeID;
			for(int i=0;i<8;i++){
				buff_8[i]=file.read();
			}
			memcpy((byte*)buff_8,nd.IV,8);
			for (int i=0;i<32;i++){
				AES_256_key[i]=file.read();
			}
			memcpy((byte*)AES_256_key,nd.AES_256,32);
			for(int i=0;i<100;i++){
				descr[i]=file.read();
			}
			memcpy((byte*)descr,nd.description,100);
			#ifdef DEBUG
				Serial.println("Succesfully loaded node config");
			#endif
		}
		else{
			Serial.println("Node not found, check configuration!");
		}
	}
	file.close();
}

}
#endif
