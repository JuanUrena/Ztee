/*ztee, por Juan Ureña.
El programa recibe como datos de entrada
el fichero donde se quiere almacenar la salida.
Ademas su entrada estandar pasa a ser un 
fichero comprimido. El fichero se descomprime 
mediante un hijo, que transmite el contenido 
con un pipe. Además sus errores se redireccionan
a "/dev/null". El padre leera del pipe y mostrara
tanto en terminal como en el fichero el contenido
del fichero indicado como entrada estandar tras 
descomprimirlo. */

#include <stdio.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <err.h>
#include <fcntl.h>

enum{
	Bsize = 1024,
	my_child=1,
};

//Programa para esperar a todos los hijos
void
wait_childs(int num_child)
{
	int status;
	for(int x=0;x<num_child;x++){ 
		waitpid(-1, &status, 0);
		if WIFEXITED(status){
	//Si da error, salgo con error al tener solo un hijo
			exit(status);
		} 
	}
}

//Código hijo
void
child(int *fd)
{
	int waste; 
	//Errores mando a null
	waste=open("/dev/null", O_WRONLY);
	dup2(waste, 2);
	close (waste);

	close(fd[0]);
	//mi salida sera la entrada de mi padre
	dup2(fd[1], 1);
	close(fd[1]);
	//ejecuto
	execl("/bin/gunzip", "gunzip", NULL);
}

//Padre funciona como una T, leyendo de una entrada y saliendo en dos posiciones
void
t_dad(int *fd, char *name_file)
{
	int file;
	char buffer[Bsize];
	int num_bytes_read=0;

	close(fd[1]);

	//Fichero de salida
	file = open(name_file, O_CREAT | O_RDWR | O_TRUNC|O_APPEND , 
		S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
	
	if (file<0)
	{
		fprintf(stderr, "\nError en el fichero indicado\n");
		exit(EXIT_FAILURE);
	}
	//Lectura por el pipe
	while ((num_bytes_read=read(fd[0], buffer, Bsize)) != 0)
	{
	//Posible error
		if (num_bytes_read<0)
		{
			fprintf(stderr, "\nError en la lectua de bytes\n");
			exit(EXIT_FAILURE);	
		}
	//Escribo ambas salidas
		write (1, buffer, num_bytes_read);
		write (file, buffer, num_bytes_read);      
	}
	//Cierro ambas salidas
	close(file);
	close(fd[0]);
}

int
main(int argc, char *argv[])
{
	//Error de entrada
	if (argc!=2){
		fprintf(stderr, "\nError en los Parámetros, indicar sólo el fichero de salida\n");
		exit(EXIT_FAILURE);
	}
	
	int fd[2];
	pipe(fd);

	switch(fork()){
		case -1:
	//Error en el fork 
			fprintf(stderr, "\nError fork, no se ha podido realizar\n");
			exit(EXIT_FAILURE);
			break;
		case 0:
			child(fd);
			exit(EXIT_SUCCESS);
			break;
		default:
			t_dad(fd, argv[1]);
			wait_childs(my_child);
	}
	exit(EXIT_FAILURE);
}
