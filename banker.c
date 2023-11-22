
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 500

//Funções
int request_func(int *request, int *available ,int **need, int **allocation, int num_resourses, int customer, int num_customers);
int release_func(int *release, int **allocation, int num_resourses, int customer);
int detect_deadlock(int num_resourses, int num_customers, int *available, int **need, int **allocation);
void show_table(int **maximum, int **allocation, int **need, int *available, int num_resourses, int num_customers);
int check_file(char *file_name, int num_instances);

//Banker
int main(int argc, char *argv[]){

    int num_instances = argc - 1; //quantidade de instancias
    int num_customers = 0, num_resourses = 0; //quantidade de customers e recursos

    int *available = (int *)malloc(num_instances * sizeof(int)); //aloca o vetor de recursos disponíveis
   
    for (int i = 1; i < argc; i++){ 
        available[i - 1] = atoi(argv[i]); //converte os argumentos para inteiros e armazena no vetor
    }
    
    //Ler arquivo de entrada e armazenar em uma matriz
    
    if(check_file("customers.txt", num_instances) == 0){
        printf("Fail to read customer.txt\n");
        exit(1);
    }

    FILE *customers_file = fopen("customers.txt", "r");
    
    char line[500]; 

    while (fgets(line, sizeof(line), customers_file) != NULL){  //conta o numero de customers
        num_customers++;
    }
    
    rewind(customers_file);

    fgets(line, sizeof(line), customers_file); //conta o numero de recursos
    char *token = strtok(line, ","); //separa a linha por virgula
    while (token != NULL){
        num_resourses++;
        token = strtok(NULL, ",");
    }

    fclose(customers_file);

    if(num_resourses != num_instances) {
        printf("Incompatibility between customer.txt and command line\n");
        exit(1);
    }
    
    int **maximum, **allocation, **need; //matrizes maximum, allocation e need 

    //aloca as colunas das matrizes
    maximum = (int **)malloc(num_customers * sizeof(int *));
    allocation = (int **)malloc(num_customers * sizeof(int *));
    need = (int **)malloc(num_customers * sizeof(int *));

    //aloca as linhas das matrizes
    for (int i = 0; i < num_customers; i++){
        maximum[i] = (int *)malloc(num_resourses * sizeof(int));
        allocation[i] = (int *)malloc(num_resourses * sizeof(int));
        need[i] = (int *)malloc(num_resourses * sizeof(int));
    }
    
    customers_file = fopen("customers.txt", "r");

    if(customers_file == NULL){
        printf("Fail to read customer.txt\n");
        exit(1);
    }

    for (int i = 0; i < num_customers; i++){ //preenche as matrizes maximum e need, e zera a matriz allocation
        fgets(line, sizeof(line), customers_file);
        char *token = strtok(line, ",");

        for (int j = 0; j < num_resourses; j++){ //preenche as matrizes maximum e need, e zera a matriz allocation
            maximum[i][j] = atoi(token);
            need[i][j] = atoi(token);
            allocation[i][j] = 0;

            token = strtok(NULL, ",");
        }
    }

    fclose(customers_file);
    
    char command[MAX];
    int customer;
    int *resources = (int *)malloc(num_resourses * sizeof(int));

    
    if(check_file("commands.txt", num_instances) == 0){
        printf("Fail to read commands.txt\n");
        exit(1);
    }

    FILE *commands_file = fopen("commands.txt", "r");

    while (fgets(line, sizeof(line), commands_file) != NULL){ //armazena os comandos em uma struct (Comando, customer, recursos)
        if(strncmp(line, "*", 1) == 0){
            show_table(maximum, allocation, need, available, num_resourses, num_customers);    
        } 
        else{
            int resources_count = 0;

            strcpy(command, strtok(line, " ")); //armazena o comando
            customer = atoi(strtok(NULL, " ")); //armazena o numero do customer

            for(int i = 0; i < num_resourses; i++){ //armazena a quantidade de instancias de cada recurso
                resources[i] = atoi(strtok(NULL, " ")); 
                resources_count ++; 
            }

            if(resources_count != num_instances){ //verifica se a quantidade de instancias é igual a quantidade de recursos
                printf("Incompatibility between commands.txt and command line\n");
                exit(1);
            }

            if(strcmp(command, "RQ") == 0){ //verifica se o comando é RQ e se for, chama a função de request
                if(request_func(resources, available, need, allocation, num_resourses, customer, num_customers) == 1){ 
                }
                
            }
            else if(strcmp(command, "RL") == 0){ //verifica se o comando é RL e se for, chama a função de release
                if(release_func(resources, allocation, num_resourses, customer) == 1){
                    for (int i = 0; i < num_resourses; i++){ //se liberou os recursos, atualiza as matrizes
                        allocation[customer][i] -= resources[i];
                        available[i] += resources[i];
                        need[customer][i] += allocation[customer][i]; //maximun - allocation
                    }
                }
            }
        }
    }
    fclose(commands_file);  
}

