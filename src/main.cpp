//
// Created by divmone on 5/13/2026.
//

#include <cstring>
#include <iostream>
#include <string>
#include "HttpServer.hpp"
#include "Scanner.hpp"

std::atomic<bool> isShutdown{false};

void signalHandler(int signal) {
    isShutdown = true;
}

void printUsage(const char* prog) {
    std::cerr << "Usage: " << prog << " [directory] [interval_seconds]\n"
              << "  directory         path to scan (default: $HOME)\n"
              << "  interval_seconds  positive integer scan interval (default: 10)\n";
}

int main(int argc, const char* argv[]) {
    std::filesystem::path dir;
    int interval = 10;

    if (argc > 3) {
        std::cerr << "Error: too many arguments\n";
        printUsage(argv[0]);
        return 1;
    }

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        dir = arg;
    } else {
        const char* home = std::getenv("HOME");
        if (home == nullptr) {
            std::cerr << "Error: $HOME is not set, pass a directory explicitly\n";
            printUsage(argv[0]);
            return 1;
        }
        dir = home;
    }

    std::error_code ec;
    if (!std::filesystem::exists(dir, ec) || ec) {
        std::cerr << "Error: directory does not exist: " << dir << '\n';
        return 1;
    }
    if (!std::filesystem::is_directory(dir, ec) || ec) {
        std::cerr << "Error: path is not a directory: " << dir << '\n';
        return 1;
    }

    if (argc > 2) {
        try {
            size_t pos = 0;
            interval = std::stoi(argv[2], &pos);
            if (pos != std::strlen(argv[2])) {
                throw std::invalid_argument("trailing characters");
            }
        } catch (const std::exception&) {
            std::cerr << "Error: interval must be an integer, got: " << argv[2] << '\n';
            printUsage(argv[0]);
            return 1;
        }
        if (interval <= 0) {
            std::cerr << "Error: interval must be positive, got: " << interval << '\n';
            return 1;
        }
    }

    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    HttpServer server{1234};
    Scanner scanner{dir, interval};
    scanner.start();
    server.setGetHandler("/media_files", [&scanner](const auto &req, auto &res) {
        auto snapshot = scanner.getCurrent();
        res.set_content(snapshot->dump(), "application/json");
    });

    auto serverThread = std::thread([&server]() {
        server.start();
    });

    while (!isShutdown.load()){
        pause();
    }

    std::cout << "Shutting down...\n";

    server.stop();
    if(serverThread.joinable()){
        serverThread.join();
    }
    scanner.stop();

    std::cout << "Bye\n";

    return 0;
}
