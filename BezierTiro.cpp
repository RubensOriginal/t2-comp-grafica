#include "BezierTiro.h"

BezierTiro::BezierTiro(Ponto a, Ponto b, Ponto c)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->index = 0;
}

bool BezierTiro::isValido()
{
    return index < 1.0f;
}

Ponto BezierTiro::proxPonto(float i)
{
    index += i;

    Ponto r1 = a * (1 - index) + b * index;
    Ponto r2 = b * (1 - index) + c * index;

    return r1 * (1 - index) + r2 * index;
}
