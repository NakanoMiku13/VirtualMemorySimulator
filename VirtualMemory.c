#define _OPEN_SYS_ITOA_EXT
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#define true 1
#define false 0
#define TRUE true
#define FALSE false
#define kilobyte 1024
#define this Table* self
#define This self->head
#define HEX "%p"
typedef struct Table Table, *Function;
typedef struct Node Node;
typedef struct Pair Pair;
struct Pair{
    char hexVal;
    char* binVal;
};
struct Node{
    int valid, modified, page;
    int* frame;
    int **virtualDirection, **physicalDirection;
    Node *next, *prev;
};
struct Table{
    Node *head, *tail;
    size_t size;
    Function (*ctr)(this),(*Add)(this,int* process,int** direction),(*deleteLast)(this),(*deleteFront)(this),(*deleteAt)(this,int page),(*addNode)(this,Node* node);
};
Function ctr(this){
    self->head = self->tail = (Node*)malloc(sizeof(Node));
    self->size = 0;
}
Node* initNode(){
    Node* node = (Node*)malloc(sizeof(Node));
    node->valid = true;
    node->modified = false;
    node->page = -1;
    node->frame = (int*)malloc(sizeof(int)*(2*kilobyte));
}
Function Add(this,int* process,int** direction){
    Node* newNode = initNode();
    if(sizeof(process) <= sizeof(newNode->frame)){
        newNode->frame = process;
        newNode->page = self->size + 1;
        newNode->virtualDirection = direction;
        newNode->physicalDirection = NULL;
        if(self->size == 0){
            self->head =  newNode;
            self->tail = newNode;
        }else{
            self->tail->next = newNode;
            newNode->prev = self->tail;
            self->tail = newNode;
        }
        self->size++;
    }
}
Function deleteLast(this){
    if(self->size > 0 && self->head != self->tail){
        Node* tmp = self->tail;
        self->tail = self->tail->prev;
        self->size--;
        free(tmp);
    }else if(self->size == 1){
        self->tail = self->head = NULL;
        self->size--;
    }
}
Function deleteFront(this){
    if(self->size > 0 && self->head != self->tail){
        Node* tmp = self->head;
        self->head = self->head->next;
        self->size--;
        //free(tmp);
    }else if(self->size == 1){
        self->tail = self->head = NULL;
        self->size--;
    }
}
Function addNode(this,Node* node){
    if(self->size == 0){
        self->head = self->tail = node;
    }else{
        self->tail->next = node;
        node->prev = self->tail;
        self->tail = node;
    }
    self->size++;
}
Table initTable(){
    Table table;
    table.ctr = ctr;
    table.Add = Add;
    table.deleteLast = deleteLast;
    table.deleteFront = deleteFront;
    //table.deleteAt = deleteAt;
    table.addNode = addNode;
    table.ctr(&table);
    return table;
}
int* createProcess(){
    int i,n = rand() % (2*kilobyte);
    int* process = (int*)malloc(sizeof(int)*n);
    for(i = 0; i < n; i++) process[i] = rand() % (kilobyte);
    return process;
}
void progressBar(char* message){
    int i = 0;
    int n = 10;
    for(i = 0; i < n; i++){
        printf("%s\n[", message);
        for(int j = 0; j < i; j++){
            printf("*");
        }
        for(int j = i; j < n-1;j++){
            printf("-");
        }
        float percentage = (float)(i+1)*10+(rand()%10)+((float)(rand()%10000)/10000);
        if(percentage < 99)printf("] %f %c\n", percentage,(char)37);
        else printf("] %f %c\n", (float)(i+1)*10,(char)37);
        usleep(200*1000);
        system("clear");
    }
}
Node* getNode(Table table,const int page){
    Node* retNode = (Node*)malloc(sizeof(Node));
    if(page > table.size) return NULL;
    while(table.size > 0 && page != table.head->page) table.deleteFront(&table);
    if(table.head->page == page) return table.head;
    else{
        printf("Node not found...\n");
        return NULL;
    }
}
char* findValue(int begin,int end,const char x, Pair arr[16]){
    if(begin >= end){
        if(arr[begin].hexVal == x) return arr[begin].binVal;
        else return "-1";
    }else{
        int mid = (begin + end) / 2;
        if(arr[mid].hexVal == x) return arr[mid].binVal;
        if((int)arr[mid].hexVal > (int)x) return findValue(begin,mid,x,arr);
        else return findValue(mid+1,end,x,arr);
    }
}
char** convertToBinary(int** hex){
    Pair hexValues[] = {{'0',"0000"},
                        {'1',"0001"},
                        {'2',"0010"},
                        {'3',"0011"},
                        {'4',"0100"},
                        {'5',"0101"},
                        {'6',"0110"},
                        {'7',"0111"},
                        {'8',"1000"},
                        {'9',"1001"},
                        {'a',"1010"},
                        {'b',"1011"},
                        {'c',"1100"},
                        {'d',"1101"},
                        {'e',"1110"},
                        {'f',"1111"},};
    char** result = (char**)malloc(sizeof(char*)*50);
    if(hex != NULL){
        char buffer[50];
        sprintf(buffer,HEX,hex);
        for(int i = 2 ; i < 14 ; i++) result[i-2] = findValue(0,16,buffer[i],hexValues);
    }
    return result;
}
void tableFormat(Table table){
    printf("-----------------------------------------------------------------\n");
    printf("|\tValid\t|Process\t|\tModified\t|Page\t|\n");
    printf("-----------------------------------------------------------------\n");
    while(table.size > 0){
        printf("|\t%d\t|",table.head->valid);
        for(int i = 0; i < sizeof(table.head->frame)/2;i++) printf("%d",table.head->frame[i]);
        printf("\t|\t%d\t\t|%d\t|\n",table.head->modified,table.head->page);
        printf("-----------------------------------------------------------------\n");
        printf("|\t\t\tVirtual Direction\t\t\t|\n");
        char** virtualDirectionBinary = convertToBinary(table.head->virtualDirection);
        char** physicalDirectionBinary;
        if(virtualDirectionBinary != NULL) for(int i = 0 ; i < 12 ; i++) printf("%s",virtualDirectionBinary[i]);
        printf("|\t\n");
        if(table.head->physicalDirection != NULL) {physicalDirectionBinary = convertToBinary(table.head->physicalDirection); printf("|\t\t\tPhysical Direction\t\t\t|\n");}
        if(physicalDirectionBinary != NULL) for(int i = 0 ; i < 12 ; i++) printf("%s",physicalDirectionBinary[i]);
        printf("\t|\n");
        table.deleteFront(&table);
        printf("-----------------------------------------------------------------\n\n");
    }
}
void printTable(Table table){
    progressBar("Creating table...\n");
    progressBar("Getting data from tables...\n");
    tableFormat(table);
    printf("Press 1 to continue...\n");
    int n;
    scanf("%d",&n);
}
void printTables(Table table,Table table2){
    progressBar("Creating table...\n");
    progressBar("Getting data from tables...\n");
    printf("Pages table\n");
    tableFormat(table);
    printf("\n\nPhysical table\n");
    tableFormat(table2);
    printf("Press 1 to continue...\n");
    int n;
    scanf("%d",&n);
}
int main(){
    srand(time(NULL));
    //We use 16, 32 and 2 "kilobyte" instead of 64, 32 and 4 "kilobyte" 'cause integer variable have 2 bytes by default,
    //so if we convert we will have
    //64 kilobyte using 32 "kilobyte"
    int virtualMemoryPosition = 0, physicalMemoryPosition = 0;
    int** virtualMemory = (int**)malloc(sizeof(int*)*(16));
    for(int i = 0; i < 16;i++) virtualMemory[i] = (int*)malloc(sizeof(int)*(2*kilobyte));
    int** physicalMemory = (int**)malloc(sizeof(int*)*(8));
    for(int i = 0; i < 8;i++) physicalMemory[i] = (int*)malloc(sizeof(int)*(2*kilobyte));
    Table pagesTable = initTable();
    Table physicalMemoryTable = initTable();
    int n, *process;
    int m = 7;
    do{
        system("clear");
        printf("1) Create process\n2) Move X process to Physical Memory\n3) See Pages Table\n4) See Physical memory table\n5) Delete a process\n6) See both tables\n%d) Exit\n",m);
        scanf("%d",&n);
        switch(n){
            case 1:{
                progressBar("Creating process...");
                process = createProcess();
                progressBar("Moving the process to the Virtual memory...\n");
                if(virtualMemoryPosition < 16){
                    virtualMemory[virtualMemoryPosition] = process;
                    pagesTable.Add(&pagesTable,process,&virtualMemory[virtualMemoryPosition]);
                    virtualMemoryPosition ++;
                }
                else printf("Critical Error...\nVirtual Memory full...\nDelete process to continue...\n");
            }
            printf("Complete...\n");
            sleep(1);
            break;
            case 2:{
                if(pagesTable.size <= 0){
                    printf("No process in memory...\n");
                }
                else{
                    Node* node = NULL;
                    do{
                        printTable(pagesTable);
                        printf("Select the process that you want to move to the Physical Memory\n");
                        int page;
                        scanf("%d",&page);
                        node = getNode(pagesTable,page);
                        if(node != NULL){
                            if(physicalMemoryPosition < 8){
                                physicalMemory[physicalMemoryPosition] = node->frame;
                                node->physicalDirection = &physicalMemory[physicalMemoryPosition];
                                physicalMemoryTable.addNode(&physicalMemoryTable,node);
                                physicalMemoryPosition ++;
                            }else{
                                printf("Critical Error...\nNo more memory space...\n");
                                //Aplicar aquí algoritmo para eliminar procesos
                            }
                        }else{
                            if(page > pagesTable.size) printf("Page out of the range, try again...\n");
                            if(node == NULL) printf("Error getting the node page...\n");
                        }
                    }while(node == NULL);
                    printf("Complete...\n");
                    sleep(3);
                }
            }
            break;
            case 3:{
                printTable(pagesTable);
            }
            break;
            case 4:{
                printTable(physicalMemoryTable);
            }
            break;
            case 6:{
                printTables(pagesTable,physicalMemoryTable);
            }
        }
    }while(n < m);
}