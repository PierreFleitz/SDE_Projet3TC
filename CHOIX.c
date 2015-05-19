#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h> 
#include <sys/ipc.h> 
#include <time.h>
#include <signal.h>
#include <string.h>

#include "semaphore.h"
#include "shmem.h"

#define TAILLE_MESSAGE 64

// Clés pour les sémaphores 100
// Clés pour la file de message 200
// Clés pour la mémoire partagée 300	
//mtype = 1 -> type de message qu'envoie CHOIX
//mtype = 2 -> type de message qu'envoie CODE

int idS1;
int idS2;
int idS3;
int idS4;
int idMsg;
int retour;

key_t key = 200; // Clé de la file de messages

struct msgbuf {
		long mtype; // type de message
		int numP; // numéro de la phrase
		int numL; // numéro de la lettre
		char mtext[TAILLE_MESSAGE]; //Contenu du message
}msg_buf;


void quitter(){
	printf("Opération de codage terminée. \n");
	exit(1);
}

int main(){
	
	signal(SIGINT,quitter);
	
	// On s'attache à S1 sémaphore propre à CODE

	if((idS1=open_semaphore(101)) == -1){
		printf("Impossible de s'attacher à S1. \n");
	}
	
	// On s'attache à S2
	
	if((idS2=open_semaphore(102)) == -1){
		printf("Impossible de s'attacher à S2. \n");
	}
	
	// On s'attache à S3
	
	if((idS3=open_semaphore(103)) == -1){
		printf("Impossible de s'attacher à S3. \n");
	}
	
	// On s'attache à S4 sémpahore propre à CHOIX
	
	if((idS4=open_semaphore(104)) == -1){
		printf("Impossible de s'attacher à S4. \n");
	}
	
	// On s'attache à la file de message
	
	if((idMsg=msgget(key,0666))==-1){
		printf("Impossible de s'attacher à la file de message. \n");
	}
	
	/*******************************************************************
							Actions de CHOIX
	*******************************************************************/

	int numPhrase;
	int numLettre;
	srand(time(NULL));
	while(1){
		
		numPhrase = getVal(idS1);
		numLettre = (rand()%26) +1;
		printf("***** Choix de la lettre ***** \n");
		printf("Le numLettre choisi %d. \n",numLettre);
		
		init_semaphore(idS2,numLettre);
		
		// Signal à CODE qu'il peut continuer sa fonction
		
		up(idS3);
		
		
		down(idS4);
		
		// Envoi numLettre et numPhrase dans la file de message
		
		msg_buf.mtype=1;
		msg_buf.numL=numLettre;
		msg_buf.numP=numPhrase+1;
		retour = msgsnd(idMsg,&msg_buf,sizeof(struct msgbuf)-4,0);
		   
		// On vérifie que l'envoi s'est bien déroulé
		
		if (retour ==-1){
			perror("Pb d'envoi du message retour.");
			exit(1);
		}
		
		signal(SIGINT,quitter);
	}
		
	
}
