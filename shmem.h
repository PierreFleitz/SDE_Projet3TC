
#ifndef __SHMEM_H_SHMEM__
#define __SHMEM_H_SHMEM__


#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>


int create_shmem(int key, int memsize)
{
	return shmget(key, memsize, 0660 | IPC_CREAT) ;
}

int open_shmem(int key, int memsize)
{
	return shmget(key, memsize, 0660) ;
}

int remove_shmem(int id) 
{
	return shmctl(id, IPC_RMID, 0) ;
}

void* attach_shmem(int id)
{
	return shmat(id, 0, 0) ;
}

int detach_shmem(const void* buffer)
{
	return shmdt(buffer)  ;
}


#endif