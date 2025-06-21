#include <iostream>
#include "tcp_cert.h"
using namespace std;


int main(){
    srand(time(0));
    string name,ip;
    int server_port;
    cout<<"pls enter user name:";
    cin>>name;
    cout<<"pls enter server ip:";
    cin>>ip;
    cout<<"pls enter server port:";
    cin>>server_port;
    TCP_client client_itself;
    client_itself.username=name;
    client_itself.connect_server(ip,server_port);
    handle_server(&client_itself);
    client_itself.disconnect();
}