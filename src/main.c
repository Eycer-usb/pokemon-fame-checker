#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include "search_criteria.h"
#include "characters.h"

/*
================================================================
Se Definen las constantes de caracterizacion de los directorios
que conforman la estructura de las rutas hacia los archivos
================================================================
*/
#define MAIN 0
#define RECURRING 1
#define GYM_LEADER 2
#define ONE_TIME 3

/*
==============================================================
Funcion interpretadora de los argumentos y flags
recibidos desde la linea de comando al ejecutarse
el programa principal

Argumentos:
- argc:             Cantidad de argumentos en la linea de comandos
- argv:             Arreglo con argumentos recibidos
- search_criteria:  Estructura de criterio de busqueda

Respuesta:
- Almacena los criterios de busqueda en la estructura search_criteria

==============================================================
*/
void parse_arguments(int argc, char *argv[], search_criteria *criteria) {
    int opt;
    static struct option long_options[] = {
        {"nocount", no_argument, 0, 'c'},
        {"list", no_argument, 0, 'l'},
        {"size", no_argument, 0, 'z'},
        {0, 0, 0, 0}
    };
    int long_index = 0;

    while ((opt = getopt_long(argc, argv, "r:s:t:clz", long_options, &long_index)) != -1) {
        switch (opt) {
            case 'r':
                criteria->region = optarg;
                break;
            case 's':
                criteria->species = optarg;
                break;
            case 't':
                if (strcmp(optarg, "main") == 0) {
                    criteria->type = MAIN;
                } else if (strcmp(optarg, "recurring") == 0) {
                    criteria->type = RECURRING;
                } else if (strcmp(optarg, "gym_leader") == 0) {
                    criteria->type = GYM_LEADER;
                } else if (strcmp(optarg, "one_time") == 0) {
                    criteria->type = ONE_TIME;
                } else {
                    fprintf(stderr, "Error: Tipo de aparicion no valido.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c':
                criteria->show_count = 0;
                break;
            case 'l':
                criteria->list_files = 1;
                break;
            case 'z':
                criteria->show_size = 1;
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
    criteria->name = strdup(argv[optind]);
}
}

/*
============================================================================
Funcion boolena, indica si un archivo coincide con el criterio de busqueda
almacenada en la estructura search_criteria

Argumentos:
- filename: Nombre del archivo a consultar
- path: path de ubicacion del archivo a consultar
- search_criteria: Estuctura criterio de busqueda

Respuesta:
- true si el nombre del archivo cumple con el criterio de busqueda
    o si el path no pertenece al criterio de busqueda
- false si el nombre del archivo NO cumple el criterio de busqueda
    o si el path no pertenece al criterio de busqueda
============================================================================

*/
bool is_match(const char *filename, const char *path, search_criteria *criteria) {
    // Comprueba si el nombre del archivo contiene el nombre proporcionado en los criterios de busqueda
    if (criteria->name != NULL && strstr(filename, criteria->name) == NULL) {
        return false;
    }

    // Comprueba si el tipo de aparicion coincide con los criterios de busqueda
    if (criteria->type != -1) {
        if (criteria->type == MAIN && strstr(path, "main") == NULL) {
            return false;
        } else if (criteria->type == RECURRING && strstr(path, "recurring") == NULL) {
            return false;
        } else if (criteria->type == GYM_LEADER && strstr(path, "gym_leader") == NULL) {
            return false;
        } else if (criteria->type == ONE_TIME && strstr(path, "one_time") == NULL) {
            return false;
        }
    }
    return true;
}

/*
============================================================================
Funcion recursiva para procesar el directorio especificado en funcion 
de los criterios de busqueda indicados en la estructura search_criteria

Argumentos:
- path:         String con la ruta del directorio a procesar
- criteria:     Criterio de busqueda
- characters:   Personajers
- count:        Contador de cantidad de resultados obtenidos
- depht:        Profundidad de la recursividad.

Resultado:
- En el argumento count se almacena el numero de coincidencias encontradas
============================================================================

*/
void process_directory(const char *path, search_criteria *criteria,
                        character **characters, int *count, int depth) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(path)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    /*
    Se navega recursivamente dentro de los directorios partiendo desde la
    ruta recibida como argumento
    */
    while ((entry = readdir(dir)) != NULL) {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);

        // Si se alcanza un directorio se estudian los elementos internos
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            switch (depth) {
                case 0: // Estamos en el directorio raiz
                    if (criteria->region == NULL || strcmp(criteria->region, entry->d_name) == 0) {
                        process_directory(new_path, criteria, characters, count, depth + 1);
                    }
                    break;
                case 1: // Estamos en un directorio de una region
                    if (criteria->species == NULL || strcmp(criteria->species, entry->d_name) == 0) {
                        process_directory(new_path, criteria, characters, count, depth + 1);
                    }
                    break;
                case 2: // Estamos en un directorio de especie (pokemon o trainer)
                    process_directory(new_path, criteria, characters, count, depth + 1);
                    break;
                default:
                    break;
            }
        // Si se alcanza un archivo se analiza y procesa segun los criterios de 
        // busqueda
        } else if (entry->d_type == DT_REG) {
            // Filtrar solo archivos HTML
            char *file_ext = strrchr(entry->d_name, '.');
            if (file_ext != NULL && strcmp(file_ext, ".html") == 0) {
                if (is_match(entry->d_name, path, criteria)) {
                    // Agregar el archivo a la lista de personajes
                    *characters = realloc(*characters, (*count + 1) * sizeof(character));
                    (*characters)[*count].name = strdup(entry->d_name);

                    // Obtener el tamaño del archivo
                    stat(new_path, &file_stat);
                    (*characters)[*count].file_size = file_stat.st_size;

                    (*count)++;
                }
            }
        }
    }

    closedir(dir);
}

