# centered_chatroom
It is a sample of centered-chatroom.
## important information
This is just a sample.You shouldn't ever use it for work,or all project that need confidentiality.
## how to compile
It mainly have two exe.Client.exe and Server.exe.
just use:
g++ -o server AES.cpp server_main.cpp TCPServer.cpp crypto_utils.cpp -lws2_32
to compile a server.exe
and use:
g++ -o server AES.cpp CLient_main.cpp TCPClient.cpp crypto_utils.cpp -lws2_32
to compile a client.exe
