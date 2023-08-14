#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <iostream>

//rotate right^n(x)
const unsigned int rotate_right(const unsigned int &n, const unsigned int &x);

//right shift right_shift^n(x)
const unsigned int shift_right(const unsigned int &n, const unsigned int &x);

//ch(x, y, z)
const unsigned int ch_function(const unsigned int &x, const unsigned int &y, const unsigned int &z);

//maj(x, y, z)
const unsigned int maj_function(const unsigned int &x, const unsigned int &y, const unsigned int &z);

//large sigma^256_0(x)
const unsigned int large_sigma_0(const unsigned int &x);

//large sigma^256_1(x)
const unsigned int large_sigma_1(const unsigned int &x);

//small sigma^256_0(x)
const unsigned int small_sigma_0(const unsigned int &x);

//small sigma^256_1(x)
const unsigned int small_sigma_1(const unsigned int &x);

unsigned int t1_line(unsigned int h, unsigned int e, unsigned int f, unsigned int g, unsigned int k, unsigned int MSG);

const unsigned int reverse_small_sigma_1(const unsigned int &x);
const unsigned int reverse_large_sigma_0(const unsigned int &x);
const unsigned int reverse_large_sigma_1(const unsigned int &x);


#endif //hash_functions
