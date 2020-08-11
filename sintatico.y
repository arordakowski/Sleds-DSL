%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "module/interfaces.h"
	#include "module/geracode.h"
	#include "module/inferenceC.h"
	#include "module/eventReceive.h"
	#include "module/eventSend.h"
	int yylex();
	int yyerror(char *s);

	FILE *arq;
	FILE *arqMsg;

	int ctrState;
	int aux, auxState, auxTimer, auxRecv, firstRecv;
	int TimerActions, ReceiveActions, LastState;

	//Enumerado de Tipo
	enum TIPOS { tInteiro=1, tReal, tString, tUse, tProgram, tFuncao, tEstado, tEntidade, tExp, Unknown, tEnum, tList};
	
	// Contador para identificar as estruturas das mensagens como TIPOS
	int tMsg = 20;

	char entidade[20];
	char comando[20];
	char firstState[20];
	
	char expressao[100][200]; 	/* Vetor para armazenamento temporario das expressoes */
	char expTimer[50][100]; 	/* Vetor para armazenamento temporario das expressoes do Timer */
	char expReceive[50][100]; 	/* Vetor para armazenamento temporario das expressoes do Receive */
	
	int exprTipos[10];
	int auxTipos;
	int contEnum;

	void eventosNesc();
	void geraArquivoSaida(char nome[20]);

	void INIstartDone(FILE *arq);
	void INIamStop(FILE *arq);
	void INITimer();
	void saveTimer();
	void saveReceive();
	void imprimirExpressao();
	void zerarVetor();
	void openLibrary(char nomeUse[], char nome[]);

	%}

	%union{
		char* word;
		int number_int;
		double number_real;
	}

	%token USE	AS	PROGRAM	STATE	EXIT	SEND	RECV 			//comandos
	%token BROADCAST	NEXT_STATE	DURING	CONST RECV_BROADCAST	//actions
	%token 	INTEGER	REAL	STRING	LIST	MAP	BOOLEAN				//declarações
	%token BACK	FRONT	NEXT	SIZE	INSERT				//funções da lista
	%token IF	ELSE	FOR	WHILE	IN					//repetições e condicionais
	%token 	DUPLO_MAIS DUPLO_MENOS							//booleans
	%token	TRUE_LITERAL FALSE_LITERAL 					//literal de booleans
	%left POINTER '.'								//ponteiros
	%nonassoc DUPLO_IGUAL DIFERENTE '>' '<'	MAIOR_IGUAL	MENOR_IGUAL			//booleans
	%token MESSAGE_TYPE MESSAGE								//mensagem
	
	%token IDENTIFIER
	%type <word> IDENTIFIER

	%token STR_LITERAL
	%type <word> STR_LITERAL

	%token INT_LITERAL
	%type <number_int> INT_LITERAL
	
	%token REAL_LITERAL
	%type <number_real> REAL_LITERAL

	%%

// Regra que indica inicio da árvore sintática
inicio: Use EnumMess Program ;

// Declaração das bibliotecas importadas para a aplicação
Use: 		/* empty */
	|	USE IDENTIFIER AS IDENTIFIER';'	{ setId($2, tUse); openLibrary($2,$4); } Use
	|	USE IDENTIFIER ';' 		{ setId($2, tUse); openLibrary($2,$2); } Use
;

// Regra sintática para declaração do ENUM com os tipos de mensagens (Message Type).
EnumMess: Message
	|	MESSAGE_TYPE  '{' 	EnumMess	
	|	IDENTIFIER ',' 		{ setEnum($1); } EnumMess
	|	IDENTIFIER '}' ';' 	{ setEnum($1); } EnumMess
;

// Regra para a definição da estrutura de uma mensagem
Message: 	/* empty */
	|	MESSAGE IDENTIFIER '{' Var_List '}' ';' 
		{ setId($2, tMsg++); printExpMsg(arqMsg, $2, expressao, aux); aux=0; } Message
;

//Regra para declaração de uma nova mensagem de uma determinada estrutura
IdentMsg:
	|	IDENTIFIER IDENTIFIER ';' {setMessage($1, $2); }
	{ sprintf(expressao[aux++]," "); } //Não serve pra nada, porém misteriosamente da erro se tirar
	IdentMsg