//Função de request de recursos
int request_func(int *request, int *available ,int **need, int **allocation, int num_resourses, int customer, int num_customers){
    FILE *file = fopen("result.txt", "a+");
    if(file == NULL){
        printf("Error opening file!\n");
    }

    for (int j = 0; j < num_resourses; j++){
        if((available[j] - request[j]) < 0){ //verifica se os recursos disponiveis são suficientes para alocar
            
            fprintf(file, "The resources " );
            for(int i = 0; i < num_resourses; i++){
                fprintf(file, "%d ", available[i]);
            }
            
            fprintf(file, "are not enough for customer %d request ", customer);
            for(int i = 0; i < num_resourses; i++){
                fprintf(file, "%d ", request[i]);
            }
            fprintf(file, "\n");
            fclose(file);
            return 0;
        }
        if((need[customer][j] - request[j]) < 0 ){ //verifica se o customer possui os recursos que deseja alocar

            fprintf(file, "The customer %d request ", customer);
            for(int i = 0; i < num_resourses; i++){
                fprintf(file, "%d ", request[i]);
            }
            fprintf(file, "was denied because exceed its maximum need\n");
            fclose(file);
            return 0;
        }
    }
   
        
    for(int j = 0; j < num_resourses; j++){ //Simulando a alocação 
        available[j] -= request[j];
        allocation[customer][j] += request[j];
        need[customer][j] -= request[j];
    }
    
    if(detect_deadlock(num_resourses, num_customers, available, need, allocation) == 0){ //Se o estado for seguro, aloca os recursos
    
        fprintf(file, "Allocate to customer %d the resources ", customer);
        for(int i = 0; i < num_resourses; i++){
            fprintf(file, "%d ", request[i]);
        }
        fprintf(file, "\n");
        fclose(file);
        return 1; 
    }
    else{ //Se o estado for inseguro, desfaz a alocação e retorna 0
        for (int j = 0; j < num_resourses; j++){
            available[j] += request[j];
            allocation[customer][j] -= request[j];
            need[customer][j] += request[j];
        }
        
        fprintf(file, "The customer %d request ", customer);
        for(int i = 0; i < num_resourses; i++){
            fprintf(file, "%d ", request[i]);
        }
        fprintf(file, "was denied because result in an unsafe state\n");
        fclose(file);
        return 0;
    }
    
}

int release_func(int *release, int **allocation, int num_resourses, int customer){ //Função de release de recursos
    FILE *file = fopen("result.txt", "a+");
    if(file == NULL){
        printf("Error opening file!\n");
    }

    for(int i = 0; i < num_resourses; i++){
        if((allocation[customer][i] - release[i]) < 0){ //Se o customer nao possui os recursos que deseja liberar, retorna 0
            fprintf(file, "The customer %d release ", customer);

            for(int i = 0; i < num_resourses; i++){
                fprintf(file, "%d ", release[i]);
            }

            fprintf(file, "was denied because exceed its maximum allocation\n");
            fclose(file);
            return 0;
        }
    }
    fprintf(file, "Release from customer %d the resources ", customer);
    for(int i = 0; i < num_resourses; i++){
        fprintf(file, "%d ", release[i]);
    }
    fprintf(file, "\n");
    fclose(file);
    return 1;
}    

