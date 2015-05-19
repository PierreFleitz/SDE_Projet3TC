#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <signal.h>
#include <string.h>
#include <sys/sem.h>

#include "semaphore.h"
#include "shmem.h"

#define TAILLE_MESSAGE 64

// Clés pour les sémaphores 100
// Clés pour la file de message 200
// Clés pour la mémoire partagée 300	
//mtype = 1 -> type de message qu'envoie CHOIX
//mtype = 2 -> type de message qu'envoie CODE

int idShmem;
int idMsg;
int idS5;
int idS6;
int numPhrase;
int retour;
char* buffer;

key_t key = 200; // Clé de la file de message
key_t keyShmem = 301; // Clé de la mémoire partagée
key_t keyS5 = 105; // Clé de S5
key_t keyS6 = 106; // Clé de S6

struct msgbuf{
	long mtype;
	int numP;
	int numL;
	char mtext[TAILLE_MESSAGE];
}msg_buf;

void quitter(){
	printf("Opération de codage terminée. \n");
	detach_shmem((void*)buffer);
	remove_shmem(idShmem);
	remove_semaphore(idS5);
	remove_semaphore(idS6);
	exit(1);
}

int main(){
	signal(SIGINT,quitter);
	
	char* tab;
	tab = (char*)malloc(64*sizeof(char));
	
	/*******************************************************************
					Créer la mémoire partagée Shmem
	*******************************************************************/

	if ((idShmem = create_shmem(301,sizeof(int))) == -1){
		printf("Impossible de créer la mémoire partagée \n");
		exit(1);
	}
	
	// On alloue de la mémoire dans la mémoire partagée

	buffer = (char*) attach_shmem(idShmem);
	*buffer = 0;
	
	/*******************************************************************
							Créer le sémaphore S5
	*******************************************************************/

	if ((idS5=create_semaphore(105)) == -1){
		printf("Erreur création de S5 \n");
		exit(1);
	}
	init_semaphore(idS5,0); // On initialise la sémaphore S5
	
	/*******************************************************************
							Créer le sémaphore S6
	*******************************************************************/


	if ((idS6=create_semaphore(106)) == -1){
		printf("Erreur création de S6 \n");
		exit(1);
	}
	init_semaphore(idS6,1); // On initialise la sémaphore S6
	
	// On s'attache à la file de message

	if((idMsg=msgget(200,0666))==-1){
		printf("Impossible de s'attacher à la file de message. \n");
	}
	
	/*******************************************************************
							Actions de RECEPTION
	*******************************************************************/

	while(1){
		
		int i=0;	
		msgrcv(idMsg,&msg_buf, sizeof(struct msgbuf),2,0);

		numPhrase = msg_buf.numP;
		
		strcpy(tab,msg_buf.mtext);

		printf("Le message reçu est: %s et le numéro de phrase est %d \n",tab,numPhrase);
		printf("***** Envoi de la phrase codée dans la mémoire partagée ***** \n");
		
		down(idS6);

		for (i=0; i < 64 ; i++){

			if (i < strlen(tab)){
				buffer[i]=tab[i];
			}
			if (i >= strlen(tab)){
				buffer[i]=0;
			}

		}

		up(idS5);



		signal(SIGINT, quitter);
	}
	free(tab);
}
		
