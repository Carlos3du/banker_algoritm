
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//gcc main.c -o exec
// ./exec
int main(int argc, char *argv[]){

    int num_instances = argc - 1; //quantidade de instancias
    int num_customers = 0, num_resources = 0; //quantidade de customers e recursos
    int avaliable[num_instances]; //vetor de recursos disponíveis

    for (int i = 1; i < argc; i++){ 
        avaliable[i - 1] = atoi(argv[i]); //converte os argumentos para inteiros e armazena no vetor
    }
    
    //Ler arquivo de entrada e armazenar em uma matriz
    FILE *file;
    file = fopen("customers.txt", "r");

    if (file == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    
    char line[100]; 
    while (fgets(line, sizeof(line), file) != NULL){  //conta o numero de customers
        num_customers++;
    }

    rewind(file);

    fgets(line, sizeof(line), file); //conta o numero de recursos
    char *token = strtok(line, ","); //separa a linha por virgula
    while (token != NULL){
        num_resources++;
        token = strtok(NULL, ",");
    }

    fclose(file);

    int maximun[num_customers][num_resources]; 
    int allocation[num_customers][num_resources];//matriz allocation, inicialmente igual a 0
    int need[num_customers][num_resources];//matriz need, inicialmente igual a maximun
    
    file = fopen("customers.txt", "r");

    if (file == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    for (int i = 0; i < num_customers; i++){ //preenche as matrizes maximun e need, e zera a matriz allocation
        fgets(line, sizeof(line), file);
        char *token = strtok(line, ",");

        for (int j = 0; j < num_resources; j++){
            maximun[i][j] = atoi(token);
            need[i][j] = atoi(token);
            allocation[i][j] = 0;

            token = strtok(NULL, ",");
        }
    }
    fclose(file);
    
    char *comando;
    
}
