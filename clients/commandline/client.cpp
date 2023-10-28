#include <string.h>
#include <iostream>
#include <string>

#include <cpp_client.h>
#include <parser.h>

void printCommandList() {
    std::cout << std::endl;
    std::cout << "Type a command:" << std::endl;
    std::cout << " 1. set <key> <value>" << std::endl;
    std::cout << " 2. get <key>" << std::endl;
    std::cout << " 3. quit" << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server address> <server port>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = atoi(argv[2]);

    RemoteCache remoteCache{ip, port};

    printCommandList();
    while (true) {
        std::cout << std::endl << ">>> ";

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
            HandlerResponse response = remoteCache.get(key);
            std::cout << "Response (" << response.statusCode << "): '" << response.result << "'" << std::endl;
        } else if (command == "quit") {
            remoteCache.closeConnection();
            break;
        } else {
            std::cerr << "Invalid command" << std::endl;
        }
    }

    return 0;
}
