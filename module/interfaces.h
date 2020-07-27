
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
