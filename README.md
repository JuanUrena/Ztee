/*Ztee, por Juan Ureña.
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
