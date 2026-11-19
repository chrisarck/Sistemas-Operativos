#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define TAM_BUFFER 5
#define ITEMS_POR_PRODUCTOR 10  

int buffer[TAM_BUFFER];
int contador = 0;   
int entrada = 0;    
int salida = 0;    

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t no_lleno = PTHREAD_COND_INITIALIZER;
pthread_cond_t no_vacio = PTHREAD_COND_INITIALIZER;

FILE *archivo;

void* productor(void* arg) {
    int id = *(int*)arg;

    for (int i = 1; i <= ITEMS_POR_PRODUCTOR; i++) {
        int item = (id * 100) + i; 

        pthread_mutex_lock(&mutex);

        while (contador == TAM_BUFFER) {
            printf("[Productor %d] Buffer lleno. Esperando espacio...\n", id);
            fprintf(archivo, "[Productor %d] Buffer lleno. Esperando espacio...\n", id);
            pthread_cond_wait(&no_lleno, &mutex);
        }

        buffer[entrada] = item;
        entrada = (entrada + 1) % TAM_BUFFER;
        contador++;

        printf("[Productor %d] Produjo: %d | Elementos en buffer: %d/%d\n", id, item, contador, TAM_BUFFER);
        fprintf(archivo, "[Productor %d] Produjo: %d | Elementos en buffer: %d/%d\n", id, item, contador, TAM_BUFFER);

        pthread_cond_signal(&no_vacio);
        pthread_mutex_unlock(&mutex);

        usleep(100000); 
    }

    return NULL;
}

void* consumidor(void* arg) {
    int id = *(int*)arg;

    for (int i = 1; i <= ITEMS_POR_PRODUCTOR; i++) {
        pthread_mutex_lock(&mutex);

        while (contador == 0) {
            printf("  [Consumidor %d] Buffer vacio. Esperando datos...\n", id);
            fprintf(archivo, "  [Consumidor %d] Buffer vacio. Esperando datos...\n", id);
            pthread_cond_wait(&no_vacio, &mutex);
        }

        int item = buffer[salida];
        salida = (salida + 1) % TAM_BUFFER;
        contador--;

        printf("  [Consumidor %d] Consumio: %d | Elementos en buffer: %d/%d\n", id, item, contador, TAM_BUFFER);
        fprintf(archivo, "  [Consumidor %d] Consumio: %d | Elementos en buffer: %d/%d\n", id, item, contador, TAM_BUFFER);

        pthread_cond_signal(&no_lleno);
        pthread_mutex_unlock(&mutex);

        usleep(150000); 
    }

    return NULL;
}

int main() {
    archivo = fopen("productor_consumidor.txt", "w");
    if (archivo == NULL) {
        perror("Error al crear productor_consumidor.txt");
        return 1;
    }

    pthread_t prod[2], cons[2];
    int ids[2] = {1, 2};

    printf("=== INICIO PRODUCTOR-CONSUMIDOR (2 Productores, 2 Consumidores) ===\n\n");
    fprintf(archivo, "=== INICIO PRODUCTOR-CONSUMIDOR (2 Productores, 2 Consumidores) ===\n\n");

    for (int i = 0; i < 2; i++) {
        pthread_create(&prod[i], NULL, productor, &ids[i]);
        pthread_create(&cons[i], NULL, consumidor, &ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(prod[i], NULL);
        pthread_join(cons[i], NULL);
    }

    printf("\n=== EJECUCION FINALIZADA CORRECTAMENTE ===\n");
    fprintf(archivo, "\n=== EJECUCION FINALIZADA CORRECTAMENTE ===\n");

    fclose(archivo);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_lleno);
    pthread_cond_destroy(&no_vacio);

    return 0;
}