#ifndef MY_PTHREAD_T_H
#define MY_PTHREAD_T_H
#define QUEUE_LEVELS 5
#define _GNU_SOURCE

/* include lib header files that you need here: */
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "ucontext.h"//add ucontext

typedef uint my_pthread_t;
typedef uint my_pthread_t;

typedef struct threadControlBlock {
    /* add something here */
    int id; //TID
    ucontext_t uc; 
    void *return_value; 
    //int mutex;
} tcb; 

/* mutex struct definition */
typedef struct my_pthread_mutex_t {
    int mid; //mutex id
    int lock;    //locking mechanism (this is given to user and the lock is used for atomicity)
    int T_hold; //holder of the mutex   
} my_pthread_mutex_t;




/* define your data structures here: */
typedef struct node {
    struct node *next;
    tcb * thread;
    int level;
} node; 

typedef struct queue {
    struct node *head;
    struct node *back;
    int size;
    int level;
    int multiplier;
    int threads_done;
} queue;

typedef struct ready_queue {

queue *queues[QUEUE_LEVELS];

} ready_queue;


typedef struct waitQueues{
    struct node *head;
    struct waitQueues *next;
    int id;
} waitQueues;



typedef struct kilist{
    struct node *begin; 
}kilist;
void myplay();

tcb* dequeue ();
void stop_itime();
void stop_itime();
void sighandler(int sig);
// Feel free to add your own auxiliary data structures


/* Function Declarations: */
void  my_pthread_mutex_search(my_pthread_mutex_t *mutex);
/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg);

/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield();

/* terminate a thread */
void my_pthread_exit(void *value_ptr);

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr);

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex);

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex);

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex);

#endif
