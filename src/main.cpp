//
// Created by divmone on 5/13/2026.
//

#include <iostream>
#include "HttpServer.hpp"
#include "Scanner.hpp"

std::atomic<bool> isShutdown{false};
std::condition_variable cv;
std::mutex m;

void signalHandler(int signal) {
    isShutdown = true;
    cv.notify_one();
}

int main(int argc, const char* argv[]) {
    auto dir = std::filesystem::path(std::getenv("HOME"));
    int interval = 10;

    if (argc > 1) {
        dir = argv[1];
    }
    if (argc > 2) {
        interval = std::stoi(argv[2]);
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    HttpServer server{1234};
    Scanner scanner{dir, interval};
    scanner.start();
    server.setGetHandler("/media_files", [&scanner](const auto &req, auto &res) {
        res.set_content(scanner.getCurrent().dump(4), "application/json");
    });

    auto serverThread = std::thread([&server]() {
        server.start();
    });

    std::unique_lock lock(m);
    cv.wait(lock, []() {
        return isShutdown.load();
    });

    std::cout << "Shutting down...\n";

    server.stop();
    serverThread.join();
    scanner.stop();

    std::cout << "Bye\n";

    return 0;
}
