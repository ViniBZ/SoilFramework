#include "soilcrypt.h"

bool SoilCrypt::K_loaded = false;
Uint32 SoilCrypt::K[64];


SoilCrypt::SoilCrypt()
{
    if(!K_loaded){fill_K();}
}

// ------------------------------------------------------------------------------------ SHA 256 
SoilString SoilCrypt::sha256(const SoilString& orig_str)
{
    SoilString res_str;
    const char* c_str = orig_str.c_str_alloc();
    Uint64 orig_len = (Uint64)str_len(c_str);
    Uint32 pad = 64 - ((orig_len + 9) % 64);
    Uint32 data_len = orig_len + pad + 9;

    unsigned char * data = new unsigned char[data_len];

    int i = 0;
    int data_i = 0;
    while(data_i < orig_len)
    {
        data[data_i] = c_str[data_i];
        data_i++;
    }
    data[data_i] = 128;
    data_i++;
    //--------------------- padding
    i = 0;
    while(i < pad)
    {
        data[data_i] = 0;
        data_i++;
        i++;
    }
    // ---- the metadata for the msg size is recorded IN BITS!
    Uint64 orig_len_bits = orig_len*8;
    i=0;
    while(i < 8)
    {
        Uint64 size_tmp = orig_len_bits >> (56 - i*8);   
        Uint8 size_byte = size_tmp;
        data[data_i] = size_byte;//BITS!
        data_i++;
        i++;
    }
    //---------------------------------------- message schedule
    Uint32 block_i = 0;
    int block_n = data_len / 64;

    int MSG_SCH_N = data_len;
    Uint32* MSG_SCH = new Uint32[MSG_SCH_N];//W
    //---------------------------------------- hash constants
    Uint32 hash_block[8]={0x6a09e667,
            0xbb67ae85,
            0x3c6ef372,
            0xa54ff53a,
            0x510e527f,
            0x9b05688c,
            0x1f83d9ab,
            0x5be0cd19};

    Uint32 a = hash_block[0];
    Uint32 b = hash_block[1];
    Uint32 c = hash_block[2];
    Uint32 d = hash_block[3];
    Uint32 e = hash_block[4];
    Uint32 f = hash_block[5];
    Uint32 g = hash_block[6];
    Uint32 h = hash_block[7];    
    //-------------------------------------------------- chunk loop
    while(block_i < block_n)
    {
        int ind_base = block_i*64;
        int j;
        i=0;
        while(i < 16)
        {
            j = ind_base + i;
            int data_ind_base = ind_base+(i*4);

            MSG_SCH[j] = 0;
            MSG_SCH[j] += 0x1000000*data[data_ind_base];
            MSG_SCH[j] += 0x10000*data[data_ind_base+1];
            MSG_SCH[j] += 0x100*data[data_ind_base+2];
            MSG_SCH[j] += data[data_ind_base+3]; 
            i++;
        }
        while(i < 64)
        {
            MSG_SCH[ind_base + i] = 0x00000000;
            i++;
        }

        int s = 0;
        j = ind_base + 16;
        while(j < (ind_base + 64))
        {
            MSG_SCH[j] = small_sigma_1(MSG_SCH[j - 2]) + MSG_SCH[j - 7] + small_sigma_0(MSG_SCH[j - 15]) + MSG_SCH[j - 16];
            j++;
        }

        a = hash_block[0];
        b = hash_block[1];
        c = hash_block[2];
        d = hash_block[3];
        e = hash_block[4];
        f = hash_block[5];
        g = hash_block[6];
        h = hash_block[7];
        
        Uint32 T1, T2;

        j=ind_base;
        i = 0;
        while(j < (ind_base + 64))
        {
            T1 = h + large_sigma_1(e) + ch_function(e, f, g) + K[i] + MSG_SCH[j];
            T2 = large_sigma_0(a) + maj_function(a, b, c);
            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;
            j++;
            i++;
        }
        

        hash_block[0] = a + hash_block[0];
        hash_block[1] = b + hash_block[1];
        hash_block[2] = c + hash_block[2];
        hash_block[3] = d + hash_block[3];
        hash_block[4] = e + hash_block[4];
        hash_block[5] = f + hash_block[5];
        hash_block[6] = g + hash_block[6];
        hash_block[7] = h + hash_block[7];

        block_i++;
    }

    i = 0;
    while(i < 8)
    {
        
        res_str.append((signed long long)hash_block[i], 16, 8, 8);
        i++;
    }

    //--------------------------------------- zero data
    i = 0;
    while(i < data_len)
    {
        data[i] = 0;
        i++;
    }    

    delete[] c_str;
    delete[] data;
    delete[] MSG_SCH;
    
    return res_str;
}
// -------------------------------------------------------------- PRINT BINARY
void SoilCrypt::printf_binary(Uint32 num)
{
    Uint32 fac = 1;
    fac <<= 31;
    char num_str[33];
    num_str[32] = 0;
    int i = 0;
    while(i < 32)
    {
        if(num & fac)
        {
            num_str[i] = '1';
        }else{
            num_str[i] = '0';
        }
        fac >>= 1;
        i++;
    }
    printf("%s",num_str);
}
