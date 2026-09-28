// ============================================================
// MAIN: ejecuta toda la bateria de experimentos (6.3.1 y 6.3.2)
// sin modificar el codigo.
//
//   ./main                 -> todo (series A, B, C, D), 10 repeticiones
//   ./main 631             -> solo 6.3.1 (series A y B)
//   ./main 632             -> solo 6.3.2 (series C y D)
//   ./main prueba          -> version chica para verificar que funciona
//   ./main prueba 631      -> combinables en cualquier orden
//   ./main reps=3          -> cambia el numero de repeticiones
//
// Salida (CSV en la carpeta actual):
//   resultados_631.csv, tabla_631.csv,
//   resultados_632.csv, curvas_632.csv, verificacion.csv
// Los graficos se generan luego con:  python3 graficar.py
// ============================================================
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "common.h"
#include "generador.h"
#include "cola_binomial.h"
#include "cola_fibonacci.h"
#include "prim.h"

using namespace std;

struct Config { char serie; int i, j; };

static int PASO_CURVA = 1024;   // checkpoint cada N llamadas a decreaseKey (16 en modo prueba)

vector<Config> configs631(bool prueba) {
    vector<Config> c;
    if (!prueba) {
        for (int j = 20; j <= 24; j++) c.push_back({'A', 20, j});   // v fijo
        for (int i = 18; i <= 22; i++) c.push_back({'B', i, 24});   // e fijo
    } else {
        for (int j = 10; j <= 14; j++) c.push_back({'A', 10, j});
        for (int i = 8; i <= 12; i++)  c.push_back({'B', i, 14});
    }
    return c;
}

vector<Config> configs632(bool prueba) {
    vector<Config> c;
    if (!prueba) {
        for (int j = 18; j <= 22; j++) c.push_back({'C', 18, j});   // v fijo
        for (int i = 14; i <= 18; i++) c.push_back({'D', i, 22});   // e fijo
    } else {
        for (int j = 8; j <= 12; j++) c.push_back({'C', 8, j});
        for (int i = 6; i <= 10; i++) c.push_back({'D', i, 10});
    }
    return c;
}

bool pesosIguales(double a, double b) {
    return fabs(a - b) <= 1e-6 * max(1.0, max(fabs(a), fabs(b)));
}

// ------------------------------------------------------------
// 6.3.1  COSTO TOTAL
// ------------------------------------------------------------
int fallos = 0;

void correr631(bool prueba, int reps, ofstream& verif) {
    ofstream res("resultados_631.csv");
    ofstream tabla("tabla_631.csv");
    res << "cola,serie,i,j,semilla,v,e,tiempo_ns,peso_mst\n";
    tabla << "serie,i,j,v,e,binomial_ms,fibonacci_ms,pesos_iguales\n";

    cout << "\n=========== 6.3.1 COSTO TOTAL ===========\n";
    cout << left << setw(6) << "Serie" << setw(4) << "i" << setw(4) << "j"
         << setw(16) << "Binomial (ms)" << setw(16) << "Fibonacci (ms)"
         << "Pesos\n";

    for (const Config& c : configs631(prueba)) {
        long long v = 1LL << c.i, e = 1LL << c.j;
        double sumaB = 0, sumaF = 0;
        bool todosOk = true;

        for (int s = 1; s <= reps; s++) {
            Grafo g = generarGrafo(c.i, c.j, s);          // NO se mide

            Resultado rb = prim<ColaBinomial, false>(g, 0, PASO_CURVA);
            Resultado rf = prim<ColaFibonacci, false>(g, 0, PASO_CURVA);

            if (rb.aristasMST != v - 1 || rf.aristasMST != v - 1) {
                cerr << "ADVERTENCIA: MST con numero incorrecto de aristas\n";
                todosOk = false;
            }
            bool ok = pesosIguales(rb.pesoTotal, rf.pesoTotal);
            if (!ok) { todosOk = false; fallos++; }

            verif << "6.3.1," << c.serie << "," << c.i << "," << c.j << "," << s << ","
                  << setprecision(12) << rb.pesoTotal << "," << rf.pesoTotal << ","
                  << (ok ? 1 : 0) << "\n";
            res << "binomial," << c.serie << "," << c.i << "," << c.j << "," << s << ","
                << v << "," << e << "," << rb.tiempoTotalNs << ","
                << setprecision(12) << rb.pesoTotal << "\n";
            res << "fibonacci," << c.serie << "," << c.i << "," << c.j << "," << s << ","
                << v << "," << e << "," << rf.tiempoTotalNs << ","
                << setprecision(12) << rf.pesoTotal << "\n";
            res.flush(); verif.flush();

            sumaB += rb.tiempoTotalNs / 1e6;
            sumaF += rf.tiempoTotalNs / 1e6;
            cerr << "  [6.3.1] serie " << c.serie << " i=" << c.i << " j=" << c.j
                 << " semilla " << s << "/" << reps << "\r";
        }
        cerr << string(70, ' ') << "\r";

        double pB = sumaB / reps, pF = sumaF / reps;
        cout << left << setw(6) << c.serie << setw(4) << c.i << setw(4) << c.j
             << setw(16) << fixed << setprecision(2) << pB
             << setw(16) << pF << (todosOk ? "OK" : "DISTINTOS") << "\n";
        tabla << c.serie << "," << c.i << "," << c.j << "," << v << "," << e << ","
              << pB << "," << pF << "," << (todosOk ? 1 : 0) << "\n";
        tabla.flush();
    }
}

