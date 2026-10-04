#include <iostream>
#include "common.h"
#include "cola_binomial.h"
#include "cola_fibonacci.h"

using namespace std;

int main() {
    cout << "Tamano de las estructuras:" << endl;
    cout << "Arista:        " << sizeof(Arista) << " bytes" << endl;
    cout << "NodoBinomial:  " << sizeof(NodoBinomial) << " bytes" << endl;
    cout << "NodoFibonacci: " << sizeof(NodoFibonacci) << " bytes" << endl;

    cout << "\nTipos basicos:" << endl;
    cout << "int:           " << sizeof(int) << " bytes" << endl;
    cout << "double:        " << sizeof(double) << " bytes" << endl;
    cout << "puntero:       " << sizeof(void*) << " bytes" << endl;

    return 0;
}