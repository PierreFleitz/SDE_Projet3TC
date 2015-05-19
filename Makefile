all : saisie code choix decode reception

saisie : SAISIE.c
				gcc SAISIE.c -o saisie

code : CODE.c semaphore.h
			gcc CODE.c -o code

choix : CHOIX.c semaphore.h
				gcc CHOIX.c -o choix

decode : DECODE.c shmem.h semaphore.h
				gcc DECODE.c -o decode

reception : RECEPTION.c shmem.h semaphore.h
						gcc RECEPTION.c -o reception

clean :
	rm -rf code rm -rf decode rm -rf saisie rm -rf choix rm -rf TUBE rm -rf reception rm -rf *.o
