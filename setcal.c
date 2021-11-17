#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 1024
#define ITEMLEN 30

// gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal
// ./setcal FILE

typedef struct{
    char *items;
    int cardinality;
}set_t;

void read_next_line(char* array, FILE* file);
bool is_last_line(char* file_line);
void string_to_set(set_t* set, char* file_line, int line_number);
bool set_constructor(set_t* set, int cardinality);

int main(int argc, char* argv[]){

    int line_number = 0;
    char file_name[10] = {'f', 'i', 'l', 'e', '.', 't', 'x', 't'}; 
    char file_line[MAXLEN];
    bool lastLine = false;
    set_t set[10];

    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }
    
    // hlavni do-while pro cteni souboru po radku
    do
    {
        read_next_line( file_line, file);
        printf("%s", file_line);

        if( file_line[0] == 'U' || file_line[0] == 'S' ){
            string_to_set( &set[line_number] ,file_line, line_number);
        }
            
        printf("%d\n", set[line_number].cardinality);


        line_number++;
    } while ( is_last_line( file_line ) == false );
    

fclose(file);  
return 0;    
}

void string_to_set(set_t* set, char* file_line, int line_number){

    int number_of_items = 0;

    for (int i = 0; file_line[i] != '\0'; i++)
    {
        if( file_line[i] == ' ' )
            number_of_items++;
    }

    set->cardinality = number_of_items;

    set->items = malloc(sizeof(char) * number_of_items * ITEMLEN);
    if(set->items == NULL){
        fprintf(stderr, "set allocation failed");
    }

   
   
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

