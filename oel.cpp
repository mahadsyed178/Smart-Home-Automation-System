#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <sstream>
#include <memory>

// ANSI color codes for console output
#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
// Function to sanitize device names
std::string sanitizeDeviceName(const std::string& name) {
    std::string sanitized = name;
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '<'), sanitized.end());
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '>'), sanitized.end());
    return sanitized;
}

// class SmartHome {
//     // Your SmartHome implementation...
// };

class Logger
{
private:
    std::ofstream logFile;
    std::vector<std::string> commandHistory;
    const size_t MAX_HISTORY_SIZE = 1000;

    std::string getCurrentTimestamp()
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    Logger() : logFile("smart_home_log.txt", std::ios::app)
    {
        if (!logFile.is_open())
        {
            throw std::runtime_error("Unable to open log file");
        }
        logFile << "\n=== New Session Started ===\n";
    }

    void logCommand(const std::string &command)
    {
        std::string timestamp = getCurrentTimestamp();

        logFile << timestamp << " - Command: " << command << std::endl;

        commandHistory.push_back(timestamp + " - " + command);

        if (commandHistory.size() > MAX_HISTORY_SIZE)
        {
            commandHistory.erase(commandHistory.begin());
        }
    }

    void logStateChange(const std::string &deviceName, const std::string &state)
    {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        logFile << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S")
                << " - State Change: " << deviceName << " - " << state << std::endl;
    }

    void displayHistory(int limit = -1) const
    {
        std::cout << CYAN << "\nCommand History:" << RESET << std::endl;
        int count = 0;
        auto it = commandHistory.rbegin();

        while (it != commandHistory.rend() && (limit == -1 || count < limit))
        {
            std::cout << *it << std::endl;
            ++it;
            ++count;
        }
    }

    void saveToFile(const std::string &filename) const
    {
        std::ofstream historyFile(filename);
        if (!historyFile.is_open())
        {
            throw std::runtime_error("Unable to create history file");
        }

        historyFile << "=== Smart Home Command History ===\n\n";
        for (const auto &command : commandHistory)
        {
            historyFile << command << "\n";
        }

        std::cout << GREEN << "History saved to " << filename << RESET << std::endl;
    }
};

class Device
{
protected:
    std::string name;
    bool isOn;

public:
    Device(const std::string &deviceName) : name(deviceName), isOn(false) {}
    virtual ~Device() = default;

    virtual void turnOn() { isOn = true; }
    virtual void turnOff() { isOn = false; }
    virtual std::string getStatus() const
    {
        return name + " is " + (isOn ? "on" : "off");
    }
    virtual void adjustSetting(int value) {}

    const std::string &getName() const { return name; }
};

class Light : public Device
{
private:
    int brightness;

public:
    Light(const std::string &name) : Device(name), brightness(0) {}

    void turnOn() override
    {
        isOn = true;
        brightness = 100;
    }

    void turnOff() override
    {
        isOn = false;
        brightness = 0;
    }

    void adjustSetting(int value) override
    {
        if (!isOn)
        {
            std::cout << RED << "Cannot adjust brightness while the light is off." << RESET << std::endl;
            return;
        }
        if (value < 0 || value > 100)
        {
            throw std::out_of_range("Brightness must be between 0 and 100");
        }
        brightness = value;
    }

    std::string getStatus() const override
    {
        return Device::getStatus() + " (Brightness: " + std::to_string(brightness) + "%)";
    }
};

class Thermostat : public Device
{
private:
    int temperature;

public:
    Thermostat(const std::string &name) : Device(name), temperature(20) {}

    void adjustSetting(int value) override
    {
        if (value < 0 || value > 40)
        {
            throw std::out_of_range("Temperature must be between 0 and 40 degrees Celsius");
        }
        temperature = value;
    }

    std::string getStatus() const override
    {
        return Device::getStatus() + " (Temperature: " + std::to_string(temperature) + "°C)";
    }
};

class SecurityCamera : public Device
{
public:
    SecurityCamera(const std::string &name) : Device(name) {}

    std::string getStatus() const override
    {
        return Device::getStatus() + (isOn ? " (Recording)" : " (Not Recording)");
    }
};

bool isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}
class SmartHome
{
private:
    std::vector<std::unique_ptr<Device>> devices;
    Logger logger;

public:
    SmartHome() = default;

    void addDevice(std::unique_ptr<Device> device)
    {
        devices.push_back(std::move(device));
        logger.logStateChange(devices.back()->getName(), "Device added");
    }

