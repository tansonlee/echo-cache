#ifndef ORCHESTRATOR_HELPERS_H
#define ORCHESTRATOR_HELPERS_H

#include <string>

// Gives the current seconds since epoch.
int getCurrentTime();

// Gives numbers sequentially from 0 to INT_MAX then restarts from 0.
int getNextId();

// Gives an int from 0 to (numWorkers - 1).
int hashKey(std::string key, int numWorkers);

struct IpAndPort {
    std::string ip;
    int port;
};

struct CommandLineArguments {
    bool success;
    int port;
    int numWorkers;
    IpAndPort* workers;
};

CommandLineArguments parseCommandLineArguments(int argc, char *argv[]);

#endif