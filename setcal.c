#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 1024
#define ITEMLEN 30

// gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal
// ./setcal FILE

typedef struct{
    char **items;
    int *size_of_elem_arr;
    int cardinality;
}set_t;

void read_line(char *line, FILE *file);
int count_elems(char *line);
int* size_of_elem_array(char *line_string, int elem_count);
int read_universum(set_t *u, char *line_string);
char* truncate_line_string(char *line_string);
void fill_universum_items(set_t *set, char *line_string);
void print_universum(set_t universum);
int make_set(char *line_string, set_t universum);

int main(int argc, char* argv[]){

    char line_string[MAXLEN];
    set_t universum;


    char file_name[] = "file.txt";
    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }


    read_line( line_string, file );
     
    if( line_string[0] != 'U'){
        fprintf(stderr, "prvni mnozina neni universum");
        return -1;
    }

    if( read_universum( &universum, line_string ) ){
        return -1;
    }

    print_universum( universum );
    
    read_line( line_string, file );
    printf("\n%s", line_string);

    if ( make_set( line_string, universum ) ){
        return 1;
    }

    



fclose(file);  
return 0;    
}

// nacte dalsi radek souboru
void read_line(char *line, FILE *file){
    fgets( line, MAXLEN, file);
}

// nacteni universa do mnoziny
int read_universum(set_t *u, char *line_string){

    u->cardinality = count_elems( line_string );
    u->size_of_elem_arr = size_of_elem_array( line_string, u->cardinality ); 
    
    u->items = malloc(sizeof(char*) * u->cardinality );
    if( u->items == NULL ){
        fprintf(stderr, "chyba alokace universum");
        return 1;
    }

    for (int i = 0; i < u->cardinality; i++)
    {
        *( u->items + i ) = malloc(sizeof(char) * u->size_of_elem_arr[i] );
        if( ( u->items + i ) == NULL ){
            fprintf(stderr, "chyba alokace universum");
            return 1;
        }
    }

    strcpy( line_string,  truncate_line_string( line_string ) );
    fill_universum_items( u, line_string );

return 0; 
}

// spocitani elementu mnoziny
int count_elems(char *line){
    int elem_count = 0;

    for (int i = 0; line[i] != '\n' ; i++)
    {
        if( line[i] == ' ' )
            elem_count++;
    }

return elem_count;
}

// vytvoreni pole obsahujici velikost vsech prvku
int* size_of_elem_array(char *line_string, int elem_count){
    int char_count = 0;
    int *array;

    array = malloc(sizeof(int) * elem_count);

    int j = 0;
    for (int i = 2; line_string[i] != EOF ; i++)
    {
        if( line_string[i] == ' ' || line_string[i] == '\n'){
            array[ j ] = char_count;
            char_count = 0;
            j++;
        }else
            char_count++;
    }

return array;
}

// upraveni vstupniho retezce na osekany format pro zapis
char* truncate_line_string(char *line_string){
    static char line_string_enhanced[MAXLEN];

    int j = 0;
    for (int i = 2; line_string[i] != '\n' && line_string[i] != EOF; i++, j++)
    {
        if( line_string[i] != ' ' ){
            line_string_enhanced[j] = line_string[i];
        }
        else{
            j--;
        }
    }
    line_string_enhanced[j] = '\0';

return line_string_enhanced;
}

// naplneni universa hodnotami
void fill_universum_items(set_t *set, char *line_string){

    int k = 0;
    for (int i = 0; i < set->cardinality; i++)
    {
        for (int j = 0; j < set->size_of_elem_arr[i]; j++, k++)
        {
            set->items[i][j] = line_string[k];
        }      
    }
}

// vypise prvky universa
void print_universum(set_t universum){
    for (int i = 0; i < universum.cardinality; i++)
    {
        for (int j = 0; j < universum.size_of_elem_arr[i]; j++)
        {
            printf("%c", universum.items[i][j]);
        }
        printf(" ");
    }
}

int make_set(char *line_string, set_t u){
    int *set;
    int number_of_elem = count_elems( line_string );
    int *size_of_elem_arr = size_of_elem_array( line_string, number_of_elem);

    strcpy( line_string,  truncate_line_string( line_string ) );
    printf("\n%s\n", line_string);
    set = malloc(sizeof(int) * number_of_elem );

    printf("\n");
        for (int i = 0; i < number_of_elem; i++)
        {
            printf("%d ", size_of_elem_arr[i]);
        }
        printf("\n");


    // tohle odmitam vysvetlovat, protoze sam nevim, proc to funguje, tak jsem to radsi dal do try catch, aby jim nespadl merlin
    int streak = 0;
    int k = 0;
    int j = 0;
    int element = 0;
    for (int i = 0; line_string[i] != '\0'; i++)
    {
        streak = 0;
        k = 0;

        if(j == u.cardinality && j != 1){
            fprintf(stderr, "prvek neni v universu\n");
            return 1;
        }   

        for (j = 0; j < u.cardinality; j++)
        {
            if(line_string[i] == u.items[j][k]){
                j--;
                i++;
                k++;
                streak++;
            }
            else{
                streak = 0;
            }

            if(streak == size_of_elem_arr[element] && streak == u.size_of_elem_arr[j + 1]){
                set[element] = j + 1;
                i--;
                element++;
                break;
            }
        }
    }
     
     
    for (int i = 0; i < number_of_elem; i++)
    {
        printf("%d", set[i]);
    }
     

return 0;
}
