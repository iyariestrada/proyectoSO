#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void fechaHora(char *buffer, size_t size){
	time_t hora = time(NULL);
	struct tm *tiempo = localtime(&hora);
	strftime(buffer, size, "%d-%m-%Y %H:%M:%S", tiempo);

}

int existeN(char *nombre){
	int descriptorIdx = open("idx", O_RDONLY);	
	if(descriptorIdx ==-1){ //sino existe, no se duplica
		return 0;  //0 si no hay duplicados
	}
	char data[200];
	ssize_t bytes_leidos;
	while((bytes_leidos = read(descriptorIdx, data, sizeof(data)-1)) >0){
		data[bytes_leidos]= '\0';
		if(strstr(data, nombre) != NULL){
			close(descriptorIdx);
			return 1; //1 si ya existe
		}
	}
	close(descriptorIdx);
	return 0;
}


int main(int argc, char *argv[]){
	int descriptorIdx;
	int descriptorIndex;
	char *nombreNota = argv[1];
	ssize_t bytes_leidos;
	char fecha_hora[50];
	if(argc !=2){
		printf("La sintaxis para este comando es: crear <nombre archivo>\nIntentelo de nuevo\n.");
		exit(EXIT_FAILURE);	
	}
	if(existeN(nombreNota)){
		printf("La nota %s ya existe\n.", nombreNota);
		exit(EXIT_FAILURE);
	}
	fechaHora(fecha_hora, sizeof(fecha_hora));
	printf("Escribe el contenido de la nota: \n");
	char contenido[1024]; //200 palabras aprox
	bytes_leidos = read(STDIN_FILENO, contenido, sizeof(contenido) -1);
	if(bytes_leidos <=0){
	printf("No se pudo leer el contenido de la nota.\n");
	exit(EXIT_FAILURE);
	}
	contenido[bytes_leidos] = '\0';

	//se guarda en el archivo idx
	descriptorIdx = open("idx", O_WRONLY | O_CREAT | O_APPEND, 0644);
	if(descriptorIdx ==-1){
	printf("Error archivo idx no se pudo abrir");
	exit(EXIT_FAILURE);
	}
	dprintf(descriptorIdx, "%s - %s\n", nombreNota, fecha_hora);
	close(descriptorIdx);

	//se guarda en el archivo index
	descriptorIndex= open("index", O_WRONLY | O_CREAT | O_APPEND, 0664);
	if(descriptorIndex ==-1){
	printf("El archivo index no se pudo abrir");
	exit(EXIT_FAILURE);
	}
	dprintf(descriptorIndex, "%s\n%s\n%s****\n", nombreNota, fecha_hora, contenido);
	close(descriptorIndex);
	printf("Se registro la nota '%s' correctamente.\n", nombreNota);
	return 0;
}
