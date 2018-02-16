// File:	my_pthread.c
// Author:	Yujie REN
// Date:	09/23/2017

// name:Benjamin Weiss bsw46,
// username of iLab:cd.cs.rutgers.edu
// iLab Server:cd.cs.rutgers.edu

#include "my_pthread_t.h"
#include "ucontext.h"
#include <sys/time.h>
#include <stdio.h>
#define stac 40000
ucontext_t thr, orig;


/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
//start Itimer
	if(getcontext(&thr)!=0){
	printf("error\n");
	exit(EXIT_FAILURE);
	}
	if(getcontext(&orig)!=0){
	printf("error\n");
	exit(EXIT_FAILURE);
	}
	thr.uc_link=0;
	thr.uc_stack.ss_sp=(int*)malloc(stac);
	thr.uc_stack.ss_size=stac;
	thr.uc_stack.ss_flags=0;	
	if(arg==NULL){
	makecontext(&thr,(void*)(*function),0);
	}else{
	makecontext(&thr,(void*)(*function),1,arg);
	}
		start_itime();
	swapcontext(&orig,&thr);
	return 0;
};

//add a node to the rear of queue


//Initial thead queue by managing main context









/* give CPU pocession to other user level threads voluntarily */
int my_pthread_yield() {
	return 0;
};

/* terminate a thread */
void my_pthread_exit(void *value_ptr) {
};

/* wait for thread termination */
int my_pthread_join(my_pthread_t thread, void **value_ptr) {
	return 0;
};

/* initial the mutex lock */
int my_pthread_mutex_init(my_pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr) {
	return 0;
};

/* aquire the mutex lock */
int my_pthread_mutex_lock(my_pthread_mutex_t *mutex) {
	return 0;
};

/* release the mutex lock */
int my_pthread_mutex_unlock(my_pthread_mutex_t *mutex) {
	return 0;
};

/* destroy the mutex */
int my_pthread_mutex_destroy(my_pthread_mutex_t *mutex) {
	return 0;
};


