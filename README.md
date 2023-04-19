# Proyecto 2 de Operativos

En este proyecto se realizo un programa en homenaje a la popular serie animada Pokemon debido a su finalizacion luego de 25 años de transmision.

La aplicacion consiste en un verificador de apariciones de pokemones y personajes de la serie animada. La implementacion esta en su totalidad en el lenguaje de programacion C y su objetivo es verificar el conocimiento obtenido en el curso de sistemas operativos sobre inodos y manejo de archivos en disco.

## Compilacion y ejecucion
La compilacion esta automatizada mediante un makefile, por lo que unicamente es necesario ejecutar el comando make.

El archivo make pone a disposicion el comando make clean para eliminar los archivos relacionados a la compilacion

## Opciones principales del programa y ejecucion

$ ./fameChecker [-r <region>] [-s <species>] [-t <type>] [-c|--nocount] [-l|--list] [--size] [name]

-r indica que debe limitarse la búsqueda a una región. El nombre de la región debe llevar la misma grafía que el directorio que la contiene.

-s indica la especie (en este caso, la especie debe ser “pokemon” para indicar que se debe contar solamente los archivos HTML en los directorios de pokémones que correspondan con los criterios de búsqueda o “trainer” para indicar que se debe contar solamente a los humanos)

-t indica el tipo de apariciones: “main” para los personajes principales, “recurring” para los personajes recurrentes, “gym_leader” para los líderes de gimnasio, y “one_time” para los personajes que aparecen solo una vez -c ó --nocount indica si debe aparecer el número de archivos encontrados. (Por defecto, se muestra. Si el flag está presente, no se debe mostrar.)

-l ó --list indica si se deben dar los nombres de los archivos encontrados. (Por defecto, no se muestran.)

--size indica si se debe mostrar el tamaño en kilobytes (1024 bytes) de los archivos. (Por defecto, no se muestra.) Si está incluido el flag -l, se muestra junto a cada archivo encontrado, su tamaño [name] permite restringir la búsqueda a archivos que comiencen con el nombre dado.

#### Universidad Simon Bolivar
#### Trimestre Enero-Marzo 2023
#### Profesor Fernando Torres Mora