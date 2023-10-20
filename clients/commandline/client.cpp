#include <string.h>
#include <iostream>
#include <string>

#include "../cpp/client.h"

void printCommandList() {
    std::cout << "Type a command:" << std::endl;
    std::cout << " 1. set <key> <value>" << std::endl;
    std::cout << " 2. get <key>" << std::endl;
    std::cout << " 3. quit" << std::endl;
    std::cout << ">>>";
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server address> <server port>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = atoi(argv[2]);

    RemoteCache remoteCache{ip, port};

    while (true) {
        printCommandList();

        std::string command;
        std::cin >> command;

        if (command == "set") {
            std::string key;
            std::string value;
            std::cin >> key >> value;
            bool success = remoteCache.set(key, value);
            if (success) {
                std::cout << "Success" << std::endl;
            } else {
                std::cout << "Failure" << std::endl;
            }
        } else if (command == "get") {
            std::string key;
            std::cin >> key;
            std::string response = remoteCache.get(key);
            std::cout << "Response: '" << response << "'" << std::endl;
        } else if (command == "quit") {
            break;
        } else {
            std::cerr << "Invalid command" << std::endl;
        }
    }

    return 0;
}
