#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

std::string trim(const std::string& input) {
    const auto begin = input.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }
    const auto end = input.find_last_not_of(" \t\r\n");
    return input.substr(begin, end - begin + 1);
}

std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string extractAfterPrefix(const std::string& input, const std::string& prefix) {
    if (input.size() <= prefix.size()) {
        return "";
    }
    return trim(input.substr(prefix.size()));
}

std::string interpretCommand(const std::string& raw) {
    const std::string input = trim(raw);
    if (input.empty()) {
        return "";
    }

    const std::string lower = toLower(input);

    if (lower == "exit" || lower == "quit") {
        return "exit";
    }
    if (lower == "help" || lower == "?" || lower == "--help") {
        return "help";
    }
    if (lower == "clear") {
        return "clear";
    }

    if (lower == "ls" || lower == "list files" || lower == "show files" || lower == "show me the files") {
        return "ls -la";
    }
    if (lower == "pwd" || lower == "show current directory" || lower == "show me the current directory" || lower == "where am i") {
        return "pwd";
    }
    if (lower == "ps" || lower == "show processes" || lower == "show running processes") {
        return "ps aux";
    }
    if (lower == "df" || lower == "show disk usage" || lower == "show storage") {
        return "df -h";
    }
    if (lower == "env" || lower == "show environment" || lower == "show me the environment") {
        return "env";
    }
    if (lower == "date" || lower == "show date" || lower == "what time is it") {
        return "date";
    }
    if (lower == "who" || lower == "show users" || lower == "who is logged in") {
        return "who";
    }
    if (lower == "free" || lower == "show memory" || lower == "show ram") {
        return "free -h";
    }
    if (lower == "cpu" || lower == "show cpu" || lower == "show processor info") {
        return "lscpu";
    }
    if (lower == "whoami" || lower == "show current user") {
        return "whoami";
    }
    if (lower == "uname" || lower == "show system info") {
        return "uname -a";
    }
    if (lower == "top" || lower == "show live processes") {
        return "top -b -n 1";
    }
    if (lower == "tree" || lower == "show tree") {
        return "tree";
    }
    if (lower.rfind("create folder ", 0) == 0 || lower.rfind("make folder ", 0) == 0 || lower.rfind("new folder ", 0) == 0) {
        const std::string prefix = lower.rfind("create folder ", 0) == 0 ? "create folder " : (lower.rfind("make folder ", 0) == 0 ? "make folder " : "new folder ");
        return "mkdir -p " + extractAfterPrefix(input, prefix);
    }
    if (lower.rfind("remove folder ", 0) == 0 || lower.rfind("delete folder ", 0) == 0) {
        const std::string prefix = lower.rfind("remove folder ", 0) == 0 ? "remove folder " : "delete folder ";
        return "rm -rf " + extractAfterPrefix(input, prefix);
    }
    if (lower.rfind("create file ", 0) == 0 || lower.rfind("make file ", 0) == 0) {
        const std::string prefix = lower.rfind("create file ", 0) == 0 ? "create file " : "make file ";
        return "touch " + extractAfterPrefix(input, prefix);
    }
    if (lower.rfind("remove file ", 0) == 0 || lower.rfind("delete file ", 0) == 0) {
        const std::string prefix = lower.rfind("remove file ", 0) == 0 ? "remove file " : "delete file ";
        return "rm -f " + extractAfterPrefix(input, prefix);
    }
    if (lower.rfind("show file ", 0) == 0 || lower.rfind("open file ", 0) == 0 || lower.rfind("read file ", 0) == 0) {
        const std::string prefix = lower.rfind("show file ", 0) == 0 ? "show file " : (lower.rfind("open file ", 0) == 0 ? "open file " : "read file ");
        return "cat " + extractAfterPrefix(input, prefix);
    }
    if (lower.rfind("install package ", 0) == 0 || lower.rfind("install ", 0) == 0) {
        const std::string package = lower.rfind("install package ", 0) == 0 ? extractAfterPrefix(input, "install package ") : extractAfterPrefix(input, "install ");
        if (!package.empty()) {
            return "sudo apt install -y " + package;
        }
    }
    if (lower == "update system" || lower == "update") {
        return "sudo apt update";
    }
    if (lower == "upgrade system" || lower == "upgrade") {
        return "sudo apt upgrade -y";
    }
    if (lower.rfind("cd ", 0) == 0) {
        return input;
    }
    return input;
}

std::string runCommand(const std::string& rawCommand) {
    const std::string command = trim(rawCommand);
    if (command.empty()) {
        return "";
    }

    if (command == "help") {
        return "HarrAi can understand natural-language requests and run Linux commands.\n"
               "Examples: ls, pwd, ps, df, env, date, who, free, cpu\n"
               "Also try: create folder demo, show current directory, install package nginx\n"
               "You can also type full shell commands directly.";
    }

    if (command == "clear") {
        std::cout << "\033[2J\033[H";
        return "";
    }

    if (command.rfind("cd ", 0) == 0) {
        const std::string target = trim(command.substr(3));
        if (target.empty()) {
            return "Usage: cd <directory>";
        }
        if (chdir(target.c_str()) != 0) {
            return "Unable to change directory.";
        }
        return "Changed directory to " + target;
    }

    const std::string shellCommand = command + " 2>&1";
    std::array<char, 128> buffer{};
    std::string output;

    FILE* pipe = popen(shellCommand.c_str(), "r");
    if (!pipe) {
        return "Failed to execute command.";
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();
    }

    const int status = pclose(pipe);
    if (WIFEXITED(status)) {
        const int exitCode = WEXITSTATUS(status);
        if (exitCode != 0 && output.empty()) {
            return "Command exited with code " + std::to_string(exitCode);
        }
        if (exitCode != 0) {
            return output + "\n[Exit code: " + std::to_string(exitCode) + "]";
        }
    }

    if (output.empty()) {
        return "Command completed successfully.";
    }
    return output;
}

int main() {
    std::cout << "========================================\n";
    std::cout << "HarrAi - Linux terminal assistant\n";
    std::cout << "Type 'help' for commands or 'exit' to quit.\n";
    std::cout << "========================================\n";

    std::string input;
    while (true) {
        std::cout << "HarrAi> ";
        std::getline(std::cin, input);

        if (!std::cin) {
            break;
        }

        const std::string interpreted = interpretCommand(input);
        if (interpreted == "exit") {
            break;
        }

        const std::string result = runCommand(interpreted);
        if (!result.empty()) {
            std::cout << result << std::endl;
        }
    }

    std::cout << "Goodbye from HarrAi.\n";
    return 0;
}
