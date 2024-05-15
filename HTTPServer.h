#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <netinet/in.h>
#include <string>

class HTTPServer
{
public:
    HTTPServer(const std::string &ip_address, int port);
    ~HTTPServer();

    void start();

private:
    int server_fd;
    int port;
    std::string ip_address;
    sockaddr_in address;

    void createSocket();
    void setSocketOptions();
    void bindSocket();
    void listenForConnections();
    void handleConnection(int new_socket);
    std::string getContentType(const std::string &file_path);
    std::string readFile(const std::string &file_path);
};

#endif
