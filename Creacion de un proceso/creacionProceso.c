#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    FILE *archivo = fopen("salida.txt", "w");
    if (archivo == NULL) {
        perror("Error al abrir el archivo salida.txt");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Error al ejecutar fork().\n");
        fclose(archivo);
        return 1;
    } 
    else if (pid == 0) {
        for (int i = 10000; i > 0; i--) {
            printf("Hijo: %d\n", i);
            fprintf(archivo, "Hijo: %d\n", i);
        }
        fclose(archivo);
        exit(0);
    } 
    else {
        for (int i = 1; i <= 10000; i++) {
            printf("Padre: %d\n", i);
            fprintf(archivo, "Padre: %d\n", i);
        }
        wait(NULL);
        fclose(archivo);
    }
    return 0;
}