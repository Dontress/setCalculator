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
