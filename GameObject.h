#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Ponto.h"

typedef struct {
    Ponto P1, P2, P3;
    Ponto PN;
    void imprime()
    {
        cout << "P1 ";  P1.imprime(); cout << endl;
        cout << "P2 ";  P2.imprime(); cout << endl;
        cout << "P3 ";  P3.imprime(); cout << endl;
    }
} Triangle;

class GameObject
{
    public:
        GameObject(string filename);
        void LeObjeto();
        int getNumLados();
        Triangle* getFaces();
        Ponto getPMin();
        Ponto getPMax();

    protected:

    private:
        Ponto pMax, pMin;
        Triangle* faces;
        string filename;
        int size;
};

#endif // GAMEOBJECT_H
