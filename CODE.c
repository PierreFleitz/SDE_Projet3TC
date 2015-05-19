#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h> 
#include <sys/ipc.h>
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
struct msqid_ds *fdm;
int retour;

void quitter(){
	printf("Opération de codage terminée. \n");
	if (msgctl(idMsg,IPC_RMID,fdm) == -1){
		printf("Erreur suppression file de message \n");
	}
	remove_semaphore(idS1);
	remove_semaphore(idS2);
	remove_semaphore(idS3);
	remove_semaphore(idS4);
	exit(1);
}


int main(){
	
	signal(SIGINT,quitter);
	
	/*******************************************************************
							Créer la file de message
	*******************************************************************/
	
	key_t key = 200; // Clé de la file de messages
	struct msgbuf {
		long mtype; // type de message (>0)
		int numP; // numéro de la phrase
		int numL; // numéro de la lettre
		char mtext[TAILLE_MESSAGE]; //Contenu du message
	}msg_buf;
	
	if((idMsg = msgget(key,0666|IPC_CREAT))==-1){
		printf("Erreur création file de message \n");
		exit(1);
	}

	/*******************************************************************
				Créer le sémaphore S1 < Envoi du numéro de phrase
	*******************************************************************/
	
	key_t keyS1 = 101; // Clé de S1
	
	if ((idS1 = create_semaphore(keyS1)) == -1)
	{
		printf("Erreur création de S1 \n");
		exit(1);
	}
	
	/*******************************************************************
				Créer le sémaphore S2 < Envoi de la lettre 
	*******************************************************************/
	
	key_t keyS2 = 102; // Clé de S2
	
	if ((idS2 = create_semaphore(keyS2)) == -1)
	{
		printf("Erreur création de S2 \n");
		exit(1);
	}

	/*******************************************************************
				Créer le sémaphore S3 propre à CODE
	*******************************************************************/
	
	key_t keyS3 = 103; // Clé de S3
	
	if ((idS3 = create_semaphore(keyS3)) == -1)
	{
		printf("Erreur création de S3 \n");
		exit(1);
	}
	
	init_semaphore(idS3,0); // On initialise S3
	
	/*******************************************************************
				Créer le sémaphore S4 propre à CHOIX
	*******************************************************************/
	
	key_t keyS4 = 104; // Clé de S4
	
	if ((idS4 = create_semaphore(keyS4)) == -1)
	{
		printf("Erreur création de S4 \n");
		exit(1);
	}
	init_semaphore(idS4,0); // On initialise S4

	/*******************************************************************
							Actions de CODE 
	*******************************************************************/
	
	int numPhrase = 0;
	int numLettre = 0;
	int k;
	int l;
	int j;
	char* phraseALire;
	phraseALire = (char*)malloc(64*sizeof(char));
	while(1){
		numPhrase++;
		
		for (j=0;j<TAILLE_MESSAGE;j++){ // On rempli phraseALire avec des caractères null
			phraseALire[j]=0;
		}
		int p=open("TUBE",O_RDONLY);
		if(p==-1){
			printf("erreur ouverture TUBE!\n");
			exit(1);
		}
		
		read(p,phraseALire,TAILLE_MESSAGE);
		
		printf("La phrase à coder est: %s \n",phraseALire);
		printf("Le numéro de cette phrase est: %d \n",numPhrase);
		
		// On initialise S1 avec la valeur de numPhrase 
		
		init_semaphore(idS1,numPhrase);
		
		// On lance la synchronisation CODE / CHOIX
		
		down(idS3); // Ici CODE attend que CHOIX

		// CHOIX a envoyé numLettre dans S2
		
		numLettre = getVal(idS2);
		
		// A partir d'ici on code la phrase
		
		for (l=0;l<TAILLE_MESSAGE;l++){
			phraseALire[l] = (char)((int)phraseALire[l] + numLettre);
			if ((int)phraseALire[l]>122 || (int)phraseALire[l]<97){ // Ici on gère le cas où on dépasse la valeur de 'z' 
				phraseALire[l] = (char)((int)phraseALire[l] - 26);
			}
		}
		
		// Ici on va remplacer par le caractère null tous les caractères différents de la phrase à coder
	
		for (k=0;k<TAILLE_MESSAGE;k++){
			if ((phraseALire[k]<97 || phraseALire[k]>122) && phraseALire[k]+26-numLettre != 32){
				phraseALire[k] = 0;
			}
			if ( phraseALire[k] + 26 - numLettre == 32){ // Ici on remet les espaces en place pour différencier les mots
				phraseALire[k] = 32;
			}
		}
		
		// Comme demander dans le sujet on fait un sleep pour faire dormir 
		
		printf("\n");
		printf("***************************************\n");
		printf("    Opération de codage en cours\n");
		printf("***************************************\n");
		printf("\n");

		sleep(1);
		
		printf("La phrase une fois codée est : %s \n",phraseALire);	
		
		// On envoie la phrase codée et le numPhrase dans la file de message
		
		msg_buf.mtype=2;
		msg_buf.numP=numPhrase;
		sprintf(msg_buf.mtext,"%s",phraseALire);
		retour = msgsnd(idMsg,&msg_buf,sizeof(struct msgbuf)-4,0);
		
		// On vérifie que l'envoi s'est bien déroulé
		
		if (retour ==-1){
			perror("Pb d'envoi du message retour :");
			exit(1);
		}
		
		up(idS4);
		signal(SIGINT,quitter);	 
	}
}	
