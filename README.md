# Tarea1-Prim-Version2
Tarea1-CC4102

Implementación del algoritmo de Prim usando una cola binomial y una cola de
Fibonacci, con un main único que corre toda la batería de experimentos
(6.3.1 y 6.3.2) sin tener que modificar el código.

## Requisitos

- Compilador de C++.
- Python con `pandas`, `matplotlib` y `numpy` (solo para graficar).

## Archivos del proyecto

| Archivo | Qué hace |
|---|---|
| `common.h` | Tipos compartidos: `Arista`, `Grafo`, `Resultado` (lo que devuelve cada corrida de Prim). |
| `generador.h` | Genera un grafo conexo aleatorio en memoria a partir de `i`, `j` y una semilla. |
| `cola_binomial.h` | Cola binomial, con contador de intercambios estructurales. |
| `cola_fibonacci.h` | Cola de Fibonacci, con contador de cortes en cascada. |
| `prim.h` | Prim genérico: corre sobre cualquiera de las dos colas, en modo normal (6.3.1) o instrumentado (6.3.2). |
| `main.cpp` | Recorre todas las configuraciones, mide, verifica y escribe los CSV. |
| `graficar.py` | Lee los CSV y genera los gráficos pedidos en el enunciado. |
| `sizeof.cpp` | Programa aparte para medir el tamaño en memoria de cada estructura (usado en la sección de estimación de memoria del informe). |

## Cómo compilar y ejecutar todo

Con los archivos `.h` y `main.cpp` en la misma carpeta:

```bash
g++ main.cpp -o main
./main
```

Esto corre **todas** las configuraciones de las partes 6.3.1 y 6.3.2 (series
A, B, C y D), con 10 repeticiones cada una, usando un grafo distinto por
repetición. Puede tardar hasta 1hr.

Al terminar, se generan varios archivos `.csv` en la misma carpeta (ver
sección siguiente). Para obtener los gráficos:

```bash
python graficar.py
```

Esto crea la carpeta `graficos/` con las 12 imágenes `.png` pedidas (4 de la
parte 6.3.1 y 8 de la parte 6.3.2).

## Opciones de `./main`

Se pueden combinar entre sí:

- `./main prueba`: versión chica, con configuraciones pequeñas, para
  comprobar que todo compila y corre bien antes de lanzar la batería
  completa.
- `./main 631`: corre solo la parte 6.3.1 (series A y B).
- `./main 632`: corre solo la parte 6.3.2 (series C y D).
- `./main reps=3`: cambia el número de repeticiones por configuración (por
  defecto son 10).

## Archivos que genera `./main`

| Archivo | Contenido |
|---|---|
| `resultados_631.csv` | Una fila por ejecución de 6.3.1: cola, serie, i, j, semilla, tiempo total y peso del MST. |
| `tabla_631.csv` | Tabla con el promedio de tiempo (ms) de cada configuración, binomial vs. Fibonacci. Es la tabla que va en el informe. |
| `resultados_632.csv` | Una fila por ejecución de 6.3.2: llamadas a `decreaseKey`, tiempo acumulado en `decreaseKey`, operaciones estructurales y cortes en cascada. |
| `curvas_632.csv` | Checkpoints (cada 1024 llamadas a `decreaseKey`) del tiempo y las operaciones acumuladas. Es la base de los gráficos de 6.3.2. |
| `verificacion.csv` | Peso del MST obtenido por cada cola en cada ejecución, y si coinciden. |

Si se corre solo `./main 631` o `./main 632`, solo se generan los CSV de esa
parte (más `verificacion.csv`).

## Medir el tamaño de las estructuras en memoria

`sizeof.cpp` es un programa aparte, no forma parte de la batería de
experimentos. Sirve para obtener el tamaño real en bytes de `Arista`,
`NodoBinomial` y `NodoFibonacci`, usado en la sección de estimación de
memoria del informe.

```bash
g++ sizeof.cpp -o sizeof
./sizeof
```
