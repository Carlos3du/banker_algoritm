
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 500
//gcc main.c -o exec
// ./exec

int request_func(int *request, int *available, int **need, int num_resources, int customer);
int release_func(int *release, int **allocation, int num_resources, int customer);

int main(int argc, char *argv[]){


    int num_instances = argc - 1; //quantidade de instancias
    int num_customers = 0, num_resources = 0; //quantidade de customers e recursos

    int *available = (int *)malloc(num_instances * sizeof(int)); //aloca o vetor de recursos disponíveis
   
    for (int i = 1; i < argc; i++){ 
        available[i - 1] = atoi(argv[i]); //converte os argumentos para inteiros e armazena no vetor
    }
    
    //Ler arquivo de entrada e armazenar em uma matriz
    FILE *customers_file;

    customers_file = fopen("customers.txt", "r");

    if (customers_file == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    
    char line[500]; 

    while (fgets(line, sizeof(line), customers_file) != NULL){  //conta o numero de customers
        num_customers++;
    }

    rewind(customers_file);

    fgets(line, sizeof(line), customers_file); //conta o numero de recursos
    char *token = strtok(line, ","); //separa a linha por virgula
    while (token != NULL){
        num_resources++;
        token = strtok(NULL, ",");
    }

    fclose(customers_file);

    int **maximum, **allocation, **need; //matrizes maximum, allocation e need 

    //aloca as colunas das matrizes
    maximum = (int **)malloc(num_customers * sizeof(int *));
    allocation = (int **)malloc(num_customers * sizeof(int *));
    need = (int **)malloc(num_customers * sizeof(int *));

    //aloca as linhas das matrizes
    for (int i = 0; i < num_customers; i++){
        maximum[i] = (int *)malloc(num_resources * sizeof(int));
        allocation[i] = (int *)malloc(num_resources * sizeof(int));
        need[i] = (int *)malloc(num_resources * sizeof(int));
    }
    
    customers_file = fopen("customers.txt", "r");

    if (customers_file == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    for (int i = 0; i < num_customers; i++){ //preenche as matrizes maximum e need, e zera a matriz allocation
        fgets(line, sizeof(line), customers_file);
        char *token = strtok(line, ",");

        for (int j = 0; j < num_resources; j++){
            maximum[i][j] = atoi(token);
            need[i][j] = atoi(token);
            allocation[i][j] = 0;

            token = strtok(NULL, ",");
        }
    }

    fclose(customers_file);
    
    
    char command[MAX];
    int customer;
    int *resources = (int *)malloc(num_resources * sizeof(int));

    FILE *commands_file;
    commands_file = fopen("commands.txt", "r");

    while (fgets(line, sizeof(line), commands_file) != NULL){ //armazena os comandos em uma struct (Comando, customer, recursos)
        if(strncmp(line, "*", 1) == 0){
            //mostrar tabela
           
        } 
        else{
            strcpy(command, strtok(line, " ")); //armazena o comando
            customer = atoi(strtok(NULL, " ")); //armazena o numero do customer
          
            for(int i = 0; i < num_resources; i++){ //armazena a quantidade de instancias de cada recurso
                resources[i] = atoi(strtok(NULL, " "));  
            }

            if(strcmp(command, "RQ") == 0){

                int call_request = request_func(resources, available, need, num_resources, customer);

                if(call_request == 1){
                    for (int i = 0; i < num_resources; i++){
                        allocation[customer][i] += resources[i];
                        available[i] -= resources[i];
                        need[customer][i] -= allocation[customer][i]; //maximun - allocation
                    }
                }
                
            }
            else if(strcmp(command, "RL") == 0){
                int call_release = release_func(resources, allocation, num_resources, customer);

                if(call_release == 1){
                    for (int i = 0; i < num_resources; i++){
                        allocation[customer][i] -= resources[i];
                        available[i] += resources[i];
                        need[customer][i] += allocation[customer][i]; //maximun - allocation
                    }
                }
            }
            else{
                printf("Comando inválido\n");
            }
        }
    }

    fclose(commands_file);
    
}

int request_func(int *request, int *available ,int **need, int num_resources, int customer){
    for (int j = 0; j < num_resources; j++){
        if((available[j] - request[j]) < 0){
            //The resources 3 3 2 are not enough to customer 4 request 4 0 0
            printf("The resources %d %d %d are not enough to customer %d request %d %d %d\n", available[0], available[1], available[2], customer, request[0], request[1], request[2]);
            return 0;
        }
        if((need[customer][j] - request[j]) < 0 ){
            printf("The customer %d request %d %d %d was denied because exceed its maximum need\n", customer, request[0], request[1], request[2]);
            return 0;
        }
    }
    printf("Allocate to customer %d the resources %d %d %d\n", customer, request[0], request[1], request[2]);
    return 1;   
}

int release_func(int *release, int **allocation, int num_resources, int customer){
    for(int i = 0; i < num_resources; i++){
        if((allocation[customer][i] - release[i]) < 0){
            printf("The customer %d release %d %d %d was denied because exceed its maximum allocation\n", customer, release[0], release[1], release[2]);
            return 0;
        }
    }
    return 1;
}    



