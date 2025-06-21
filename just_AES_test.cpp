#include "AES.h"
#include "base64.h"
#include <cassert>
#define AES_key "notakey"
using namespace std;
int main(){
    string plain = "hello world!";
    string cipher = AES_Encrypt(plain, AES_key);
    string encoded = base64_encode(cipher);

    string decoded = base64_decode(encoded);
    string result = AES_Decrypt(decoded, AES_key);
    cout<<result<<"=?"<<plain;
    assert(result == plain); 

}