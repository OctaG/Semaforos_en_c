/*
  Equipo 14:
  - Octavio Garduza
  - Christian Dalma

  Actividad 10 | Concurrencia

  Ejercicio 1.


*/

#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>


#define ENANOS 7
#define SILLAS 4
#define N_HILOS ENANOS + 1

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t silla, turno;
int esperando;

void* enanito(void*);
void* blanca_nieves(void*);

int main(int argc, const char * argv[]){

  pthread_t hilos[N_HILOS];
  int result;

  sem_init(&silla, 0, SILLAS);
  sem_init(&turno, 0, 0);

  esperando = 0;

  result = pthread_create(&hilos[0], NULL, blanca_nieves, NULL);

  for(int i = 1; i <= ENANOS; ++i){
    result = pthread_create(&hilos[i], NULL, enanito, (void*) i);
  }

  for(int i = 0; i <= N_HILOS; ++i){
    result = pthread_join(hilos[i], NULL);
  }

  sem_destroy(&silla);
  sem_destroy(&turno);

  pthread_exit(NULL);
}

void* enanito(void* arg){
  int id = (int) arg;
  int mi_turno;
  int sillas_restantes;
  int rndm_t = (random() % 14)+3;

  while(1){
    //Se va a trabajar a la mina
    sleep(rndm_t);

    //Checar si hay una silla disponible
    sem_wait(&silla);
    sem_getvalue(&silla, &sillas_restantes);
    printf("El valor del semáforo silla es: %d", sillas_restantes);
    printf("Enanito %d se ha sentado en la mesa\n", id);

    //Indica a blanca nieves que se ha sentado
    printf("El enanito %d ha indicado a Blanca Nieves que se ha sentado\n", id);
    pthread_mutex_lock(&mutex);
    esperando++;
    pthread_mutex_unlock(&mutex);
    //Esperar a que blanca nieves sirva
    sem_wait(&turno);
    sem_getvalue(&turno, &mi_turno);
    printf("El valor del semáforo turno es: %d", mi_turno);
    //Tiempo que tarda en comer
    printf("El enanito %d está comiendo\n", id);
    sleep(5);
    printf("El enanito %d se ha levantado de la mesa\n", id);
    //Liberar silla
    sem_post(&silla);
  }
}

void* blanca_nieves(void* arg){
  while(1){
    if(esperando > 0){
      printf("Blanca nieves ha regresado de su paseo\n");
      //Tiempo de preparación
      printf("Blanca Nieves está preparando la comida\n");
      sleep(2);
      //Servirle a alguien
      sem_post(&turno);
      //Indicar que hay un enanito menos esperando
      pthread_mutex_lock(&mutex);
      esperando--;
      printf("Le he servido a un enanito. Ahora puede empezar a comer\n");
      pthread_mutex_unlock(&mutex);
    }
    else{
      printf("No hay ningún enanito esperando. Me voy a ir a dar un paseo\n");
      sleep(10);
    }
  }
}
