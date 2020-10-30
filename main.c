/*
  Equipo 14:
  - Octavio Garduza
  - Christian Dalma

  Actividad 10 | Concurrencia

  Ejercicio 1

  Implementación en C de un programa que resuelve el siguiente problema
  utilizando semáforos:

  Blancanieves y los siete enanitos viven en una casa donde solo existen
  cuatro sillas, que los enanitos utilizan para comer.

  - Cuando un enanito vuelve de trabajar en la mina, comprueba si hay una
    silla libre para sentarse.

  - Si existe una silla libre, entonces indica a Blancanieves que ya está
    sentado, y espera pacientemente su turno a que le sirvan.

  - Cuando le ha servido, Blancanieves le indica que puede empezar a comer.

  - El enanito come y cuando acaba, deja la silla libre y vuelve a la mina.

  - Blancanieves, cuando no tiene ningún enanito pendiente de
    servirle, se va a dar una vuelta.
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
  if(result)
    printf("Error al crear blanca_nieves");

  for(int i = 1; i <= ENANOS; ++i){
    result = pthread_create(&hilos[i], NULL, enanito, (void*) i);
    if(result)
      printf("Error al crear enanito");
  }

  for(int i = 0; i <= N_HILOS; ++i){
    result = pthread_join(hilos[i], NULL);
    if(result)
      printf("Error al adjuntar los threads");
  }

  sem_destroy(&silla);
  sem_destroy(&turno);

  pthread_exit(NULL);
}

void* enanito(void* arg){
  int id = (int) arg;
  int rndm_t = (random() % 14)+3;

  while(1){
    //Se va a trabajar a la mina
    sleep(rndm_t);
    printf("Enanito %d ha llegado de la mina\n", id);
    //Checar si hay una silla disponible
    sem_wait(&silla);
    printf("Enanito %d se ha sentado en la mesa\n", id);

    //Indica a blanca nieves que se ha sentado
    printf("El enanito %d ha indicado a Blanca Nieves que se ha sentado\n", id);
    pthread_mutex_lock(&mutex);
    esperando++;
    pthread_mutex_unlock(&mutex);
    //Esperar a que blanca nieves sirva
    sem_wait(&turno);
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
      //Tiempo de preparación
      printf("Blanca Nieves está preparando la comida de un enanito más\n");
      sleep(3);
      //Servirle a alguien
      printf("Le he servido a un enanito. Ahora puede empezar a comer\n");
      sem_post(&turno);
      //Indicar que hay un enanito menos esperando
      pthread_mutex_lock(&mutex);
      esperando--;
      pthread_mutex_unlock(&mutex);
    }
    else{
      printf("No hay ningún enanito esperando. Me voy a ir a dar un paseo\n");
      sleep(8);
      printf("Blanca nieves ha regresado de su paseo\n");
    }
  }
}
