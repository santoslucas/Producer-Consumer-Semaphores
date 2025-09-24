#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MSG_SIZE 20

int main() {
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid > 0) {
        // PRODUTOR
        close(fd[0]); // fecha a leitura
        srand(time(NULL));

        int N = 1;
        for (int i = 0; i < 10; i++) {  // exemplo: gera 10 números
            int delta = rand() % 100 + 1;
            N += delta;

            char buffer[MSG_SIZE];
            snprintf(buffer, MSG_SIZE, "%d", N);  // converte para string
            write(fd[1], buffer, MSG_SIZE);      // sempre escreve 20 bytes
        }

        // manda 0 para sinalizar o fim
        char buffer[MSG_SIZE];
        snprintf(buffer, MSG_SIZE, "%d", 0);
        write(fd[1], buffer, MSG_SIZE);

        close(fd[1]);
    } 
    else {
        // CONSUMIDOR
        close(fd[1]); // fecha a escrita

        char buffer[MSG_SIZE];
        while (1) {
            read(fd[0], buffer, MSG_SIZE);
            int num = atoi(buffer);

            if (num == 0) break;

            // checa se é primo (exemplo simples)
            int primo = 1;
            for (int i = 2; i * i <= num; i++) {
                if (num % i == 0) {
                    primo = 0;
                    break;
                }
            }

            if (primo) {
                printf("%d é primo\n", num);
            } else {
                printf("%d não é primo\n", num);
            }
        }

        close(fd[0]);
    }

    return 0;
}
