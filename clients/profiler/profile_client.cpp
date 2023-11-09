#include <iostream>
#include <echo_cache_client.h>
#include <chrono>

double sendSetRequests(RemoteCache client, int n) {
    auto t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < n; ++i) {
        auto t3 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = t3 - t1;
        std::cerr << ms.count() << " " << i << std::endl;
        client.set("name", "tanson");
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t2 - t1;
    return ms.count();
}

int main() {
    int numRequests = 10000;
    RemoteCache client("127.0.0.1", 8000);
    double duration = sendSetRequests(client, numRequests);
    std::cerr << "(" << numRequests << "): " << duration << std::endl;
    client.initiateAndCloseConnection();
    return 0;
}