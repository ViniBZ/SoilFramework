#ifndef SOILCRYPT_H
#define SOILCRYPT_H

#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>
#include "hash_functions.h"
#include "soilstring.h"

#define BUTTON_IN_PAD 4
typedef unsigned char UCHAR;
typedef unsigned int UINT4;

class SoilCrypt : public SoilObject
{
public:

    static bool K_loaded;
    static Uint32 K[64];

    SoilCrypt();
    SoilString sha256(const SoilString& orig_str);
    void printf_binary(Uint32 num);

    static void fill_K()
    {
        if(!K_loaded)
        {
            K_loaded = true;
            #include "sha256_k_values.h"
        }
    }
};


#endif // SOILCRYPT_H