
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAMESIZE 51
#define BLOCKSIZE 512
#define MAXBLOCKS 5000
#define NUM_BLOCKS 1024
#define INPUTSIZE 4096

typedef struct fnode{
    char name[NAMESIZE];
    int isDirectory;
    struct fnode *parent;
    struct fnode *child;
    struct fnode *prev;
    struct fnode *next;
    int blockCount;
    int blockPointers[MAXBLOCKS];
    int fileSize;
}fnode;

typedef struct freeBlock{
    int index;
    struct freeBlock *prev;
    struct freeBlock *next;
}freeBlock;

unsigned char virtualDisk[NUM_BLOCKS][BLOCKSIZE];

freeBlock *head = NULL;
freeBlock *tail = NULL;
fnode *root = NULL;
fnode *cwd = NULL;

void rootdir(){
    root = malloc(sizeof(fnode));
    if(root == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }

    strcpy(root->name, "/");
    root->isDirectory = 1;
    root->parent = NULL;
    root->child = NULL;
    root->next = NULL;
    root->prev = NULL;
    root->blockCount = 0;
    root->fileSize = 0;

    cwd = root;
}

void initializeFreeBlocks(){
    for(int i = 0; i < NUM_BLOCKS; i++) {
        freeBlock *newBlock = malloc(sizeof(freeBlock));
        if(newBlock == NULL) {
            printf("Memory allocation failed\n");
            exit(1);
        }

        newBlock->index = i;
        newBlock->next = NULL;
        newBlock->prev = tail;

        if(head == NULL){
            head = newBlock;
        }
        else{
            tail->next = newBlock;
        }

        tail = newBlock;
    }
}

fnode *createNode(char name[], int isDirectory){
    if(strlen(name) == 0 || strlen(name) >= NAMESIZE) {
        printf("Invalid name\n");
        return NULL;
    }
    fnode *newNode = malloc(sizeof(fnode));
    if(newNode == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }
    strcpy(newNode->name, name);
    newNode->isDirectory = isDirectory;
    newNode->parent = NULL;
    newNode->child = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->blockCount = 0;
    newNode->fileSize = 0;

    return newNode;
}

void insertChild(fnode *parent, fnode *newNode){
    newNode->parent = parent;
    if(parent->child == NULL){
        parent->child = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    }
    else{
        fnode *first = parent->child;
        fnode *last = first->prev;
        last->next = newNode;
        newNode->prev = last;
        newNode->next = first;
        first->prev = newNode;
    }
}

fnode *search(char name[]){
    if(cwd->child == NULL){
        return NULL;
    }
    fnode *temp = cwd->child;
    do {
        if(strcmp(temp->name, name) == 0){
            return temp;
        }
        temp = temp->next;
    }while(temp != cwd->child);

    return NULL;
}

void removeChild(fnode *node){
    fnode *parent = node->parent;
    if(node->next == node){
        parent->child = NULL;
    }
    else{
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if(parent->child == node){
            parent->child = node->next;
        }
    }

    node->next = NULL;
    node->prev = NULL;
    node->parent = NULL;
}

void mkdir(char name[]){
    if(search(name) != NULL){
        printf("Name already exists\n");
        return;
    }
    fnode *newNode = createNode(name, 1);
    if(newNode == NULL){
        return;
    }
    insertChild(cwd, newNode);
    printf("Directory '%s' created successfully.\n", name);
}

void create(char name[]){
    if(search(name) != NULL){
        printf("Name already exists\n");
        return;
    }
    fnode *newNode = createNode(name, 0);
    if(newNode == NULL){
        return;
    }
    insertChild(cwd, newNode);
    printf("File '%s' created successfully.\n", name);
}

void ls(){
    if(cwd->child == NULL){
        printf("(empty)\n");
        return;
    }
    fnode *temp = cwd->child;
    do{
        if(temp->isDirectory){
            printf("%s/\n", temp->name);
        }
        else{
            printf("%s\n", temp->name);
        }

        temp = temp->next;
    }while(temp != cwd->child);
}

void cd(char name[]){
    if(strcmp(name, "..") == 0){
        if(cwd->parent != NULL){
            cwd = cwd->parent;
        }
        printf("Moved to / \n");
        return;
    }

    if(strcmp(name, "/") == 0){
        cwd = root;
        return;
    }
    fnode *node = search(name);
    if(node == NULL || !node->isDirectory){
        printf("Directory not found\n");
        return;
    }
    cwd = node;
    printf("Moved to /%s\n", name);
}

