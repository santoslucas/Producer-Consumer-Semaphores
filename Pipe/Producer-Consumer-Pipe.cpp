#include <bits/stdc++.h>
using namespace std;

#define MSG_SIZE 20 // 20 bytes

int fileDesc[2];

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

void producer(int numberToGenerate) {
    string buffer;
    int lastNum = -1;
    int write_pipe = fileDesc[1];
    srand(time(0));

    for (int i = 0; i < numberToGenerate; i++) {
        int delta = (rand() % 100) + 1, newNum;

        if (lastNum == -1) {
            lastNum = delta;
        } else {
            newNum = lastNum + delta;
            lastNum = newNum;
        }

        buffer = to_string(lastNum);
        buffer.resize(MSG_SIZE, ' ');  
        write(write_pipe, buffer.c_str(), MSG_SIZE);

        cout << "Produced: " << buffer << endl;
        
        if (delta % 2 == 0) {
            usleep(100000); // 0.1 seconds
        }
    }

    buffer = to_string(0);
    buffer.resize(MSG_SIZE, ' ');  
    write(write_pipe, buffer.c_str(), MSG_SIZE);
    cout << "Produced: end" << endl;

    close(write_pipe);
}

void consumer() {
    int read_pipe = fileDesc[0];
    string buffer;
    buffer.resize(MSG_SIZE);

    while (true) {
        ssize_t bytesRead = read(read_pipe, &buffer[0], MSG_SIZE);
        if (bytesRead <= 0) break;

        buffer.resize(bytesRead);

        int num = atoi(buffer.c_str());
        if (num == 0) {
            cout << "Consumed: end" << endl;
            break;
        }

        cout << "Consumed: " << num
             << (isPrime(num) ? " (prime)" : " (not prime)")
             << endl;

        buffer.resize(MSG_SIZE);
    }

    close(read_pipe);
}


int main() {
    int numberToGenerate;
    cout << "Quantos numeros deseja gerar? ";
    cin >> numberToGenerate;

    pipe(fileDesc);

    pid_t pid = fork();

    if (pid > 0) { // producer
        close(fileDesc[0]); // close read end
        producer(numberToGenerate);
    } else { // consumer
        close(fileDesc[1]); //close write end
        consumer();
    }

    return 0;
}