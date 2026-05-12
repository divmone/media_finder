//
// Created by divmone on 5/13/2026.
//

#include <iostream>
#include <filesystem>
#include <set>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

int main(int argc, const char* argv[]) {
    json j;
    j["audio"] = json::array();
    j["video"] = json::array();
    j["images"] = json::array();

    auto homeDir = fs::path(std::getenv("HOME"));

    std::set<std::string> audioExts = {".json", ".wav"};
    std::set<std::string> imageExts = {".png", ".bmp"};
    std::set<std::string> videoExts = {".mp4", ".avi"};

    for (const auto& dirEntry: fs::recursive_directory_iterator(homeDir)) {
        if (!dirEntry.is_regular_file()) {
            continue;
        }

        auto ext = dirEntry.path().extension();

        if (audioExts.count(ext))
        {
            j["audio"].push_back(dirEntry.path());
        }
        else if (videoExts.count(ext)) {
            j["video"].push_back(dirEntry.path());
        }
        else if (imageExts.count(ext)) {
            j["images"].push_back(dirEntry.path());
        }
    }

    std::cout << j.dump(4);

    return 0;
}