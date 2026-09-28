#pragma once
#include <vector>
using namespace std;

struct NodoFibonacci {
    int vertex; double key;
    NodoFibonacci *parent, *child, *left, *right;
    int degree; bool flag;
    NodoFibonacci(int v, double k)
        : vertex(v), key(k), parent(nullptr), child(nullptr),
          left(this), right(this), degree(0), flag(false) {}
};

class ColaFibonacci {
    NodoFibonacci* minimum;
    vector<NodoFibonacci*> pos;
    vector<NodoFibonacci*> tablaGrados, raices, hijos;
    long long cortes, cortesEnCascada;

    void agregarRaiz(NodoFibonacci* n) {
        n->parent = nullptr; n->flag = false;
        if (!minimum) { n->left = n->right = n; minimum = n; return; }
        n->right = minimum->right; n->left = minimum;
        minimum->right->left = n; minimum->right = n;
        if (n->key < minimum->key) minimum = n;
    }

    NodoFibonacci* link(NodoFibonacci* a, NodoFibonacci* b) {
        if (b->key < a->key) swap(a, b);
        b->left->right = b->right; b->right->left = b->left;
        b->left = b->right = b;
        b->parent = a; b->flag = false;
        if (!a->child) a->child = b;
        else {
            b->right = a->child->right; b->left = a->child;
            a->child->right->left = b; a->child->right = b;
        }
        a->degree++;
        return a;
    }

    void consolidar() {
        if (!minimum) return;
        raices.clear();
        NodoFibonacci* act = minimum;
        do { raices.push_back(act); act = act->right; } while (act != minimum);

        for (NodoFibonacci* x : raices) {
            int d = x->degree;
            while ((int)tablaGrados.size() <= d) tablaGrados.push_back(nullptr);
            while (tablaGrados[d]) {
                NodoFibonacci* y = tablaGrados[d];
                tablaGrados[d] = nullptr;
                x = link(x, y);
                d = x->degree;
                while ((int)tablaGrados.size() <= d) tablaGrados.push_back(nullptr);
            }
            tablaGrados[d] = x;
        }
        minimum = nullptr;
        for (NodoFibonacci*& x : tablaGrados) {
            if (!x) continue;
            x->left = x->right = x;
            agregarRaiz(x);
            x = nullptr;
        }
    }

    void cut(NodoFibonacci* x, NodoFibonacci* y) {
        if (x->right == x) y->child = nullptr;
        else {
            x->left->right = x->right; x->right->left = x->left;
            if (y->child == x) y->child = x->right;
        }
        y->degree--;
        x->parent = nullptr; x->flag = false;
        x->left = x->right = x;
        agregarRaiz(x);
        cortes++;                         // operacion estructural
    }

    void cascadingCut(NodoFibonacci* y) {
        while (y) {
            NodoFibonacci* z = y->parent;
            if (!z) return;
            if (!y->flag) { y->flag = true; return; }
            cut(y, z);
            cortesEnCascada++;
            y = z;
        }
    }

public:
    ColaFibonacci(int n) : minimum(nullptr), pos(n, nullptr), cortes(0), cortesEnCascada(0) {}

    void construir(const vector<double>& costos) {
        minimum = nullptr;
        for (int v = 0; v < (int)costos.size(); v++) {
            NodoFibonacci* nuevo = new NodoFibonacci(v, costos[v]);
            agregarRaiz(nuevo);
            pos[v] = nuevo;
        }
    }

    bool vacia() const { return minimum == nullptr; }

    bool contiene(int v) const {
        return v >= 0 && v < (int)pos.size() && pos[v] != nullptr;
    }

    int extraerMin() {
        NodoFibonacci* z = minimum;
        if (z->child) {
            NodoFibonacci* ini = z->child;
            hijos.clear();
            NodoFibonacci* h = ini;
            do { hijos.push_back(h); h = h->right; } while (h != ini);
            for (NodoFibonacci* x : hijos) {
                x->parent = nullptr; x->flag = false;
                x->left = x->right = x;
                agregarRaiz(x);
            }
            z->child = nullptr;
        }
        if (z->right == z) minimum = nullptr;
        else {
            NodoFibonacci* sig = z->right;
            z->left->right = z->right; z->right->left = z->left;
            minimum = sig;
        }
        pos[z->vertex] = nullptr;
        if (minimum) consolidar();
        int v = z->vertex;
        delete z;
        return v;
    }

    void decreaseKey(int vertex, double nuevaKey) {
        NodoFibonacci* x = contiene(vertex) ? pos[vertex] : nullptr;
        if (!x || nuevaKey > x->key) return;
        x->key = nuevaKey;
        NodoFibonacci* y = x->parent;
        if (y && x->key < y->key) {
            cut(x, y);
            cascadingCut(y);
        }
        if (x->key < minimum->key) minimum = x;
    }

    long long operaciones() const { return cortes; }
    long long cortesCascada() const { return cortesEnCascada; }
};
