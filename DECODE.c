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

char* buffer;
int idMsg;
int idShmem;
int idS5;
int idS6;
int numPhrase;
int numLettre;
int retour;
key_t key = 200; // Clé de la file de messages
key_t keyShmem = 301; // Clé de la mémoire partagée
key_t keyS5 = 105; // Clé de S5
key_t keyS6 = 106; // Clé de S5

struct msgbuf {
		long mtype; // type de message (>0)
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
	
	FILE* fichier;
	fichier = fopen("SecretDefense.txt","w");
	char* phraseADecoder;
	phraseADecoder = (char*)malloc(64*sizeof(char));
	
	// On s'attache à la file de message
	
	if((idMsg=msgget(200,0666))==-1){
		printf("Impossible de s'attacher à la file de message. \n");
	}
	
	// On s'attache à la mémoire partagée 

	if ((idShmem = open_shmem(301,sizeof(int))) == -1){
		printf("Impossible de s'attacher à la mémoire partagée \n");
	}
	char* buffer = (char*) attach_shmem(idShmem);
	*buffer = 0;
	
	// On s'attache à la sémaphore S5
	
	if ((idS5 = open_semaphore(105))==-1){
		printf("Impossible de s'attacher à la sémaphore S5");
	}
	if ((idS6 = open_semaphore(106))==-1){
		printf("Impossible de s'attacher à la sémaphore S5");
	}

	/*******************************************************************
							Actions de DECODE
	*******************************************************************/

	while(1){

		int i=0;
		int k=0;
		int j=0;

		memset(phraseADecoder, 0, 64*sizeof(char)); //on initialise phraseADecoder à O

		// On va receptionner numLettre et numPhrase envoyés par CHOIX

		msgrcv(idMsg,&msg_buf,sizeof(struct msgbuf),1,0);
		numPhrase = msg_buf.numP;
		numLettre = msg_buf.numL;
		
		printf("Pour la phrase n°%d le numéro de la Lettre est %d \n",numPhrase,numLettre);
		
		// On va ensuite remplir la mémoire allouée dans la Shmem avec la phrase à décoder

		down(idS5);
		for(i=0;i<strlen(buffer);i++){
			phraseADecoder[i]=buffer[i];
		}
		up(idS6);
		printf("Voici la phrase à décoder: %s c'est la phrase n° %d \n",phraseADecoder,numPhrase);

		
		fprintf(fichier,"Phrase n°%d, codée: %s \n",numPhrase,phraseADecoder);

		// On va alors décoder la phrase que l'on écrira dans le fichier SecreDefense

		for(i=0;i<strlen(phraseADecoder);i++){

			phraseADecoder[i] = (char)((int)phraseADecoder[i] - numLettre);
			
			if ((int)phraseADecoder[i]<97){ // Ici on gère le cas où on passe en dessous de la valeur de 'a'
				phraseADecoder[i] = (char)((int)phraseADecoder[i] + 26);
			}


		}

		// Ici on va remplacer par le caractère null tous les caractères différents de la phrase à décoder
		
		for (k=0;k<TAILLE_MESSAGE;k++){
			
			if (((int)phraseADecoder[k]<97 || (int)phraseADecoder[k]>122) && (int)phraseADecoder[k]-26+numLettre != 32){
				phraseADecoder[k] = 0;
			}
			if ( (int)phraseADecoder[k] - 26 + numLettre == 32){ // Ici on remet les espaces en place pour différencier les mots
				phraseADecoder[k] = 32;
			}
		}
		printf("\n");
		printf("***************************************\n");
		printf("    Opération de décodage en cours\n");
		printf("***************************************\n");
		printf("\n");
		printf("Voici la phrase décodée: %s \n", phraseADecoder);
		fprintf(fichier,"Et voici la phrase décodée : %s\n",phraseADecoder); 



        signal(SIGINT,quitter);
	}
free(phraseADecoder);
}
