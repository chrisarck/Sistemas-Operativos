#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_FILOSOFOS 5
#define COMIDAS_POR_FILOSOFO 3

pthread_mutex_t tenedores[NUM_FILOSOFOS];
pthread_mutex_t mutex_salida = PTHREAD_MUTEX_INITIALIZER;
FILE *archivo;

void registrar_evento(const char* mensaje) {
    pthread_mutex_lock(&mutex_salida);
    printf("%s", mensaje);
    fprintf(archivo, "%s", mensaje);
    fflush(archivo);
    pthread_mutex_unlock(&mutex_salida);
}

void* filosofo(void* arg) {
    int id = *(int*)arg;
    int tenedor_izq = id;
    int tenedor_der = (id + 1) % NUM_FILOSOFOS;
    char buffer[150];

    for (int comida = 1; comida <= COMIDAS_POR_FILOSOFO; comida++) {
        snprintf(buffer, sizeof(buffer), "[Filosofo %d] Esta pensando...\n", id + 1);
        registrar_evento(buffer);
        usleep(100000); 

        snprintf(buffer, sizeof(buffer), "[Filosofo %d] Tiene hambre e intenta tomar tenedores.\n", id + 1);
        registrar_evento(buffer);

        if (id == NUM_FILOSOFOS - 1) {
            pthread_mutex_lock(&tenedores[tenedor_der]);
            snprintf(buffer, sizeof(buffer), "  [Filosofo %d] Tomo tenedor derecho (%d).\n", id + 1, tenedor_der + 1);
            registrar_evento(buffer);

            pthread_mutex_lock(&tenedores[tenedor_izq]);
            snprintf(buffer, sizeof(buffer), "  [Filosofo %d] Tomo tenedor izquierdo (%d).\n", id + 1, tenedor_izq + 1);
            registrar_evento(buffer);
        } else {
            pthread_mutex_lock(&tenedores[tenedor_izq]);
            snprintf(buffer, sizeof(buffer), "  [Filosofo %d] Tomo tenedor izquierdo (%d).\n", id + 1, tenedor_izq + 1);
            registrar_evento(buffer);

            pthread_mutex_lock(&tenedores[tenedor_der]);
            snprintf(buffer, sizeof(buffer), "  [Filosofo %d] Tomo tenedor derecho (%d).\n", id + 1, tenedor_der + 1);
            registrar_evento(buffer);
        }

        snprintf(buffer, sizeof(buffer), ">>> [Filosofo %d] ESTA COMIENDO (Comida %d/%d) <<<\n", id + 1, comida, COMIDAS_POR_FILOSOFO);
        registrar_evento(buffer);
        usleep(150000); 

        pthread_mutex_unlock(&tenedores[tenedor_izq]);
        pthread_mutex_unlock(&tenedores[tenedor_der]);

        snprintf(buffer, sizeof(buffer), "[Filosofo %d] Termino de comer y solto los tenedores (%d y %d).\n\n", id + 1, tenedor_izq + 1, tenedor_der + 1);
        registrar_evento(buffer);
    }

    snprintf(buffer, sizeof(buffer), "=== [Filosofo %d] HA TERMINADO TODAS SUS COMIDAS ===\n", id + 1);
    registrar_evento(buffer);

    return NULL;
}

int main() {
    archivo = fopen("cena_filosofos.txt", "w");
    if (archivo == NULL) {
        perror("Error al crear cena_filosofos.txt");
        return 1;
    }

    pthread_t hilos[NUM_FILOSOFOS];
    int ids[NUM_FILOSOFOS];

    registrar_evento("=== INICIO DE LA CENA DE LOS FILOSOFOS (5 Filosofos) ===\n\n");

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        ids[i] = i;
        if (pthread_create(&hilos[i], NULL, filosofo, &ids[i]) != 0) {
            perror("Error al crear hilo de filosofo");
            return 1;
        }
    }

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    registrar_evento("\n=== TODOS LOS FILOSOFOS HAN TERMINADO DE CENAR ===\n");

    for (int i = 0; i < NUM_FILOSOFOS; i++) {
        pthread_mutex_destroy(&tenedores[i]);
    }
    pthread_mutex_destroy(&mutex_salida);
    fclose(archivo);

    return 0;
}