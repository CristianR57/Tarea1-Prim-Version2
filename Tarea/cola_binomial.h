#pragma once
#include <vector>
#include <utility>
using namespace std;

struct NodoBinomial {
    int vertex; double key;
    NodoBinomial *parent, *child, *sibling;
    int degree;
    NodoBinomial(int v, double k)
        : vertex(v), key(k), parent(nullptr), child(nullptr),
          sibling(nullptr), degree(0) {}
};

// Interfaz comun con ColaFibonacci (la usa prim.h):
//   construir, vacia, extraerMin, contiene, decreaseKey, operaciones, cortesCascada
class ColaBinomial {
    NodoBinomial *head, *minimum;
    vector<NodoBinomial*> pos;
    long long intercambios;

    NodoBinomial* link(NodoBinomial* a, NodoBinomial* b) {
        if (b->key < a->key) swap(a, b);
        b->parent = a; b->sibling = a->child; a->child = b; a->degree++;
        return a;
    }

    NodoBinomial* mergeListas(NodoBinomial* h1, NodoBinomial* h2) {
        vector<NodoBinomial*> lista;
        for (NodoBinomial* h : {h1, h2}) {
            for (NodoBinomial* a = h; a;) {
                NodoBinomial* sig = a->sibling;
                a->sibling = nullptr; a->parent = nullptr;
                lista.push_back(a);
                a = sig;
            }
        }
        vector<NodoBinomial*> tabla;
        for (NodoBinomial* arbol : lista) {
            NodoBinomial* act = arbol;
            int g = act->degree;
            while (true) {
                if (g >= (int)tabla.size()) tabla.resize(g + 1, nullptr);
                if (!tabla[g]) { tabla[g] = act; break; }
                NodoBinomial* otro = tabla[g];
                tabla[g] = nullptr;
                act = link(act, otro);
                g = act->degree;
            }
        }
        NodoBinomial *nh = nullptr, *ult = nullptr;
        for (NodoBinomial* a : tabla) {
            if (!a) continue;
            a->parent = nullptr; a->sibling = nullptr;
            if (!nh) nh = ult = a; else { ult->sibling = a; ult = a; }
        }
        return nh;
    }

    void actualizarMinimum() {
        minimum = nullptr;
        for (NodoBinomial* a = head; a; a = a->sibling)
            if (!minimum || a->key < minimum->key) minimum = a;
    }

public:
    ColaBinomial(int n) : head(nullptr), minimum(nullptr), pos(n, nullptr), intercambios(0) {}

    void construir(const vector<double>& costos) {
        int n = costos.size();
        head = minimum = nullptr;
        vector<NodoBinomial*> grados;
        for (int v = 0; v < n; v++) {
            NodoBinomial* act = new NodoBinomial(v, costos[v]);
            pos[v] = act;
            int g = act->degree;
            while (true) {
                if (g >= (int)grados.size()) grados.resize(g + 1, nullptr);
                if (!grados[g]) { grados[g] = act; break; }
                NodoBinomial* otro = grados[g];
                grados[g] = nullptr;
                act = link(act, otro);
                g = act->degree;
            }
        }
        NodoBinomial* ult = nullptr;
        for (NodoBinomial* a : grados) {
            if (!a) continue;
            a->parent = nullptr; a->sibling = nullptr;
            if (!head) head = ult = a; else { ult->sibling = a; ult = a; }
        }
        actualizarMinimum();
    }

    bool vacia() const { return minimum == nullptr; }

    bool contiene(int v) const {
        return v >= 0 && v < (int)pos.size() && pos[v] != nullptr;
    }

    // Extrae el minimo, libera el nodo y devuelve el vertice
    int extraerMin() {
        NodoBinomial* m = minimum;
        NodoBinomial* ant = nullptr; NodoBinomial* act = head;
        while (act != m) { ant = act; act = act->sibling; }
        if (!ant) head = m->sibling; else ant->sibling = m->sibling;

        NodoBinomial *nh = nullptr, *ult = nullptr;
        for (NodoBinomial* h = m->child; h;) {
            NodoBinomial* sig = h->sibling;
            h->parent = nullptr; h->sibling = nullptr;
            if (!nh) nh = ult = h; else { ult->sibling = h; ult = h; }
            h = sig;
        }
        pos[m->vertex] = nullptr;
        head = mergeListas(head, nh);
        actualizarMinimum();
        int v = m->vertex;
        delete m;
        return v;
    }

    void decreaseKey(int vertex, double nuevaKey) {
        NodoBinomial* x = contiene(vertex) ? pos[vertex] : nullptr;
        if (!x || nuevaKey > x->key) return;
        x->key = nuevaKey;
        while (x->parent && x->key < x->parent->key) {
            NodoBinomial* p = x->parent;
            swap(x->vertex, p->vertex);
            swap(x->key, p->key);
            pos[x->vertex] = x;
            pos[p->vertex] = p;
            intercambios++;               // operacion estructural
            x = p;
        }
        if (!minimum || x->key < minimum->key) minimum = x;
    }

    long long operaciones() const { return intercambios; }
    long long cortesCascada() const { return 0; }
};
