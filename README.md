# Car-Tunnel-Simulation

To Run: g++ -fpermissive main.cpp -lpthread -o A

This program uses pthreads, pthread mutexes and pthread condition variables to simulate a car tunnel.
A car tunnel restricts the number of northbound/southbound cars and limits the total number of cars in the tunnel.
The program takes in an input file with the limits and the car sequence through io redirection. Each car creates a new
thread. If the car can enter the tunnel, it does, otherwise the condition variable has the car wait. The program outputs
when a car arrives at the tunnel, enters the tunnel, exits the tunnel. At the end it prints the total number of cars
that went north, went south, had to wait.