;

// Regra raiz da aplicação (Basicamente: Program, Var_List e StateDef)
Program: PROGRAM IDENTIFIER { geraArquivoSaida($2); aux = 0; } 
		'(' Var_List ')' '{' IdentMsg Const_List Var_List  {imprimirExpressao();}
		StateDef '}' 
		{ printExit(arq); printStates(arq); eventosNesc(); 
		fprintf(arq,"\n}"); printf("\nCódigo em nesC gerado com sucesso!\n");}
;

// Regra para a definição de um estado
StateDef:	/* empty */
	|	STATE  IDENTIFIER	{ storeState($2); sprintf(expressao[aux++],"\n\tvoid state_%s(", $2); } 
		'(' Var_List ')' 	{ setState($2, exprTipos); 
					if(strcmp(firstState,"")==0){strcpy(firstState, $2);} 
	 				ctrState++; sprintf(expressao[aux++],"){");
					sprintf(expressao[aux++],"\n\t\tcontrolState = %d;\n", ctrState); }
		'{' ActionList '}' 	{ sprintf(expressao[aux++],"\t}\n");   
	 				if(TimerActions==1){saveTimer(); sprintf(expressao[aux++],"\n}\n");TimerActions=0;} 
					aux = storeExpState(expressao, aux); } 
		StateDef
;

// Regra para a chamada de um novo estado
State: 	IDENTIFIER 			{ sprintf(expressao[aux++],"\tstate_%s(", $1);  }
		'(' ExpList ')' ';' 	{ sprintf(expressao[aux++],");\n"); } 
	|	EXIT ';' 		{ sprintf(expressao[aux++],"\tstate_exit();\n"); }
	;

// Regra para definição das constantes
Const_List:		/* empty */
	|	CONST IDENTIFIER '=' INT_LITERAL ';'	
	{ sprintf(expressao[aux++],"\tconst int %s = %d;\n", $2, $4); setId($2, tInteiro); } Const_List
	|	CONST IDENTIFIER '=' REAL_LITERAL ';'	
	{ sprintf(expressao[aux++],"\tconst double %s = %.2f;\n", $2, $4); setId($2, tReal); } Const_List
	|	CONST IDENTIFIER '=' STR_LITERAL ';'	
	{ sprintf(expressao[aux++],"\tconst char %s[20] = sprintf(%s; \"%s\");\n", $2, $2, $4); setId($2, tString); } Const_List
;

// Regra para definição das variaveis
Var_List:		/* empty */
	|	Simple_Type Var Var_List
	|	LIST '<' Simple_Type '>' IDENTIFIER '[' INT_LITERAL ']' ';' 
		{setList($5, exprTipos[auxTipos-1], $7); 
	 	 exprTipos[auxTipos] =  exprTipos[auxTipos] + tList; 
	 	 sprintf(expressao[aux++],"%s[%d];\n", $5, $7); }
	Var_List 
;

// Identificação das variaveis
Var: 		/* empty */
	|	IDENTIFIER ';' { sprintf(expressao[aux++],"%s;\n", $1); setId($1,  exprTipos[auxTipos-1]); }  
	|	IDENTIFIER { sprintf(expressao[aux++],"%s, ", $1);  setId($1, exprTipos[auxTipos-1]); } ',' Var	
	|	IDENTIFIER { sprintf(expressao[aux++],"%s", $1);  setId($1, exprTipos[auxTipos-1]); }
	|	Assignment
;
 
// Decalaração de tipos simples
Simple_Type: 	INTEGER 	{ sprintf(expressao[aux++],"\tint "); exprTipos[auxTipos++] = tInteiro; }
	|	REAL		{ sprintf(expressao[aux++],"\tdouble ");  exprTipos[auxTipos++] = tReal; }
	|	STRING		{ sprintf(expressao[aux++],"\tchar[30] ");  exprTipos[auxTipos++] = tString; }
//	|	BOOLEAN		{ sprintf(expressao[aux++],"\tbool "); }
;

