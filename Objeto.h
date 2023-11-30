#ifndef OBJETO_H
#define OBJETO_H

#include "Ponto.h"
#include "GameObject.h"

class Objeto
{
    public:
        Objeto(GameObject* objeto, Ponto pos);
        GameObject* getObjeto();
        Ponto getPosicao();
        Ponto getPMin();
        Ponto getPMax();

    protected:

    private:
        GameObject* objeto;
        Ponto pos;
};

#endif // OBJETO_H
