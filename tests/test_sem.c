/*
 * File:   test_sem.c
 * Author: azarias
 *
 * Created on 2 mars 2017, 21:08:07
 */

#include <stdio.h>
#include <stdlib.h>
#include <CUnit/Basic.h>
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

void test_sem()
{
	int sem_set_id = std_sem_create(2); //two semaphores


	CU_ASSERT_NOT_EQUAL(std_sem_post(sem_set_id, 0), -1);
	CU_ASSERT_NOT_EQUAL(std_sem_post(sem_set_id, 1), -1);

	CU_ASSERT_NOT_EQUAL(std_sem_get(sem_set_id, 1), -1);
	CU_ASSERT_NOT_EQUAL(std_sem_get(sem_set_id, 0), -1);

	CU_ASSERT_NOT_EQUAL(std_sem_destroy(sem_set_id, 2), -1);
}

int main()
{
	CU_pSuite pSuite = NULL;

	/* Initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
		return CU_get_error();

	/* Add a suite to the registry */
	pSuite = CU_add_suite("test_sem", init_suite, clean_suite);
	if (NULL == pSuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Add the tests to the suite */
	if ((NULL == CU_add_test(pSuite, "test1", test_sem))) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/* Run all tests using the CUnit Basic interface */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();
	return CU_get_error();
}
