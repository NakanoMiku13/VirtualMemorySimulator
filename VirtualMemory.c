#include<stdio.h>
#include<stdlib.h>
#define true 1
#define false 0
#define TRUE true
#define FALSE false
#define kilobyte 1024
#define this Table* self
#define This self->head
typedef struct Table Table, *Function;
typedef struct Node Node;
struct Node{
    int valid, modified, page;
    int* frame, virtualDirection, physicalDirection;
    Node *next, *prev;
};
struct Table{
    Node *head, *tail;
    size_t size;
    Function (*ctr)(this),(*Add)(this,int* process),(*deleteLast)(this),(*deleteFront)(this),(*deleteAt)(this,int page);
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
Function Add(this,int* process){
    Node* newNode = initNode();
    if(sizeof(process) <= sizeof(newNode->frame)){
        newNode->frame = process;
        newNode->page = self->size + 1;
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
Table initTable(){
    Table table;
    table.ctr = ctr;
    table.Add = Add;
    table.deleteLast = deleteLast;
    table.deleteFront = deleteFront;
    table.ctr(&table);
    return table;
}
int main(){
    //We use 16, 32 and 2 "kilobyte" instead of 64, 32 and 4 "kilobyte" 'cause integer variable have 2 bytes by default, so if we convert we will have
    //64 kilobyte using 32 "kilobyte"
    int* virutalMemory = (int*)malloc(sizeof(int)*(32*kilobyte));
    int* physicalMemory = (int*)malloc(sizeof(int)*(16*kilobyte));
    Table pagesTable = initTable();
    Table physicalMemoryTable = initTable();
    int* tmp = (int*)malloc(sizeof(int)*kilobyte);
    pagesTable.Add(&pagesTable,tmp);
    pagesTable.Add(&pagesTable,tmp);
    pagesTable.Add(&pagesTable,tmp);
    pagesTable.Add(&pagesTable,tmp);
    while(pagesTable.size > 0){
        printf("s: %d\n",pagesTable.head->page);
        pagesTable.deleteFront(&pagesTable);
    }
}