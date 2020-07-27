
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


// Esta função imprime o métod Send no arquivo em nesC
void printSend(FILE *arq ) {
    
    fprintf(arq, "\nvoid send ( int MsgType, int idMsg, GENERICMESSAGE* pktGeneric, int dest ){\n");
    fprintf(arq, "\tpktGeneric->idMsg = idMsg;\n");
    fprintf(arq, "\tpktGeneric->MsgType = MsgType;\n");
//fprintf(arq, "%s* %s = (%s*)(call Packet.getPayload(&pkt, sizeof(%s)));\n", ptr->idTypePackage, ptr->idPckMessage, ptr->idTypePackage, ptr->idTypePackage);
    fprintf(arq, "\tif(dest==0) {\n\t\tcall AMSend.send(AM_BROADCAST_ADDR, &pktGeneric, sizeof(GENERICMESSAGE));\n\t}");
    fprintf(arq, "else {\n\t\tcall AMSend.send(dest, &pktGeneric, sizeof(GENERICMESSAGE));\n\t}\n}");

}

int contEnum;
char vetEnum[10][20]; 	/* Vetor para armazenamento temporario das expressoes */

void setEnum(char idEnum[35]){
    strcpy(vetEnum[contEnum], idEnum);
    printf("\nvet[%d] = %s\n", contEnum, idEnum);
    contEnum++;
}

void printEnum(FILE *arq){
    fprintf(arq,"\n\tenum Booleans { FALSE = 0, TRUE = 1 };");
    fprintf(arq,"\n\tenum Message_Type { %s = 1", vetEnum[0] );
    for(int x=1; x<contEnum; x++) { 
        fprintf(arq,", %s", vetEnum[x]); 
    }
    fprintf(arq," } IDMSG;"); 
}
