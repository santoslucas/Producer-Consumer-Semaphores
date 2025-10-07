#include <bits/stdc++.h>
#include <semaphore.h>
#include <thread>
#include <mutex>

using namespace std;

vector<int> MEM;
const int NUM_IT = 100000;

sem_t emptySlots; // counts free slots (initial = BUFFER_SIZE)
sem_t filledSlots; // counts occupied slots (initial = 0)
sem_t semMem;

int MEM_SIZE;
int producedCount = 0;
int consumedCount = 0;
int consPos = 0;
int prodPos = 0;

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= sqrt(n); ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

int generateRandomNumber() {
    random_device rd;  
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 10000000);

    return dist(gen);
}

void* producer(void* arg)  {
    while (true) {
        sem_wait(&emptySlots);
        sem_wait(&semMem);
        
        if (producedCount >= NUM_IT) {
            sem_post(&semMem);
            sem_post(&emptySlots);
            break;
        }

        int value = generateRandomNumber();
        MEM[prodPos] = value;
        prodPos = (prodPos + 1) % MEM_SIZE;
        producedCount++;

        sem_post(&semMem);
        sem_post(&filledSlots);
    }

    return nullptr;
}

void* consumer(void* arg) {
    while (true) {
        sem_wait(&filledSlots);
        sem_wait(&semMem);

        if (consumedCount >= NUM_IT) {
            sem_post(&semMem);
            sem_post(&filledSlots);
            break;
        }

        int value = MEM[consPos];
        consPos = (consPos + 1) % MEM_SIZE;
        consumedCount++;

        sem_post(&semMem);
        sem_post(&emptySlots);

        bool prime = isPrime(value);
        printf("Consumer %lu consumiu %d -> %s (consumedCount=%d)\n",
               pthread_self(), value, prime ? "PRIMO" : "NAO PRIMO", consumedCount);
        
        if (consumedCount >= NUM_IT) {
            break;
        }
    }

    return nullptr;
}


int main() {
    int numP, numC;

    cout << "Digite o tamanho maximo da memoria: " << endl;
    cin >> MEM_SIZE;

    MEM.resize(MEM_SIZE);

    sem_init(&emptySlots, 0, MEM_SIZE);
    sem_init(&filledSlots, 0, 0);
    sem_init(&semMem, 0, 1);

    cout << "Digite o numero de threads do produtor: " << endl;
    cin >> numP;
    
    cout << "Digite o numero de threads do consumidor: " << endl;
    cin >> numC;

    vector<pthread_t> producers(numP), consumers(numC);

    for (int i = 0; i < numP; ++i) pthread_create(&producers[i], nullptr, producer, nullptr);
    for (int i = 0; i < numC; ++i) pthread_create(&consumers[i], nullptr, consumer, nullptr);

    for (int i = 0; i < numP; ++i) pthread_join(producers[i], nullptr);

    for (int i = 0; i < numC; ++i) {
        sem_post(&filledSlots);
    }

    for (int i = 0; i < numC; ++i) pthread_join(consumers[i], nullptr);

    sem_destroy(&emptySlots);
    sem_destroy(&filledSlots);
    sem_destroy(&semMem);

    return 0;
}