/*
 * File:   lattisigns512-20130329/fastrandombytes.h
 * Author: Gim Güneysu, Tobias Oder, Thomas Pöppelmann, Peter Schwabe
 * Public Domain
 */

#ifndef FASTRANDOMBYTES_H
#define FASTRANDOMBYTES_H

#include <sodium.h>
#include <iostream>

namespace nfl {

void fastrandombytes(unsigned char *r, unsigned long long rlen) {
    static int rc = sodium_init();
    
    if (rc < 0) {
        std::cerr << "LibSodium initialization failed" << std::endl;
        throw std::runtime_error("LibSodium can not be initialized");
    }

    randombytes_buf(reinterpret_cast<void*>(r), rlen);  
}

}

#endif
