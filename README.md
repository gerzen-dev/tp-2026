# TP Integrador — Buffet Albert Einstein

Trabajo Práctico grupal de **Algoritmos y Estructuras de Datos** (UTN FRBA) —
unidad de **archivos binarios en C/C++**.

> **Grupo:** 10
> **Integrantes:** Nombre: Gustavo Leonel Erzen - Usuario de GitHub: gerzen-dev
                   Nombre: Nazareno Andres Battilocchi - Usuario de GitHub: nbattilocchi-bit


Explicación de cada Programa:
-----------------------------

Normalizacion.cpp
el programa normalizacion.cpp comienza leyendo los archivos binarios y los carga en vectores, luego identifica las fechas unicas y en cada una filtra las comandas por el id de mozo y los ordena, terminando con el stock actualizando el archivo inventario.dat

Ventas.cpp
el programa ventas.cpp comienza leyendo los archivos binarios, luego comienza con el cifrado de las contraseñas con un desplazado simple de caracteres, crea (o abre) un archivo segun la fecha ingresada e inicia un bucle donde los mozos pueden, con su contraseña, agregar ventas. y al finalizar el dia el archivo se ordena y se guarda con la fecha ingresada, ademas de actualizar el stock en el inventario.dat

Cierre.cpp:
En este programa, recibe n cantidad de planillas diarias que fueron traídas desde normalizacion.cpp o fueron creadas en ventas.cpp. El programa le pide al 
usuario que ingrese el nombre del archivo semanal (respetando el formato "comandas_semana_sX-mm.dat") donde se colocará el contenido de las planillas diarias.
Acto seguido se le pregunta al usuario si desea agregar los archivos diarios. En caso negativo, el programa finaliza y no se crea ningun archivo, mientras que
si ocurre lo contrario (respondemos con un Si(1)) entramos a un bucle while donde se nos solicitará el nombre de los archivos diarios (siguiendo el formato 
"comandas_dd-mm-aaaa.dat"). Se llama a la función crearArchivo donde se ejecutará 2 funciones, las cuales consiste en utilizar el patrón algorítmico de apareo y después, reescribir lo obtenido en el apareo en el archivo semanal. Una vez finalizado eso, se le consulta al usuario si desea seguir agregando más archivos al
comandas_semana. Si se niega(respondiendo con un 0) se sale del bucle while y se llama a una función encargada de ordenar el Archivo semanal recien creado
mediante ordenamiento burbuja y por id de Mozo. Este archivo recién creado será utilizado en el programa Resumen.cpp.

Resumen.cpp:
En dicho programa, se lee n cantidad de archivos semanales que fueron generados a partir del programa Cierre.cpp
Se le pregunta al usuario al principio de la ejecución si desea añadir un archivo analizo. En caso de que el usuario no quiera añadir un archivo a analizar,
el programa finaliza. En caso positivo que el usuario quiera realizar un análisis del archivo semanal, se le solicita el nombre de dicho archivo (respetando el
formato de "comandas_semana_sX-mm.dat" y acto seguido se llama a una función, el cual consiste en un Corte de Control (por Id de Mozo) donde se va acumulando el 
total de comisiones de cada Mozo( con su respectivo ID y Nombre), la cantidad de productos vendidos por cada mozo, y se obtiene las ventas totales que generó en 
la semana.
Al finalizar dicho corte de control, se le pregunta al usuario si quiere ingresar otro archivo semanal para analizar. Si responde que no(0) se termina el bucle
while y finaliza el programa.

