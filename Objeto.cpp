#include "Objeto.h"

Objeto::Objeto(GameObject* objeto, Ponto pos)
{
    this->objeto = objeto;
    this->pos = pos;
}

GameObject* Objeto::getObjeto() {
    return this->objeto;
}

Ponto Objeto::getPosicao() {
    return this->pos;
}

Ponto Objeto::getPMin() {
    return this->objeto->getPMin() + this->pos;
}

Ponto Objeto::getPMax() {
    return this->objeto->getPMax() + this->pos;
}
