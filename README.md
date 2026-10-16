# TP Integrador — Buffet Albert Einstein

Trabajo Práctico grupal de **Algoritmos y Estructuras de Datos** (UTN FRBA) —
unidad de **archivos binarios en C/C++**.

> **Grupo:** 10
> **Integrantes:** Nombre: Gustavo Leonel Erzen - Usuario de GitHub: gerzen-dev
> _(nombre — usuario de GitHub, uno por línea)_

Explicación de cada Programa:
-----------------------------

Cierre.cpp:
En este programa, recibe n cantidad de planillas diarias que fueron traídas desde normalizacion.cpp o fueron creadas en ventas.cpp. El programa le pide al 
usuario que ingrese el nombre del archivo semanal (respetando el formato "comandas_semana_sX-mm.dat") donde se colocará el contenido de las planillas diarias.
Acto seguido se le pregunta al usuario si desea agregar los archivos diarios. En caso negativo, el programa finaliza y no se crea ningun archivo, mientras que
si ocurre lo contrario (respondemos con un Si(1)) entramos a un bucle while donde se nos solicitará el nombre de los archivos diarios (siguiendo el formato 
"comandas_dd-mm-aaaa.dat"). Se llama a la función crearArchivo donde abrimos la planilla diaria en modo lectura, y la semanal, en modo append. Ahi se escribe
el contenido de la planilla diaria dentro del archivo semanal. Una vez finalizado eso, se le consulta al usuario si desea seguir agregando más archivos al
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


## Qué se entrega

- Este mismo repo (uno por grupo), con los **4 programas** `.cpp` en la raíz
  (cada uno con su propio `main`, no hay `main.cpp`).
- El **README** con el grupo, los integrantes (nombre + usuario de GitHub) y, si
  hace falta, cualquier aclaración de cómo correrlo.
- Los `.dat` de `datos/` son los **provistos por la cátedra** y ya vienen en el
  repo: no los borren ni agreguen otros. Cualquier `.dat` que ustedes copien a la
  raíz o generen al correr **no** se versiona (el `.gitignore` ya los deja afuera).
  La cátedra corre sus programas sobre su propio dataset.
- **Commits repartidos:** cada integrante tiene que tener commits propios a lo
  largo del trabajo. El historial es parte de lo que se mira para la defensa
  individual (no vale un único commit final ni que suba todo una sola persona).
