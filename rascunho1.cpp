#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <random>
#include <chrono>
#include <cstdlib>
#include <cmath>

const int MESSAGE_BUFFER_SIZE = 20;

bool isPrime(int n) {
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= static_cast<int>(std::sqrt(n)); ++i) {
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}


void producer(std::queue<int>& q, std::mutex& m, std::condition_variable& cv, int numbers_to_generate) {
    std::mt19937 rng(static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(1, 100);

    std::cout << "[PRODUCER] Starting generation." << std::endl;

    long long current_number = 1;

    for (int i = 0; i < numbers_to_generate; ++i) {
        if (i > 0) {
            int delta = dist(rng);
            current_number += delta;
        }

        {
            std::lock_guard<std::mutex> lock(m);
            q.push(static_cast<int>(current_number));
        }
        cv.notify_one();

        std::cout << "[PRODUCER] Generating: " << current_number << std::endl;
    }

    {
        std::lock_guard<std::mutex> lock(m);
        q.push(0);
    }
    cv.notify_one();

    std::cout << "[PRODUCER] Sent termination signal (0) and exiting." << std::endl;
}

void consumer(std::queue<int>& q, std::mutex& m, std::condition_variable& cv) {
    std::cout << "[CONSUMER] Waiting for numbers." << std::endl;
    while (true) {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&q] { return !q.empty(); });

        int value = q.front();
        q.pop();
        lock.unlock();

        if (value == 0) {
            std::cout << "[CONSUMER] Received termination signal (0). Exiting." << std::endl;
            break;
        }

        bool prime = isPrime(value);
        std::cout << "[CONSUMER] Received: " << value << " -> Is it prime? "
                  << (prime ? "Yes" : "No") << std::endl;
    }
}

int main() {
    int numbers_to_generate = 0;
    std::cout << "Enter the number of integers to generate: ";
    if (!(std::cin >> numbers_to_generate) || numbers_to_generate < 0) {
        std::cerr << "Invalid number. Exiting." << std::endl;
        return 1;
    }

    std::queue<int> q;
    std::mutex m;
    std::condition_variable cv;

    std::thread consumer_thread(consumer, std::ref(q), std::ref(m), std::ref(cv));
    std::thread producer_thread(producer, std::ref(q), std::ref(m), std::ref(cv), numbers_to_generate);

    producer_thread.join();
    consumer_thread.join();

    return 0;
}