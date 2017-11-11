/*
 * Name: Preston Tighe
 * 5343 Operating Systems
 * Program 5
 *
 * Command: gcc -std=c99 -D _XOPEN_SOURCE -pthread -o threadQueue threadQueue.c -lm && ./threadQueue
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condc, condp = PTHREAD_COND_INITIALIZER;

struct cArray {
    unsigned int *array;
    int offset;
    int size;
};

struct object {
    int thread;
    int n;
    struct cArray *arr;
};

void* produce(void *p){
    srand(time(0));

    struct object *objectPtr = p;

    int n = objectPtr->n;
    int thread = objectPtr->thread;
    unsigned int total = 0;

    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&mutex);
        while (objectPtr->arr->size == 5){
            pthread_cond_wait(&condp, &mutex);
        }
        unsigned int r = rand() % 100;
        objectPtr->arr->array[objectPtr->arr->offset] = r;
        objectPtr->arr->offset++;
        if (objectPtr->arr->offset == 5){
            objectPtr->arr->offset = 0;
        }
        objectPtr->arr->size++;
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);
        printf("\t+ Producer thread %i produced a %u\n", thread, r);
        sleep(rand() % 2);
        total += r;
    }
    printf("Total produced by producer thread %i = %u\n", thread, total);

    pthread_exit((void*)0);
}

void* consume(void *c){
    struct object *objectPtr = c;

    int n = objectPtr->n;
    int thread = objectPtr->thread;
    unsigned int total = 0;

    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&mutex);
        unsigned int c;
        while (objectPtr->arr->size == 0){
            pthread_cond_wait(&condc, &mutex);
        }
        if (objectPtr->arr->offset == 0) {
            c = objectPtr->arr->array[4];
            objectPtr->arr->offset = 4;
        } else {
            objectPtr->arr->offset--;
            c = objectPtr->arr->array[objectPtr->arr->offset];
        }
        objectPtr->arr->size--;
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&mutex);
        printf("\t- Consumer thread %i consumed a %u\n", thread, c);
        sleep(rand()%2);
        total += c;
    }
    printf("Total consumed by consumer thread %i = %u\n", thread, total);

    pthread_exit((void*)0);
}

int main(int argc, char *argv[])
{
    int n;
    if (argc == 2) {
        n = atoi(argv[1]);
    } else {
        n = 10;
    }

    struct cArray cArr;
    cArr.array = (unsigned int*) malloc((sizeof(unsigned int)) * 5);
    cArr.offset = 0;
    cArr.size = 0;

    struct object object1;
    struct object object2;
    struct object object3;
    struct object object4;
    
    object1.thread = 1;
    object2.thread = 2;
    object3.thread = 3;
    object4.thread = 4;
    if (n % 2 != 0) {
        object1.n = n / 2 + 1;
    } else {
        object1.n = n / 2;
    }
    object2.n = n / 2;
    if (n % 2 != 0) {
        object3.n = n / 2 + 1;
    } else {
        object3.n = n / 2;
    }
    object4.n = n / 2;
    object1.arr = &cArr;
    object2.arr = &cArr;
    object3.arr = &cArr;
    object4.arr = &cArr;


    void* return1;
    void* return2;
    void* return3;
    void* return4;

    pthread_t thread1;
    pthread_t thread2;
    pthread_t thread3;
    pthread_t thread4;

    pthread_create(&thread1, NULL, produce, (void *) &object1);
    pthread_create(&thread2, NULL, produce, (void *) &object2);
    pthread_create(&thread3, NULL, consume, (void *) &object3);
    pthread_create(&thread4, NULL, consume, (void *) &object4);

    pthread_join(thread1, &return1);
    pthread_join(thread2, &return2);
    pthread_join(thread3, &return3);
    pthread_join(thread4, &return4);

    free(cArr.array);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);

    pthread_exit((void*)0);
}
