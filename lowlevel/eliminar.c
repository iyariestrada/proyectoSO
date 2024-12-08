#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//#define ORIGINAL_FILE "index"
//#define TEMP_FILE "index_temp"
//#define SEPARATOR "****"

void eliminarN(const char *nombre) {
    int descriptorIndex = open("index", O_RDONLY);
    if (descriptorIndex < 0) {
        printf("Error el archivo 'index' no se pudo abrir\n");
        exit(EXIT_FAILURE);
    }

    int descriptorTemp = open("indexTemp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (descriptorTemp <0) {
        printf("Error al crear el archivo 'temporal'\n");
        close(descriptorIndex);
        exit(EXIT_FAILURE);
    }

    char buffer[1024]; //tam de la nota
    ssize_t bytes_leidos;
    int Encontrado = 0;
    int ignorar = 0;
    char linea[1024];
    size_t cont = 0; //llevar la cuenta de las lineas leidas

    while ((bytes_leidos = read(descriptorIndex, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_leidos; i++) {
            if (buffer[i] == '\n' || cont >= sizeof(linea) - 1) { //fin de renglon o se acabo el espacio
                linea[cont] = '\0';
                cont = 0;
                if (strstr(linea, "****")) { // Si se encuentra un separador
                    if (Encontrado && ignorar) {
                        ignorar = 0;
                        Encontrado = 0; // Fin de la nota a omitir
                    } else {
                        write(descriptorTemp, linea, strlen(linea));
                        write(descriptorTemp, "\n", 1); // se agerga salto
                    }
                    continue; //va a sig iteracion
                }

                if (!ignorar && strstr(linea, nombre)) { //encuentra el nombre
                    Encontrado = 1;
                    ignorar = 1; // Activa ignorar lineas
                    continue;
                }

                if (!ignorar) {
                    write(descriptorTemp, linea, strlen(linea));
                    write(descriptorTemp, "\n", 1); // se agerga salto
                }
            } else {
                linea[cont++] = buffer[i];
            }
        }
    }

    close(descriptorIndex);
    close(descriptorTemp);

    if (Encontrado) {
        if (unlink("index") != 0) { //elimina el archivo original
            printf("Error al eliminar el archivo original 'index'\n");
            exit(EXIT_FAILURE);
        }
        if (rename("indexTemp", "index") != 0) { 
            printf("Error al renombrar el archivo temporal");
            exit(EXIT_FAILURE);
        }
        write(STDOUT_FILENO, "Nota eliminada correctamente.\n", 30);
    } else {
        unlink("indexTemp"); // Elimina indexTemp si no se encuentra la nota
        write(STDOUT_FILENO, "No se encontró la nota.\n", 24);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("La sintaxis para el comando 'eliminar' es: eliminar <archivo>\n");
        exit(EXIT_FAILURE);
    }

    eliminarN(argv[1]);
    return 0;
}

