#include <iostream>
#include <string>

#include <cpp_client.h>

std::string testBadGet(RemoteCache& remoteCache) {
    HandlerResponse response = remoteCache.get("asdfasdfasdfasdfasdfasdf");
    if (response.statusCode != StatusCode::keyNotFound) {
        return "Found key we were not supposed to find.";
    }
    return "";
}

std::string testSet(RemoteCache& remoteCache) {
    bool success = remoteCache.set("name", "tanson");
    if (!success) {
        return "Set not working.";
    }
    return "";
}

std::string testGet(RemoteCache& remoteCache) {
    remoteCache.set("name", "tanson");
    HandlerResponse response = remoteCache.get("name");
    if (response.statusCode != StatusCode::success || response.result != "tanson") {
        return "Get not working.";
    }
    return "";
}

void printResult(std::string result) {
    if (result.length() == 0) {
        std::cout << "✅" << std::endl;
    }
    else std::cout << "❌ " << result << std::endl;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server address> <server port>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int port = atoi(argv[2]);

    RemoteCache remoteCache{ip, port};
    std::cout << "-----TESTING-----" << std::endl;

    std::string testBadGetResult = testBadGet(remoteCache);
    printResult(testBadGetResult);
    std::string testSetResult = testSet(remoteCache);
    printResult(testSetResult);
    std::string testGetResult = testGet(remoteCache);
    printResult(testGetResult);

    return 0;
}