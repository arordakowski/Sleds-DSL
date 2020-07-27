
/* Arquivo responsável pela geração da tradução com base na árvore sintática*/
/* Alexandre Ordakowski: alexandre.ordako@gmail.com */

//void INIradio(FILE *arq){
//  fprintf(arq,"\nevent void AMControl.startDone(error_t err){\n");
//  fprintf(arq,"\tif(err == SUCCESS){\n\t\tprintf(\"PPL: started\");\n\t} else {\n\t\tprintf(\"APPL start error\");\n\t}\n}\n");
//}

void INIboot(FILE *arq, char StateINI[]){
  fprintf(arq,"\n/* NesC language envents */\n");
  fprintf(arq,"\nevent void Boot.booted() {\n\tcall AMControl.start();\n");
  fprintf(arq,"\tstate_%s();\n}\n", StateINI);
}


void INIstartDone(FILE *arq){
  fprintf(arq,"\nevent void AMControl.startDone(error_t err){\t}\n");
  fprintf(arq,"\nevent void AMSend.sendDone( ) {\t}\n");
}

void printExit(FILE *arq){
  fprintf(arq,"\n\tvoid state_exit( ){\n\t\tcall AMControl.stop();\n\t}\n");
}
