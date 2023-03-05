#include <iostream>
#include <fstream>
#include <cstring>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<regex>

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <URL> <output file>" << endl;
        return 1;
    }

    // Phân tích URL thành tên miền, tên file và cổng
    string url = argv[1];
    string domain, file;
    string filename = argv[2];
    
    int port = 80;
    size_t pos = url.find("://");
    if (pos != string::npos) {
        url.erase(0, pos + 3);
    }
    pos = url.find("/");
    if (pos != string::npos) {
        domain = url.substr(0, pos);
        file = url.substr(pos);
    } else {
        domain = url;
        file = "/";
    }
    pos = domain.find(":");
    if (pos != string::npos) {
        port = stoi(domain.substr(pos + 1));
        domain = domain.substr(0, pos);
    }

    if(file=="/")
    {
        filename = domain+ "_index.txt";
    }
    // Tạo socket và kết nối đến máy chủ web
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent* server = gethostbyname(domain.c_str());
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Error: Failed to connect to server" << endl;
        return 1;
    }

    // Gửi yêu cầu HTTP đến máy chủ web
    string request = "GET " + file + " HTTP/1.1\r\n";
    request += "Host: " + domain + "\r\n";
    request += "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:97.0) Gecko/20100101 Firefox/97.0\r\n";
    request += "Connection: close\r\n";
    request += "\r\n";
    send(sockfd, request.c_str(), request.size(), 0);

    // Lưu kết quả vào file output.txt
    ofstream outfile(filename);
    char buffer[1024];
    int n = 0;
    while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        outfile.write(buffer, n);
    }

    // Đóng file và socket
    outfile.close();
    close(sockfd);

    return 0;
}