/*
============================================================================

Funcion para imprimir los resultados de acuerdo con los criterios de busqueda

Argumentos:
characters:     Estructura de Personajes
count:          Cantidad de archivos encontrados o coincidencias de busqueda
criteria:       Criterio de Busqueda

Respuesta
Se imprime por la salida estandar los resultados obtenidos
segun los criterios de busqueda recibidos

============================================================================
*/
void print_results(character *characters, int count, search_criteria *criteria) {
    
    if (criteria->show_count) {
        printf("Cantidad de archivos encontrados: %d\n", count);
    }

    if (criteria->list_files) {
        for (int i = 0; i < count; i++) {
            if (criteria->show_size) {
                printf("%s (%lu KB)\n", characters[i].name, characters[i].file_size / 1024);
            } else {
                printf("%s\n", characters[i].name);
            }
        }
    }
}

/*
============================================================================

Funcion para liberar la memoria utilizada por las estructuras de datos
de personajes

Argumentos:
- Characters:   Arreglo de personajes
- count:        Cantidad de personajes en el arreglo

Resultado:
- Se liberan count estructuras del arreglo de characters
============================================================================
*/
void free_resources(character *characters, int count) {
    
    for (int i = 0; i < count; i++) {
        free(characters[i].name);
    }
    free(characters);
}


/*
=======================================================================================
Funcion de entrada del programa principal

El programa recibe los siguientes argumentos:

-r indica que debe limitarse la búsqueda a una región. El nombre de la región
    debe llevar la misma grafía que el directorio que la contiene.

-s indica la especie (en este caso, la especie debe ser “pokemon” para indicar
    que se debe contar solamente los archivos HTML en los directorios de pokémones
    que correspondan con los criterios de búsqueda o “trainer” para indicar que se 
    debe contar solamente a los humanos)

-t indica el tipo de apariciones: “main” para los personajes principales,
    “recurring” para los personajes recurrentes, “gym_leader” para los líderes de
    gimnasio, y “one_time” para los personajes que aparecen solo una vez -c 
    ó --nocount indica si debe aparecer el número de archivos encontrados.
    (Por defecto, se muestra. Si el flag está presente, no se debe mostrar.)

-l ó --list indica si se deben dar los nombres de los archivos encontrados.
    (Por defecto, no se muestran.)

--size indica si se debe mostrar el tamaño en kilobytes (1024 bytes) de los archivos.
    (Por defecto, no se muestra.) Si está incluido el flag -l, se muestra junto a cada
    archivo encontrado, su tamaño [name] permite restringir la búsqueda a archivos 
    que comiencen con el nombre dado.

========================================================================================
*/
int main(int argc, char *argv[]) {
    // Crear e inicializar la estructura search_criteria
    search_criteria criteria = {NULL, NULL, -1, 1, 0, 0, NULL};

    // Analizar los argumentos de la linea de comando
    parse_arguments(argc, argv, &criteria);

    // Crear un puntero para almacenar la lista de personajes y una variable para el recuento de personajes
    character *characters = NULL;
    int count = 0;

    // Procesar el directorio raiz en funcion de los criterios de busqueda
    process_directory(".", &criteria, &characters, &count, 0);

    // Imprimir los resultados de acuerdo con los criterios de busqueda
    print_results(characters, count, &criteria);

    // Liberar recursos utilizados por las estructuras de datos
    free_resources(characters, count);

    return 0;
}