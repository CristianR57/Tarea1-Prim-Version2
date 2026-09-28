#pragma once
#include <vector>
using namespace std;

struct Arista { int destino; double peso; };
using Grafo = vector<vector<Arista>>;

// Punto de la curva "acumulado vs llamadas a decreaseKey" (se guarda en memoria)
struct Punto { long long llamadas, tiempoNs, operaciones; };

struct Resultado {
    long long aristasMST = 0;
    double pesoTotal = 0.0;
    long long tiempoTotalNs = 0;      // Prim completo (solo modo sin instrumentar)
    long long llamadas = 0;           // llamadas a decreaseKey
    long long tiempoDecreaseNs = 0;   // tiempo acumulado en decreaseKey (modo instrumentado)
    long long operaciones = 0;        // intercambios (binomial) / cortes (Fibonacci)
    long long cortesCascada = 0;      // solo Fibonacci
    vector<Punto> curva;              // checkpoints cada `paso` llamadas
};
