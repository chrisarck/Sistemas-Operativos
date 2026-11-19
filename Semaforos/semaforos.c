#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>      
#include <unistd.h>

#define NOMBRE_SEMAFORO "/mi_semaforo_so"

FILE *archivo;
sem_t *semaforo;

void* contar(void* arg) {
    int id_hilo = *(int*)arg;

    for (int i = 1; i <= 10000; i++) {
        if (i == 4000) {
            sem_wait(semaforo);
            printf("\n>>> [HILO %d] ENTRANDO A SECCION CRITICA (4000 - 6000) <<<\n", id_hilo);
            fprintf(archivo, "\n>>> [HILO %d] ENTRANDO A SECCION CRITICA (4000 - 6000) <<<\n", id_hilo);
        }

        printf("[Hilo %d] %d\n", id_hilo, i);
        fprintf(archivo, "[Hilo %d] %d\n", id_hilo, i);

        if (i == 6000) {
            printf(">>> [HILO %d] SALIENDO DE SECCION CRITICA <<<\n\n", id_hilo);
            fprintf(archivo, ">>> [HILO %d] SALIENDO DE SECCION CRITICA <<<\n\n", id_hilo);
            sem_post(semaforo);
        }
    }

    return NULL;
}

int main() {
    archivo = fopen("semaforos.txt", "w");
    if (archivo == NULL) {
        perror("Error al crear semaforos.txt");
        return 1;
    }

    sem_unlink(NOMBRE_SEMAFORO);

    semaforo = sem_open(NOMBRE_SEMAFORO, O_CREAT, 0644, 1);
    if (semaforo == SEM_FAILED) {
        perror("Error al crear el semaforo");
        fclose(archivo);
        return 1;
    }

    pthread_t hilo1, hilo2;
    int id1 = 1, id2 = 2;

    if (pthread_create(&hilo1, NULL, contar, &id1) != 0) {
        perror("Error al crear el hilo 1");
        return 1;
    }
    if (pthread_create(&hilo2, NULL, contar, &id2) != 0) {
        perror("Error al crear el hilo 2");
        return 1;
    }

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);

    fclose(archivo);
    sem_close(semaforo);
    sem_unlink(NOMBRE_SEMAFORO);

    printf("Ejecucion terminada. Revisa el archivo semaforos.txt\n");
    return 0;
}