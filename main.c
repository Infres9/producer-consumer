
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
#define SEM_DISPLAY 3 //Using a mutex for the display
#define BUFFER_SIZE 5//Buffer can contain 3 items max

#define MAX_BUFFER 1024

/**
 * 
 * Buffer must be mute-locked before calling this function
 * 
 * @param mem the shared memory
 */
void buffer_print(shared_mem mem, int sem_set_id)
{
	std_sem_get(sem_set_id, SEM_DISPLAY);
	printf("-------------------------------------\n");
	int size = mem.size;
	printf("Buffer  (%d): ", size);
	for (int i = 0; i < size - 1; i++) {
		printf("| %c ", mem.address[i]);
	}
	printf("| %d |\n", (int) mem.address[size - 1]);
	printf("-------------------------------------\n");
	std_sem_post(sem_set_id, SEM_DISPLAY);
}

/**
 * Done before multi-process => no need to lock
 * @param mem
 */
void buffer_init(shared_mem mem)
{
	mem.address[mem.size - 1] = 0;
	for (int i = 0; i < mem.size - 1; i++) {
		mem.address[i] = '\0';
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
	c = mem.address[0];
	int index = mem.address[mem.size - 1];
	for (int i = 1; i < index; i++) {
		mem.address[i - 1] = mem.address[i];
	}
	mem.address[index - 1] = '\0';
	mem.address[mem.size - 1]--;
	buffer_print(mem, sem_set_id);
	std_sem_post(sem_set_id, SEM_MUTEX);
	return c;
}

void buffer_write(shared_mem mem, int sem_set_id, char c)
{
	//lock buffer
	std_sem_get(sem_set_id, SEM_MUTEX);
	int index = mem.address[mem.size - 1];
	mem.address[index] = c;
	mem.address[mem.size - 1]++;
	buffer_print(mem, sem_set_id);
	std_sem_post(sem_set_id, SEM_MUTEX);
}

void producer(shared_mem mem, int sem_set_id)
{
	char *str = "Hello This is a long string"; //str to "produce" to the customer

	for (int i = 0; str[i] != '\0'; i++) {
		char c = str[i];
		std_sem_get(sem_set_id, SEM_EMPTYCOUNT);
		buffer_write(mem, sem_set_id, c);
		
		std_sem_get(sem_set_id, SEM_DISPLAY);
		printf("------------------------------\n");
		printf("Producer : %.*s\n", (int) (strlen(str) - i), str + i);
		printf("------------------------------\n");
		std_sem_post(sem_set_id, SEM_DISPLAY);

		std_sem_post(sem_set_id, SEM_FILLCOUNT);
	}
	std_sem_get(sem_set_id, SEM_EMPTYCOUNT);
	buffer_write(mem, sem_set_id, '\0');
	std_sem_post(sem_set_id, SEM_FILLCOUNT);

}

void consumer(shared_mem mem, int sem_set_id)
{
	char *total = "";
	char read;
	do {
		std_sem_get(sem_set_id, SEM_FILLCOUNT);
		read = buffer_read(mem, sem_set_id);
		total = std_concat(total, read);
		
		std_sem_get(sem_set_id, SEM_DISPLAY);
		printf("------------------------------\n");
		printf("Consumer : %s\n", total);
		printf("------------------------------\n");
		std_sem_post(sem_set_id, SEM_DISPLAY);
		
		std_sem_post(sem_set_id, SEM_EMPTYCOUNT);
	} while (read != '\0');
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

	shared_mem mem = std_malloc(sizeof(char) * (BUFFER_SIZE + 1));

	buffer_init(mem);

	nsems = 4;

	sem_set_id = std_sem_create(nsems);

	std_sem_set(sem_set_id, SEM_MUTEX, 1); //possible to read at first
	
	std_sem_set(sem_set_id, SEM_DISPLAY, 1); //possible to display at first

	std_sem_set(sem_set_id, SEM_EMPTYCOUNT, BUFFER_SIZE); //Remaining place

	std_sem_print(sem_set_id);

	pid_t pid = fork();

	switch (pid) {
	case -1:
		printf("Error while forking\n");
		return(EXIT_FAILURE);
	case 0:
		producer(mem, sem_set_id);
		exit(EXIT_SUCCESS);
	default:
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