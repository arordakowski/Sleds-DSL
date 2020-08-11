
/* Arquivo Arquivo desenvolvido para implementar a tabela de tipos */
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

//Estrutura base do nó.
struct identSend {
    char idMessageType[35];
    char idTypePackage[35];
    char idPckMessage[35];
    struct identSend *pLink;
};
//Definição de um tipo Identificador
typedef struct identSend IDSEND;

//Nó raiz
IDSEND* raizSend = NULL; 

//Esta função insere identificadores e seus tipos em uma lista encadeada.
void setSend(char idMessT[35], char typePck[35], char idPck[35]) {

    int verifica=0;     //Verifica se o pacote já está presente na lista do send
    IDSEND* ptr = (IDSEND*) malloc(sizeof(IDSEND));
    ptr = raizSend;

    while (ptr != NULL && verifica ==0 ) {  // Enquanto o novo nó for diferente de NULL faça:
        if(strcmp(idMessT, ptr->idMessageType)==0){
            verifica = 1;
        }
        ptr = ptr->pLink;                   // O nó recebe o valor do próximo nó na lista
    }

    if(verifica ==0){
        // Novo nó na lista  
        IDSEND* novoIDSEND = (IDSEND*) malloc(sizeof(IDSEND));
        // Inserção de dados no novo nó
        strcpy(novoIDSEND->idMessageType,idMessT); // Identificador
        strcpy(novoIDSEND->idTypePackage,typePck); // Identificador
        strcpy(novoIDSEND->idPckMessage,idPck); // Identificador
        novoIDSEND->pLink = NULL;
        // Inserção do novo nó na lista, seguindo o estilho PILHA
        if(raizSend != NULL) {                  // Se a raiz não estiver vazia
            novoIDSEND->pLink = raizSend;   // O link do novo dó passa a ser a raiz 
        }
        raizSend = novoIDSEND;              // O novo nó passa a ser a raiz
        //printf("\nIDSEND: %s -> tipo: %s  PCT: %s\n", idMessT, typePck, idPck);
    }
}
