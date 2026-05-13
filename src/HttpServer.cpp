//
// Created by divmone on 5/13/2026.
//

#include "HttpServer.hpp"

HttpServer::HttpServer(int port)
    :port_(port) {
}

void HttpServer::start() {
    std::cout << "Start listen...\n";
    server_.listen("127.0.0.1", port_);
}

void HttpServer::stop() {
    server_.stop();
}

void HttpServer::setGetHandler(const std::string &pattern, httplib::Server::Handler handler) {
    server_.Get(pattern, handler);
}
