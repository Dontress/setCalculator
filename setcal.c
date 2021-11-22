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
}universum_t;

typedef struct{
    int *set;
    int *size_of_elem_arr;
    int cardinality;
}set_t;

void read_line(char *line, FILE *file);
int count_elems(char *line);
int* size_of_elem_array(char *line_string, int elem_count);
int read_universum(universum_t *u, char *line_string);
char* truncate_line_string(char *line_string);
void fill_universum_items(universum_t *set, char *line_string);
void print_universum(universum_t universum);
set_t make_set(char *line_string, universum_t u);
void print_set(set_t set, universum_t u, int number_of_set);

int main(){

    char line_string[MAXLEN];
    universum_t universum;
    set_t set[10];

    char file_name[] = "file.txt";
    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }

    read_line( line_string, file ); 
    if( line_string[0] != 'U' ){    // podminka na maximalni delku prvku ITEMLEN
        fprintf(stderr, "prvni mnozina neni platne universum");
        return -1;
    }

    if( read_universum( &universum, line_string ) ){
        return -1;
    }
    //print_universum( universum );
    
    read_line( line_string, file );
    set[0] = make_set(line_string, universum);
    print_set(set[0], universum, 0);

    read_line( line_string, file );
    set[1] = make_set(line_string, universum);
    print_set(set[1], universum, 1);

    read_line( line_string, file );
    set[2] = make_set(line_string, universum);
    print_set(set[2], universum, 2);

fclose(file);  
return 0;    
}

// nacte dalsi radek souboru
void read_line(char *line, FILE *file){
    fgets( line, MAXLEN, file);
}

// nacteni universa do mnoziny
int read_universum(universum_t *u, char *line_string){

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
        if( line_string[i] != ' ' && line_string[i] != '(' && line_string[i] != ')' ){
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
void fill_universum_items(universum_t *set, char *line_string){

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
void print_universum(universum_t universum){
    for (int i = 0; i < universum.cardinality; i++)
    {
        for (int j = 0; j < universum.size_of_elem_arr[i]; j++)
        {
            printf("%c", universum.items[i][j]);
        }
        printf(" ");
    }
}

set_t make_set(char *line_string, universum_t u){
    set_t set;
    set.cardinality = count_elems( line_string );
    //set.size_of_elem_arr = size_of_elem_array( line_string, set.cardinality );

    int char_count = 0;
    set.size_of_elem_arr = malloc(sizeof(int) * set.cardinality  );

    int j = 0;
    for (int i = 2; line_string[i] != EOF && j < set.cardinality ; i++)
    {
        if( line_string[i] == ' ' || line_string[i] == '\n'){
            set.size_of_elem_arr[ j ] = char_count;
            char_count = 0;
            j++;
        }else
            char_count++;
    }

    
    

    if(line_string[0] == 'R'){
    for (int i = 0; i < set.cardinality; i++)
        {
            set.size_of_elem_arr[i] -= 1;
        }
    }
    strcpy( line_string,  truncate_line_string( line_string ) );
    

    set.set = malloc(sizeof(int) * set.cardinality );

    // tohle odmitam vysvetlovat, protoze sam nevim, proc to funguje... snad jim to neshodi merlina
    int streak = 0;
    int k = 0;
    j = 0;
    int element = 0;
    for (int i = 0; line_string[i] != '\0'; i++)
    {
        streak = 0;
        k = 0;

        if(j == u.cardinality && j != 1){
            fprintf(stderr, "chybne zadani mnoziny ci relace\n");
            exit( 1 );
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

            if(streak == set.size_of_elem_arr[element] && streak == u.size_of_elem_arr[j + 1]){
                set.set[element] = j + 1;
                i--;
                element++;
                break;
            }
        }
    }
 
return set;
}

void print_set(set_t set, universum_t u, int number_of_set){
    printf("set %d is: \n", number_of_set);
    for (int i = 0; i < set.cardinality; i++)
    {
        for (int j = 0; j < set.size_of_elem_arr[i]; j++)
        {
            printf("%c", u.items[ set.set[i] ][ j ]);
        }
        printf(" ");
    }
    printf("\n");
}
