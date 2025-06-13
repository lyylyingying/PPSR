#ifndef SHE_HPP
#define SHE_HPP

#include "config.h"

class SHE
{
private:
    ZZ p;
    ZZ q;
    ZZ L;
    ZZ N;
    int k0;
    int k1;
    int k2;

    ZZ r;
    ZZ r_prime;
    ZZ plaintext_range;

public:
    SHE(int k0, int k1, int k2);
    ~SHE();

    ZZ modN(ZZ a, ZZ b);
    ZZ moduloN(ZZ a);
    ZZ encrypt(ZZ m);
    ZZ decrypt(ZZ c);
    ZZ sum(ZZ ciphertext1, ZZ ciphertext2);
    ZZ multiply(ZZ ciphertext1, ZZ ciphertext2);
    ZZ addL(ZZ a);
    void printL();
    long GetBinaryBits(const ZZ& n);
    ZZ relinearize(ZZ ciphertext);
};

#endif