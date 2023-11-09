# Echo Cache

## Table of Contents

-   [Introduction](#introduction)
-   [Usage](#usage)
-   [Use cases](#use-cases)
-   [Architecture](#architecture)
    -   [Worker](#worker)
    -   [Orchestrator](#orchestrator)
    -   [Scaling](#scaling)
    -   [Fault Tolerance](#fault-tolerance)
    -   [Custom Network Protocol](#custom-network-protocol)
-   [Storage Engine](#storage-engine)

## Introduction

Echo cache is a distributed, scalable, in-memory caching solution prioritizing performance and simplicity. This project was created to explore distributed systems, networking, and architectural design.

### Goals for this cache

-   **Highly consistent and partition tolerant**: In the [CAP theorem](https://en.wikipedia.org/wiki/CAP_theorem), prioritize C and P.
-   **Fault tolerant**: The cache can still function and recover if any worker nodes fail.
-   **Performance**: Designed to be highly performant without bottlenecks.

## Usage

You will need to run multiple workers, one orchestrator, then your application which uses the cache. It is highly recommended to use a provided client since they properly implement the custom network protocol that is used. However, if you are using a language without support, read the [Custom Network Protocol](#custom-network-protocol) section before implementing a client yourself.

### Quick Start

Make sure you have CMake and a C++ compiler installed.

```bash
git clone https://github.com/tansonlee/echo-cache.git
chmod u+x run.sh
./run.sh
```

### Detailed Start

1. Build the project.

```bash
git clone https://github.com/tansonlee/echo-cache.git
mkdir build
cd build
cmake ..
make
```

2. Run the workers. In this case, we will be running 2 workers.

```bash
# In shell 1.
./build/worker/worker <worker1 port>
# In shell 2.
./build/worker/worker <worker2 port>
```

3. Run the orchestrator. In this case, we are assuming we have 2 workers.

```bash
# In shell 3
./build/orchestrator/orchestrator <orchestrator port> <worker1 ip> <worker1 port> <worker2 ip> <worker2 port>
```

4. Run the client program. You can also run the CLI tool for testing as shown here.

```bash
# In shell 4
./build/clients/commandline <orchestrator ip> <orchestrator port>
```

### Concrete Example

```bash
# In shell 1.
./build/worker/worker 3000
# In shell 2.
./build/worker/worker 3001
# In shell 3
./build/orchestrator/orchestrator 8000 127.0.0.1 3000 127.0.0.1 3001
# In shell 4
./build/clients/commandline 127.0.0.1 8000

Type a command:
 1. set <key> <value>
 2. get <key>
 3. quit

>>> set name tanson
Success

>>> get name
Response (0): 'tanson'
```

### C++ Client Example

```c++
#include <iostream>
#include <echo_cache_client.h>

int main() {
    RemoteCache client("127.0.0.1", 8000);
    client.set("name", "tanson");
    std::cout << client.get("name") << std::endl; // prints "tanson"
}
```

### Python Client Example

```python
from echo_cache_client import RemoteCache

client = RemoteCache("127.0.0.1", 8000)
client.set("name", "tanson");
print(client.get("name")) # prints "tanson"
```

## Use cases

This cache is right for you if:

-   You require access from multiple machines
-   You require scalability
-   You require some level of fault tolerance
-   You require consistency
-   You do not need durability since there are no guarantees on cache eviction

Do not use this cache if:

-   Your application cannot handle cache misses. To optimize throughput, there is no guarantee on the durability of writes. Echo Cache uses a [least recently used (LRU)]() cache eviction strategy.

Some common use cases are:

-   A caching layer between a database and an application where the application is sharded or runs on multiple machines. In this case, the cache can be shared between them.
-   Sharing values across multiple servers or applications.
-   Saving repeated expensive calculations used across more than one machine.

## Architecture

![Architecture](architecture.png)

### Worker

A worker contains the actual in-memory store and receives messages from the orchestrator to execute queries on the cache. Workers also perform cache management in the form of memory usage by implementing a least recently used (LRU) cache eviction strategy.

### Orchestrator

The orchestrator acts as a proxy between applications and workers. It takes requests fro clients then sends messages to the appropriate worker(s) to perform the actual execution.

To ensure that the orchestrator does not act as a bottleneck, it implements a multi-threading scheme similar to that of TCP. On every new connection by a client, it will create a new thread dedicated to that client. This means that when multiple clients connect to the orchestrator, they will not compete for execution. This prevents [head-of-line blocking](https://en.wikipedia.org/wiki/Head-of-line_blocking). Additionally, this removes the overhead of connection establishment on every request since the connection may stay open across multiple requests.

You may have noticed that this is tricky if clients do not cooperate. If clients do not signal closing of a connection, the orchestrator may create an ever growing number of threads and the physical machine may run out of resources to manage them. To solve this, the orchestrator enacts a idle client detection scheme. If there is no activity from a client for a specified amount of time, the orchestrator will close the connection.

This is implemented with a "custodian" thread which finds all connections and when they were last used. If the custodian determines that the client is idle, it will kill the connection and thread, thus reclaiming the resources they were using.

### Scaling

The orchestrator partitions the keyspace into `n` partitions where `n` is the number of workers running. Each worker is responsible for 2 partitions of the keyspace. This means that the cache can scale linearly by running more workers. Deploying more workers allows the cache to have more storage space and be more performant.

### Fault Tolerance

All data is replicated across 2 workers since each worker is responsible for 2 partitions of the keyspace. This means that if a worker goes down, the data will still be available on another worker.

Every write to the cache writes the data to the 2 workers that are responsible for the keyspace partition that the key belongs to. Every read first queries one of these workers then potentially the second worker if the first worker failed.

### Custom Network Protocol

The network protocol used between clients and the orchestrator is a custom protocol built on top of network sockets. Since the protocol is custom, it is highly recommended to use one of the supported clients.

The protocol is similar to TCP since each client gets its own dedicated thread. This allows for higher throughput since clients do not need to wait for requests from other clients. This prevents [head-of-line blocking](https://en.wikipedia.org/wiki/Head-of-line_blocking). Additionally, this removes the overhead of connection establishment on every request since the connection may stay open across multiple requests.

However, resources may leak if clients do not close their connections when they are finished since the number of threads that the orchestrator process is running may continue to increase forever. To prevent this, the orchestrator periodically prunes idle connections.

Ultimately, this means that there is no guarantee on the lifetime of a connection so a client _must_ be tolerant to connections being closed by the server. The client should just re-establish a new connection and continue sending requests.

## Storage Engine

Data is stored in-memory to enable fast I/O which leads to high throughput. The concrete data structure used to store key-value pairs is a [hash table](https://en.wikipedia.org/wiki/Hash_table) which uses [Cuckoo hashing](https://en.wikipedia.org/wiki/Cuckoo_hashing) for hash collision resolution.

The reason Cuckoo hashing was chosen is because it enables `O(1)` **worst case** lookup, `O(1)` average case insert, and `O(1)` **worst case** lookup. This comes at a cost of larger memory usage and potentially slower insert times.

Echo Cache is tolerant to memory overflows. It implements a [least recently used (LRU)](https://en.wikipedia.org/wiki/Cache_replacement_policies#LRU) cache eviction policy. This means that Echo Cache will not use more memory than allocated to it.

## Next Steps

1. Improve worker failure recovery. At the moment, when a worker comes back up, its cache is empty. We could have the orchestrator siphon the right data into a worker by fetching from the worker's partners.
2. Perform leader election. There is only a single orchestrator and if it crashes, the cache will become unavailable. It would be better to elect a new leader which will act as the new orchestrator.
