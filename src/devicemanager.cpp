#include "../include/devicemanager.h"
#include <cstdio>
#include <sstream>
#include <iostream>
#include <cstdlib>

std::string trim(const std::string& s) {
    auto start = s.find_first_not_of(" \t\n");
    auto end = s.find_last_not_of(" \t\n");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}

std::vector<InputDevice> listXInputDevices() {
    std::vector<InputDevice> devices;
    FILE* fp = popen("xinput list --short", "r");
    if (!fp) return devices;
    char line[512];
    while (fgets(line, sizeof(line), fp)) {
        std::string str(line);
        if (str.find("id=") != std::string::npos) {
            InputDevice dev;
            auto id_pos = str.find("id=");
            auto name = str.substr(str.find("↳") + 3, id_pos - str.find("↳") - 4);
            dev.name = trim(name);
            dev.id = str.substr(id_pos + 3, str.find("[", id_pos) - id_pos - 3);
            // get props
            std::string cmd = "xinput list-props " + dev.id;
            FILE* p = popen(cmd.c_str(), "r");
            if (p) {
                char prop_line[512];
                while (fgets(prop_line, sizeof(prop_line), p)) {
                    std::string prop(prop_line);
                    if (prop.find("(") != std::string::npos) {
                        auto key = trim(prop.substr(1, prop.find("(") - 2));
                        auto val = trim(prop.substr(prop.find(":") + 1));
                        dev.properties[key] = val;
                    }
                }
                pclose(p);
            }
            devices.push_back(dev);
        }
    }
    pclose(fp);
    return devices;
}

void applyXInputChanges(const InputDevice& device) {
    for (auto& [key, val] : device.pending_changes) {
        std::string cmd = "pkexec xinput set-prop " + device.id + " " + key + " " + val;
        std::cout << "Applying: " << cmd << std::endl;
        system(cmd.c_str());
    }
}

std::string getIcon(const std::string& name) {
    std::string lower;
    for (auto c : name) lower += std::tolower(c);
    if (lower.find("mouse") != std::string::npos) return "M";
    if (lower.find("power") != std::string::npos) return "P";
    if (lower.find("video") != std::string::npos) return "V";
    if (lower.find("keyboard") != std::string::npos) return "K";
    if (lower.find("touchpad") != std::string::npos) return "T";
    return "🔧";
}
