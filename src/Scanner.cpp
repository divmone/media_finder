//
// Created by divmone on 5/13/2026.
//

#include "Scanner.hpp"

#include <bits/this_thread_sleep.h>

Scanner::Scanner(const fs::path &path, int interval)
    : dir_(path), interval_(interval){

}

Scanner::~Scanner() {
    stop();
}

json Scanner::getCurrent() {
    std::lock_guard lock(m_);
    return currentScan_;
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

        std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
        if (audioExts_.count(ext))
        {
            j["audio"].push_back(dirEntry.path());
        }
        else if (videoExts_.count(ext)) {
            j["video"].push_back(dirEntry.path());
        }
        else if (imageExts_.count(ext)) {
            j["images"].push_back(dirEntry.path());
        }
    }
    return j;
}

void Scanner::start() {
    currentScan_ = scan();
    isRunning_ = true;
    thread_ = std::thread(&Scanner::run, this);
}

void Scanner::stop() {
    isRunning_ = false;
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Scanner::run() {
    while (isRunning_) {
        std::this_thread::sleep_for(std::chrono::seconds(interval_));
        json newScan = scan();
        {
            std::lock_guard lock(m_);
            currentScan_ = newScan;
        }
    }
}
