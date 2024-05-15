#include "HTTPServer.h"

const int PORT = 8080;
const std::string IP_ADDRESS = "127.0.0.1"; 

int main()
{
    HTTPServer server(IP_ADDRESS, PORT);
    server.start();
    return 0;
}
