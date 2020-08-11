
/* Arquivo Arquivo desenvolvido para implementar a tabela de tipos */
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

FILE *arqInter;

char CompName[20][30];
char CompRealName[20][30];

char arquivo[30];
int contador;


void typeChar(FILE *arq, int x) ;

//Estrutura base do nó.
struct ident
{
    char Component_name[35];  // Identificador do Componente
    char Func_name[35]; 	    // Identificador da função 
    int Type_return;          // Tipo do retorno da função
    int Param_types[10];      // Lista de tipos dos parâmetros da função
    struct ident *pLink;
};
typedef struct ident Iden;

//Nó raiz
Iden* raizIdent = NULL; 

//Nome dos componentes importados
void setCompName(char realName[], char name[]){
  contador++;
  sprintf(CompName[contador], "%s", name);
  sprintf(CompRealName[contador], "%s", realName);
}

void setCommand(char nameComp[], char nameFunc[], int tReturn, int tipos[]){
  
  //printf("%s->%s %d %d\n", nameComp, nameFunc, tReturn, tipos[1]);

  // Novo nó na lista  
  Iden* novoIdentifier = (Iden*) malloc(sizeof(Iden));
  
  // Inserção de dados no novo nó
  strcpy(novoIdentifier->Component_name, nameComp);
  strcpy(novoIdentifier->Func_name, nameFunc);
  novoIdentifier->Type_return = tReturn;
  novoIdentifier->pLink = NULL;

  for( int i=0; i<10; i++){
    if(tipos[i]!=0){
      novoIdentifier->Param_types[i] = tipos[i];
    } else i =10;
  }

  // Inserção do novo nó na lista, seguindo o estilho PILHA
  if(raizIdent != NULL) {                  // Se a raiz não estiver vazia
    novoIdentifier->pLink = raizIdent;   // O link do novo dó passa a ser a raiz 
  }
  raizIdent = novoIdentifier;              // O novo nó passa a ser a raiz
}


// Esta função gera os arquivos de interface dos componentes de biblioteca
void gerarInterfaces(){	
  //printf("\narquivo: %s\nContador:%d\n", CompRealName[1], contComp);
  for(int i=1; i<=20; i++) {
    if(strcmp(CompRealName[i], "")!=0){
      sprintf(arquivo, "./output/%s.nc", CompRealName[i]);
      //printf("\narquivo: %s", arquivo);
      arqInter = fopen(arquivo,"w");
      fprintf(arqInter, "interface %s{", CompRealName[i]); 
      
      // Novo nó para percorrer a lista com o valor da raiz
      Iden* ptr = (Iden*) malloc(sizeof(Iden)); 
      ptr = raizIdent;                                          

      while (ptr != NULL) {                         
        if(strcmp(ptr->Component_name, CompName[i])==0){          

          fprintf(arqInter, "\n\tcommand");
          typeChar(arqInter, ptr->Type_return);
          fprintf(arqInter, " %s (", ptr->Func_name);

          for(int x=0; x<10;x++){
            if(ptr->Param_types[x]!=0){
              typeChar(arqInter, ptr->Param_types[x]);
            } else x=10;
          }
          fprintf(arqInter,");");
        } 
        ptr = ptr->pLink;         // O nó recebe o valor do próximo nó na lista
      }
      fprintf(arqInter, "\n}");
    }
  }
}

void typeChar(FILE *arq, int x) {
  
  switch(x){
    case 0: return;
    case 1: fprintf(arq," int "); return;
    case 2: fprintf(arq," real "); return;
    case 3: fprintf(arq," char "); return;
    case 6: fprintf(arq,"  "); return;
    case 7: fprintf(arq," int "); return;
    case 404: fprintf(arq," unknow "); return;
  }

}


void generateImports(FILE *arq){
  for(int i=1; i<=contador; i++) {
    if(strcmp(CompName[i], CompRealName[i]) == 0){
      fprintf(arq,"\t\tinterface %s;\n", CompRealName[i]);
    } else {
      fprintf(arq,"\t\tinterface %s as %s;\n",CompName[i],CompRealName[i]);
    }
  }
}

//Métodos que geram código especifico do nesC de maneira automática
/* Método que realiza as traduções iniciais */
int contEnum;
char vetEnum[10][20]; 	/* Vetor para armazenamento temporario das expressoes */

void setEnum(char idEnum[35]){
    strcpy(vetEnum[contEnum], idEnum);
    //printf("\nvet[%d] = %s\n", contEnum, idEnum);
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

void initModulo(FILE *arq, char nome[]){

	fprintf(arq,"\n#include \"Complements.h\"\n");
	fprintf(arq,"\nmodule %s @safe(){\n\tuses{\n", nome);
	fprintf(arq,"\t\tinterface Boot;\n\t\tinterface Packet;\n\t\tinterface AMPacket;\n\t\tinterface AMSend;\n");
	fprintf(arq,"\t\tinterface Receive;\n\t\tinterface SplitControl as AMControl;\n\t\tinterface Timer<TMilli> as Timer;\n");
	generateImports(arq);
	fprintf(arq,"\t}\n}\nimplementation {");
	printEnum(arq);
	fprintf(arq,"\n\tmessage_t pkt;\n\tint controlState;\n\tint auxSend;\n");
}

void INIboot(FILE *arq, char StateINI[]){
  fprintf(arq,"\n/* NesC language envents */\n");
  fprintf(arq,"\nevent void Boot.booted() {\n\tcall AMControl.start();\n");
  fprintf(arq,"\tstate_%s();\n}\n", StateINI);
}


void INIstartDone(FILE *arq){
  fprintf(arq,"\nevent void AMControl.startDone(error_t err){\t}\n");
  fprintf(arq,"\nevent void AMControl.stopDone(error_t err){\t}\n");
  fprintf(arq,"\nevent void AMSend.sendDone( message_t* msg, error_t err) {\t}\n");
}

void printExit(FILE *arq){
  fprintf(arq,"\n\tvoid state_exit( ){\n\t\tcall AMControl.stop();\n\t}\n");
}
