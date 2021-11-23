Update 23.11. - postup a rozdeleni tasku  
1) Petr - komentare a uprava kodu (mozna :D )  
2) Libor - funkce pro cteni prikazu  
3) Kazdy vytvoreni sve vlastni branche
4) naimplementovani jednotlivych funkci pro provedeni prikazu a tisku vysledku  
-> Julie - prvni tri prikazy z mnozin i relaci  
-> Tomas - ostatni prikazy z mnozin  
-> Libor - ostatni prikazy z relaci  

Update 22.11. nacitani mnozin i relaci funguje (snad), kdyztak napiste a ja vam pomuzu s pochopenim.
Jeste zitra dodelam nejako komentare a mozna fakt mozna upravim kod.


Last update:
21.11. 15:10 funguje a je docela upravene nacteni universa, vypis univerza a osetrene kdyz v souboru jako prvni neni universum

18:58: funguju nacteni mnoziny ze souboru ( mnozina je pole intů, které říkají, jaký má prvnek index v universum )



Načetl bych univerzum a uložil ho do jedné struktury. Další množiny bych načetl, našel jaké mají jejich prvky indexy v univerzu. 
To znamená že budeme mít jenom jeden pointer na pointer ve kterém budou uložené všechny přípustné prvky množin a všechny ostatní množiny budou čistě pole intů s indexy prvků podle universa.

universum:
U Apple Lemon Orange Banana Peach

v kodu:
{  Apple, Lemon, Orange, Banana, Peach } 


ostatní množiny: 
S Apple Banana Peach

v kodu:
{ 0, 3, 4 }
