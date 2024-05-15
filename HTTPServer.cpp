#include "HTTPServer.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fstream>
#include <sstream>

const int MAX_CONNECTIONS = 10;
const int BUFFER_SIZE = 1024;

HTTPServer::HTTPServer(const std::string &ip_address, int port) : ip_address(ip_address), port(port)
{
    createSocket();
    setSocketOptions();
    bindSocket();
}

HTTPServer::~HTTPServer()
{
    close(server_fd);
}

void HTTPServer::createSocket()
{
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void HTTPServer::setSocketOptions()
{
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        std::cerr << "Setsockopt failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void HTTPServer::bindSocket()
{
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(ip_address.c_str());
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void HTTPServer::start()
{
    listenForConnections();
}

void HTTPServer::listenForConnections()
{
    if (listen(server_fd, MAX_CONNECTIONS) < 0)
    {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on " << ip_address << ":" << port << std::endl;

    int new_socket;
    struct sockaddr_in client_address;
    socklen_t addrlen = sizeof(client_address);

    while (true)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_address, &addrlen)) < 0)
        {
            std::cerr << "Accept failed" << std::endl;
            exit(EXIT_FAILURE);
        }

        handleConnection(new_socket);
        close(new_socket);
    }
}

std::string HTTPServer::getContentType(const std::string &file_path)
{
    if (file_path.find(".html") != std::string::npos)
    {
        return "text/html";
    }
    else if (file_path.find(".css") != std::string::npos)
    {
        return "text/css";
    }
    else if (file_path.find(".js") != std::string::npos)
    {
        return "application/javascript";
    }
    else
    {
        return "text/plain";
    }
}
void HTTPServer::handleConnection(int new_socket)
{
    char buffer[BUFFER_SIZE] = {0};
    read(new_socket, buffer, BUFFER_SIZE);

    std::string request(buffer);

    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;

    if (method == "GET")
    {
        if (path == "/" || path == "/index.html")
        {
            std::string file_path = "index.html";
            std::string content_type = getContentType(file_path);
            std::string response = "HTTP/1.1 200 OK\nContent-Type: " + content_type + "\n\n" + readFile(file_path);
            send(new_socket, response.c_str(), response.length(), 0);
            std::cout << "Response sent" << std::endl;
        }
        else
        {
            std::string response = "HTTP/1.1 404 Not Found\nContent-Type: text/html\n\n<html><body><h1>404 Not Found</h1></body></html>";
            send(new_socket, response.c_str(), response.length(), 0);
            std::cout << "404 Response sent" << std::endl;
        }
    }
}


std::string HTTPServer::readFile(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (file)
    {
        std::ostringstream content;
        content << file.rdbuf();
        return content.str();
    }
    else
    {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return "";
    }
}
