# 7005-c-project

So this project is to mimic a client and server sending and receiving on different machines over UDP.  

The server can be executed with no parameters.  The default port is 7005.    
./rob_srv 

The client can be executed with the server ip address that it attempting to reach at a parameter.  During execution, it'll ask for either GET or SEND followed by the port number, and the file that the user it attempting to either send or get from  the server. 
./rob_clnt 192.168.1.70 //if the server was on 192.168.1.70

The client will then ask the user to enter GET or SEND, with the port number, and the file.  ex. SEND 7002 beemovie.txt
