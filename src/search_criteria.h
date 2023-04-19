/*
===================================================================
Criterio de Busqueda

Se almacenan los argumentos de la busqueda recibida por el usuario
por la linea de comandos

Miembros:
- region:       Region en el cual el personaje hace su aparicion
- species:      Puede ser Pokemon o Trainer
- type:         Indica el tipo de aparicion
                esta puede ser: Main, recurring, gym_leader
                y one_time
- show_count:   Booleano para indicar si muestra la cuenta de 
                resultados obtenidos
-list_files:    Boleano, 1 si se debe mostrar la lista de archivos
-show_size:     Boleano, 1 si se debe mostrar el tamaño de los archivos
- name:         Nombre del Personaje

===================================================================
*/
#if !defined(SEARCH_CRITERIA_H)
#define SEARCH_CRITERIA_H

typedef struct search_criteria search_criteria;

#include <stdlib.h>

struct search_criteria {
    char *region;
    char *species;
    int type;
    int show_count;
    int list_files;
    int show_size;
    char *name;
};


#endif // SEARCH_CRITERIA_H