void pwd(){
    fnode *temp = cwd;
    fnode *path[100];
    int count = 0;

    while(temp != NULL && count < 100){
        path[count] = temp;
        count++;
        temp = temp->parent;
    }
    for(int i = count - 1; i >= 0; i--){
        if(i == count - 1){
            printf("/");
        }
        else{
            printf("%s", path[i]->name);
            if(i != 0){
                printf("/");
            }
        }
    }

    printf("\n");
}

int allocateBlock(){
    if(head == NULL){
        printf("Disk is full!\n");
        return -1;
    }
    freeBlock *temp = head;
    int index = temp->index;
    head = head->next;
    if(head != NULL){
        head->prev = NULL;
    }
    else{
        tail = NULL;
    }

    free(temp);
    return index;
}

int freeBlocks(int index){
    freeBlock *newBlock = malloc(sizeof(freeBlock));
    if(newBlock == NULL){
        printf("Memory allocation failed\n");
        return 0;
    }
    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = NULL;
    if(head == NULL){
        head = newBlock;
        tail = newBlock;
    }
    else{
        tail->next = newBlock;
        newBlock->prev = tail;
        tail = newBlock;
    }
    return 1;
}

void delete(char name[]){
    fnode *node = search(name);
    if(node == NULL){
        printf("File not found\n");
        return;
    }
    if(node->isDirectory){
        printf("Cannot delete a directory using delete\n");
        return;
    }
    for(int i = 0; i < node->blockCount; i++){
        if(!freeBlocks(node->blockPointers[i])) {
            printf("Deletion failed: unable to return disk block\n");
            return;
        }
    }
    removeChild(node);
    free(node);
    printf("File deleted successfully.\n");
}

void rmdir(char name[]){
    fnode *node = search(name);
    if(node == NULL){
        printf("Directory not found\n");
        return;
    }
    if(!node->isDirectory){
        printf("Not a directory\n");
        return;
    }
    if(node->child != NULL){
        printf("Directory is not empty\n");
        return;
    }
    removeChild(node);
    free(node);
    printf("Directory removed successfully\n", name);
}

void writeFile(char name[], char content[]){
    fnode *file = search(name);
    if(file == NULL){
        printf("File not found\n");
        return;
    }
    if(file->isDirectory){
        printf("Cannot write to a directory\n");
        return;
    }
    int size = strlen(content);
    int requiredBlocks = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    if(requiredBlocks > MAXBLOCKS){
        printf("File is too large\n");
        return;
    }
    int freeCount = 0;
    freeBlock *temp = head;
    while(temp != NULL){
        freeCount++;
        temp = temp->next;
    }
    if(requiredBlocks > freeCount + file->blockCount){
        printf("Not enough disk space\n");
        return;
    }
    for(int i = 0; i < file->blockCount; i++){
        if(!freeBlocks(file->blockPointers[i])){
            printf("Unable to return old blocks\n");
            return;
        }
    }
    file->blockCount = 0;
    int offset = 0;
    for(int i = 0; i < requiredBlocks; i++){
        int block = allocateBlock();
        if(block == -1){
            printf("Allocation failed\n");
            return;
        }
        file->blockPointers[file->blockCount] = block;
        file->blockCount++;
        int remaining = size - offset;
        int bytesToWrite = remaining < BLOCKSIZE ? remaining : BLOCKSIZE;
        memcpy(virtualDisk[block], content + offset, bytesToWrite);
        offset += bytesToWrite;
    }
    file->fileSize = size;
    printf("Data written successfully (size=%d bytes).\n", size);
}

void readFile(char name[]){
    fnode *file = search(name);
    if(file == NULL){
        printf("File not found\n");
        return;
    }
    if(file->isDirectory){
        printf("Cannot read a directory\n");
        return;
    }
    if(file->fileSize == 0){
        printf("File is empty\n");
        return;
    }
    int remaining = file->fileSize;
    for(int i = 0; i < file->blockCount; i++){
        int block = file->blockPointers[i];
        int bytesToRead = remaining < BLOCKSIZE ? remaining : BLOCKSIZE;
        for(int j = 0; j < bytesToRead; j++){
            printf("%c", virtualDisk[block][j]);
        }
        remaining -= bytesToRead;
    }
    printf("\n");
}

