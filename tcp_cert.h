#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cmath>
#include<string>
#include<vector>
#include<thread>
#include<map>
#include<mutex>
#include <random>
#include "AES.h"
#define ulong unsigned long long
#define default_start 1024
#define default_port 8989
ulong find_prime(ulong start=default_start);
ulong quickPow(ulong horse,ulong rider,ulong base);
ulong find_reverse(ulong a, ulong m);
struct RSA_key{
    ulong p_key;
    ulong s_key;
    ulong N_key;
};
RSA_key RSA_gen(ulong prime_start=default_start);
ulong symmetry_key_crypt(ulong symmetry_key,ulong p_key,ulong N_key);
ulong symmetry_key_decrypt(ulong symmetry_key,RSA_key key);
class TCP_server{
    public:
        TCP_server();
        ~TCP_server();
        bool init(int port=default_port);
        SOCKET acceptClient();
        std::string receiveMessage(SOCKET client);
        void sendMessage(SOCKET client, const std::string& message);
        void stop();
        SOCKET serverSocket;
        std::mutex user_mutex;
        std::map<std::string, SOCKET> online_users;         // <username, socket>
        std::map<SOCKET, std::string> socket_to_user;       // <socket, username>
        std::map<SOCKET, std::string> socket_aes_key;       // 每个连接的 AES 密钥

};
class TCP_client{
    public:
        TCP_client();
        ~TCP_client();
        bool connect_server(const std::string&ip,int port=default_port);
        void sendMessage(const std::string& msg);
        std::string receiveMessage();
        void disconnect();
        SOCKET clientSocket;
        std::string username;
};
void handle_client(SOCKET client_socket, TCP_server* server);//for server
void handle_server(TCP_client*s);//for client