int detect_deadlock(int num_resourses, int num_customers, int *available, int **need, int **allocation){
    int work[num_resourses]; //Vetor de recursos disponiveis
    int finish[num_customers]; //Vetor de customers terminados

    for(int i = 0; i < num_resourses; i++){ //Copia available para work
        work[i] = available[i];
    }

    for(int i = 0; i < num_customers; i++){ //Inicializa finish com false para todos os customers
        finish[i] = 0;
    }

    while(1){ //Enquanto existir um customer que nao terminou
        int found = 0;

        for(int i = 0; i < num_customers; i++){ 
            if(finish[i] == 0){ //Se o customer nao terminou 
                int j;
                for(j = 0; j < num_resourses; j++){ //Verifica se o customer pode ser executado
                    if(need[i][j] > work[j]){ //Se a necessidade do customer for maior que o recurso disponivel 
                        break;
                    }
                }
                if(j == num_resourses){ //Se o customer pode ser executado 
                    for(j = 0; j < num_resourses; j++){ //Libera os recursos alocados pelo customer
                        work[j] += allocation[i][j];
                    }

                    finish[i] = 1; //Marca o customer como terminado
                    found = 1; //Encontrou um customer que pode ser executado
                }
            }
        }

        if(found == 0){ //Se nao encontrou um customer que pode ser executado
            break;
        }
    }

    for(int i = 0; i < num_customers; i++){ //Verifica se todos os customers terminaram
        if(finish[i] == 0){ //Se algum customer nao terminou, Deadlock detectado
            return 1; 
        }
    }

    return 0; //Nenhum deadlock detectado
}

void show_table(int **maximum, int **allocation, int **need, int *available, int num_resourses, int num_customers){

    FILE *file = fopen("result.txt", "a");
    if (file == NULL) {
        printf("Error opening file!\n");
    }

    fprintf(file, "MAXIMUM | ALLOCATION | NEED\n");
    for (int i = 0; i < num_customers; i++) {
        for (int j = 0; j < num_resourses; j++) {
            fprintf(file, "%d ", maximum[i][j]);
        }
        fprintf(file, "| ");
        for (int j = 0; j < num_resourses; j++) {
            fprintf(file, "%d ", allocation[i][j]);
        }
        fprintf(file, "| ");
        for (int j = 0; j < num_resourses; j++) {
            fprintf(file, "%d ", need[i][j]);
        }
        fprintf(file, "\n");
    }

    fprintf(file, "AVAILABLE:");
    for(int i = 0; i < num_resourses; i++){
        fprintf(file," %d", available[i]);
    }
    fprintf(file, "\n");

    fclose(file);
}

int check_file(char *file_name, int num_instances){ //Verifica se o arquivo de entrada é valido
    FILE *file = fopen(file_name, "r");

    if(file == NULL){ //Verifica se o arquivo existe
        printf("Fail to read %s\n", file_name);
        exit(1);
    }

    char linha[500];

    if(strcmp(file_name, "customers.txt") == 0){
        while (fgets(linha, sizeof(linha), file) != NULL){
          
            linha[strcspn(linha, "\n")] = 0; //Remove o \n da linha
            int len = strlen(linha);
            int num_count = 0, char_count = 0;
        
            while(char_count < len){
                if(isdigit(linha[char_count])){ 
                    while (char_count < len && isdigit(linha[char_count])) char_count++; //Conta a quantidade de digitos
                    num_count++;//Conta a quantidade de numeros
                }
                else if(linha[char_count] == ','){
                    if (char_count == len - 1 || !isdigit(linha[char_count + 1])) return 0; //Verifica se a virgula é seguida de um digito
                    char_count++; //Pula a virgula
                }
                else{
                    return 0;
                }
            }
            if (num_count != num_instances) return 0; //Verifica se a quantidade de numeros é igual a quantidade de instancias
        }
        fclose(file);
        return 1;
    }
    else if(strcmp(file_name, "commands.txt") == 0){
        while(fgets(linha, sizeof(linha), file) != NULL){

            linha[strcspn(linha, "\n")] = 0;
            if(strcmp(linha, "*") == 0) return 1; //Verifica se o comando é *
            if(strncmp(linha, "RQ ", 3) != 0 && strncmp(linha, "RL ", 3) != 0) return 0; //Verifica se o comando é RQ ou RL

            int num_count = 0;
            char *token = strtok(linha + 3, " ");

            while(token != NULL){ //Verifica se os argumentos são numeros
                for(int i = 0; token[i] != '\0'; i++){
                    if (!isdigit(token[i])) return 0;
                }
                num_count++;
                token = strtok(NULL, " ");
            }

            if(num_count != num_instances + 1){ //Verifica se a quantidade de numeros é igual a quantidade de instancias + 1
               return 0;
            }
            
        }
        fclose(file);
        return 1;
    }
    
}