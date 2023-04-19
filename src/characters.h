/*
==================================================================
Estructura Character para los personajes de la serie

Miembros:
- name:         Nombre del Personaje
- region:       Region en el cual el personaje hace su aparicion
- species:      Puede ser Pokemon o Trainer
- type:         Indica el tipo de aparicion
                esta puede ser: Main, recurring, gym_leader
                y one_time
- file_size:    Indica el tamaño del archivo en kilobytes

==================================================================
*/


#if !defined(CHARACTERS_H)
#define CHARACTERS_H

typedef struct character character;

#include <stdlib.h>

struct character
{
    char *name;
    char *region;
    char *species;
    int type;
    unsigned long file_size;
};

#endif // CHARACTERS_H