// Regra que indica uma lista de ações
ActionList: 	
	|	Action	ActionList
;

// Regra que indica as ações
Action:		Next_State
	|	Broadcast	
	|	Send		
	|	Recv
	|	During
	|	While
	|	For
	|	If 		
	|	Else
	|	Assignment
	|	Method_call
;

// Regra para mudança de estado
Next_State: NEXT_STATE State; 

// Regra para envio de mensagens BROADCAST
Broadcast: 	BROADCAST '(' IDENTIFIER ',' IDENTIFIER ',' IDENTIFIER ')' ';' 
{//setSend($3, getTypeMessage($7), $7);
 //sprintf(expressao[aux++],"\tsend(%s, %s, %s, 0);\n", $3, $7, $5);
sprintf(expressao[aux++],"\tcall AMSend.send(AM_BROADCAST_ADDR, &pkt, sizeof(%s));\n",getTypeMessage($7));
}		 
;

// Regra para envio de mensagens a uma lista de destinatarios
Send: 	SEND '('IDENTIFIER ',' IDENTIFIER ',' IDENTIFIER ',' IDENTIFIER')' ';'
{//setSend($3, getTypeMessage($9), $9);
sprintf(expressao[aux++],"auxSend = 0;\n");
 sprintf(expressao[aux++],"\twhile(%s[auxSend]!=0){\n", $7); 
 sprintf(expressao[aux++],"\t\tcall AMSend.send(%s[auxSend], &pkt, sizeof(%s));\n",$7, getTypeMessage($9));
 sprintf(expressao[aux++],"\t\tauxSend++;\n");
 sprintf(expressao[aux++],"\t}\n");
}
;


Recv:	RECV  '(' IDENTIFIER ',' IDENTIFIER ',' IDENTIFIER ')'   
	{aux = storeExpState(expressao, aux); ReceiveActions=1;
	if(firstRecv==0) {sprintf(expressao[aux++],"\t\tif(controlState == %d){\n", ctrState); firstRecv++;}
	else sprintf(expressao[aux++],"\t}else if(controlState == %d){\n", ctrState); 
	sprintf(expressao[aux++],"\t\t%s* %s = (%s*)payload;\n", getTypeMessage($7), $7, getTypeMessage($7) );} 
	'{' ActionList '}' 
	{ saveReceive(); }

	| RECV_BROADCAST '(' IDENTIFIER ',' IDENTIFIER ',' IDENTIFIER ')'  
	{aux = storeExpState(expressao, aux); ReceiveActions=1;
	if(firstRecv==0) {sprintf(expressao[aux++],"\tif(controlState == %d){\n", ctrState); firstRecv++;}
	else  sprintf(expressao[aux++],"\t} else if(controlState == %d){\n", ctrState);
	sprintf(expressao[aux++],"\t\t%s* %s = (%s*)payload;\n", getTypeMessage($7), $7, getTypeMessage($7) ); }
 	'{' ActionList '}'	
	 {saveReceive(); } 
;

During:	DURING { sprintf(expressao[aux++],"\tcall Timer.startOneShot("); TimerActions = 1; }
		'('Exp')'  { sprintf(expressao[aux++],");");} During_Recv
;

During_Recv: '{'{ aux = storeExpState(expressao, aux); sprintf(expressao[aux++],"\tif(controlState == %d){\n", ctrState); } 
			ActionList '}'
	|	Recv
;


While: 	WHILE '(' { sprintf(expressao[aux++],"\twhile( "); } Exp ')' { sprintf(expressao[aux++],"){\n "); } 
		'{' ActionList '}' { sprintf(expressao[aux++],"\t}\n"); }
;



For:	FOR IDENTIFIER IN IDENTIFIER 
		{ sprintf(expressao[aux++],"\tfor (int x=0; x < compList.size(%s); x++){\n", $4);
		 sprintf(expressao[aux++],"\t\tint %s = %s[x];\n", $2, $4); }
	'{' ActionList '}' 
		{ sprintf(expressao[aux++],"\t}\n"); } 
;

