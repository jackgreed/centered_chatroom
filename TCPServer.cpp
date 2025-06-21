#include "tcp_cert.h"
#include "base64.h"
#include <random>
using namespace std;
std::vector<std::string> split_string(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string temp;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!temp.empty()) tokens.push_back(temp);
            temp.clear();
        } else {
            temp += ch;
        }
    }
    if (!temp.empty()) tokens.push_back(temp);
    return tokens;
}

TCP_server::TCP_server(){
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);
    serverSocket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
}
TCP_server::~TCP_server(){
    closesocket(serverSocket);
    WSACleanup();
}
bool TCP_server::init(int port){
    sockaddr_in addr{};
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(port);
    bind(serverSocket,(SOCKADDR*)&addr,sizeof(addr));
    listen(serverSocket,SOMAXCONN);
    return 0;
}
SOCKET TCP_server::acceptClient(){
    return accept(serverSocket,nullptr,nullptr);
}
std::string TCP_server::receiveMessage(SOCKET client) {
    char buffer[1024];
    int bytes = recv(client, buffer, sizeof(buffer), 0);
    if (bytes <= 0) return "";
    return std::string(buffer, bytes);
}

void TCP_server::sendMessage(SOCKET client, const std::string& message) {
    send(client, message.c_str(), message.size(), 0);
}

void TCP_server::stop() {
    closesocket(serverSocket);
}
void handle_client(SOCKET client_socket, TCP_server* server){
    string username=server->receiveMessage(client_socket);
    if (username.empty()) {
        closesocket(client_socket);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(server->user_mutex);
        if (server->online_users.count(username)) {
            server->sendMessage(client_socket, "REJECT"); // 名字重复
            closesocket(client_socket);
            return;
        }

        server->online_users[username] = client_socket;
        server->socket_to_user[client_socket] = username;
        server->sendMessage(client_socket, "ACCEPT");
    }
    //名称-socket绑定
    //公钥获取
    std::string pk_str = server->receiveMessage(client_socket);
    if (pk_str.empty()) {closesocket(client_socket);return;}
    //cout<<"[DEBUG]"<<pk_str<<endl;
    ulong client_pk = strtoull(split_string(pk_str,',')[0].c_str(), nullptr, 10);
    ulong client_Nk = strtoull(split_string(pk_str,',')[1].c_str(), nullptr, 10);
    //cout<<"[DEBUG]"<<client_pk<<","<<client_Nk<<endl;
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<ulong> dis(1, std::numeric_limits<ulong>::max());
    ulong AES_key=dis(gen)%65536;
    //cout<<"[DEBUG]"<<AES_key<<endl;
    ulong encrypted_key = symmetry_key_crypt(AES_key,client_pk,client_Nk); // 使用客户端公钥加密
    //cout<<"[DEBUG]"<<encrypted_key<<endl;
    server->sendMessage(client_socket, std::to_string(encrypted_key));
    string aes_key=to_string(AES_key);
    server->socket_aes_key[client_socket] = aes_key;
    //分发对称钥完成
    //广播循环
    while (true) {
        std::string cipher = server->receiveMessage(client_socket);
        if (cipher.empty()) break;
        //cout<<"[DEBUG]:"<<cipher<<endl;
        cipher=base64_decode(cipher);
        std::string plain = AES_Decrypt(cipher, aes_key);
        
        std::string full_msg = "["+username + "]" + plain;
        std::cout<<full_msg<<endl;
        std::lock_guard<std::mutex> lock(server->user_mutex);
        for (auto& [other_name, other_socket] : server->online_users) {
            if (other_socket != client_socket) {
                std::string enc = AES_Encrypt(full_msg, server->socket_aes_key[other_socket]);
                enc=base64_encode(enc);
                //cout<<"[DEBUG]:"<<enc<<endl;
                server->sendMessage(other_socket, enc);
            }
        }
    }
    {
        std::lock_guard<std::mutex> lock(server->user_mutex);
        server->online_users.erase(username);
        server->socket_to_user.erase(client_socket);
        server->socket_aes_key.erase(client_socket);
    }

    closesocket(client_socket);

}