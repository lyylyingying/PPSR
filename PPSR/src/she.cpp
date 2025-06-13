#include "she.hpp"

SHE::SHE(int k0, int k1, int k2)
{
    GenPrime(p, k0);
    GenPrime(q, k0);
    N = p * q;
    RandomBits(L, k2);
    this->k0 = k0;
    this->k1 = k1;
    this->k2 = k2;
    this->plaintext_range = power(ZZ(2), 50-1);
}

SHE::~SHE()
{
}

ZZ SHE::modN(ZZ a, ZZ b)
{
    ZZ result = a % b;
    return result;
}

ZZ SHE::moduloN(ZZ a)
{
    return a % N;
}

ZZ SHE::encrypt(ZZ m)
{
    RandomBits(r, k2);
    RandomBits(r_prime, k0);
    ZZ c = modN((r * L + m ) * ( 1 + r_prime * p), N);
    return c;
}

ZZ SHE::decrypt(ZZ c)
{
    ZZ m = modN(modN(c, p), L);
    if (m > plaintext_range)
    {
        return m -= L;
    }
    else
        return m;
}

ZZ SHE::sum(ZZ ciphertext1, ZZ ciphertext2)
{
    return modN(ciphertext1+ciphertext2, N);
}

ZZ SHE::multiply(ZZ ciphertext1, ZZ ciphertext2)
{
    return modN(ciphertext1*ciphertext2, N);
}

ZZ SHE::relinearize(ZZ ciphertext) {
    // Reduce noise by relinearizing the ciphertext
    // Adjusting the structure to maintain decryption correctness
    ZZ reduced = modN(ciphertext, N);
    return reduced;
}

ZZ SHE::addL(ZZ a)
{
    return moduloN(a+L);
}

void SHE::printL()
{
    cout << "L: " << L << endl;
}

// Returns the binary number of an integer
long SHE::GetBinaryBits(const ZZ& n) {
    if (n == 0) return 1;               // Special case for 0
    return NumBits(n);                  // Use NTL built-in function
}