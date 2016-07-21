#include "AES.h"
#include "printf.h"
#include <Arduino.h>

AES aes;

byte key [32];
byte plain[] = "Add NodeAdd NodeAdd NodeAdd NodeAdd Node";
//real iv = iv x2 ex: 01234567 = 0123456701234567
unsigned long long int my_iv;

void setup ()
{
  Serial.begin (57600) ;
  printf_begin();
  randomSeed(analogRead(0));
  my_iv=aes.generate_IV();
  aes.generate_key(256,key);
  Serial.print("generated key : ");
  for(int i=0;i<32;i++){
    Serial.print((byte)key[i]);
  }
  Serial.println();
  delay(500);
  printf("\n===testing mode\n") ;
}

void loop () 
{
  prekey_test () ;
  delay(2000);
}

void prekey (int bits)
{
  aes.iv_inc();
  byte iv [N_BLOCK] ;
  byte plain_p[48];
  byte cipher [48] ;
  byte check [48] ;
  unsigned long ms = micros ();
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_encrypt(plain,sizeof(plain),cipher,key,bits,iv);
  Serial.println(micros() - ms);
  ms = micros ();
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_decrypt(cipher,48,check,key,bits,iv);
  Serial.print("Decryption took: ");
  Serial.println(micros() - ms);
  Serial.print("Key used : ");
  for(int i=0;i<32;i++){
    Serial.print((byte)key[i]);
  }
  printf("\n\nPLAIN :");  
  aes.printArray(plain,(bool)true);
  printf("\nCIPHER:");
  aes.printArray(cipher,(bool)false);
  printf("\nCHECK :");
  aes.printArray(check,(bool)true);
  printf("\nIV    :");
  aes.printArray(iv,16);
  printf("\n============================================================\n");
}

void prekey_test ()
{
  prekey (256) ;
}
