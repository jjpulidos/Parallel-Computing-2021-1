# Practica 1: Compuatación paralela y distribuida UNAL 2021-I

## ¿Que es?


Implementaciones secuencial y paralelizada (mediante `pthread`) del filtro de mediana, usado para filtrar ciertos tipos de ruido, en particular el llamado "de sal y pimienta". Los ejecutables resultantes leen una imagen y crean una segunda imagen, filtrada.

## Dependencias

- Obligatorias
  * `OpenCV`
- Opcionales
  * `make` para compilar facilmente en Linux
  * `cmake` 
  * `python` para ejecutar el script que prueba todos los tamaños e usos de hilos

## Para compilar:

### Usando Make
  ```sh
  $ make # compila el programa secuencial y el programa con hilos
  $ make d #compila con simbolos de debugging
  $ make all #preserva archivos objeto
  ```
  
  Para ejecutar:
  
  Algoritmo secuencial con ventana de filtro de 5 pixeles de lado. Lee la imagen en `in/img.jpg` y escribe el resultado en `out/img.jpg`.
  ```sh
  $ ./sequential 5 in/img.jpg out/img.jpg
  ```
  
  Algoritmo paralelo con ventana de filtro de 5 pixeles de lado, haciendo uso de 4 hilos `POSIX`. Lee la imagen en `in/img.jpg` y escribe el resultado en `out/img.jpg`.
  ```sh
  $ ./paralel 5 4 in/img.jpg out/img.jpg
  ```
  
  Script que transforma todas las imagenes presentes en el directorio `in`, haciendo uso de 1, 2, 4, 8 y 16 hilos, y registrando el tiempo tomado por cada coombinación de parámetros en el archivo `log.txt`.
  ```sh
  $ python script_ejecutar_todo.py
  $ cat log.txt
  ```

### Usando CMake y Clion

Verifica que en CMakeLists.txt en la linea 3 este correctamente configurado el directorio de OpenCV

```sh
set(OpenCV_DIR /Users/<your-username>/Path-to-OpenCV)
```

Si vas a Construir el proyecto Sequencial entonces cambia la linea 9

```sh
add_executable(Parallel-Computing-2021-1 input.cpp image-effect-<par o seq>.cpp )
```

Como fue añadido en el .gitgnore que no se guardara la carpeta **cmake-build-debug**, entonces necesitaras configurar el
proyecto con click derecho en el proyecto (Clion) y "Reload CMake Project", el cual creará la carpet necesaria donde se
encontrarán los ejecutables despues de hacer build.

Finalmente se ejecutarían reemplazando de los comandos con Make "./paralel" por "cmake-build-debug/Parallel-Computing-2021-1"

por ejemplo: 

```sh
cmake-build-debug/Parallel-Computing-2021-1 5 4  lena.jpeg lenaExport2.jpg
```
