#include <iostream>
#include <string.h>
#include <cstdint>
#include <vector>
#include <map>

using namespace std;

static const char base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const map<char, uint8_t> dec_base64_map = {
  {'A', 0},
  {'B', 1},
  {'C', 2},
  {'D', 3},
  {'E', 4},
  {'F', 5},
  {'G', 6},
  {'H', 7},
  {'I', 8},
  {'J', 9},
  {'K', 10},
  {'L', 11},
  {'M', 12},
  {'N', 13},
  {'O', 14},
  {'P', 15},
  {'Q', 16},
  {'R', 17},
  {'S', 18},
  {'T', 19},
  {'U', 20},
  {'V', 21},    
  {'W', 22},
  {'X', 23},
  {'Y', 24},
  {'Z', 25},
  {'a', 26},
  {'b', 27},
  {'c', 28},
  {'d', 29},    
  {'e', 30},
  {'f', 31},    
  {'g', 32},
  {'h', 33},    
  {'i', 34},
  {'j', 35},    
  {'k', 36},
  {'l', 37},    
  {'m', 38},    
  {'n', 39},    
  {'o', 40},
  {'p', 41},    
  {'q', 42},
  {'r', 43},    
  {'s', 44},
  {'t', 45},    
  {'u', 46},
  {'v', 47},    
  {'w', 48},
  {'x', 49},    
  {'y', 50},
  {'z', 51},    
  {'0', 52},
  {'1', 53},    
  {'2', 54},
  {'3', 55},    
  {'4', 56},
  {'5', 57},    
  {'6', 58},
  {'7', 59},    
  {'8', 60},    
  {'9', 61},    
  {'+', 62},    
  {'/', 63}   
};

void to_b64(uint8_t b[3], char c[4])
{
    c[0] = base64[b[0] >> 2];
    c[1] = base64[((b[0] & 0b11) << 4) | (b[1] >> 4)];
    c[2] = base64[((b[1] & 0b1111) << 2) | (b[2] >> 6)];
    c[3] = base64[b[2] & 0b111111];
}

void from_b64(char c[4], uint8_t b[3])
{
    uint8_t c0, c1, c2, c3;
    c0 = dec_base64_map.at(c[0]);
    c1 = dec_base64_map.at(c[1]);
    c2 = dec_base64_map.at(c[2]);
    c3 = dec_base64_map.at(c[3]);

    b[0] = (c0 << 2) | (c1 >> 4);
    b[1] = ((c1 & 0b1111) << 4) | (c2 >> 2);
    b[2] = ((c2 & 0b11) << 6) | c3;
}

void encode(int argc, char**argv)
{
    if (argc < 1)
    {
        std::cerr << "Please specify a string to encode" << std::endl;
        return;
    }

    vector<char> tmp;

    char* str = argv[0];
    char b64_c[4];
    uint8_t b64_u8[3];
    size_t len = strlen(str);
    size_t i = 0;
    for (; i + 2 < len; i+=3)
    {
        b64_u8[0] = str[i];
        b64_u8[1] = str[i + 1];
        b64_u8[2] = str[i + 2];

        to_b64(b64_u8, b64_c);
        
        for (size_t j = 0; j < 4; j++)
        {
            tmp.push_back(b64_c[j]);
        }
    }

    if (i < len)
    {
        for (size_t j = 0; j < 3; j++)
        {
            if (i + j < len)
            {
                b64_u8[j] = str[i + j];
            }
            else
            {
                b64_u8[j] = '=';
            }
        }

        to_b64(b64_u8, b64_c);
        
        for (size_t j = 0; j < 4; j++)
        {
            tmp.push_back(b64_c[j]);
        }
    }

    for (char c : tmp)
    {
        std::cout << c;
    }
    std::cout << std::endl;

    for (char c: tmp)
    {
        std::cout << "0x" << std::hex << (c & 0xFF) << " ";
    }
    std::cout << std::endl;
}

bool check_valid_b64(char c)
{
    return (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c >= '0' && c <= '9') ||
           c == '+' || c == '/' || c == '=';
}

void decode(int argc, char**argv) {
    if (argc < 1)
    {
        std::cerr << "Please specify a string to decode" << std::endl;
        return;
    }

    vector<char> tmp;

    char* str = argv[0];

    // check str
    size_t len = strlen(str);
    if (len % 4 != 0)
    {
        std::cerr << "Invalid base64 string" << std::endl;
        return;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (!check_valid_b64(str[i]))
        {
            std::cerr << "Invalid base64 string" << std::endl;
            return;
        }
    }

    char b64_c[4];
    uint8_t b64_u8[3];
    for (size_t i = 0; i < len; i+=4)
    {
        for (size_t j = 0; j < 4; j++)
        {
            b64_c[j] = str[i + j];
        }

        from_b64(b64_c, b64_u8);

        for (size_t j = 0; j < 3; j++)
        {
            tmp.push_back(b64_u8[j]);
        }
    }

    for (char c : tmp)
    {
        std::cout << c;
    }
    std::cout << std::endl;
}

int main(int argc, char**argv)
{
    if (argc < 2)
    {
        std::cerr << "Please specify an operation" << std::endl;
        return -1;
    }

    if (strcmp(argv[1], "encode") == 0)
    {
        encode(argc - 2, argv + 2);
    }
    else if (strcmp(argv[1], "decode") == 0)
    {
        decode(argc - 2, argv + 2);
    }
    else
    {
        std::cerr << "Unknown operation: " << argv[1] << std::endl;
    }

    return 0;
}