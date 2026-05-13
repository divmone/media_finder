//
// Created by divmone on 5/13/2026.
//

#pragma once

#include <httplib.h>

class HttpServer {
public:
    HttpServer(int port);

    void start();
    void stop();

    void setGetHandler(const std::string &pattern, httplib::Server::Handler handler);
private:
    httplib::Server server_;
    int port_;
};
