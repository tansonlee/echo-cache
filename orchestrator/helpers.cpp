#include "helpers.h"
#include <chrono>
#include <climits>

int getCurrentTime() {
  int ms = std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();

  return ms / 1000;
}

int nextId = 0;

int getNextId() {
  int result = nextId;
  nextId += 1;
  nextId = nextId % INT_MAX;
  return result;
}

int hashKey(std::string key, int numWorkers) {
  int result = 0;
  int len = key.length();
  for (int i = 0; i < len; ++i) {
    result += (int)key.at(i);
    result = result % numWorkers;
  }

  return result;
}

CommandLineArguments parseCommandLineArguments(int argc, char *argv[]) {
  // Ensure there are an even number of args.
  if (argc % 2 != 0) {
    return {false, 0, 0, {}};
  }

  int port = atoi(argv[1]);
  if (port == 0) {
    return {false, 0, 0, {}};
  }

  int numWorkers = (argc - 2) / 2;
  IpAndPort *workers = new IpAndPort[numWorkers];

  for (int i = 2; i < argc; i += 2) {
    std::string ip = argv[i];
    int port = atoi(argv[i + 1]);
    if (port == 0) {
      return {false, 0, 0, {}};
    }

    workers[(i - 2) / 2] = {ip, port};
  }

  return {true, port, numWorkers, workers};
}