#include "tcp_cert.h"
#include "base64.h"
#include <atomic>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")

TCP_client::TCP_client() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

TCP_client::~TCP_client() {
    closesocket(clientSocket);
    WSACleanup();
}

bool TCP_client::connect_server(const std::string& ip, int port) {
    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    return connect(clientSocket, (SOCKADDR*)&server, sizeof(server)) != SOCKET_ERROR;
}

void TCP_client::sendMessage(const std::string& msg) {
    send(clientSocket, msg.c_str(), msg.size(), 0);
}

std::string TCP_client::receiveMessage() {
    char buffer[1024];
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytes <= 0) return "";
    return std::string(buffer, bytes);
}

void TCP_client::disconnect() {
    closesocket(clientSocket);
}
void handle_server(TCP_client*s){
    s->sendMessage(s->username);
    string mes=s->receiveMessage();
    while(mes!="ACCEPT"){
        mes=s->receiveMessage();
    }
    RSA_key mykey=RSA_gen();
    //cout<<"[DEBUG]"<<mykey.p_key<<","<<mykey.N_key<<endl;
    s->sendMessage(std::to_string(mykey.p_key)+','+std::to_string(mykey.N_key));
    mes=s->receiveMessage();
    while(mes=="")mes=s->receiveMessage();
    //cout<<"[DEBUG]"<<mes<<endl;
    ulong raw_key=symmetry_key_decrypt(strtoull(mes.c_str(),NULL,10),mykey);
    //cout<<"[DEBUG]"<<raw_key<<endl;
    string AES_key=to_string(raw_key);
    std::atomic<bool> running(true);
    std::thread receiver([&](){
        while(running){
            string cipher=s->receiveMessage();
            if(cipher.empty())break;
            //cout<<"[DEBUG]:"<<cipher<<endl;
            cipher=base64_decode(cipher);
            string plain=AES_Decrypt(cipher,AES_key);
            
            std::cout<<"\n[From Server]"<<plain<<endl;
        }
        running=false;
    });
    std::string input;
    while (running) {
        std::getline(std::cin, input);
        std::string cipher;
        if (input == "/quit") {
            running = false;
            break;
        }
             
        cipher = AES_Encrypt(input,AES_key);
        

        cipher=base64_encode(cipher);
        //cout<<"[DEBUG]:"<<cipher<<endl;
        s->sendMessage(cipher);
    }

    if (receiver.joinable()) receiver.join();
    s->disconnect();


}