If: IF { sprintf(expressao[aux++],"\tif( "); } 
	'(' Exp ')' { sprintf(expressao[aux++]," ){\n"); } 
	'{' ActionList '}' 
	{ sprintf(expressao[aux++],"\n\t}\n");}
;

Else: 	ELSE { sprintf(expressao[aux++],"\telse {\n"); }
		'{' ActionList '}'
		{ sprintf(expressao[aux++],"\t}\n"); }
;

Method_call: 	Exp';' {  sprintf(expressao[aux++],";\n"); }
;

Assignment:	
	Exp '=' { sprintf(expressao[aux++]," = "); } 
	Exp ';' { sprintf(expressao[aux++],";\n"); }
;


Exp: 	
	|	IDENTIFIER '.'	{ sprintf(expressao[aux++],"%s.", $1); } Exp
	|	IDENTIFIER { sprintf(expressao[aux++],"%s", $1); exprTipos[auxTipos++]=getIdentInt($1, 0); } Exp
	|	STR_LITERAL { sprintf(expressao[aux++],"%s", $1); exprTipos[auxTipos++]=tString; } Exp
	|	INT_LITERAL { sprintf(expressao[aux++],"%d", $1); exprTipos[auxTipos++]=tInteiro; } Exp
	|	REAL_LITERAL { sprintf(expressao[aux++],"%f", $1); exprTipos[auxTipos++]=tReal; } Exp
	|	TRUE_LITERAL 	{ sprintf(expressao[aux++],"TRUE"); }
	|	FALSE_LITERAL 	{ sprintf(expressao[aux++],"FALSE"); } 
	|	'(' { sprintf(expressao[aux++],"( "); } ExpList ')' { sprintf(expressao[aux++]," )"); } Exp
	|	ExprMat  Exp
	|	ExprBool Exp
	|	Incremento Exp	
	|	SetMsg
	|	Entidade Exp
	|	List_Func
;

Incremento: 	IDENTIFIER DUPLO_MAIS { sprintf(expressao[aux++],"%s++", $1); } 
	|	IDENTIFIER DUPLO_MENOS { sprintf(expressao[aux++],"%s--", $1); }
;

List_Func: IDENTIFIER '.' INSERT	{ sprintf(expressao[aux++],"\tcall compList.insert(%s, ", $1); } 
		'(' Exp ')' 	{ sprintf(expressao[aux++],")"); setCommand("compList", "size", tInteiro, exprTipos); }
	| IDENTIFIER '.' SIZE '(' ')'  
		{ sprintf(expressao[aux++],"call compList.size(%s)", $1);  setCommand("compList", "size", tInteiro, exprTipos); } 
	| IDENTIFIER '.' NEXT '(' ')' 
		{ sprintf(expressao[aux++],"%s[x]", $1); setCommand("compList", "next", tInteiro, 0); }
	| IDENTIFIER '.' FRONT '(' ')' 
		{ sprintf(expressao[aux++],"%s[%d]", $1, getIdentInt($1, 3)); setCommand("compList", "front", tInteiro, 0);}
	| IDENTIFIER '.' BACK '(' ')' { sprintf(expressao[aux++],"%s[0] =", $1); setCommand($1, "back", tInteiro, 0); } 
;

SetMsg:	IDENTIFIER POINTER IDENTIFIER 
	{if(ReceiveActions==0){
		char structMsg[120];
		sprintf(structMsg,"\t%s* %s = (%s*)(call Packet.getPayload(&pkt, sizeof(%s)));\n",getTypeMessage($1), $1, 			getTypeMessage($1), getTypeMessage($1)); 
		storeMsgHeader(structMsg);
	}
	
	sprintf(expressao[aux++],"%s->%s", $1, $3); }
;

Entidade: 	
	IDENTIFIER POINTER IDENTIFIER '('
	{ sprintf(expressao[aux++],"call %s.%s(", $1, $3); zerarVetor(); }
	ExpList ')' { sprintf(expressao[aux++]," )"); setCommand($1, $3, 0, exprTipos); 
	}
;

