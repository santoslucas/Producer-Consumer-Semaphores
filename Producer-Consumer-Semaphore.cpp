#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

vector<int> MEM;
vector<int> bufferOccupation;  

const int NUM_IT = 100000;

sem_t emptySlots; 
sem_t filledSlots; 
sem_t semMem;

atomic<int> producedCount{0}; 
atomic<int> consumedCount{0};

int MEM_SIZE;
int prodPos = 0;
int consPos = 0;

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
    static thread_local mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(1, 10000000);
    return dist(gen);
}

void recordOccupation() {
    int filled;
    sem_getvalue(&filledSlots, &filled);
    bufferOccupation.push_back(filled);
}

void* producer(void* arg) {
    while (true) {
        sem_wait(&emptySlots);
        sem_wait(&semMem);

        if (producedCount >= NUM_IT) {
            sem_post(&semMem);
            break;
        }

        int value = generateRandomNumber();
        MEM[prodPos] = value;
        prodPos = (prodPos + 1) % MEM_SIZE;
        producedCount++;

        recordOccupation();

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
            break;
        }

        int value = MEM[consPos];
        consPos = (consPos + 1) % MEM_SIZE;
        consumedCount++;

        recordOccupation();

        sem_post(&semMem);
        sem_post(&emptySlots);

        printf("Consumer %lu consumiu %d -> %s\n",
               pthread_self(), value, isPrime(value) ? "PRIMO" : "NAO PRIMO");

        if (consumedCount >= NUM_IT) break;
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

    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numP; ++i)
        pthread_create(&producers[i], nullptr, producer, nullptr);
    for (int i = 0; i < numC; ++i)
        pthread_create(&consumers[i], nullptr, consumer, nullptr);

    for (int i = 0; i < numP; ++i)
        pthread_join(producers[i], nullptr);

    for (int i = 0; i < numC; ++i)
        sem_post(&filledSlots); 

    for (int i = 0; i < numC; ++i)
        pthread_join(consumers[i], nullptr);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "\nTempo total de execucao: " << elapsed.count() << " segundos\n";

    ofstream csv("ocupacao_buffer.csv");
    csv << "operacao,ocupacao\n";
    for (size_t i = 0; i < bufferOccupation.size(); ++i)
        csv << i << "," << bufferOccupation[i] << "\n";
    csv.close();

    cout << "Arquivo CSV 'ocupacao_buffer.csv' gerado com sucesso.\n";

    sem_destroy(&emptySlots);
    sem_destroy(&filledSlots);
    sem_destroy(&semMem);

    return 0;
}