// ------------------------------------------------------------
// 6.3.2  COSTO AMORTIZADO
// ------------------------------------------------------------
template <class Cola>
Resultado correrUna(const Grafo& g) { return prim<Cola, true>(g, 0, PASO_CURVA); }

void escribir632(ofstream& res, ofstream& curvas, const string& cola,
                 const Config& c, int s, const Resultado& r) {
    long long v = 1LL << c.i, e = 1LL << c.j;
    res << cola << "," << c.serie << "," << c.i << "," << c.j << "," << s << ","
        << v << "," << e << "," << r.llamadas << "," << r.tiempoDecreaseNs << ","
        << r.operaciones << "," << r.cortesCascada << ","
        << setprecision(12) << r.pesoTotal << "\n";
    for (const Punto& p : r.curva)
        curvas << cola << "," << c.serie << "," << c.i << "," << c.j << "," << s << ","
               << p.llamadas << "," << p.tiempoNs << "," << p.operaciones << "\n";
}

void correr632(bool prueba, int reps, ofstream& verif) {
    ofstream res("resultados_632.csv");
    ofstream curvas("curvas_632.csv");
    res << "cola,serie,i,j,semilla,v,e,llamadas,tiempo_decrease_ns,"
        << "operaciones,cortes_cascada,peso_mst\n";
    curvas << "cola,serie,i,j,semilla,llamadas,tiempo_ns,operaciones\n";

    cout << "\n=========== 6.3.2 COSTO AMORTIZADO (decreaseKey) ===========\n";
    cout << left << setw(6) << "Serie" << setw(4) << "i" << setw(4) << "j"
         << setw(12) << "llamadas" << setw(14) << "bin t(ms)" << setw(14) << "fib t(ms)"
         << setw(14) << "bin swaps" << setw(14) << "fib cortes" << "Pesos\n";

    for (const Config& c : configs632(prueba)) {
        long long v = 1LL << c.i;
        double sLl = 0, sTb = 0, sTf = 0, sOb = 0, sOf = 0;
        bool todosOk = true;

        for (int s = 1; s <= reps; s++) {
            Grafo g = generarGrafo(c.i, c.j, s);

            // Toda la medicion queda en memoria; el CSV se escribe DESPUES de Prim
            Resultado rb = correrUna<ColaBinomial>(g);
            Resultado rf = correrUna<ColaFibonacci>(g);

            if (rb.aristasMST != v - 1 || rf.aristasMST != v - 1) {
                cerr << "ADVERTENCIA: MST incorrecto\n"; todosOk = false;
            }
            bool ok = pesosIguales(rb.pesoTotal, rf.pesoTotal);
            if (!ok) { todosOk = false; fallos++; }
            verif << "6.3.2," << c.serie << "," << c.i << "," << c.j << "," << s << ","
                  << setprecision(12) << rb.pesoTotal << "," << rf.pesoTotal << ","
                  << (ok ? 1 : 0) << "\n";

            escribir632(res, curvas, "binomial", c, s, rb);
            escribir632(res, curvas, "fibonacci", c, s, rf);
            res.flush(); curvas.flush(); verif.flush();

            sLl += rb.llamadas;
            sTb += rb.tiempoDecreaseNs / 1e6; sTf += rf.tiempoDecreaseNs / 1e6;
            sOb += rb.operaciones;            sOf += rf.operaciones;
            cerr << "  [6.3.2] serie " << c.serie << " i=" << c.i << " j=" << c.j
                 << " semilla " << s << "/" << reps << "\r";
        }
        cerr << string(70, ' ') << "\r";

        cout << left << setw(6) << c.serie << setw(4) << c.i << setw(4) << c.j
             << setw(12) << fixed << setprecision(0) << sLl / reps
             << setw(14) << setprecision(2) << sTb / reps
             << setw(14) << sTf / reps
             << setw(14) << setprecision(0) << sOb / reps
             << setw(14) << sOf / reps << (todosOk ? "OK" : "DISTINTOS") << "\n";
    }
}

// ------------------------------------------------------------
int main(int argc, char* argv[]) {
    bool prueba = false, solo631 = false, solo632 = false;
    int reps = 10;

    for (int a = 1; a < argc; a++) {
        string arg = argv[a];
        if (arg == "prueba") prueba = true;
        else if (arg == "631") solo631 = true;
        else if (arg == "632") solo632 = true;
        else if (arg.rfind("reps=", 0) == 0) reps = stoi(arg.substr(5));
        else { cerr << "Argumento desconocido: " << arg << "\n"; return 1; }
    }
    if (prueba) PASO_CURVA = 16;
    bool hacer631 = solo631 || !solo632;
    bool hacer632 = solo632 || !solo631;

    ofstream verif("verificacion.csv");
    verif << "parte,serie,i,j,semilla,peso_binomial,peso_fibonacci,iguales\n";

    cout << "Modo: " << (prueba ? "PRUEBA" : "COMPLETO")
         << " | repeticiones por configuracion: " << reps << "\n";

    if (hacer631) correr631(prueba, reps, verif);
    if (hacer632) correr632(prueba, reps, verif);

    cout << "\n=========== VERIFICACION DE PESO DEL MST ===========\n";
    if (fallos == 0)
        cout << "Ambas implementaciones produjeron MST del mismo peso total "
                "en TODAS las ejecuciones.\n";
    else
        cout << "ATENCION: " << fallos << " ejecuciones con pesos distintos "
                "(ver verificacion.csv)\n";

    cout << "\nListo. Ahora ejecuta:  python3 graficar.py\n";
    return 0;
}
