%{

#include <stdio.h>
#include <string.h>
#include "sintatico.tab.h"
void showError();

int linha =1;

%}

integer					([0-9])+
real					[0-9]+\.[0-9]+
identifier  			([a-zA-Z][a-zA-Z0-9]*)+
string                  \"([a-zA-Z][a-zA-Z0-9]*)+\"

%%
use                     { return USE; }
as                      { return AS; }
Program                 { return PROGRAM; }
exit                    { return EXIT; }
STATE					{ return STATE; }
nextState               { return NEXT_STATE; }
broadcast               { return BROADCAST; }
send                    { return SEND; }
"on recvBroadcast"      { return RECV_BROADCAST; }
"on recv"               { return RECV; }
during                  { return DURING; }

message                 { return MESSAGE; }
"message type"          { return MESSAGE_TYPE; }

const                   { return CONST; }
int                     { return INTEGER; }
real 	              	{ return REAL; }
bool                    { return BOOLEAN; }
string	              	{ return STRING; }
list                    { return LIST; }
map	                	{ return MAP; }

insert                  { return INSERT; }
next                    { return NEXT; }
back                    { return BACK; }
front                   { return FRONT; }
size                    { return SIZE; }


if                      		{ return IF; }
else                    		{ return ELSE; }
for                     		{ return FOR; }
while                   		{ return WHILE; }
in					{ return IN; }

"->"					{ return POINTER; }
"."	 				{ return *yytext; }

"{"					{ return *yytext; }
"}"					{ return *yytext; }
"("					{ return *yytext; }
")"					{ return *yytext; }
"["					{ return *yytext; }
"]"					{ return *yytext; }
";"					{ return *yytext; }
","					{ return *yytext; }
"|"					{ return *yytext; }
"=="					{ return DUPLO_IGUAL; }
"!="					{ return DIFERENTE; }
"="					{ return *yytext; }
">"					{ return *yytext; }
"<"					{ return *yytext; }
">="					{ return MAIOR_IGUAL; }
"<="					{ return MENOR_IGUAL; }
"++"					{ return DUPLO_MAIS; }
"--"					{ return DUPLO_MENOS; }

"+"					{ return *yytext; }
"-"					{ return *yytext; }
"*"					{ return *yytext; }
"/"					{ return *yytext; }


[ \t]					{ }
[ \n]					{ }//printf("%d", linha++);}

true                    		{ return TRUE_LITERAL; }
false                   		{ return FALSE_LITERAL; }

{integer}				{ yylval.number_int = atoi(yytext) ; return INT_LITERAL; }
{real}					{ yylval.number_real = atof(yytext) ; return REAL_LITERAL; }
{string}				{ yylval.word =  strdup(yytext); return STR_LITERAL;}
{identifier}				{ yylval.word =  strdup(yytext); return IDENTIFIER; }


%%

void showError(){
    printf("Erro, linha %d", linha);
}
