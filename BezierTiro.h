#ifndef BEZIERTIRO_H
#define BEZIERTIRO_H

#include "Ponto.h"

class BezierTiro
{
    public:
        BezierTiro(Ponto a, Ponto b, Ponto c);

        Ponto proxPonto(float i);
        bool isValido();
    protected:

    private:
        Ponto a;
        Ponto b;
        Ponto c;

        float index;
};

#endif // BEZIERTIRO_H
