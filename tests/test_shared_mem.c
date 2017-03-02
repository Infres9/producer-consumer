/*
 * File:   test_shared_mem.c
 * Author: azarias
 *
 * Created on 2 mars 2017, 20:07:17
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "../utils.h"

/*
 * CUnit Test Suite
 */

int init_suite(void)
{
	return 0;
}

int clean_suite(void)
{
	return 0;
}

void test_mem()
{
	shared_mem mem = std_malloc(20);

	std_shm_print(mem);

	pid_t pid = fork();
	pid_t pid2;
	int state;

	switch (pid) {
	case -1:
		printf("Error while creating child process \n");
		break;
	case 0:
		printf("Child : PID=%d\n", getpid());
		sleep(2); /* Attendre 2 secondes */
		printf("Child : ended\n");
		exit(0);
	default:
		printf("Pere : PID=%d\n", getpid());
		pid2 = wait(&state);
		printf("Fater : Child %d stoped with code %d\n", pid2, state);
		std_free(mem);
		printf("Child: End\n");
	}

	CU_ASSERT(2 * 2 == 4);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* Initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* Add a suite to the registry */
	pSuite = CU_add_suite("test_shared_mem", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Add the tests to the suite */
	if ((NULL == CU_add_test(pSuite, "test1", test_mem))) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
