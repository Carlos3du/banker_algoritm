
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 500
//gcc main.c -o exec
// ./exec

int request_func(int *request, int *available ,int **need, int **allocation, int num_resourses, int customer, int num_customers);
int release_func(int *release, int **allocation, int num_resourses, int customer);
int detect_deadlock(int num_resourses, int num_customers, int *available, int **need, int **allocation);

int main(int argc, char *argv[]){

    int num_instances = argc - 1; //quantidade de instancias
    int num_customers = 0, num_resourses = 0; //quantidade de customers e recursos

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
        num_resourses++;
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
        maximum[i] = (int *)malloc(num_resourses * sizeof(int));
        allocation[i] = (int *)malloc(num_resourses * sizeof(int));
        need[i] = (int *)malloc(num_resourses * sizeof(int));
    }
    
    customers_file = fopen("customers.txt", "r");

    if (customers_file == NULL){
        printf("Erro ao abrir o arquivo\n");
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

    FILE *commands_file;
    commands_file = fopen("commands.txt", "r");

    while (fgets(line, sizeof(line), commands_file) != NULL){ //armazena os comandos em uma struct (Comando, customer, recursos)
        if(strncmp(line, "*", 1) == 0){
            //mostrar tabela
           
        } 
        else{
            strcpy(command, strtok(line, " ")); //armazena o comando
            customer = atoi(strtok(NULL, " ")); //armazena o numero do customer
          
            for(int i = 0; i < num_resourses; i++){ //armazena a quantidade de instancias de cada recurso
                resources[i] = atoi(strtok(NULL, " "));  
            }

            if(strcmp(command, "RQ") == 0){ //verifica se o comando é RQ e se for, chama a função de request
                if(request_func(resources, available, need, allocation, num_resourses, customer, num_customers) == 1){ 
                    for (int i = 0; i < num_resourses; i++){ //se alocou os recursos, atualiza as matrizes 
                        allocation[customer][i] += resources[i];
                        available[i] -= resources[i];
                        need[customer][i] -= allocation[customer][i]; //maximun - allocation
                    }
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
            else{
                printf("Comando inválido\n");
            }
        }
    }

    fclose(commands_file);
    
}

//Função de request de recursos
int request_func(int *request, int *available ,int **need, int **allocation, int num_resourses, int customer, int num_customers){
    for (int j = 0; j < num_resourses; j++){
        if((available[j] - request[j]) < 0){ //verifica se os recursos disponiveis são suficientes para alocar
            
            printf("The resources %d %d %d are not enough to customer %d request %d %d %d\n", 
                available[0], available[1], available[2], customer, request[0], request[1], request[2]);
            return 0;
        }
        if((need[customer][j] - request[j]) < 0 ){ //verifica se o customer possui os recursos que deseja alocar
            printf("The customer %d request %d %d %d was denied because exceed its maximum need\n",
                 customer, request[0], request[1], request[2]);
            return 0;
        }
    }
   
        
    for(int j = 0; j < num_resourses; j++){ //Simulando a alocação (temporario)
        available[j] -= request[j];
        allocation[customer][j] += request[j];
        need[customer][j] -= request[j];
    }
    
    if(detect_deadlock(num_resourses, num_customers, available, need, allocation) == 0){ //Se o estado for seguro, aloca os recursos
        printf("Allocate to customer %d the resources %d %d %d\n", 
            customer, request[0], request[1], request[2]);
        return 1; 
    }
    else{ //Se o estado for inseguro, desfaz a alocação e retorna 0
        for (int j = 0; j < num_resourses; j++){
            available[j] += request[j];
            allocation[customer][j] -= request[j];
            need[customer][j] += request[j];
        }

        printf("The customer %d request %d %d %d was denied because result in an unsafe state\n",
            customer, request[0], request[1], request[2]);
        return 0;
    }
      
}


int release_func(int *release, int **allocation, int num_resourses, int customer){ //Função de release de recursos
    for(int i = 0; i < num_resourses; i++){
        if((allocation[customer][i] - release[i]) < 0){ //Se o customer nao possui os recursos que deseja liberar, retorna 0
            printf("The customer %d release %d %d %d was denied because exceed its maximum allocation\n", 
                customer, release[0], release[1], release[2]);
            return 0;
        }
    }
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
