#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

// Definir las constantes para los tipos de personajes
#define MAIN 0
#define RECURRING 1
#define GYM_LEADER 2
#define ONE_TIME 3

// Definir una estructura `character` que almacene informacion sobre el personaje
typedef struct {
    char *name;
    char *region;
    char *species;
    int type;
    unsigned long file_size;
} character;

// Definir una estructura `search_criteria` que almacene los criterios de busqueda
typedef struct {
    char *region;
    char *species;
    int type;
    int show_count;
    int list_files;
    int show_size;
    char *name;
} search_criteria;

// Funcion para analizar los argumentos de la linea de comando
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

// Funcion para comprobar si un archivo coincide con los criterios de busqueda
bool is_match(const char *filename, search_criteria *criteria) {
    // Comprueba si el nombre del archivo contiene el nombre proporcionado en los criterios de busqueda
    if (criteria->name != NULL && strstr(filename, criteria->name) == NULL) {
        return false;
    }

    // Comprueba si el tipo de aparicion coincide con los criterios de busqueda
    if (criteria->type != -1) {
        if (criteria->type == MAIN && strstr(filename, "_main") == NULL) {
            return false;
        } else if (criteria->type == RECURRING && strstr(filename, "_recurring") == NULL) {
            return false;
        } else if (criteria->type == GYM_LEADER && strstr(filename, "_gym_leader") == NULL) {
            return false;
        } else if (criteria->type == ONE_TIME && strstr(filename, "_one_time") == NULL) {
            return false;
        }
    }

    return true;
}

// Funcion para procesar el directorio especificado en funcion de los criterios de busqueda
void process_directory(const char *path, search_criteria *criteria, character **characters, int *count, int depth) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    if ((dir = opendir(path)) == NULL) {
        perror("Error al abrir el directorio");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char new_path[1024];
        snprintf(new_path, sizeof(new_path), "%s/%s", path, entry->d_name);

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
        } else if (entry->d_type == DT_REG) {
            // Filtrar solo archivos HTML
            char *file_ext = strrchr(entry->d_name, '.');
            if (file_ext != NULL && strcmp(file_ext, ".html") == 0) {
                if (is_match(entry->d_name, criteria)) {
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

// Funcion para imprimir los resultados de acuerdo con los criterios de busqueda
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

// Funcion para liberar la memoria utilizada por las estructuras de datos
void free_resources(character *characters, int count) {
    
    for (int i = 0; i < count; i++) {
        free(characters[i].name);
    }
    free(characters);
}

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