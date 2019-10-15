#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    sem_t *sem1 ,*sem2;
    int num_threads,count;
} barrier_t;


// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    b->num_threads=num_threads;
    b->sem1 = sem_open("/sema", 1);
    b->sem2= sem_open("/semb", 0);
}

void barrier(barrier_t *b) {
    sem_wait(b->sem1);
    b->count--;
    if(b->count==0){
        sem_post(b->sem2);
      
    }
        sem_post(b->sem1);
        sem_wait(b->sem2);
        sem_post(b->sem2);
    
    
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    sleep(1);
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}


// run with a single argument indicating the number of
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    int num_threads = atoi(argv[1]);
    pthread_t p[num_threads];
    tinfo_t t[num_threads];
    
    printf("parent: begin\n");
    barrier_init(&b, num_threads);
    
    int i;
    for (i = 0; i < num_threads; i++) {
        t[i].thread_id = i;
        pthread_create(&p[i], NULL, child, &t[i]);
    }
    
    for (i = 0; i < num_threads; i++)
        pthread_join(p[i], NULL);
    sem_close(b.sem1);
    sem_close(b.sem2);
    sem_unlink("/sem1");
    sem_unlink("/sem2");
    printf("parent: end\n");
    return 0;
}

