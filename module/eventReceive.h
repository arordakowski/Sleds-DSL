
/* Arquivo Arquivo desenvolvido para implementar o evento RECEIVE */
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

char expReceive[50][100]; 	// Vetor para armazenamento das expressoes do Receive 
int lastPosition;		// Salva a ultima posição do vetor

//Esta função insere dados em uma lista encadeada.
void imputReceive(char action[50][200], int aux) {

    // Inserção de valores
    for(int i=0; i<aux; i++) {               // Laço para a inserção de action no receive  
	//printf("%s", expReceive[lastPosition]);
        strcpy(expReceive[lastPosition++], action[i]);
    }
}

/* Método que imprime as expressoes do Receive */
void printReceive(FILE *arq){	
	// Declaração do evento RECEIVE para cada tipo de mensagem
	fprintf(arq,"\n\n// Receive");
	fprintf(arq,"\nevent message_t* Receive.receive(message_t* msg, void* payload, uint8_t len){\n");
	fprintf(arq,"\n\t//O desempacotamento deve ser realizado em cada IF\n");
 
	// Laço com o código das ações tomada ao receber um pacote
        for(int i=0; i<lastPosition; i++){
        	fprintf(arq,"%s", expReceive[i]);     // Imprimindo a Action
	}
    fprintf(arq,"\t}\n\treturn msg;\n}\n");                        // Implementando retorno da msg e encerramento do RECEIVE
}
