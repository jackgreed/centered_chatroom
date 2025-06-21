#pragma once
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdint>
typedef uint8_t state_t[16];
void AES_EncryptBlock(const uint8_t* input, const uint8_t* key, uint8_t* output);
void AES_DecryptBlock(const uint8_t* input, const uint8_t* key, uint8_t* output);
std::string AES_Decrypt(std::string input, std::string key);
std::string AES_Encrypt(std::string input, std::string key);
