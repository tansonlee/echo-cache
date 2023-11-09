_term() { 
    echo "Caught SIGTERM signal!" 
    kill $worker_pid_1
    kill $worker_pid_2
    kill $orchestrator_pid
}

mkdir -p build
cd build
cmake ..
make

# Run worker 1
./worker/worker 3000 1000 1> /dev/null 2> /dev/null &
worker_pid_1=$!

# Run worker 2
./worker/worker 3001 1000 1> /dev/null 2> /dev/null &
worker_pid_2=$!

# Run orchestrator
./orchestrator/orchestrator 8000 127.0.0.1 3000 127.0.0.1 3001 1> /dev/null 2> /dev/null &
orchestrator_pid=$!

# Run client
./clients/commandline/main 127.0.0.1 8000

kill $worker_pid_1
kill $worker_pid_2
kill $orchestrator_pid