#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>

#include "GameObject.h"
#include "Ponto.h"

GameObject::GameObject(string filename, float multiplier)
{
    this->faces = NULL;
    this->size = 0;
    this->filename = filename;
    this->multi = multiplier;
}

void GameObject::LeObjeto()
{
    ifstream arq;
    arq.open(this->filename.c_str());

    if (!arq)
    {
        std::cout << "Erro na abertura do arquivo " << this->filename << "." << endl;
        exit(1);
    }

    int sizex, sizez;

    string a;

    arq >> a;
    arq >> size;

    arq >> sizex >> sizez;

    faces = new Triangle[size];
    float x,y,z;
    string color;

    bool first = true;

    cout << "Carregando " << this->filename << endl;
    cout << "Size: " << size << endl;

    for (int i=0;i<size;i++)
    {
        // Le os trs vŽrtices
        arq >> x >> y >> z; // Vertice 1
        faces[i].P1.set(x * multi,y * multi,z * multi);
        arq >> x >> y >> z; // Vertice 2
        faces[i].P2.set(x * multi,y * multi,z * multi);
        arq >> x >> y >> z >> color; // Vertice 3
        faces[i].P3.set(x * multi,y * multi,z * multi);
        // cout << i << ": ";
        // faces[i].imprime();

        Ponto normal = Ponto();

        ProdVetorial(faces[i].P2 - faces[i].P1, faces[i].P3 - faces[i].P1, normal);
        VetUnitario(normal);

        faces[i].PN = normal;

        if (first) {
            this->pMin = faces[i].P1;
            this->pMax = faces[i].P1;
            first = false;
        } else {
            this->pMin = ObtemMinimo(this->pMin, faces[i].P1);
            this->pMax = ObtemMaximo(this->pMax, faces[i].P1);
        }

        this->pMin = ObtemMinimo(this->pMin, faces[i].P2);
        this->pMax = ObtemMaximo(this->pMax, faces[i].P2);

        this->pMin = ObtemMinimo(this->pMin, faces[i].P3);
        this->pMax = ObtemMaximo(this->pMax, faces[i].P3);

        // Falta ler o RGB da face....
    }
}

int GameObject::getNumLados()
{
    return this->size;
}

Triangle* GameObject::getFaces()
{
    return this->faces;
}

Ponto GameObject::getPMin()
{
    return this->pMin;
}

Ponto GameObject::getPMax()
{
    return this->pMax;
}
