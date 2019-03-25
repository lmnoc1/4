////////////////////// new
#include <stdio.h>
#include <stdlib.h>    
#include <unistd.h>     
#include <sys/types.h>  
#include <pthread.h>
#include <semaphore.h>  
#include <sys/wait.h>
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
void cwait(struct condition *c, char who[]);
void cpost(struct condition *c, char who[]);
void monitor_produce(int item, int index);
void monitor_consume();
void monitor_enter(char who[]);
void monitor_exit(char who[]);

char par[] = "Parent";
char chi[] = "Child";
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

	cou++;
	sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
	printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i 	by %s\n", cou, mut, nex, nfu, nem, "Initial");
	////////////////////// new


	pthread_t tid1;
	int id = 1;
	pthread_create(&tid1, NULL, child, &id);
	int data[] = {10, 20, 30, 40};
	int i, in = 0; // Index for writing
	for(i = 0; i < N; i++) {

		monitor_produce(data[i], in);
		in = (in + 1) % BUFFER_SIZE;

	}
	// Join and cleanup omitted
}

void *child(void* id_void) {
	sleep(2); // Delay child start for 2 sec
	int *id = id_void;
	int i;
	for(i = 0; i < N/2; i++) {
		monitor_consume(); // CORRECTED
	}
	pthread_exit(0);
}

////////////////////// Modified
void cwait(struct condition *c, char who[]) {
////////////////////// Modified

	c->count++;
	if (next_count > 0) {
		sem_post(&next);

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new
	}
	else{
		sem_post(&mutex);

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new
	}
	sem_wait(&(c->sem));

	////////////////////// new
	cou++;
	sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
	printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
	////////////////////// new

	c->count--;
}

////////////////////// Modified
void cpost(struct condition *c, char who[]) {
////////////////////// Modified

	if (c->count > 0) {
		next_count++;
		sem_post(&(c->sem));

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new

		sem_wait(&next);

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new

		next_count--;
	}
}

void monitor_produce(int item, int index) {

	monitor_enter(par);
	if (count == BUFFER_SIZE) {
		cwait(&notfull, par);
	}
	buffer[index] = item;
	printf("Parent: %d\n", item);
	count++;
	cpost(&notempty, par);
	monitor_exit(par);

}

void monitor_consume() {

	monitor_enter(chi);
	if (count == 0) {
		cwait(&notempty, chi);
	}
	int data = buffer[out];
	printf("Child: %d\n", data);
	count--;
	cpost(&notfull, chi);
	out = (out + 1) % BUFFER_SIZE;
	monitor_exit(chi);

}

void monitor_enter(char who[]) {
	sem_wait(&mutex);

	////////////////////// new
	cou++;
	sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
	printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
	////////////////////// new

}

void monitor_exit(char who[]) {
	if (next_count > 0) {
		sem_post(&next);

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new
	}
	else {
		sem_post(&mutex);

		////////////////////// new
		cou++;
		sem_getvalue(&mutex, &mut); sem_getvalue(&next, &nex); sem_getvalue(&(notfull.sem), &nfu); sem_getvalue(&(notempty.sem), &nem);
		printf("	(b %i) mutex: %i  next: %i  notfull: %i  notempty: %i	by %s\n", cou, mut, nex, nfu, nem, who);
		////////////////////// new
	}
}