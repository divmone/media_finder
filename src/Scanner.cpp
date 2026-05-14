//
// Created by divmone on 5/13/2026.
//

#include "Scanner.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>

Scanner::Scanner(const fs::path &path, int interval)
    : dir_(path), interval_(interval){

}

Scanner::~Scanner() {
    stop();
}

std::shared_ptr<const json> Scanner::getCurrent() const {
    return currentScan_.load(std::memory_order_acquire);
}

json Scanner::scan() {
    json j;
    j["audio"] = json::array();
    j["video"] = json::array();
    j["images"] = json::array();

    for (const auto& dirEntry: fs::recursive_directory_iterator(dir_, fs::directory_options::skip_permission_denied)) {
        if (!dirEntry.is_regular_file()) {
            continue;
        }
        auto ext = dirEntry.path().extension().string();

        std::transform(ext.begin(), ext.end(), ext.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (audioExts_.count(ext)) {
            j["audio"].push_back(dirEntry.path());
        }
        else if (videoExts_.count(ext)) {
            j["video"].push_back(dirEntry.path());
        }
        else if (imageExts_.count(ext)) {
            j["images"].push_back(dirEntry.path());
        }
    }

    std::cout << "Scanning is completed: " << dir_ << '\n';
    return j;
}

void Scanner::start() {
    currentScan_.store(std::make_shared<const json>(scan()), std::memory_order_release);
    isRunning_ = true;
    thread_ = std::thread(&Scanner::run, this);
}

void Scanner::stop() {
    isRunning_ = false;
    stopCv_.notify_all();
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Scanner::run() {
    while (isRunning_) {
        std::unique_lock lock(stopM_);
        if (stopCv_.wait_for(lock, std::chrono::seconds(interval_),
                             [this]{ return !isRunning_.load(); })) {
            break;
        }
        auto newScan = std::make_shared<const json>(scan());
        currentScan_.store(std::move(newScan), std::memory_order_release);
    }
}
