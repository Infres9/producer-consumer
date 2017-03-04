

/* 
 * File:   utils.h
 * Author: azarias
 *
 * Created on 2 mars 2017, 19:49
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

typedef struct shared_mem {
	int descripteur;
	char *address;
	int size;
} shared_mem;


/**
 * Creates a shared memory adress
 * with the given size
 * @param size size of the memory to allocate
 * @return newly created shared_mem
 */
shared_mem std_malloc(size_t size);

/**
 * Frees the memory from
 * shared memory
 * 
 * @param mem
 * @return return value of shmctl
 */
int std_free(shared_mem mem);

/**
 * Used to help debugging,
 * prints informations about the shared memory
 * 
 * @param to_print
 * @return the return value of shmctl
 */
int std_shm_print(shared_mem to_print);

/**
 * Creates a set of nsems
 * semaphore and init the values to 0
 * 
 * @param nsems the number of semaphore to put in the set
 * @return semaphore set id
 */
int std_sem_create(int nsems);


/**
 * Destroys the sem with the given ID
 * 
 * @param sem_id id of semaphore set
 * @param nsems number of semaphore to destroy
 * @return wether the sem was destroyed
 */
int std_sem_destroy(int sem_id, int nsmes);

/**
 * Add a single operation
 * to the semaphore with the given id
 * at the given position
 * 
 * @param sem_id
 * @param sem_num
 * @return 
 */
int std_sem_post(int sem_id, int sem_num);

/**
 * Tries to "get" a semaphore value
 * if there is not enough sem_value
 * will sleep
 * 
 * @param sem_id
 * @param sem_num
 * @return 
 */
int std_sem_get(int sem_id, int sem_num);

/**
 * Sets the number of operations
 * of the given semaphore to value
 * 
 * @param sem_id id of semaphore set
 * @param semno number of sem to change value
 * @param value value to set for the semaphore
 * @return call result
 */
int std_sem_set(int sem_id, int semno, int value);


/**
 * Prints out the value 
 * of the data of the semaphore
 * with the given id
 * 
 * @param sem_id
 * @return call result
 */
int std_sem_print(int sem_id);

/**
 * Add the given char at the end of the string
 * 
 * @param origin the original string
 * @param nw_char the char to add at the end of the string
 * @return the newly create string with the char at the end
 */
char *std_concat(char *origin, char nw_char);

#endif /* UTILS_H */

