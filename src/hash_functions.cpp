#include "hash_functions.h"


//-------------------------------------------------------------- ROTATE RIGHT
const unsigned int rotate_right(const unsigned int &n, const unsigned int &x)
{
    return (x >> n) | (x << (32 - n));
}
//-------------------------------------------------------------- SHIFT RIGHT
const unsigned int shift_right(const unsigned int &n, const unsigned int &x)
{
    return x >> n;
}
//-------------------------------------------------------------- CH
const unsigned int ch_function(const unsigned int &x, const unsigned int &y, const unsigned int &z)
{
    return (x & y) ^ (~x & z);
}
//-------------------------------------------------------------- MAJ
const unsigned int maj_function(const unsigned int &x, const unsigned int &y, const unsigned int &z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}
//-------------------------------------------------------------- LARGE SIGMA 0
const unsigned int large_sigma_0(const unsigned int &x)
{
    return rotate_right(2, x) ^ rotate_right(13, x) ^ rotate_right(22, x);
}
//-------------------------------------------------------------- LARGE SIGMA 1
const unsigned int large_sigma_1(const unsigned int &x)
{
    return rotate_right(6, x) ^ rotate_right(11, x) ^ rotate_right(25, x);
}
//-------------------------------------------------------------- SMALL SIGMA 0
const unsigned int small_sigma_0(const unsigned int &x)
{
    return rotate_right(7, x) ^ rotate_right(18, x) ^ shift_right(3, x);
}
//-------------------------------------------------------------- SMALL SIGMA 1
const unsigned int small_sigma_1(const unsigned int &x)
{
    return rotate_right(17, x) ^ rotate_right(19, x) ^ shift_right(10, x);
}