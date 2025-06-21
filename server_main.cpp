#include <iostream>
#include "tcp_cert.h"
using namespace std;


int main(){
    srand(time(0));
    cout<<"pls input port:";
    int port;
    cin>>port;
    vector<thread> threadPool;
    TCP_server server_itself;
    server_itself.init(port);
    while(1){
        SOCKET client=server_itself.acceptClient();
        threadPool.emplace_back(handle_client,client,&server_itself);
        
    }
    for(auto &th:threadPool){
        if(th.joinable())th.join();
    }
    server_itself.stop();
}