#include "tcp_cert.h"
ulong find_prime(ulong start){
    ulong temp = start;
    while (true) {
        bool is_prime = true;
        for (ulong i = 2; i <= sqrt(temp); i++) {
            if (temp % i == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) break;
        temp++;
    }
    return temp;
}
ulong quickPow(ulong horse,ulong rider,ulong base){
    horse%=base;
    ulong res=1;
    while(rider>0){
        if(rider%2){
            res=res*horse %base;
        }
        horse=horse*horse%base;
        rider/=2;
    }
    return res;
}
ulong find_reverse(ulong a, ulong m) {
    long x1 = 1, x2 = 0;
    long orig_m = m;
    while (m) {
        long q = a / m;
        long t = a % m;
        long x = x1 - q * x2;
        x1 = x2;
        x2 = x;
        a = m;
        m = t;
    }
    return (x1 % orig_m + orig_m) % orig_m;
}
ulong gcd(ulong a,ulong b){
    while(b!=0){
        ulong temp=b;
        b=a%b;
        a=temp;
    }
    return a;
}
RSA_key RSA_gen(ulong prime_start){
    ulong bigPrime_p=find_prime(prime_start);
    ulong bigPrime_q=find_prime(2*prime_start);
    ulong N_key=bigPrime_p*bigPrime_q;
    ulong r=(bigPrime_p-1)*(bigPrime_q-1);
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<ulong> dis(1, std::numeric_limits<ulong>::max());
    ulong p_key=dis(gen)%r;
    while(gcd(p_key, r)!=1){
        p_key=rand()%r;
    }
    ulong s_key=find_reverse(p_key,r);
    return {p_key,s_key,N_key};

} 
ulong symmetry_key_crypt(ulong symmetry_key,ulong p_key,ulong N_key){
    return quickPow(symmetry_key,p_key,N_key);
}
ulong symmetry_key_decrypt(ulong symmetry_key,RSA_key key){
    return quickPow(symmetry_key,key.s_key,key.N_key);
}