
/* Arquivo responsável pela geração da tradução com base na árvore sintática*/
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

//Estrutura base do nó.
struct saveState {
    char id[35];
    char msgHeader[120];
    char expressao[100][300]; 	/* Vetor para armazenamento temporario das expressoes */
	int lastPosi;
    struct saveState *pLink;
};

//Definição de um tipo Identificador
typedef struct saveState newState;

//Nó raiz
newState* raizState = NULL; 

//Esta função insere identificadores e seus tipos em uma lista encadeada.
void storeState(char iden[35])  {
   
	newState* novoState = (newState*) malloc(sizeof(newState));
	strcpy(novoState->id,iden); // Identificador
	//printf("Novo State: %s \n", novoState->id);
    
	novoState->lastPosi = 0;
    novoState->pLink = NULL;

    // Inserção do novo nó na lista, seguindo o estilho PILHA
    if(raizState != NULL) {                  // Se a raiz não estiver vazia
        novoState->pLink = raizState;   // O link do novo dó passa a ser a raiz 
    }
    raizState = novoState;              // O novo nó passa a ser a raiz
}

int storeExpState(char exp[100][200], int auxExp){
	int last = raizState->lastPosi;
	//printf("LastPosi: last:%d + aux:%d = %d\n", last, auxExp, last+auxExp);    
	for(int cont=0; cont<auxExp; cont++){
    	strcpy(raizState->expressao[last+cont], exp[cont]);
	}
	raizState->lastPosi = last+auxExp;
	//printf("Store: %s com soma%d \n", raizState->id, raizState->lastPosi);
	return 0;
}

void storeMsgHeader(char msg[120]){
	strcpy(raizState->msgHeader, msg);
	//printf("Store msg: %s \n", raizState->msgHeader);
}

void printStates(FILE *arq) {

    // Novo nó para percorrer a lista com o valor da raiz
	newState* ptr = (newState*) malloc(sizeof(newState));
    ptr = raizState;

    while (ptr != NULL) {                   // Enquanto o novo nó for diferente de NULL faça:		
		if(strcmp(ptr->msgHeader,"")!=0){
			int pular=0;
			for(int cont=0; cont<ptr->lastPosi; cont++){
				fprintf(arq,"%s",ptr->expressao[cont]);				
				if(pular == 0 && strcmp(ptr->expressao[cont],"){")==0){
					fprintf(arq,"\n\t%s",ptr->msgHeader);				
					pular = 1;
				}				
			}	
		} else {
			for(int cont=0; cont<ptr->lastPosi; cont++){
				fprintf(arq,"%s",ptr->expressao[cont]);
			}
		}
        ptr = ptr->pLink;                   // O nó recebe o valor do próximo nó na lista
    }
}

