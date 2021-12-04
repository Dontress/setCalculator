#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAXLEN 1024
#define ITEMLEN 30
#define MAXLINES 1000
#define NUM_OF_COMMANDS 19
#define COMMAND_MAXLEN 15
#define SET_ID_MAXLEN 4

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
    char set_or_rel;
    int *set;
    int *size_of_elem_arr;
    int cardinality;
}set_t;

bool read_line(char *line, FILE *file);
int count_sets(char *line, FILE *file );
int count_elems(char *line);
int* size_of_elem_array(char *line_string, int elem_count);
int read_universum(universum_t *u, char *line_string);
void universum_to_set(universum_t *u, set_t *set);
char* truncate_line_string(char *line_string);
void fill_universum_items(universum_t *set, char *line_string);
void print_universum(universum_t universum);
set_t make_set(char *line_string, universum_t u);
void print_set(set_t set, universum_t u, int number_of_set);
void print_command_result(char *line_string, set_t* set, universum_t u, char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN]);
int load_set_id(char *line_string, int *pos_after_command);
void copy_set(int *set_1, int *set_2, int n);
void transitive(set_t set);
void function(set_t set);
void domain(set_t set, universum_t u);
void codomain(set_t set, universum_t u);

int main(){

    char line_string[MAXLEN] = "";
    char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN] = {
        "empty", "card", "complement", "union", "intersect",
        "minus", "subseteq", "subset", "equals", "reflexive",
        "symmetric", "antisymmetric", "transitive", "function", "domain",
        "codomain", "injective", "surjective", "bijective"
    };
    int sets_in_file = 0;
    //int commands_in_file = 0;
    int line_no = 0;

    universum_t *universum = malloc(sizeof(universum_t));
    if( universum == NULL ){
        fprintf(stderr, "chyba alokace univerza");
        return -1;
    }

    

    char file_name[] = "file.txt";
    FILE* file;
    file = fopen( file_name, "r" );
    if(file == NULL){
        fprintf(stderr, "cannot open file");
        return -1;
    }

    /**
    sets_in_file = count_sets( line_string, file );

    if (sets_in_file > (MAXLINES - 1)) { // (MAXLINES - 1) = (MAXLINES - radek univerza)
        fprintf(stderr, "presazen podporovany pocet radku");
        return -1;
    }
*/

    set_t *set = malloc(sizeof(set_t));
    if( set == NULL ){
        fprintf(stderr, "chyba alokace setu");
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

    // ulozi univerzum jako set a vytiskne ho
    
    sets_in_file++;
    universum_to_set(universum, set);
    //set[line_no] = make_set(line_string, *universum);
    print_set(set[line_no], *universum, line_no);
    //printf("universum: ");
    //print_universum( *universum );
    //printf("\n");

    // cte, uklada a tiskne mnoziny a relace ze souboru
    while (read_line( line_string, file ) && (line_string[0] == 'S' || line_string[0] == 'R')) {

        line_no++;
        if (line_no > (MAXLINES - 1)) {
            fprintf(stderr, "presazen podporovany pocet radku");
            return -1;
        }

        set = realloc(set, sizeof(set_t)*(line_no + 1));
        if( set == NULL ){
            fprintf(stderr, "chyba alokace pole setu");
            return -1;
        }
        sets_in_file++;
        set[line_no] = make_set(line_string, *universum);
        print_set(set[line_no], *universum, line_no);
    }

    /**
    for (int i = 0; i < sets_in_file + 1; i++)
    {
        read_line( line_string, file );
        set[i] = make_set(line_string, *universum);
        print_set(set[i], *universum, i);
    }
    */

    // cteni a provadeni prikazu
    while (read_line( line_string, file ) && line_string[0] == 'C') {

        line_no++;
        if (line_no > (MAXLINES - 1)) {
            fprintf(stderr, "presazen podporovany pocet radku");
            return -1;
        }

        print_command_result(line_string, set, *universum, commands);
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

void universum_to_set(universum_t *u, set_t *set) {
    set->set = malloc(sizeof(int) * u->cardinality );
    if( set->set == NULL ){
        fprintf(stderr, "chyba alokace set");
        exit( 1 );
    }
    set->size_of_elem_arr = malloc(sizeof(int) * u->cardinality);
    if( set->size_of_elem_arr == NULL ){
        fprintf(stderr, "chyba alokace set");
        exit( 1 );
    }
    for (int i = 0; i < u->cardinality; i++) {
        set->set[i] = i;
        set->size_of_elem_arr[i] = u->size_of_elem_arr[i];
    }
    set->cardinality = u->cardinality;
    set->set_or_rel = 'S';
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

    if ((line_string[0] == 'S') || (line_string[0] == 'R')) {
        set.set_or_rel = line_string[0];
    } else if (line_string[0] == 'U') {
        set.set_or_rel = 'S';
    }

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
                fprintf(stderr, "neplatne zadana relace\n");  // osetreni uplnosti relace (ma sudy pocet prvku => kazdy prvek je ve dvojci)
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
        /**
        if(j == u.cardinality && j != 1){
            fprintf(stderr, "chybne zadani mnoziny ci relace\n");
            exit( 1 );
        }   
        */
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
    if (number_of_set == 0) {
        printf("U ");
    } else {
        printf("%c ", set.set_or_rel);
    }

    for (int i = 0; i < set.cardinality; i++)
    {
        if (set.set_or_rel == 'R' && (i % 2 == 0)) {
            printf("(");
        }
        for (int j = 0; j < set.size_of_elem_arr[i]; j++)
        {
            printf("%c", u.items[ set.set[i] ][ j ]);
        }
        if (set.set_or_rel == 'R' && (i % 2 == 1)) {
            printf(")");
        }
        printf(" ");
    }
    printf("\n");
}

// precti prikaz a zavolej funkci pro provedeni prikazu
void print_command_result(char *line_string, set_t* set, universum_t u, char commands[NUM_OF_COMMANDS][COMMAND_MAXLEN]) {

    if (line_string[0] != 'C' || line_string[1] != ' ' ||  line_string[2] < 'a' || line_string[2] > 'z') {
        fprintf(stderr, "chybne zadany prikaz\n");
        exit( 1 );
    }

    // zjisti nazev prikazu
    char cur_command[COMMAND_MAXLEN] = "";
    int pos_after_command = -1;
    for (int i = 0; i < COMMAND_MAXLEN; i++) {
        if (line_string[i + 2] == ' ') {
            pos_after_command = (i + 2);
            break;
        }
        cur_command[i] = line_string[i + 2];
    }

    // vrat id prikazu z pole prikazu, pokud nenajdes, vypis chybu
    int cur_command_id = -1;
    for (int i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp( cur_command, commands[i] ) == 0) {
            cur_command_id = i;
            break;
        }
    }
    if (cur_command_id == -1) {
        fprintf(stderr, "prikaz neexistuje\n");
        exit( 1 );
    }

    // zjisti, jestli je za prikazem zadane id setu
    int *pos_after_command_ptr = &pos_after_command;
    int command_set_id_1 = load_set_id(line_string, pos_after_command_ptr);

    // na zaklade zadaneho prikazu zkontroluj, jestli je zadany spravny pocet mnozin ci relaci
    // a over, jestli se skutecne jedna o mnozinu ci relaci
    if (cur_command_id >= 0 && cur_command_id <= 2) {     // empty - complement: 1 mnozina na vstupu
        if (line_string[ pos_after_command ] != '\n') {
            fprintf(stderr, "chybne zadany prikaz\n");
            exit( 1 );
        }
        if (set[ command_set_id_1 ].set_or_rel != 'S') {
            fprintf(stderr, "prikaz ocekaval 1 argument typu mnozina\n");
            exit( 1 );
        }

        // sem funkce

    } else if (cur_command_id >= 3 && cur_command_id <= 8) {     // union - equals: 2 mnoziny na vstupu
        int command_set_id_2 = load_set_id(line_string, pos_after_command_ptr);

        if (line_string[ pos_after_command ] != '\n') {
            fprintf(stderr, "chybne zadany prikaz\n");
            exit( 1 );
        }
        if (set[ command_set_id_1 ].set_or_rel != 'S' || set[ command_set_id_2 ].set_or_rel != 'S') {
            fprintf(stderr, "prikaz ocekaval 2 argumenty typu mnozina\n");
            exit( 1 );
        }

        // sem funkce

    } else if (cur_command_id >= 9 && cur_command_id <= 15) {    // reflexive - codomain: 1 relace na vstupu
        if (line_string[ pos_after_command ] != '\n') {
            fprintf(stderr, "chybne zadany prikaz\n");
            exit( 1 );
        }
        if (set[ command_set_id_1 ].set_or_rel != 'R') {
            fprintf(stderr, "prikaz ocekaval 1 argument typu relace\n");
            exit( 1 );
        }

        // sem funkce
        // priklad:
        switch (cur_command_id) {
            case 12:
                transitive ( set[ command_set_id_1 ] );
                break;
            case 13:
                function ( set[ command_set_id_1 ] );
                break;
            case 14:
                domain( set[ command_set_id_1 ], u );
                break;
            case 15:
                codomain( set[ command_set_id_1 ], u );
                break;
        }

    } else if (cur_command_id >= 16 && cur_command_id <= 18) {     // injective - bijective: 1 relace a 2 mnoziny
        int command_set_id_2 = load_set_id(line_string, pos_after_command_ptr);
        int command_set_id_3 = load_set_id(line_string, pos_after_command_ptr);

        if (line_string[ pos_after_command ] != '\n') {
            fprintf(stderr, "chybne zadany prikaz\n");
            exit( 1 );
        }
        if (set[ command_set_id_1 ].set_or_rel != 'R' ||
            set[ command_set_id_2 ].set_or_rel != 'S' ||
            set[ command_set_id_3 ].set_or_rel != 'S') {
            fprintf(stderr, "prikaz ocekaval prvni argument typu relace a dalsi 2 argumenty typu mnozina\n");
            exit( 1 );
        }

        // sem funkce

    }

}

// nacte id mnoziny u prikazu
int load_set_id(char *line_string, int *pos_after_command) {

    int position = *pos_after_command;
    char command_set[SET_ID_MAXLEN] = "";
    int command_set_id = 0;

    position++;
    if ( line_string[ position ] == ' ' || line_string[ position ] == '\n' || line_string[ position ] == '\0') {
            fprintf(stderr, "u prikazu chybi zadana mnozina\n");
            exit( 1 );
    }
    for (int i = 0; i < SET_ID_MAXLEN; i++) {
        if ( line_string[ position ] >= '0' && line_string[ position ] <= '9' ) {
            command_set[i] = line_string[ position ];
        } else if ( line_string[ position ] == ' ' || line_string[ position ] == '\n' ) {
            command_set_id = atoi(command_set);
            break;
        } else {
            fprintf(stderr, "u prikazu byl zadan neciselny identifikator mnoziny\n");
            exit( 1 );
        }
        position++;
    }
    *pos_after_command = position;

    if (atoi(command_set) > 1000 || atoi(command_set) < 1) {
        fprintf(stderr, "u prikazu byl zadan neplatny identifikator mnoziny\n");
        exit( 1 );
    }

    return (command_set_id - 1); // odecitame 1, protoze v argumentech se indexuje od 1
}

// zkopiruje pole integeru
void copy_set(int *set_1, int *set_2, int n) {
    for (int i = 0; i < n; i++) {
        set_1[i] = set_2[i];
    }
}

// 12 - tranzitivita relace
void transitive(set_t set) {

    bool is_transitive = false;

    for (int i = 0; i < set.cardinality; i += 2) {
        for (int j = 0; j < set.cardinality; j += 2) {
            if (set.set[i + 1] == set.set[j]) {     // aRb && bRc

                is_transitive = false;

                for (int k = 0; k < set.cardinality; k += 2) {
                    if (( set.set[i] == set.set[k] ) && ( set.set[j + 1] == set.set[k + 1] )) {     // aRc
                        is_transitive = true;
                        break;
                    }
                }

                if (!is_transitive) {
                    break;
                }
            }
        }
        if (!is_transitive) {
            printf("false\n");
            break;
        }
    }

    if (is_transitive) {
        printf("true\n");
    }

}

// 13 - zjisti jestli je relace funkci
void function(set_t set) {

    bool is_function = true;

    for (int i = 0; i < set.cardinality; i += 2) {
        for (int j = (i + 2); j < set.cardinality; j += 2) {
            if ( set.set[i] == set.set[j] && set.set[i + 1] != set.set[j + 1] ) {
                is_function = false;
                break;
            }
        }

        if (!is_function) {
            printf("false\n");
            break;
        }
    }

    if (is_function) {
        printf("true\n");
    }

}

// 14 - definicni obor relace
void domain(set_t set, universum_t u) {

    printf("XD");

    int cardinality = set.cardinality;
    int *copied_set = malloc(sizeof(int) * cardinality);
    if( copied_set == NULL ){
        fprintf(stderr, "chyba alokace setu");
        exit ( 1 );
    }
    copy_set(copied_set, set.set, cardinality);

    // usporadej prvni prvky dvojic do prvni poloviny pole
    for (int i = 0; i < (cardinality / 2); i++) {
        copied_set[i] = copied_set[2*i];
    }
    cardinality = (cardinality / 2);

    // oznac opakujici se prvky ktere nebudes tisknout
    for (int i = 0; i < cardinality; i++) {
        for (int j = (i + 1); j < cardinality; j++) {
            if ( copied_set[i] == copied_set[j]) {
                copied_set[j] = -1;
            }
        }
    }

    // tiskni vysledek
    printf("S ");
    for (int i = 0; i < cardinality; i++) {
        if (copied_set[i] == -1) {
            continue;
        }
        for (int j = 0; j < u.size_of_elem_arr[ copied_set[i] ]; j++) {
            printf("%c", u.items[ copied_set[i] ][ j ]);
        }
        printf(" ");
    }
    printf("\n");
    
    free(copied_set);
}

// 15 - obor hodnot relace
void codomain(set_t set, universum_t u) {

    int cardinality = set.cardinality;
    int *copied_set = malloc(sizeof(int) * cardinality);
    if( copied_set == NULL ){
        fprintf(stderr, "chyba alokace setu");
        exit ( 1 );
    }
    copy_set(copied_set, set.set, cardinality);

    // usporadej druhe prvky dvojic do prvni poloviny pole
    for (int i = 0; i < (cardinality / 2); i++) {
        copied_set[i] = copied_set[2*i + 1];
    }
    cardinality = (cardinality / 2);

    // oznac opakujici se prvky ktere nebudes tisknout
    for (int i = 0; i < cardinality; i++) {
        for (int j = (i + 1); j < cardinality; j++) {
            if ( copied_set[i] == copied_set[j]) {
                copied_set[j] = -1;
            }
        }
    }

    // tiskni vysledek
    printf("S ");
    for (int i = 0; i < cardinality; i++) {
        if (copied_set[i] == -1) {
            continue;
        }
        for (int j = 0; j < u.size_of_elem_arr[ copied_set[i] ]; j++) {
            printf("%c", u.items[ copied_set[i] ][ j ]);
        }
        printf(" ");
    }
    printf("\n");
    
    free(copied_set);
}

