#ifndef Rsa_h
#define Rsa_h

#include <Arduino.h>

struct RSAKey{
	unsigned long e;
	unsigned long n;
	unsigned long d;
};

class Rsa {
	public:
	Rsa();
	unsigned int generatePrime();
	boolean isPrime(unsigned int a);
	void generateKey();
	unsigned long gcd(unsigned long a, unsigned long b);
	unsigned long getE();
	void setE(unsigned long e);
	unsigned long getN();
	void setN(unsigned long n);
	unsigned long getD();
	void setD(unsigned long d);
	private:
	RSAKey _rsaKey;
};
#endif
