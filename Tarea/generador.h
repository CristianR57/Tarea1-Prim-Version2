#pragma once
#include <random>
#include <unordered_set>
#include <utility>
#include "common.h"

// Mismo procedimiento que el generador original: arbol cobertor + aristas
// aleatorias sin repetir. Misma semilla => mismo grafo.
inline Grafo generarGrafo(int i, int j, int semilla) {
    int v = 1 << i;
    long long e = 1LL << j;
    Grafo grafo(v);
    mt19937 gen(semilla);
    uniform_real_distribution<double> distPeso(0.000001, 1.0);
    unordered_set<long long> aristas;
    aristas.reserve((size_t)e * 2);

    for (int nodo = 1; nodo < v; nodo++) {
        uniform_int_distribution<int> distNodo(0, nodo - 1);
        int padre = distNodo(gen);
        double peso = distPeso(gen);
        grafo[nodo].push_back({padre, peso});
        grafo[padre].push_back({nodo, peso});
        aristas.insert((long long)padre * v + nodo);
    }

    uniform_int_distribution<int> distNodo(0, v - 1);
    while ((long long)aristas.size() < e) {
        int u = distNodo(gen), w = distNodo(gen);
        if (u == w) continue;
        if (u > w) swap(u, w);
        long long clave = (long long)u * v + w;
        if (aristas.count(clave)) continue;
        double peso = distPeso(gen);
        grafo[u].push_back({w, peso});
        grafo[w].push_back({u, peso});
        aristas.insert(clave);
    }
    return grafo;
}