    void listDevices() const
    {
        std::cout << CYAN << "\nDevices in the Smart Home:" << RESET << std::endl;
        for (const auto &device : devices)
        {
            std::cout << "- " << device->getName() << std::endl;
        }
    }

void executeCommand(const std::string& command) {
    logger.logCommand(command);
    std::istringstream iss(command);
    std::string action;
    iss >> action;

    std::string remainingCommand;
    std::getline(iss, remainingCommand);
    remainingCommand.erase(0, remainingCommand.find_first_not_of(" "));

    std::string deviceName;
    std::string settingStr;
    int settingValue = 0;
    bool hasSetting = false;

    if (action == "set") {
        size_t pos = remainingCommand.find_last_of(' ');
        if (pos != std::string::npos) {
            deviceName = sanitizeDeviceName(remainingCommand.substr(0, pos));
            settingStr = remainingCommand.substr(pos + 1);

            // Validate if the setting string is numeric
            if (!isNumeric(settingStr)) {
                std::cerr << RED << "Error: Invalid setting value. Please provide a numeric value." << RESET << std::endl;
                return;
            }
            settingValue = std::stoi(settingStr);
            hasSetting = true;
        }
    } else {
        deviceName = sanitizeDeviceName(remainingCommand);
    }

    try {
        auto it = std::find_if(devices.begin(), devices.end(),
            [&deviceName](const auto& device) { return device->getName() == deviceName; });

        if (it == devices.end()) {
            throw std::runtime_error("Device not found: " + deviceName);
        }

        if (action == "on") {
            (*it)->turnOn();
            logger.logStateChange(deviceName, "Turned ON");
        } else if (action == "off") {
            (*it)->turnOff();
            logger.logStateChange(deviceName, "Turned OFF");
        } else if (action == "status") {
            std::cout << (*it)->getStatus() << std::endl;
            logger.logStateChange(deviceName, "Status checked: " + (*it)->getStatus());
        } else if (action == "set" && hasSetting) {
            (*it)->adjustSetting(settingValue);
            logger.logStateChange(deviceName, "Setting adjusted to " + std::to_string(settingValue));
        } else {
            throw std::runtime_error("Invalid action or missing setting for: " + action);
        }
    } catch (const std::exception& e) {
        std::cerr << RED << "Error: " << e.what() << RESET << std::endl;
        logger.logStateChange("ERROR", e.what());
    }
}


    void showStatus() const
    {
        std::cout << CYAN << "\nDevice Statuses:" << RESET << std::endl;
        for (const auto &device : devices)
        {
            std::cout << device->getStatus() << std::endl;
        }
    }

    void showHistory(int limit = -1) const
    {
        logger.displayHistory(limit);
    }

    void saveHistory(const std::string &filename) const
    {
        logger.saveToFile(filename);
    }
};

void printLogo()
{
    std::cout << BLUE << R"(
   ___                      _     _    _
  / __|                    | |   | |  | |
 | (_  _ _ __   _ _ _ _| |_  | |_| | __  _ _ __   _
  \_ \| '_ ` _ \ / ` | '| _| |  _  |/ _ \| ' ` _ \ / _ \
  _) | | | | | | (| | |  | |_  | |  | | () | | | | | |  _/
 |__/|| || ||\_,||   \| ||  ||\_/|| || ||\_|



)" << RESET << std::endl;

    std::cout << YELLOW << "Developed by:" << RESET << std::endl;
    std::cout << GREEN << "  Abdullah Salman EL-121" << std::endl;
    std::cout << "  Syed Mahad Ali EL-120" << std::endl;
    std::cout << "  M.Hashim EL-119" << std::endl;
    std::cout << "  Hamza Pasha EL-147" << std::endl;
    std::cout << "  Suleiman Farooqui EL-125" << RESET << std::endl;
    std::cout << std::endl;
}

int main()
{
    SmartHome smartHome;

    smartHome.addDevice(std::make_unique<Light>("Living Room Light"));
    smartHome.addDevice(std::make_unique<Thermostat>("Main Thermostat"));
    smartHome.addDevice(std::make_unique<SecurityCamera>("Front Door Camera"));

    printLogo();
    std::cout << YELLOW << "Welcome to the Smart Home Automation System!" << RESET << std::endl;
    std::cout << "Available commands:" << std::endl;
    std::cout << "- list : List all devices" << std::endl;
    std::cout << "- on <device> : Turn on a device" << std::endl;
    std::cout << "- off <device> : Turn off a device" << std::endl;
    std::cout << "- status <device> : Check device status" << std::endl;
    std::cout << "- set <device> <value> : Adjust device setting" << std::endl;
    std::cout << "- show : Show status of all devices" << std::endl;
    std::cout << "- history [n] : Show last n commands (shows all if n not specified)" << std::endl;
    std::cout << "- save <filename> : Save command history to file" << std::endl;
    std::cout << "- exit : Exit the program" << std::endl;

    std::string command;
    while (true)
    {
        std::cout << "\nEnter a command: ";
        std::getline(std::cin, command);

        if (command == "exit")
        {
            break;
        }
        else if (command == "list")
        {
            smartHome.listDevices();
        }
        else if (command == "show")
        {
            smartHome.showStatus();
        }
        else if (command.substr(0, 7) == "history")
        {
            try
            {
                int limit = -1;
                if (command.length() > 8)
                {
                    limit = std::stoi(command.substr(8));
                }
                smartHome.showHistory(limit);
            }
            catch (const std::exception &e)
            {
                std::cout << RED << "Invalid history command format" << RESET << std::endl;
            }
        }
        else if (command.substr(0, 4) == "save")
        {
            if (command.length() > 5)
            {
                smartHome.saveHistory(command.substr(5));
            }
            else
            {
                std::cout << RED << "Please specify a filename" << RESET << std::endl;
            }
        }
        else
        {
            smartHome.executeCommand(command);
        }
    }

    std::cout << YELLOW << "Thank you for using the Smart Home Automation System!" << RESET << std::endl;
    return 0;
}
