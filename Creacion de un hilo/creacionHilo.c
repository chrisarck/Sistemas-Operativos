
#include <stdio.h>
#include <pthread.h>

FILE *archivo;

void *funcionHilo(void *arg) {
    int i;
    for (i = 10000; i >= 1; i--) {
        fprintf(archivo, "Hilo secundario: %d\n", i);
    }
    return NULL;
}

int main() {
    pthread_t hilo;
    int i;
    archivo = fopen("creacionHilo.txt", "w");
    if (archivo == NULL) {
        printf("Error al crear el archivo.\n");
        return 1;
    }
    pthread_create(&hilo, NULL, funcionHilo, NULL);
    for (i = 1; i <= 10000; i++) {
        fprintf(archivo, "Proceso principal: %d\n", i);
    }
    pthread_join(hilo, NULL);
    fclose(archivo);
    printf("Programa terminado. Revisa creacionHilo.txt\n");
    return 0;
}
