#include "Rsa.h"

Rsa::Rsa(){
	randomSeed(analogRead(0));	
}

unsigned int Rsa::generatePrime(){
	unsigned int a;
	a=random(2837,65533);	//prevent too small prime numbers
	while(isPrime(a)==false){
		a=random(2837,65533);
	}
	return a;
}

boolean Rsa::isPrime(unsigned int a){
	for(int i=3;i<a;i++){
		if(a%i==0){
			return false;
		}
	}
	return true;
}

void Rsa::generateKey(){
	unsigned long n,phi,e,d;
	unsigned int p,q;
	p=generatePrime();
	q=generatePrime();
	n=p*q;
	phi=(p-1)*(q-1);
	e=random(3,phi);	//2 and phi are not coprime, just skip it
	while(gcd(e,phi)!=1){
		if(e==phi){
			e=random(3,phi);
		}
		e++;
	}
	d=random(3,phi);
	while(((e*d)%phi)!=1){
		if(d==phi){
			d=random(3,phi);
		}
		d++;
	}
	setE(e);
	setN(n);
	setD(d);
}

unsigned long Rsa::gcd(unsigned long a, unsigned long b){
	unsigned long num, denom, rem;
	if(a>b){
		num=a;
		denom=b;
	}
	else{
		num=b;
		denom=a;
	}
	rem=num%denom;
	while(rem!=0){
		num=denom;
		denom=rem;
		rem=num%denom;
	}
	return denom;
}

unsigned long Rsa::getE(){
	return _rsaKey.e;
}

void Rsa::setE(unsigned long e){
	_rsaKey.e=e;
}

unsigned long Rsa::getN(){
	return _rsaKey.n;
}

void Rsa::setN(unsigned long n){
	_rsaKey.n=n;
}

unsigned long Rsa::getD(){
	return _rsaKey.d;
}

void Rsa::setD(unsigned long d){
	_rsaKey.d=d;
}
