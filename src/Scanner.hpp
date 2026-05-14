//
// Created by divmone on 5/13/2026.
//

#pragma once

#include <nlohmann/json.hpp>
#include <filesystem>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <thread>

using json = nlohmann::json;
namespace fs = std::filesystem;


class Scanner {
public:
    Scanner(const fs::path &path, int interval);
    ~Scanner();

    void start();
    void stop();
    std::shared_ptr<const json> getCurrent() const;
private:
    void run();

    json scan();

    fs::path dir_;
    int interval_;
    std::atomic<bool> isRunning_{false};
    std::thread thread_;
    std::mutex stopM_;
    std::condition_variable stopCv_;

    std::atomic<std::shared_ptr<const json>> currentScan_;

    std::unordered_set<std::string> audioExts_ = {
        ".mp3", ".wav", ".flac", ".aac",
        ".m4a",".opus", ".wma", ".aiff",
        ".ape",".dsd",".amr", ".webm"
    };
    std::unordered_set<std::string> imageExts_ = {
        ".png", ".bmp",".jpg", ".jpeg",
        ".gif",".tiff", ".tif", ".webp",
        ".heic",".heif",".avif", ".svg",
    };
    std::unordered_set<std::string> videoExts_ = {
        ".mp4", ".avi",".mkv",".mov",
        ".wmv", ".flv",".webm",".m4v",
        ".3gp",".ts",".mts", ".m2ts",
        ".ogv",".vp8", ".vp9", ".av1"
    };
};
