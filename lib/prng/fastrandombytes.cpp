/*
 * File:   lattisigns512-20130329/fastrandombytes.c
 * Author: Gim Güneysu, Tobias Oder, Thomas Pöppelmann, Peter Schwabe
 * Public Domain
 */

#include <inttypes.h>
#include <iostream>
#include <sodium.h>
// #include "nfl/prng/crypto_stream_salsa20.h"
// #include "nfl/prng/randombytes.h"

namespace nfl {

// static size_t constexpr crypto_stream_salsa20_KEYBYTES = 32;
// static size_t constexpr crypto_stream_salsa20_NONCEBYTES = 8;

// static int init = 0;
// static unsigned char key[crypto_stream_salsa20_KEYBYTES];
// static unsigned char nonce[crypto_stream_salsa20_NONCEBYTES] = {0};

// void fastrandombytes(unsigned char *r, unsigned long long rlen) {
//   unsigned long long n = 0;
//   int i;
//   if (!init) {
//     randombytes(key, crypto_stream_salsa20_KEYBYTES);
//     init = 1;
//   }
//   nfl_crypto_stream_salsa20_amd64_xmm6(r, rlen, nonce, key);

//   // Increase 64-bit counter (nonce)
//   for (i = 0; i < crypto_stream_salsa20_NONCEBYTES; i++) n ^= ((unsigned long long)nonce[i]) << 8 * i;
//   n++;
//   for (i = 0; i < crypto_stream_salsa20_NONCEBYTES; i++) nonce[i] = (n >> 8 * i) & 0xff;
// }

void fastrandombytes(unsigned char *r, unsigned long long rlen) {
    static int rc = sodium_init();
    // static unsigned char key[crypto_stream_KEYBYTES];
    // static unsigned char nonce[crypto_stream_NONCEBYTES];
    
    if (rc < 0) {
        std::cerr << "LibSodium initialization failed" << std::endl;
        throw std::runtime_error("LibSodium can not be initialized");
    }

    randombytes_buf(reinterpret_cast<void*>(r), rlen);

    // if (!init) {
    //     randombytes_buf(reinterpret_cast<void*>(key), crypto_stream_KEYBYTES);
    //     init = 1;
    // }
    // crypto_stream_salsa20(r, rlen, nonce, key);

    // // Increase 64-bit counter (nonce)
    // size_t n = 0;
    // for (size_t i = 0; i < crypto_stream_salsa20_NONCEBYTES; i++) {
    //     n ^= ((unsigned long long)nonce[i]) << 8 * i;
    // }
    
    // n++;
    
    // for (size_t i = 0; i < crypto_stream_salsa20_NONCEBYTES; i++) {
    //     nonce[i] = (n >> 8 * i) & 0xff;
    // }
}

}  // namespace nfl
