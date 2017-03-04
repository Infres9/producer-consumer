/* 
 * File:   utils.h
 * Author: azarias
 *
 * Created on 2 mars 2017, 19:49
 */

#include "utils.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h> /* strftime */
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>

#ifdef UTILS_H  

shared_mem std_malloc(size_t size)
{
	shared_mem mem;
	mem.descripteur = shmget(IPC_PRIVATE, size, IPC_CREAT | IPC_EXCL | 0660);
	mem.address = (void*) - 1;
	mem.size = size;

	if (mem.descripteur != -1) {
		mem.address = shmat(mem.descripteur, NULL, 0);
	}

	return mem;
}

int std_free(shared_mem mem)
{
	int ret = shmdt(mem.address);

	if (ret != -1) {
		ret = shmctl(mem.descripteur, IPC_RMID, 0);
	}
	return ret;
}

int std_shm_print(shared_mem to_print)
{
	struct shmid_ds data;
	int ret = shmctl(to_print.descripteur, IPC_STAT, &data);

	if (ret != -1) {
		printf("Segment de memoire partagee de %d octets ", (int) data.shm_segsz);
		printf("cree par le processsus %d.\n", data.shm_cpid);
	}
	return ret;
}

int std_sem_create(int nsems)
{
	int id_sem = semget(IPC_PRIVATE, nsems, 0666 | IPC_CREAT | IPC_EXCL);
	for (int sem_no = 0; sem_no < nsems; sem_no++) {
		semctl(id_sem, sem_no, SETVAL, 0);
	}
	return id_sem;
}

int std_sem_set(int sem_id, int semno, int value)
{
	semctl(sem_id, semno, SETVAL, value);
}

int std_sem_destroy(int sem_id, int nsems)
{
	return semctl(sem_id, nsems, IPC_RMID, 0);
}

int _sem_op(int sem_id, int sem_num, int op)
{
	struct sembuf sem;
	sem.sem_num = sem_num;
	sem.sem_op = op;
	sem.sem_flg = 0;
	return semop(sem_id, &sem, 1);
}

int std_sem_post(int sem_id, int sem_num)
{
	return _sem_op(sem_id, sem_num, 1);
}

int std_sem_get(int sem_id, int sem_num)
{
	return _sem_op(sem_id, sem_num, -1);
}

void _print_val(int sem_id, int semnum)
{
	int op = semctl(sem_id, semnum, GETVAL);
	printf("Sem n°%d = %d\n", semnum, op);
}

int std_sem_print(int sem_id)
{
	struct semid_ds data;
	int ret = semctl(sem_id, 0 /* ignored */, IPC_STAT, &data);
	if (ret != -1) {
		int nsems = (int) data.sem_nsems;
		printf("Number of semaphores %d\n", nsems);
		for (int i = 0; i < nsems; i++) {
			_print_val(sem_id, i);
		}
	}
	return ret;
}

char *std_concat(char* origin, char nw_char)
{
	size_t len = strlen(origin);
	char *str2 = malloc(len + 2);	
	strcpy(str2, origin);
	str2[len] = nw_char;
	str2[len + 1] = '\0';
	return str2;
}

#endif
