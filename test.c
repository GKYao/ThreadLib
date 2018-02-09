#include "my_pthread_t.h"
#include "ucontext.h"
#include <stdio.h>
#include <sys/time.h>
#define stac 40000
ucontext_t thr, orig,main_context,op_context;
queue run_q;
kilist* kilhim=NULL;//where threads go to be reaped
tcb *current=NULL; //One thread exevuting

//signal handler
int count = 0;
int id=0;
int SYS=0;//IF I am in OS or not used to preven sighandlers from messing up the system
struct itimerval it;
struct sigaction act, oact;


void start_itime(){ //later put up a way to tell how much time using multileveled queue
	//setup Itimer
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

void sighandler(int sig)
{	//must be fixed this is for changing priority
	if(SYS==0){
		SYS=1;
		stop_itime();
		tcb *swap_tcb = (tcb *) malloc(sizeof(tcb));
		swap_tcb=current;
		current=dequeue();
		if(current==NULL){
			current==swap_tcb;
		}else{
			enqueue(swap_tcb);
		}	
		start_itime();
		SYS=0;
		setcontext(&current->uc);
	}else{
		return;
	}    
}


//start Itimer
void my_pthread_exit(void *value_ptr) {
SYS=1;		//do a seperate case for main context in that the whole program will end.
stop_itime();
tcb *nswap_tcb = (tcb *) malloc(sizeof(tcb));// might want to include kill
nswap_tcb=dequeue(); //when kill list is made to put for reapable threads is created, then edit code
if(kilhim==NULL){
kilhim= (kilist *) malloc(sizeof(kilist));
node *temp = (node *) malloc(sizeof(node));
temp->thread=current;
temp->next=NULL;
kilhim->begin=temp;
}else{
node *temp = (node *) malloc(sizeof(node));
temp->thread=current;
temp->next=NULL;
temp->next=kilhim->begin;
kilhim->begin=temp;
}			//do it here
current=nswap_tcb;
start_itime();
SYS=0;
setcontext(&current->uc);
};



/* create a new thread */
int my_pthread_create(my_pthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
//start Itimer
   			//replace with dequeue regarding getting thread, orig is not meant to be permanent.
//get context
	SYS=1;
	if(current==NULL){

	if(getcontext(&main_context)!=0){
		printf("error\n");
		exit(EXIT_FAILURE);
	}
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
	}else{
	stop_itime();
	}

	if(getcontext(&thr)!=0){
	printf("error\n");
	exit(EXIT_FAILURE);
	}
	if(getcontext(&op_context)!=0){
	printf("error\n");
	exit(EXIT_FAILURE);
	}	
	op_context.uc_link=0;	//later change instead of 0 some other type of link to exit
	op_context.uc_stack.ss_sp=(int*)malloc(stac);
	op_context.uc_stack.ss_size=stac;
	op_context.uc_stack.ss_flags=0;
	if(arg==NULL){
	makecontext(&op_context,(void*)(*my_pthread_exit),0);
	}



	thr.uc_link=&op_context;	// some other type of link to exit so no problem is encountered
	thr.uc_stack.ss_sp=(int*)malloc(stac);
	thr.uc_stack.ss_size=stac;
	thr.uc_stack.ss_flags=0;	
	if(arg==NULL){
	makecontext(&thr,(void*)(*function),0);
	}else{
	makecontext(&thr,(void*)(*function),1,arg);
	}
	tcb *new_tcb = (tcb *) malloc(sizeof(tcb));
	new_tcb->id=id;
	*thread=id;
	id=id+1;		//all thar is occuring is a context switch with our new thread.
	new_tcb->uc=thr;
	tcb *temp_tcb = (tcb *) malloc(sizeof(tcb));
	temp_tcb=current;
	enqueue(current);
	current=new_tcb; //now it is that our new tcb is going to run
	start_itime();
	SYS=0;
    swapcontext(&temp_tcb->uc,&current->uc);	
	return 0;
};

 
void myfo(){
    printf("billy\n");
my_pthread_t * th=malloc(sizeof(my_pthread_t));
//my_pthread_create(th,NULL, (void *)(*myplay),NULL);
	my_pthread_yield();
	
//    setcontext(&current->uc);

}

void myplay(){
    printf("bob\n");
my_pthread_t * th=malloc(sizeof(my_pthread_t));
my_pthread_create(th,NULL, (void *)(*myfo),NULL);
	//my_pthread_yield();
	printf("MOMMY\n");
//    setcontext(&current->uc);

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
/*void thread_init(){
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
}*/

int my_pthread_yield() {
	SYS=1;
	stop_itime();
	tcb *tem_tcb = (tcb *) malloc(sizeof(tcb));
	tem_tcb=dequeue();
	//worry about null case next
	if(tem_tcb!=NULL){
	// dont do anything otherwise for now later change priority queue if encountering a yield and no other threads, rerunning not allowed
	enqueue(current);
	current=tem_tcb;
	}
	start_itime();
	SYS=0;
	setcontext(&current->uc);
	return 0;
};

/*int my_pthread_join(my_pthread_t thread, void **value_ptr) {
	
	SYS=1;
	int x=(int)thread;
	if(){
	
	}
	while(ptr!=NULL){
	if(
	ptr->thread->tid=

	}
	SYS=0;
	//figure out quick value point
	return 0;
};*/



int main(){
   // thread_init();
    //printf("%d\n",current->id );
//Createthread
    my_pthread_t * thread=malloc(sizeof(my_pthread_t));
	 
    my_pthread_create(thread,NULL, (void *)(*myplay),NULL);
	my_pthread_yield();
    
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
