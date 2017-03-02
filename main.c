
/* 
 * File:   main.c
 * Author: azarias
 *
 * Created on 2 mars 2017, 19:22
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen */
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include "utils.h"

#define SEM_MUTEX 0
#define SEM_FILLCOUNT 1
#define SEM_EMPTYCOUNT 2
#define BUFFER_SIZE 3//Buffer can contains 3 items max

#define MAX_BUFFER 1024

/**
 * Memory must be locked
 * before reading
 * 
 * @param mem
 * @param sem_set_id
 * @return 
 */
int buffer_index(shared_mem mem)
{
	return *(mem.adress + mem.size - 1);
}

/**
 * Done before multi-process => no need to lock
 * @param mem
 */
void buffer_init(shared_mem mem)
{
	*(mem.adress + mem.size - 1) = 0; // last element = size of queue
	for (int i = 0; i < mem.size - 1; i++) {
		*(mem.adress + i) = '0';
	}
}

/**
 * Critical section
 * @return the last element added to the buffer
 */
char buffer_read(shared_mem mem, int sem_set_id)
{
	char c;
	std_sem_get(sem_set_id, SEM_MUTEX);
	int index = buffer_index(mem);
//	printf("Size (read): %d\n", index);
	c = *(char*) (mem.adress + index - 1);
	*(mem.adress + mem.size - 1) = index - 1;
	std_sem_post(sem_set_id, SEM_MUTEX);
	return c;
}

void buffer_write(shared_mem mem, int sem_set_id, char c)
{
	//lock buffer
	std_sem_get(sem_set_id, SEM_MUTEX);
	int index = buffer_index(mem);
	*(mem.adress + index) = c;
	*(mem.adress + mem.size - 1) = index + 1;
//	printf("Size (write): %d\n", buffer_index(mem));
	std_sem_post(sem_set_id, SEM_MUTEX);
}

void producer(shared_mem mem, int sem_set_id)
{
	char *str = "Hello This is a long string"; //str to "produce" to the customer

	for (int i = 0; str[i] != '\0'; i++) {
		char c = str[i];
		std_sem_get(sem_set_id, SEM_EMPTYCOUNT);
		buffer_write(mem, sem_set_id, c);
		std_sem_post(sem_set_id, SEM_FILLCOUNT);
	}
	//send '\0'
	std_sem_get(sem_set_id, SEM_EMPTYCOUNT);
	buffer_write(mem, sem_set_id, '\0');
	std_sem_post(sem_set_id, SEM_FILLCOUNT);
	
	printf("End of producer\n");

}

void consumer(shared_mem mem, int sem_set_id)
{
	char read;
	do {
		std_sem_get(sem_set_id, SEM_FILLCOUNT);
		read = buffer_read(mem, sem_set_id);
		printf("consummer : %c\n", read);
		std_sem_post(sem_set_id, SEM_EMPTYCOUNT);
	} while (read != '\0');
	printf("end of consummer\n");
}

int main(int argc, char** argv)
{

	/*
	 * 
	 * Id of the semaphore set
	 * The semaphore set contains three semaphores
	 *  - One for the fillCount
	 *  - One for the EmptyCount
	 *  - One to controll access to the shared memory
	 */
	int sem_set_id;

	int nsems;

	shared_mem mem = std_malloc(MAX_BUFFER);
	
	buffer_init(mem);

	nsems = 3;

	sem_set_id = std_sem_create(nsems);

	std_sem_set(sem_set_id, SEM_MUTEX, 1); //possible to read at first

	std_sem_set(sem_set_id, SEM_EMPTYCOUNT, BUFFER_SIZE); //Remaining place

	std_sem_print(sem_set_id);

	pid_t pid = fork();

	switch (pid) {
	case -1:
		printf("Error while forking\n");
		return(EXIT_FAILURE);
	case 0:
		printf("Child process\n");
		producer(mem, sem_set_id);
		exit(EXIT_SUCCESS);
	default:
		printf("Father process\n");
		consumer(mem, sem_set_id);
		int code;
		wait(&code);
		printf("Child terminated with result %d\n", code);
	}


	return(EXIT_SUCCESS);
}

/**
 * Producer-consumer from wikipedia
 semaphore fillCount = 0; // items produced
semaphore emptyCount = BUFFER_SIZE; // remaining space

procedure producer() {
    while (true) {
	item = produceItem();
	down(emptyCount);
	putItemIntoBuffer(item);
	up(fillCount);
    }
}

procedure consumer() {
    while (true) {
	down(fillCount);
	item = removeItemFromBuffer();
	up(emptyCount);
	consumeItem(item);
    }
}
 */