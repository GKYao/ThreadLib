#include "my_pthread_t.h"
#include "ucontext.h"
#include <stdio.h>
#include <sys/time.h>
#define stac 40000
ucontext_t thr, orig,main_context;
queue run_q;
tcb *current;
//signal handler
int count = 0;
int id=0;
void sighandler(int sig)
{
     printf("signal occurred %d times\n", ++count);
}


//start Itimer
void start_itime(){
	//setup Itimer
    struct itimerval it;
    struct sigaction act, oact;
    act.sa_handler = sighandler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGPROF, &act, &oact); 
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 25000;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 25000;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 25000;
    it.it_value.tv_sec = 0;
    it.it_value.tv_usec = 25000;
    setitimer(ITIMER_PROF, &it, NULL);
}
//Stop Itimer
void stop_itime(){
	setitimer(ITIMER_PROF, 0, 0);
	
}


/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
//start Itimer
   			//replace with dequeue regarding getting thread, orig is not meant to be permanent.
//get context
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

 void myplay(){
    printf("bob\n");
	my_pthread_yield();
    setcontext(&current->uc);

}

//add a node to the rear of queue
int enqueue (tcb *thread) {
    node *temp = (node *) malloc(sizeof(node));
    if (temp == NULL) {
    return 0;
	    }
	temp->thread= thread;
	temp->next = NULL;
    if (run_q.back) {
        run_q.back->next = temp;
        run_q.back = temp;
        }
    else
        {
        run_q.back= temp;
        run_q.head = temp;
        }
	run_q.size++;

        return 1;
}





//Initial thead queue by managing main context, this must be eliminated effective immediately as we cannot have an init later on.
void thread_init(){
	//initialized queue
	run_q.head = run_q.back = NULL;
	run_q.size = 0;
	//get main context
	tcb *main_tcb = (tcb *) malloc(sizeof(tcb));
    main_tcb->id=id;//(IMPORTANT!!! main ID??????)
	id=id+1;
	if(getcontext(&main_tcb->uc)!=0){
	printf("error\n");
	exit(EXIT_FAILURE);
	}
	//set main as current thread context
    current = main_tcb;
	return;
}

int my_pthread_yield() {
	stop_itime();
	printf("HAHAHA");
	enqueue(current);
	enqueue(current);
	dequeue();
	start_itime();
	setcontext(&orig);
	return 0;
};




int main(){
    thread_init();
    printf("%d\n",current->id );
//Createthread
    my_pthread_t * thread=malloc(sizeof(my_pthread_t));
    my_pthread_create(thread,NULL, (void *)(*myplay),NULL);
    printf("tim tam\n");



    return 0;

}


tcb* dequeue (queue q){
tcb *ret = NULL;
if (run_q.head==NULL){
ret = NULL;
}
else if (run_q.head->next == NULL){
	ret = run_q.head->thread;
	free(run_q.head);
	run_q.head = NULL;
	run_q.back = NULL;
	run_q.size--;
}
else {
node *temp = (node *) malloc(sizeof(node));
 	temp = run_q.head;
 	run_q.head = run_q.head->next;
  	ret = temp->thread;
  	free(temp);
              	run_q.size--;
	}
return ret;
}
