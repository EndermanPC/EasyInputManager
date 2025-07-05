#pragma once
#include <string>
#include <map>
#include <vector>

struct InputDevice {
    std::string id;
    std::string name;
    std::map<std::string, std::string> properties;
    std::map<std::string, std::string> pending_changes;
};

std::vector<InputDevice> listXInputDevices();
std::string getIcon(const std::string& name);
void applyXInputChanges(const InputDevice& device);
