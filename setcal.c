#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 1024
#define ITEMLEN 30
#define MAXLINES 1000
#define NUM_OF_COMMANDS 19
#define COMMAND_MAXLEN 15

// gcc -std=c99 -Wall -Wextra -Werror setcal.c -o setcal
// ./setcal FILE
// valgrind --track-origins=yes --leak-check=full -s ./setcal      => memory leaks check
// prekladani s -g, aby valgrind ukazoval cisla radku s chybami:
// gcc -g -std=c99 -Wall -Wextra -Werror setcal.c -o setcal

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

bool read_line(char *line, FILE *file);
int count_sets(char *line, FILE *file );
int count_elems(char *line);
int* size_of_elem_array(char *line_string, int elem_count);
int read_universum(universum_t *u, char *line_string);
char* truncate_line_string(char *line_string);
void fill_universum_items(universum_t *set, char *line_string);
void print_universum(universum_t universum);
set_t make_set(char *line_string, universum_t u);
void print_set(set_t set, universum_t u, int number_of_set);
//void print_command_result(char *line_string, set_t* set, char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN]);

int main(){

    char line_string[MAXLEN] = "";
    /**
    char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN] = {
        "empty", "card", "complement", "union", "intersect",
        "minus", "subseteq", "subset", "equals", "reflexive",
        "symmetric", "antisymmetric", "transitive", "function", "domain",
        "codomain", "injective", "surjective", "bijective"
    };
    */
    int sets_in_file = 0;
    int commands_in_file = 0;

    universum_t *universum = malloc(sizeof(universum_t));
    if( universum == NULL ){
        fprintf(stderr, "chyba alokace univerza");
        return -1;
    }

    set_t *set;

    char file_name[] = "file.txt";
    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }

    sets_in_file = count_sets( line_string, file );

    if (sets_in_file > (MAXLINES - 1)) { // (MAXLINES - 1) = (MAXLINES - radek univerza)
        fprintf(stderr, "presazen podporovany pocet radku");
        return -1;
    }

    rewind(file);

    // alokuj pamet pro pole setu
    set = malloc(sets_in_file*sizeof(set_t));
    if( set == NULL ){
        fprintf(stderr, "chyba alokace pole setu");
        return -1;
    }

    read_line( line_string, file ); 
    if( line_string[0] != 'U'){    // podminka na maximalni delku prvku ITEMLEN
        fprintf(stderr, "prvni mnozina neni platne universum");
        return -1;
    }

    if( read_universum( universum, line_string ) ){
        return -1;
    }
    printf("universum: ");
    print_universum( *universum );
    printf("\n");
    
    for (int i = 0; i < sets_in_file; i++)
    {
        read_line( line_string, file );
        set[i] = make_set(line_string, *universum);
        print_set(set[i], *universum, i);
    }

    // cteni a provadeni prikazu
    while (read_line( line_string, file )) {
        
        commands_in_file++;
        if ((sets_in_file + commands_in_file) > (MAXLINES - 1)) {
            fprintf(stderr, "presazen podporovany pocet radku");
            return -1;
        }

        //print_command_result(line_string, set, commands);
    }
    
    for (int i = 0; i < sets_in_file; i++)
    {
        free(set[i].set);
        free(set[i].size_of_elem_arr);
    }

    for (int i = 0; i < universum->cardinality; i++)
    {
        free(universum->items[i]);
    }
    free(universum->items);
    free(universum->size_of_elem_arr);
    free(universum);
    free(set);
    

fclose(file);  
return 0;    
}

// nacte dalsi radek souboru
bool read_line(char *line, FILE *file){
    if (fgets( line, MAXLEN, file) != NULL) {
        return true;
    }
    return false;
}

// spocitani mnozin a relaci v souboru
int count_sets(char *line, FILE *file ) {

    int sets_in_file = 0;

    while (fgets( line, MAXLEN, file ) != NULL)
    {
        if ( line[0] == 'S' || line[0] == 'R') {
            sets_in_file++;
        }
    }

    return sets_in_file;
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

    for (int i = 1; line[i] != '\n' ; i++)
    {
        if( line[i] == ' ' ){
            if( line[i + 1] != ' ' && line[i - 1] != ' ' && line[i + 1] != '\n')    // osetreni mezer navic
            elem_count++;
        }
            
    }

return elem_count;
}

// vytvoreni pole obsahujici velikost vsech prvku
int* size_of_elem_array(char *line_string, int elem_count){
    int char_count = 0;
    int *array = malloc(sizeof(int) * elem_count);

    int j = 0;
    for (int i = 2; i < MAXLEN; i++) // puvodni podminka line_string[i] != EOF
    {
        if( line_string[i] == ' ' ){
            array[ j ] = char_count;
            char_count = 0;
            j++;
        }else if( line_string[i] == '\n'){
            array[ j ] = char_count;
            char_count = 0;
            j++;
            break;
        }else{
            char_count++;
            
        }
            
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

// vytvori set ci mnozinu (pracujeme s tim stejne)
set_t make_set(char *line_string, universum_t u){
    set_t set;
    set.cardinality = count_elems( line_string );

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
            if( set.cardinality % 2  ){
                fprintf(stderr, "neplatne zadana relace");  // osetreni uplnosti relace (ma sudy pocet prvku => kazdy prvek je ve dvojci)
                exit( 1 );
            }
        }
    }
    strcpy( line_string,  truncate_line_string( line_string ) );
    

    set.set = malloc(sizeof(int) * set.cardinality );

    // tohle odmitam vysvetlovat, protoze sam nevim, proc to funguje... snad jim to neshodi merlina
    // "if it works, it works"
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
            }else if( streak == set.size_of_elem_arr[element] && streak != u.size_of_elem_arr[j + 1]){
                i = i - streak;
                streak = 0;
                j++;
                k = 0;
            }
        }
    }
 
return set;
}

// vypise mnozinu ci relaci
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

// precti prikaz a zavolej funkci pro provedeni prikazu
/**
void print_command_result(char *line_string, set_t* set, char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN]) {

    if (line_string[0] != 'C' && line_string[1] != ' ') {
        fprintf(stderr, "chybne zadany prikaz");
        exit( 1 );
    }

    char cur_command[COMMAND_MAXLEN];
    for (int i = 0; i < COMMAND_MAXLEN; i++) {
        if (line_string[i + 2] == ' ') {
            break;
        }
        cur_command[i] = line_string[i + 2];
    }
    printf("%s\n", cur_command); // vymazat

    for (int i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp( cur_command, commands[i] ) == 0) {
            printf ("%d\n", i);
        }
    }

    printf("%d\n", set[2].set[2]);

}
*/