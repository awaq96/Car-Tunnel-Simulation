#include <iostream>
#include <string>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
// g++ -fpermissive main.cpp -lpthread -o A
/*

This program uses pthreads, pthread mutexes and pthread condition variables to simulate a car tunnel.
A car tunnel restricts the number of northbound/southbound cars and limits the total number of cars in the tunnel.
The program takes in an input file with the limits and the car sequence through io redirection. Each car creates a new
 thread. If the car can enter the tunnel, it does, otherwise the condition variable has the car wait. The program outputs
 when a car arrives at the tunnel, enters the tunnel, exits the tunnel. At the end it prints the total number of cars
 that went north, went south, had to wait.
*/


using namespace std;

#define MAXTHREADS 128 // Thread limit given by Dr. Paris

// Global Variables
// Thread
pthread_t cartid[MAXTHREADS];
// Counters
static int maxcars=0, nblimit=0, sblimit=0, tunnelcurr=0,nbcurr=0,sbcurr=0,carwait=0;

// Mutex and condition
static pthread_mutex_t carLock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t waiting = PTHREAD_COND_INITIALIZER;


// Thread for Northbound cars
void *northBound(void *argList){

    // Lock mutex
    pthread_mutex_lock(&carLock);

    // Setup the variables the function took in
    int *arr = (int*)argList;
    int thisCarSeqNumber = arr[0];
    unsigned  int transitTime =arr[1];

    bool didCarWait = false;

    // Tell the world car has arrived at tunnel
    cout << "Northbound car # "<<thisCarSeqNumber<<" arrives at the tunnel."<<endl;

    //Check if car can enter tunnel
    nbcurr++;
    while(maxcars<tunnelcurr || nblimit<nbcurr){
        // If car cannot enter, thread waits
        didCarWait=true;
        pthread_cond_wait(&waiting,&carLock);
    }

    // Tell world car has entered the tunnel
    cout <<"Northbound car # " << thisCarSeqNumber << " enters the tunnel." <<endl;

    // Update counters
    if(didCarWait)
        carwait++;

    tunnelcurr++;

    // Release Mutex
    pthread_mutex_unlock(&carLock);

    // Sleep tunnel travel time
    sleep(transitTime);

    // Lock for a second time
    pthread_mutex_lock(&carLock);

    // Tell the world car leaves the tunnel
    cout << "Northbound car # " <<thisCarSeqNumber << " exits the tunnel."<<endl;

    // Update counters
    tunnelcurr--;
    nbcurr--;

    // Broadcast an OK
    pthread_cond_broadcast(&waiting);

    // Release the mutex
    pthread_mutex_unlock(&carLock);

}

// Thread for Southbound cars
void *southBound(void *argList){

    // Lock mutex
    pthread_mutex_lock(&carLock);

    int *arr = (int*)argList;
    int thisCarSeqNumber = arr[0];
    unsigned  int transitTime =arr[1];


    bool didCarWait = false;

    // Tell the world car has arrived at tunnel
    cout << "Southbound car # "<<thisCarSeqNumber<<" arrives at the tunnel."<<endl;
    sbcurr++;

    //Check if car can enter tunnel
    while(maxcars<tunnelcurr || sblimit<sbcurr){
        // If car cannot enter, thread waits
        didCarWait=true;
        pthread_cond_wait(&waiting,&carLock);

    }

    // Tell world car has entered the tunnel
    cout <<"Southbound car # " << thisCarSeqNumber << " enters the tunnel." <<endl;

    // Update counters
    if(didCarWait)
        carwait++;

    tunnelcurr++;

    // Release Mutex

    pthread_mutex_unlock(&carLock);

    // Sleep tunnel travel time
    sleep(transitTime);

    // Lock for a second time

    pthread_mutex_lock(&carLock);

    // Tell the world car leaves the tunnel
    cout << "Southbound car # " <<thisCarSeqNumber << " exits the tunnel."<<endl;

    // Update counters
    tunnelcurr--;
    sbcurr--;

    // Broadcast an OK
    pthread_cond_broadcast(&waiting);

    // Release the mutex
    pthread_mutex_unlock(&carLock);

}


int main() {

    char direction;
    unsigned int delay,transitTime;
    int nThreads=1,nbcount=0,sbcount=0;
    int threads[20],i=0,cartotal=0;
    int send2threadN[2],send2threadS[2],Sid=1,Nid=1;

    // Limit inputs
    cin>>maxcars;
    cin >> nblimit;
    cin>>sblimit;

    // Output Limits
    cout << "Maximum number of cars in the tunnel: " << maxcars<<endl;
    cout<< "Maximum number of northbound cars: " <<nblimit<<endl;
    cout <<"Maximum number of southbound cars: "<<sblimit<<endl;

    // This structure implemented is based off Dr. Paris' suggested structure on piazza
    while(cin >> delay >>direction>>transitTime){

        sleep(delay); // delay depends on first input value of each line

        // Two threads, depending on direction of car
        threads[i]=nThreads;

        // Northbound
        if (direction == 'N'){
            send2threadN[0]=Nid;
            Nid++;
            send2threadN[1]=transitTime;
            nbcount++;
            pthread_create(&cartid[nThreads],NULL,northBound, (void *)send2threadN);

        }
        else {
            // Southbound
            send2threadS[0]=Sid;
            Sid++;
            send2threadS[1]=transitTime;
            sbcount++;
            pthread_create(&cartid[nThreads],NULL,southBound, (void *)send2threadS);

        }

        // Update Counts
        nThreads++;
        i++;
        cartotal++;

        // check to make sure program is taking in the entire file
        //cout << delay << direction << timetaken <<endl;

    }



    for(i=0;i<cartotal;i++){
        pthread_join(cartid[i],NULL);
    }


    // Summary Output
    cout << nbcount <<" northbound car(s) crossed the tunnel."<<endl;
    cout << sbcount <<" southbound car(s) crossed the tunnel."<<endl;
    cout << carwait << " car(s) had to wait."<<endl;



    return 0;
}

