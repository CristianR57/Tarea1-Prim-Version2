#pragma once
#include <chrono>
#include <limits>
#include "common.h"

// Prim generico sobre cualquier cola con la interfaz comun.
// INSTR = false: mide solo el tiempo total (sin relojes internos) -> 6.3.1
// INSTR = true : mide cada decreaseKey y guarda checkpoints en memoria -> 6.3.2
template <class Cola, bool INSTR>
Resultado prim(const Grafo& grafo, int raiz, int paso) {
    using reloj = chrono::steady_clock;
    int n = grafo.size();
    vector<double> costos(n, numeric_limits<double>::infinity());
    vector<int> parent(n, -1);
    costos[raiz] = 0.0;
    Resultado r;

    auto ini = reloj::now();

    Cola Q(n);
    Q.construir(costos);

    while (!Q.vacia()) {
        int u = Q.extraerMin();
        if (parent[u] != -1) { r.aristasMST++; r.pesoTotal += costos[u]; }

        for (const Arista& a : grafo[u]) {
            int v = a.destino;
            if (Q.contiene(v) && a.peso < costos[v]) {
                costos[v] = a.peso;
                parent[v] = u;
                if constexpr (INSTR) {
                    auto t0 = reloj::now();
                    Q.decreaseKey(v, a.peso);
                    auto t1 = reloj::now();
                    r.tiempoDecreaseNs +=
                        chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count();
                    r.llamadas++;
                    if (r.llamadas % paso == 0)
                        r.curva.push_back({r.llamadas, r.tiempoDecreaseNs, Q.operaciones()});
                } else {
                    Q.decreaseKey(v, a.peso);
                    r.llamadas++;
                }
            }
        }
    }

    auto fin = reloj::now();
    r.tiempoTotalNs = chrono::duration_cast<chrono::nanoseconds>(fin - ini).count();
    r.operaciones = Q.operaciones();
    r.cortesCascada = Q.cortesCascada();
    return r;
}
