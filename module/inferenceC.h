
/* Arquivo Arquivo desenvolvido para implementar a tabela de tipos */
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

//Estrutura base do nó.
struct identifier {
    char id[35];
    char compName[35];
    int type;
    int returnOrSize;
    int param[10];
    struct identifier *pLink;
};
//Definição de um tipo Identificador
typedef struct identifier Identifier;

//Nó raiz
Identifier* raiz = NULL; 

//Esta função insere identificadores e seus tipos em uma lista encadeada.
void setState(char iden[35], int paramT[10]) {
    // Novo nó na lista  
    Identifier* novoIdentifier = (Identifier*) malloc(sizeof(Identifier));

    // Inserção de dados no novo nó
    strcpy(novoIdentifier->id,iden); // Identificador
    novoIdentifier->type = 7; // 7: tipo State de acordo com o enumerado
    
    for(int cont=0; cont<10; cont++){   // Laço para inserção da lista de parametros
        if(paramT[cont]!=0){
            novoIdentifier->param[cont]=paramT[cont];
        } else {
            cont = 10;
        }
    }
    novoIdentifier->pLink = NULL;

    // Inserção do novo nó na lista, seguindo o estilho PILHA
    if(raiz != NULL) {                  // Se a raiz não estiver vazia
        novoIdentifier->pLink = raiz;   // O link do novo dó passa a ser a raiz 
    }
    raiz = novoIdentifier;              // O novo nó passa a ser a raiz
    //printf("Identifier: %s -> tipo: %d\n", iden, tipo);
}

//Esta função insere identificadores e seus tipos em uma lista encadeada.
void setId(char iden[35], int tipo) {
    // Novo nó na lista  
    Identifier* novoIdentifier = (Identifier*) malloc(sizeof(Identifier));

    // Inserção de dados no novo nó
    strcpy(novoIdentifier->id,iden);
    if(tipo == 0) tipo = 404;
    novoIdentifier->type = tipo;
    novoIdentifier->pLink = NULL;

    // Inserção do novo nó na lista, seguindo o estilho PILHA
    if(raiz != NULL) {                  // Se a raiz não estiver vazia
        novoIdentifier->pLink = raiz;   // O link do novo dó passa a ser a raiz 
    }
    raiz = novoIdentifier;              // O novo nó passa a ser a raiz
    //printf("Identifier: %s -> tipo: %d\n", iden, tipo);
}


//Esta função insere uma lista na tabela de simbolos
void setList(char iden[35], int tipo, int tam) {

    // Novo nó na lista  
    Identifier* novoIdentifier = (Identifier*) malloc(sizeof(Identifier));

    // Inserção de dados no novo nó
    strcpy(novoIdentifier->id,iden);

    novoIdentifier->type = tipo;

    novoIdentifier->returnOrSize = tam;
    novoIdentifier->pLink = NULL;

    // Inserção do novo nó na lista, seguindo o estilho PILHA
    if(raiz != NULL) {                  // Se a raiz não estiver vazia
        novoIdentifier->pLink = raiz;   // O link do novo dó passa a ser a raiz 
    }
    raiz = novoIdentifier;              // O novo nó passa a ser a raiz
    //printf("Identifier: %s -> tipo: %d\n", iden, tipo);
}


// Esta função recebe um identificador e retorna seu tipo
int getIdentInt(char ident[35], int x) {
    
    //printf("Identificador: %s ", ident);
    int tipo = 404;   // Inicialização do retorno
    // Novo nó para percorrer a lista com o valor da raiz
    Identifier* ptr = (Identifier*) malloc(sizeof(Identifier)); 
    ptr = raiz;                                          

    while (ptr != NULL) {                                       // Enquanto o novo nó for diferente de NULL faça:
        if(strcmp(ptr->id, ident) == 0 ) {   // Se o id do nó for igual ao identificador buscado
            tipo = ptr->type;                                       // A variavel tipo recebe o tipo no nó
            if(x==3){
                tipo = ptr->returnOrSize;
                //printf("\nRealmente o tamanho é: %d", tipo);
            }
            ptr = NULL;                                             // O nó passa a ser NULL e encerra o laço While
        } else {                                                // Se não 
            ptr = ptr->pLink;                                       // O nó recebe o valor do próximo nó na lista
        }
    }
    //  printf("tipo: %d\n", tipo);
    return tipo;    // Retorna o tipo do identificador buscado
}

