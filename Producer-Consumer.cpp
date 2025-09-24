// #include <bits/stdc++.h>
#include <iostream>
#include <windows.h>
#include <sys/types.h>
using namespace std;

bool isPrime(int n) {
    
}

int main() {
    int number_to_generate;
    cout << "Quantos numeros deseja gerar? ";
    cin >> number_to_generate;

    int fd[2];
    pipe(fd);

    pid_t pid = fork()
}