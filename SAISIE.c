#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define TAILLE_MESSAGE 64

// Clés pour les sémaphores 100
// Clés pour la file de message 200
// Clés pour la mémoire partagée 300	
//mtype = 1 -> type de message qu'envoie CHOIX
//mtype = 2 -> type de message qu'envoie CODE

void quitter(){ //Fonction qui permet de supprimer le tube nommé et de quitter le processus SAISIE
	printf("Opération de codage terminée. \n");
	remove("TUBE");
	exit(1);
}

int main(){
	
	signal(SIGINT,quitter); //Permet de supprimer tube à n'importe quel moment avant de rentrer dans la boucle while
	
	/*******************************************************************
				Création du tube nommé TUBE
	*******************************************************************/
	
	int p = mknod("TUBE", S_IFIFO|0666,0); 
	if (p==-1){ //Vérifie que le tube est créé
		printf("Erreur création TUBE \n");
		exit(1);
	}
	
	p=open("TUBE", O_WRONLY);
	if (p == -1) { //vérifie que le tube est ouvert
		printf("Erreur ouverture TUBE! \n"); 
		exit(1);
	}

	/*******************************************************************
							Actions de SAISIE
	*******************************************************************/
	
	int k=0;
	while(1){
		
		printf("Entrez une phrase à coder: ");
		char phrase[TAILLE_MESSAGE];
		gets(phrase);
		if(strlen(phrase)>64){ //Vérifie que l'on respecte la taille maximum d'une phrase à coder
			printf("Erreur phrase trop longue! Rentrez une phrase moins longue. \n");
		}
		if(strlen(phrase)<64){
			write(p,phrase,TAILLE_MESSAGE);
			printf("Phrase envoyée dans le tube nommé. \n");
		}
		signal(SIGINT,quitter); //Permet de quitter la boucle et de supprimer le tube
	}
	close(p);	
}
