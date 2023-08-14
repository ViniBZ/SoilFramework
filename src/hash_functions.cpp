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
//-------------------------------------------------------------- T1 LINE
unsigned int t1_line(unsigned int h, unsigned int e, unsigned int f, unsigned int g, unsigned int k, unsigned int MSG)
{
    unsigned int T1 = h + large_sigma_1(e) + ch_function(e, f, g) + k + MSG;
    return T1;
}
//-------------------------------------------------------------- SMALL SIGMA 1
const unsigned int reverse_small_sigma_1(const unsigned int &x)
{
    char H[32];
    char K[32];

    unsigned int bit = 0b10000000000000000000000000000000;
    int i = 0;
    while(i < 32)
    {
        if(x&bit)
        {
            H[i] = 1;
        }else{
            H[i] = 0;
        }
        bit= bit >> 1;
        i++;
    }

    //Its possible to revert small_sigma_1 by
    //guessing the value of two bits
    //this loop represents the 4 possibilities of guesses


    int p = 0;
    while(p < 4)
    {
        //guess the fifth and sixteenth bits
        K[4] =  (char)p/2;
        K[15] = (char)p%2;

        //these could be loops

        K[13] = K[15] ^ H[0];
        K[17] = K[15] ^ H[2];
        K[19] = K[17] ^ H[4];
        K[21] = K[19] ^ H[6];
        K[23] = K[21] ^ H[8];

        K[6] = K[4] ^ K[13] ^ H[23];
        K[8] = K[6] ^ K[15] ^ H[25];
        K[10] = K[8] ^ K[17] ^ H[27];
        K[12] = K[10] ^ K[19] ^ H[29];
        K[14] = K[12] ^ K[21] ^ H[31];

        K[16] = K[14] ^ H[1];
        K[18] = K[16] ^ H[3];
        K[20] = K[18] ^ H[5];
        K[22] = K[20] ^ H[7];
        K[24] = K[22] ^ H[9];

        K[11] = K[13] ^ K[20] ^ H[30];
        K[9] = K[11] ^ K[18] ^ H[28];
        K[7] = K[9] ^ K[16] ^ H[26];
        K[5] = K[7] ^ K[14] ^ H[24];

        K[3] = K[5] ^ K[12] ^ H[22];
        K[2] = K[4] ^ K[11] ^ H[21];
        K[1] = K[3] ^ K[10] ^ H[20];
        K[0] = K[2] ^ K[9] ^ H[19];
        K[31] = K[1] ^ K[8] ^ H[18];
        K[30] = K[0] ^ K[7] ^ H[17];
        K[29] = K[31] ^ K[6] ^ H[16];
        K[28] = K[30] ^ K[5] ^ H[15];
        K[27] = K[29] ^ K[4] ^ H[14];
        K[26] = K[28] ^ K[3] ^ H[13];
        K[25] = K[27] ^ K[2] ^ H[12];

        unsigned int res = 0;
        bit = 0b10000000000000000000000000000000;
        i=0;
        while(i < 32)
        {
            if(K[i])
            {
                res = res|bit;
            }
            bit= bit >> 1;
            i++;
        }

        unsigned int test = small_sigma_1(res);
        if(test == x)
        {
            return res;
        }

        p++;
    }
    return 0;
    
}
//-------------------------------------------------------------- REVERSE LARGE SIGMA 0
const unsigned int reverse_large_sigma_0(const unsigned int &x)
{
    unsigned char H[32];
    unsigned char K[32];

    unsigned int bit = 0b10000000000000000000000000000000;
    int i = 0;
    while(i < 32)
    {
        if(x&bit)
        {
            H[i] = 1;
        }else{
            H[i] = 0;
        }
        bit= bit >> 1;
        i++;
    }

    //Its possible to revert large_sigma_0 by
    //guessing the value of five bits
    //this loop represents the 32 possibilities of guesses


    unsigned int p = 0;
    while(p < 32)
    {
        //GUESS 6 BITS
        K[0] = (p&16) >> 4;
        K[1] = (p&8) >> 3;
        K[2] = (p&4) >> 2;
        K[21] = (p&2) >> 1;
        K[22] = p&1;

        //printf("\nTRY:%d%d%d%d%d%d%d",K[0],K[1],K[2],K[3],K[4],K[5],K[6]);

        K[10] = K[21] ^ K[1] ^ H[23];
        K[11] = K[22] ^ K[2] ^ H[24];
        K[12] = K[0] ^ K[21] ^ H[2];
        K[13] = K[1] ^ K[22] ^ H[3];

        K[23] = K[11] ^ K[0] ^ H[13];
        K[24] = K[12] ^ K[1] ^ H[14];
        K[25] = K[13] ^ K[2] ^ H[15];

        K[3] = K[23] ^ K[12] ^ H[25];
        K[4] = K[24] ^ K[13] ^ H[26];

        K[14] = K[2] ^ K[23] ^ H[4];
        K[15] = K[3] ^ K[24] ^ H[5];
        K[16] = K[4] ^ K[25] ^ H[6];

        K[26] = K[14] ^ K[3] ^ H[16];
        K[27] = K[15] ^ K[4] ^ H[17];

        K[5] = K[25] ^ K[14] ^ H[27];
        K[6] = K[26] ^ K[15] ^ H[28];
        K[7] = K[27] ^ K[16] ^ H[29];

        K[17] = K[5] ^ K[26] ^ H[7];
        K[18] = K[6] ^ K[27] ^ H[8];

        K[28] = K[16] ^ K[5] ^ H[18];
        K[29] = K[17] ^ K[6] ^ H[19];
        K[30] = K[18] ^ K[7] ^ H[20];

        K[8] = K[28] ^ K[17] ^ H[30];
        K[9] = K[29] ^ K[18] ^ H[31];

        K[19] = K[7] ^ K[28] ^ H[9];
        K[20] = K[8] ^ K[29] ^ H[10];

        K[31] = K[20] ^ K[11] ^ H[1];
        

        unsigned int res = 0;
        bit = 0b10000000000000000000000000000000;
        i=0;
        while(i < 32)
        {
            if(K[i])
            {
                res = res|bit;
            }
            bit= bit >> 1;
            i++;
        }

        unsigned int test = large_sigma_0(res);
        if(test == x)
        {
            return res;
        }

        p++;
    }
    

    return 0;
}
//-------------------------------------------------------------- REVERSE LARGE SIGMA 1
const unsigned int reverse_large_sigma_1(const unsigned int &x)
{
    unsigned char H[32];
    unsigned char K[32];

    unsigned int bit = 0b10000000000000000000000000000000;
    int i = 0;
    while(i < 32)
    {
        if(x&bit)
        {
            H[i] = 1;
        }else{
            H[i] = 0;
        }
        bit= bit >> 1;
        i++;
    }

    //Its possible to revert large_sigma_1 by
    //guessing the value of seven bits
    //this loop represents the 128 possibilities of guesses


    unsigned int p = 0;
    while(p < 128)
    {
        //GUESS 6 BITS
        K[0] = (p&64) >> 6;
        K[1] = (p&32) >> 5;
        K[2] = (p&16) >> 4;
        K[3] = (p&8) >> 3;
        K[4] = (p&4) >> 2;
        K[5] = (p&2) >> 1;
        K[6] = p&1;

        //printf("\nTRY:%d%d%d%d%d%d%d",K[0],K[1],K[2],K[3],K[4],K[5],K[6]);


        K[18] = K[5] ^ K[0] ^ H[11];
        K[19] = K[6] ^ K[1] ^ H[12];

        K[14] = K[19] ^ K[0] ^ H[25];

        K[23] = K[18] ^ K[4] ^ H[29];
        K[24] = K[19] ^ K[5] ^ H[30];

        K[10] = K[5] ^ K[23] ^ H[16];
        K[11] = K[6] ^ K[24] ^ H[17];

        K[28] = K[1] ^ K[14] ^ H[7];
        K[29] = K[24] ^ K[10] ^ H[3];

        K[15] = K[2] ^ K[29] ^ H[8];
        K[16] = K[11] ^ K[29] ^ H[22];

        K[20] = K[15] ^ K[1] ^ H[26];

        K[7] = K[2] ^ K[20] ^ H[13];

        K[21] = K[16] ^ K[2] ^ H[27];

        K[25] = K[20] ^ K[6] ^ H[31];
        K[26] = K[21] ^ K[7] ^ H[0];
        K[12] = K[7] ^ K[25] ^ H[18];

        K[8] = K[3] ^ K[21] ^ H[14];
        K[9] = K[28] ^ K[23] ^ H[2];

        K[13] = K[8] ^ K[26] ^ H[19];

        K[22] = K[9] ^ K[4] ^ H[15];

        K[17] = K[22] ^ K[3] ^ H[28];

        K[27] = K[0] ^ K[13] ^ H[6];
        K[30] = K[3] ^ K[16] ^ H[9];
        K[31] = K[4] ^ K[17] ^ H[10];


        unsigned int res = 0;
        bit = 0b10000000000000000000000000000000;
        i=0;
        while(i < 32)
        {
            if(K[i])
            {
                res = res|bit;
            }
            bit= bit >> 1;
            i++;
        }

        unsigned int test = large_sigma_1(res);
        if(test == x)
        {
            return res;
        }

        p++;
    }
    

    return 0;
}