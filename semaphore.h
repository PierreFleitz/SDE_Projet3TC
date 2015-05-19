
#ifndef __SEMAPHORE_H_SEMAPHORE__
#define __SEMAPHORE_H_SEMAPHORE__


#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>


int create_semaphore(int key)
{
	return semget(key, 1, 0660 | IPC_CREAT) ;
}

int open_semaphore(int key)
{
	return semget(key, 1, 0660) ;
}

int remove_semaphore(int id) 
{
	return semctl(id, 0, IPC_RMID) ;
}

int init_semaphore(int id, int val)
{
	return semctl(id, 0, SETVAL, val) ;
}
int getVal(int id)
{
	return semctl(id, 0, GETVAL);
}

int up(int id)
{
	struct sembuf op ;
	op.sem_num = 0 ;
	op.sem_op = 1 ;
	op.sem_flg = 0 ;
	
	return semop(id, &op, 1) ;
}

int down(int id)
{
	struct sembuf op ;
	op.sem_num = 0 ;
	op.sem_op = -1 ;
	op.sem_flg = 0 ;
	
	return semop(id, &op, 1) ;
}


#endif
