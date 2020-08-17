/*
randombytes/devurandom.h version 20080713
D. J. Bernstein
Public domain.
*/

#ifndef RANDOMBYTES_H
#define RANDOMBYTES_H

#include <sodium.h>
#include <iostream>

namespace nfl {

void randombytes(unsigned char *r, unsigned long long rlen) {
    static int rc = sodium_init();
    
    if (rc < 0) {
        std::cerr << "LibSodium initialization failed" << std::endl;
        throw std::runtime_error("LibSodium can not be initialized");
    }

    randombytes_buf(reinterpret_cast<void*>(r), rlen);
}

}

#endif
