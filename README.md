# 🏠 Smart Home Automation System (Console-Based)

Welcome to the **Smart Home Automation System**, a simple yet effective C++ project simulating a smart home environment. This console-based system allows users to control and monitor devices such as lights, thermostats, and security cameras with ease.

---

## 📌 Overview

This project is a text-based smart home controller written in C++. It allows users to:

- Turn devices on/off
- Adjust device-specific settings (like brightness or temperature)
- View individual and overall device statuses
- Log command history
- Save interaction logs to a file

The system uses object-oriented programming principles and logs all activities for future review.

---

## 🎯 Features

- ✅ **Device Management**
  - Add multiple devices (`Light`, `Thermostat`, `SecurityCamera`)
  - Dynamic device status tracking
  
- 💡 **Device-Specific Actions**
  - Light: On/Off with adjustable brightness (0–100%)
  - Thermostat: Set temperature (0–40°C)
  - Security Camera: On/Off with recording status

- 📋 **Logging & History**
  - Timestamped command logging
  - Saveable command history (`smart_home_log.txt`)
  - Display recent commands via `history [n]`

- 🖥️ **User Interface**
  - Interactive command-line interface
  - Color-coded terminal output using ANSI escape codes

- 📄 **Persistence**
  - Logs are written to a file (`smart_home_log.txt`)
  - Option to export history to a custom file

---

## 🧱 Design Choices

- **OOP Principles**: Inheritance and polymorphism are used to extend a generic `Device` base class into specific devices like `Light`, `Thermostat`, and `SecurityCamera`.

- **Logger Class**: Centralized logging system to handle command and state changes with timestamps.

- **Command Parser**: Parses textual commands into structured actions (e.g., `set Thermostat 25`).

- **Error Handling**: Graceful error messages and input validation (e.g., invalid device or non-numeric setting values).

- **Smart Pointers (`std::unique_ptr`)**: For safe and automatic memory management of dynamically allocated devices.

- **Sanitization**: Device names are sanitized to prevent invalid characters like `<` and `>` from affecting behavior.

---

## 🚀 Future Improvements

- 📡 **Add Networking Support**
  - Enable remote control via sockets or HTTP endpoints

- 📱 **GUI Integration**
  - Develop a graphical front-end using Qt or a web interface

- 📦 **Device Persistence**
  - Store device states in a database or file for recovery across sessions

- 🎙️ **Voice Command Support**
  - Integrate speech recognition for hands-free control

- 🔐 **User Authentication**
  - Implement role-based access control and secure command execution

- 📈 **Analytics Dashboard**
  - Visualize usage patterns and device performance over time





## 📂 How to Run

1. **Compile** the code:
   ```bash
   g++ -std=c++17 -o SmartHome SmartHome.cpp