void df(){
    int freeCount = 0;
    freeBlock *temp = head;
    while(temp != NULL){
        freeCount++;
        temp = temp->next;
    }
    int usedCount = NUM_BLOCKS - freeCount;
    float usage = ((float)usedCount / NUM_BLOCKS) * 100;

    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedCount);
    printf("Free Blocks: %d\n", freeCount);
    printf("Disk Usage: %.2f%%\n", usage);
}

void freeDirectory(fnode *dir){
    if(dir->child == NULL){
        return;
    }
    fnode *first = dir->child;
    fnode *temp = first;
    do{
        fnode *nextNode = temp->next;
        if(temp->isDirectory) {
            freeDirectory(temp);
        }
        free(temp);
        temp = nextNode;

    }while(temp != first);
    dir->child = NULL;
}

void cleanup(){
    freeDirectory(root);
    free(root);
    root = NULL;
    cwd = NULL;
    freeBlock *temp = head;
    while(temp != NULL){
        freeBlock *nextNode = temp->next;
        free(temp);
        temp = nextNode;
    }
    head = NULL;
    tail = NULL;
    printf("Memory released. Exiting program...\n");
}

void prompt(){
    if(cwd == root){
        printf("/ > ");
    }
    else{
        printf("%s > ", cwd->name);
    }
}

int main(){
    rootdir();
    initializeFreeBlocks();

    printf("Compact VFS - ready. Type 'exit' to quit.\n");
    char input[INPUTSIZE];
    while(1){
        prompt();
        if(fgets(input, sizeof(input), stdin) == NULL){
            cleanup();
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        char *ptr = input;
        while(*ptr == ' ' || *ptr == '\t'){
            ptr++;
        }
        if(*ptr == '\0'){
            continue;
        }
        char *command = ptr;
        while(*ptr != '\0' && *ptr != ' ' && *ptr != '\t'){
            ptr++;
        }
        if(*ptr != '\0'){
            *ptr = '\0';
            ptr++;
        }
        while(*ptr == ' ' || *ptr == '\t'){
            ptr++;
        }
        char *name = ptr;
        while(*ptr != '\0' && *ptr != ' ' && *ptr != '\t'){
            ptr++;
        }
        char *content = NULL;
        if(*ptr != '\0'){
            *ptr = '\0';
            ptr++;

            while(*ptr == ' ' || *ptr == '\t'){
                ptr++;
            }
            content = ptr;
        }
        if(strcmp(command, "mkdir") == 0){
            if(*name != '\0'){
                mkdir(name);
            }
            else{
                printf("Usage: mkdir directory_name\n");
            }
        }
        else if(strcmp(command, "create") == 0){
            if(*name != '\0'){
                create(name);
            }
            else{
                printf("Usage: create filename\n");
            }
        }
        else if(strcmp(command, "ls") == 0){
            ls();
        }
        else if(strcmp(command, "cd") == 0){
            if(*name != '\0'){
                cd(name);
            }
            else{
                printf("Usage: cd directory_name\n");
            }
        }
        else if(strcmp(command, "pwd") == 0){
            pwd();
        }
        else if(strcmp(command, "delete") == 0){
            if(*name != '\0'){
                delete(name);
            }
            else{
                printf("Usage: delete filename\n");
            }
        }
        else if(strcmp(command, "rmdir") == 0){
            if(*name != '\0'){
                rmdir(name);
            }
            else{
                printf("Usage: rmdir directory_name\n");
            }
        }
        else if(strcmp(command, "write") == 0){
            if(*name == '\0' || content == NULL){
                printf("Usage: write filename \"content\"\n");
                continue;
            }

            if(content[0] == '"' || content[0] == '\''){
                char quote = content[0];
                content++;
                size_t len = strlen(content);
                if(len == 0 || content[len - 1] != quote){
                    printf("Missing closing quote\n");
                    continue;
                }
                content[len - 1] = '\0';
            }
            writeFile(name, content);
        }
        else if(strcmp(command, "read") == 0){
            if(*name != '\0'){
                readFile(name);
            }
            else{
                printf("Usage: read filename\n");
            }
        }
        else if(strcmp(command, "df") == 0){
            df();
        }
        else if(strcmp(command, "exit") == 0){
            cleanup();
            break;
        }
        else{
            printf("Invalid command\n");
        }
    }
    return 0;
}