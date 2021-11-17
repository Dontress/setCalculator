#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 1024

// gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal
// ./setcal FILE

typedef struct{
    char* items;
    int cardinality;
}set_t;

void read_next_line(char* array, FILE* file);
bool is_last_line(char* file_line);

int main(int argc, char* argv[]){

    char file_name[10] = {'f', 'i', 'l', 'e', '.', 't', 'x', 't'}; 
    char file_line[MAXLEN];
    bool lastLine = false;

    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }
    
    do
    {
        read_next_line( file_line, file);
        printf("%s", file_line);

    } while ( is_last_line( file_line ) == false );
    

fclose(file);  
return 0;    
}

bool set_constructor(set_t* set, int cardinality){
    set->cardinality = cardinality;

    set->items = malloc(sizeof(char) * cardinality);
    if( set->items == NULL ){
        fprintf(stderr, "set allocation failed");
        return 0;
    }

return 1;
}

void read_next_line(char* file_line, FILE* file){
    fgets( file_line, MAXLEN, file);
}

bool is_last_line(char* file_line){
    if( strchr(file_line, '\n') == false )
        return true;
    else
        return false;
}