ExprMat:	'+' { sprintf(expressao[aux++]," + "); }
		|	'-' { sprintf(expressao[aux++]," - "); }
		|	'*' { sprintf(expressao[aux++]," * "); }
		|	'/' { sprintf(expressao[aux++]," / "); } 
;

ExprBool: 	 DUPLO_IGUAL  		{ sprintf(expressao[aux++]," == "); } 
		|	MAIOR_IGUAL 	{ sprintf(expressao[aux++],"<="); } 
		|	MENOR_IGUAL 	{ sprintf(expressao[aux++],">="); } 
		|	DIFERENTE	{ sprintf(expressao[aux++],"!="); } 
		|	'>' 		{ sprintf(expressao[aux++],">"); } 
		|	'<' 		{ sprintf(expressao[aux++],"<"); }
;

ExpList: 	Exp		
		| 	Exp ',' { sprintf(expressao[aux++],", "); } ExpList
		|	Exp ';' { sprintf(expressao[aux++],";\n"); }
;

%%
	/* Método que incializa os cabeçalahos dos eventos */
	void eventosNesc(){
		INIboot(arq, firstState);
		
		INITimer();
		printReceive(arq);
		
		INIstartDone(arq);
		//INIamStop(arq);
	}

	/* Método que imprime as expressoes */
	void imprimirExpressao(){
		for(int i=0; i<aux; i++) {
			fprintf(arq,"%s",expressao[i]);
		}
		aux = 0;
	}

	/*Zerar vetores*/
	void zerarVetor(){
		for(int i=0; i<aux; i++){
			exprTipos[i] = 0;
		}
		auxTipos = 0;
	}

	/* Método que armazena as expressoes do Timer */
	void saveTimer(){
		
		while(strcmp(expTimer[auxTimer],"")!=0){
			auxTimer++;
		}
		if(auxTimer==0){
			sprintf(expTimer[auxTimer],"\tif(controlState == %d){\n", ctrState);
		} else{	
			sprintf(expTimer[auxTimer],"\telse if(controlState == %d){\n", ctrState);
		}
		for(int i=0; i<aux; i++) {
			sprintf(expTimer[auxTimer],"%s %s",expTimer[auxTimer], expressao[i]);
			//strcpy(expTimer[auxTimer++],expressao[i]);
		}
		aux = 0;
	}

	/* Método que imprime as expressoes do Timer */
	void INITimer(){
		//printf("aux timer: %d", auxTimer);
		fprintf(arq,"\n//timer fired\nevent void Timer.fired(){\n");
		for(int i=0; i<=auxTimer; i++) {
			fprintf(arq,"%s",expTimer[i]);
		}
		fprintf(arq,"}\n");
	}

	/* Método que armazena as expressoes do Receive */
	void saveReceive(){
		imputReceive(expressao, aux);
		ReceiveActions=0;		
		aux = 0;
	}


	void openLibrary(char nome[35], char nomeUse[35]) {

		FILE *arquivo;

		char endereco[35];
		sprintf(endereco, "./output/%s.nc", nome);
		arquivo = fopen(endereco, "r");

		if(arquivo != NULL){
			setCompName(nome, nomeUse);
			//printf("\nA interface da bilioteca %s já existe.\n", nome);
		} else {
			setCompName(nome, nomeUse);
			//printf("\nCriando arquivo do Componente %s\n", nome);
		}
	}

	void geraArquivoSaida(char nome[20]){
		char nomeArq[20];
		sprintf(nomeArq, "./output/%s.nc", nome);
		arq = fopen(nomeArq,"w");
		initModulo(arq, nome);
		imprimirExpressao();
	}


	int main() {
		
		arqMsg = fopen("./output/Complements.h","w");
		fprintf(arqMsg,"\n#ifndef COMPLEMENTS_H\n#define COMPLEMENTS_H\n");
		fprintf(arqMsg,"\nenum {\n\tAM = 6,\n\tTIMER_PERIOD_MILLI = 150\n};\n");

		yyparse();

		fprintf(arqMsg,"\n#endif");
		gerarInterfaces();

	}

	int yyerror(char *s) {
		printf("Syntax Error on line:\n %s\n", s);
		return 0;
	}