// Esta função recebe um tipo e retorna seu identificador
char * getIdent(int x) {

    char *resIdent;     // Inicialização do retorno
    resIdent=malloc(35*sizeof(char));

    // Novo nó para percorrer a lista com o valor da raiz
    Identifier* ptr = (Identifier*) malloc(sizeof(Identifier));
    ptr = raiz;

    while (ptr != NULL) {                   // Enquanto o novo nó for diferente de NULL faça:
        if(ptr->type == x) {                // Se o tipo do nó for igual ao tipo buscado
            strcpy(resIdent,ptr->id);           // A variavel resIdent recebe o identificador do nó
            ptr = NULL;                         // O nó passa a ser NULL e encerra o laço While
        } else {                            // Se não
            ptr = ptr->pLink;                   // O nó recebe o valor do próximo nó na lista
        }
    }
    return resIdent;    // retorna o identificador do tipo buscado
}

// Esta função insere um tipo de mensagem e seu pacote na tabela de simbolos
char * getTypeMessage(char namePct[]){
        
    char *resIdent;     // Inicialização do retorno
    resIdent=malloc(35*sizeof(char));

    // Novo nó para percorrer a lista com o valor da raiz
    Identifier* ptr = (Identifier*) malloc(sizeof(Identifier));
    ptr = raiz;

    while (ptr != NULL) {                   // Enquanto o novo nó for diferente de NULL faça:
        if(strcmp(namePct, ptr->compName) == 0) {                // Se o tipo do nó for igual ao tipo buscado
            strcpy(resIdent,ptr->id);           // A variavel resIdent recebe o identificador do nó
            ptr = NULL;                         // O nó passa a ser NULL e encerra o laço While
        } else {                            // Se não
            ptr = ptr->pLink;                   // O nó recebe o valor do próximo nó na lista
        }
    }
    return resIdent;    // retorna o identificador do tipo buscado   
}

//  MANUSEIO DAS MENSAGENS

//Abaixo estão os métods que implementam os STRUCTS das mensagens = Complements.h

char tiposMessage[8][30]; 	/* Vetor para armazenamento dos structs de tipos de msg*/
char pacotesMsg[8][30]; 	/* Vetor para armazenamento dos pacotes de mensagens */
int contMess;

// Esta função insere um tipo de mensagem e seu pacote na tabela de simbolos
void setMessage(char tipoMessage[], char namePct[]){

    strcpy(tiposMessage[contMess++], tipoMessage);
    strcpy(pacotesMsg[contMess], namePct);
  
    // Novo nó na lista  
    Identifier* novoIdentifier = (Identifier*) malloc(sizeof(Identifier));
  
    // Inserção de dados no novo nó
    strcpy(novoIdentifier->compName, namePct);
    strcpy(novoIdentifier->id, tipoMessage);
    novoIdentifier->pLink = NULL;

    // Inserção do novo nó na lista, seguindo o estilho PILHA
    novoIdentifier->pLink = raiz;   // O link do novo dó passa a ser a raiz 
    raiz = novoIdentifier;              // O novo nó passa a ser a raiz
}

/* Método que imprime os STRUCTS de cada tipo de mensagem */
void printExpMsg(FILE *arqMsg, char msg[35], char expressao[50][200], int aux){
    
    fprintf(arqMsg,"\ntypedef nx_struct %s{\n", msg);
	for(int i=0; i<aux; i++) {
		fprintf(arqMsg,"%s",expressao[i]);
	}
    fprintf(arqMsg,"\n} %s;\n ", msg);
}

