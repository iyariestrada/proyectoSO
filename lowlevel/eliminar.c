#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void eliminarIdx(const char *nombre) {
    int descriptorIdx = open("idx", O_RDONLY);
    if (descriptorIdx < 0) {
        printf("El archivo 'idx' no se pudo abrir\n");
        exit(EXIT_FAILURE);
    }

    int descriptorTempIdx = open("idxTemp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (descriptorTempIdx < 0) {
        printf("No se pudo crear 'idxTemp'\n");
        close(descriptorIdx);
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    ssize_t bytes_leidos;
    char linea[1024];
    size_t cont = 0;
    int encontrado = 0;

    while ((bytes_leidos = read(descriptorIdx, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_leidos; i++) {
            if (buffer[i] == '\n' || cont >= sizeof(linea) - 1) {
                linea[cont] = '\0';
                cont = 0;
                
                if (strstr(linea, nombre) == linea) { //Comparar el nombre con el registro actual
                    encontrado = 1;
                    continue; // siguiente ciclo
                }
                write(descriptorTempIdx, linea, strlen(linea));
                write(descriptorTempIdx, "\n", 1);
            } else {
                linea[cont++] = buffer[i];
            }
        }
    }

    close(descriptorIdx);
    close(descriptorTempIdx);

    if (encontrado) {
        if (unlink("idx") != 0) {
            printf("No se pudo eliminar el archivo 'idx'\n");
            unlink("idxTemp");
            exit(EXIT_FAILURE);
        }

        if (rename("idxTemp", "idx") != 0) {
            printf("No se pudo renombrar 'idxTemp' a 'idx'\n");
            unlink("idxTemp");
            exit(EXIT_FAILURE);
        }
    } else {
        unlink("idxTemp");
        printf("No se encontro registro en 'idx'.\n");
    }
}

void eliminarN(const char *nombre) {
    int descriptorIndex = open("index", O_RDONLY);
    if (descriptorIndex < 0) {
        printf("Error el archivo 'index' no se pudo abrir\n");
        exit(EXIT_FAILURE);
    }

    int descriptorTemp = open("indexTemp", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (descriptorTemp <0) {
        printf("Error al crear el archivo 'temporal'\n");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    ssize_t bytes_leidos;
    int encontrado = 0;
    int ignorar = 0;
    char linea[1024];
    size_t cont = 0; //llevar la cuenta de las lineas leidas

    while ((bytes_leidos = read(descriptorIndex, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_leidos; i++) {
            if (buffer[i] == '\n' || cont >= sizeof(linea) - 1) { //fin de renglon o se acabo el espacio
                linea[cont] = '\0';
                cont = 0;

                if (strcmp(linea, "****") ==0) { // Si se encuentra un separador
                    if (ignorar) {
                        ignorar = 0;
                    } else {
                        write(descriptorTemp, linea, strlen(linea));
                        write(descriptorTemp, "\n", 1); // se agerga salto
                    }
                    continue; //va a sig iteracion
                }

                if (!ignorar && strcmp(linea, nombre) ==0) { //encuentra el nombre
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

    
        if (unlink("index") != 0) { //elimina el archivo original
            printf("Error al eliminar el archivo original 'index'\n");
            unlink("indexTemp");
            exit(EXIT_FAILURE);
        }
        if (rename("indexTemp", "index") != 0) { 
            printf("Error al renombrar el archivo temporal");
            unlink("indexTemp");
            exit(EXIT_FAILURE);
        }
        printf("Nota eliminada correctamente.\n");
        eliminarIdx(nombre); //llama funcion elimina de idx
    
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("La sintaxis para el comando 'eliminar' es: eliminar <archivo>\n");
        exit(EXIT_FAILURE);
    }

    eliminarN(argv[1]);
    return 0;
}
