////////////////////// new
#include <stdio.h>
#include <stdlib.h>    
#include <unistd.h>     
#include <sys/types.h>  
#include <pthread.h>
#include <semaphore.h>  
#include <sys/wait.h>
#include <string.h>
////////////////////// new

#define N 4
#define BUFFER_SIZE 2

sem_t mutex, next;

////////////////////// new
int mut, nex, nfu, nem;
int cou = 0;
////////////////////// new

int next_count = 0;

struct condition {
	sem_t sem;
	int count;
};


////////////////////// new
void *child(void* id_void);
void cwait(struct condition *c, char who[], char sem[]);
void cpost(struct condition *c, char who[], char sem[]);
void monitor_produce(int item, int index);
void monitor_consume();
void monitor_enter(char who[]);
void monitor_exit(char who[]);
void semprint(sem_t s1, sem_t s2, sem_t s3, sem_t s4, char who[], char what[], char how[]);

char par[] = "Parent";
char chi[] = "Child";
char what[] = "initial";
////////////////////// new



struct condition notfull, notempty;
int buffer[BUFFER_SIZE];
int count = 0; // Items in buffer
int out = 0; // Index for reading

int main(int argc, char *argv[]) {
	notfull.count = 0; 
	notempty.count = 0;

	// Initilization code removed (see P3)

	////////////////////// new
	sem_init(&mutex, 0, 1);
	sem_init(&next, 0, 0);
	sem_init(&(notfull.sem), 0, 0);
	sem_init(&(notempty.sem), 0, 0);


	semprint(mutex, next, notfull.sem, notempty.sem, "Initial", "Initial", "Initial"); ///// print
	////////////////////// new


	pthread_t tid1;
	int id = 1;
	pthread_create(&tid1, NULL, child, &id);
	int data[] = {10, 20, 30, 40};
	int i, in = 0; // Index for writing
	for(i = 0; i < N; i++) {
		//printf("pppppppppppp %i\n",i);
		monitor_produce(data[i], in);
		in = (in + 1) % BUFFER_SIZE;

	}
	// Join and cleanup omitted
}

void *child(void* id_void) {
	sleep(2); // Delay child start for 2 sec

	//printf("child awake\n");
	int *id = id_void;
	int i;
	for(i = 0; i < N/2; i++) {
		//printf("ccccccccccc %i\n",i);
		monitor_consume(); // CORRECTED
	}
	pthread_exit(0);
}

////////////////////// Modified
void cwait(struct condition *c, char who[], char sem[]) {
////////////////////// Modified

	c->count++;
	if (next_count > 0) {
		sem_post(&next);

		strcpy(what, "next");////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "increase"); ///// print
	}
	else{
		sem_post(&mutex);

		strcpy(what, "mutex");////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "increase"); ///// print
	}
	sem_wait(&(c->sem));

	strcpy(what, sem);////////////////////////////////////////////////////
	semprint(mutex, next, notfull.sem, notempty.sem, who, what, "decrease"); ///// print

	c->count--;
}

////////////////////// Modified
void cpost(struct condition *c, char who[], char sem[]) {
////////////////////// Modified

	if (c->count > 0) {
		next_count++;
		sem_post(&(c->sem));


		strcpy(what, sem);////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "increase"); ///// print

		sem_wait(&next);

		strcpy(what, "next");////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "decrease"); ///// print

		next_count--;
	}
}

void monitor_produce(int item, int index) {
	//printf("count %i\n", count);
	monitor_enter(par);
	if (count == BUFFER_SIZE) {
		//printf("parent enter cwait\n");
		cwait(&notfull, par, "notfull");
	}
	buffer[index] = item;
	printf("Parent: %d\n", item);
	count++;


	cpost(&notempty, par, "notempty");
	
	monitor_exit(par);

}

void monitor_consume() {
	//printf("count %i\n", count);
	monitor_enter(chi);
	if (count == 0) {
		printf("child enter cwait\n");

		cwait(&notempty, chi, "notempty");

	}
	int data = buffer[out];
	printf("Child: %d\n", data);
	count--;
	cpost(&notfull, chi, "notfull");
	out = (out + 1) % BUFFER_SIZE;
	monitor_exit(chi);

}

void monitor_enter(char who[]) {
	sem_wait(&mutex);

	strcpy(what, "mutex");////////////////////////////////////////////////////
	semprint(mutex, next, notfull.sem, notempty.sem, who, what, "decrease"); ///// print

}

void monitor_exit(char who[]) {
	if (next_count > 0) {
		sem_post(&next);

		strcpy(what, "next");////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "increase"); ///// print

	}
	else {
		sem_post(&mutex);

		strcpy(what, "mutex");////////////////////////////////////////////////////
		semprint(mutex, next, notfull.sem, notempty.sem, who, what, "increase"); ///// print
		
	}
}

void semprint(sem_t s1, sem_t s2, sem_t s3, sem_t s4, char who[], char what[], char how[]) {
	cou++;
	sem_getvalue(&s1, &mut); sem_getvalue(&s2, &nex); sem_getvalue(&s3, &nfu); sem_getvalue(&s4, &nem);
	printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s	%s	%s\n", cou, mut, nex, nfu, nem, who, what, how);
}