// **********************************************************************
// PUCRS/Escola Politécnica
// COMPUTAÇÃO GRÁFICA
//
// Programa básico para criar aplicacoes 3D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>

using namespace std;


#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"
#include "Ponto.h"
#include "TextureClass.h"
#include "BezierTiro.h"
#include "GameObject.h"
#include "Objeto.h"

Temporizador T;
double AccumDeltaT=0;


GLfloat AspectRatio, angulo=0;

// Controle do modo de projecao
// 0: Projecao Paralela Ortografica; 1: Projecao Perspectiva
// A funcao "PosicUser" utiliza esta variavel. O valor dela eh alterado
// pela tecla 'p'
int ModoDeProjecao = 1;


// Controle do modo de projecao
// 0: Wireframe; 1: Faces preenchidas
// A funcao "Init" utiliza esta variavel. O valor dela eh alterado
// pela tecla 'e'
int ModoDeExibicao = 1;

double nFrames=0;
double TempoTotal=0;
Ponto CantoEsquerdo (-20,-1,-10);
int cam = 1;

int pontos = 0;

GLuint Tex[2];

float rotBase = 0.0f;
float rotBraco = 45.0f;
Ponto pontoBraco (0, 0, 0);

float posBaseX = -5.0f;
float posBaseZ = 10.0f;

float forca = 10.0f;

vector<Ponto> pontosTiros;

vector<BezierTiro*> tiros;

bool testTiro = false;

bool parede[25][15];

GameObject** objetos;

vector<Objeto*> inimigos;

bool VerificaColisao(Ponto tiro);

// **********************************************************************
//  void init(void)
//        Inicializa os parametros globais de OpenGL
// **********************************************************************
void init(void)
{
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // Fundo de tela preto

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE );
    glEnable(GL_NORMALIZE);
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    //glShadeModel(GL_FLAT);

    Tex[0] = LoadTexture("parede-i.jpg");
    Tex[1] = LoadTexture("grama.jpg");

    objetos = new GameObject*[1];

    objetos[0] = new GameObject("dog.tri");
    objetos[0]->LeObjeto();

    inimigos.push_back(new Objeto(objetos[0], Ponto(20, 0, 0)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(12, 0, 0)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(10, 0, -5)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(15, 0, 10)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(15, 0, 4)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(21, 0, 10)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(25, 0, 0)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(18, 0, -2)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(16, 0, -5)));
    inimigos.push_back(new Objeto(objetos[0], Ponto(10, 0, 10)));

    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 25; j++) {
            parede[j][i] = true;
        }
    }

    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
    if (ModoDeExibicao) // Faces Preenchidas??
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualização da tela em 30
    {
        AccumDeltaT = 0;
        angulo+= 1;
        glutPostRedisplay();

        if (!testTiro) {
            for (int i = 0; i < tiros.size(); i++)
            {
                BezierTiro* tiro = tiros[i];
                if (tiro->isValido())
                {
                    Ponto pontoTiro = tiro->proxPonto(0.05f);
                    bool colisao = VerificaColisao(pontoTiro);
                    if (colisao) {
                        tiros.erase(tiros.begin()+i);
                        delete tiro;
                    }
                }
                else
                {
                    tiros.erase(tiros.begin()+i);
                    delete tiro;
                }
            }

        }
        if (TempoTotal > 5.0)
        {
            cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
            cout << "Nros de Frames sem desenho: " << nFrames << endl;
            cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
            TempoTotal = 0;
            nFrames = 0;
        }



    }


}


// **********************************************************************
//  void DesenhaCubo()
// **********************************************************************
void DesenhaCubo(float tamAresta)
{
    glBegin ( GL_QUADS );
    // Front Face
    glNormal3f(0,0,1);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    // Back Face
    glNormal3f(0,0,-1);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    // Top Face
    glNormal3f(0,1,0);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    // Bottom Face
    glNormal3f(0,-1,0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Right face
    glNormal3f(1,0,0);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Left Face
    glNormal3f(-1,0,0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glEnd();

}
void DesenhaParalelepipedo()
{
    glPushMatrix();
        glTranslatef(0,0,-1);
        glScalef(1,1,2);
        glutSolidCube(2);
        //DesenhaCubo(1);
    glPopMatrix();
}

void DesenhaObjeto(GameObject* objeto, bool inimigo) {
    for (int i = 0; i < objeto->getNumLados(); i++) {
        Triangle tri = objeto->getFaces()[i];

        Ponto P1 = tri.P1;
        Ponto P2 = tri.P2;
        Ponto P3 = tri.P3;
        Ponto PN = tri.PN;

        glBegin(GL_TRIANGLES);
            if (inimigo)
                glColor3f(1.0f, 0.25f, 0.25f);
            else
                glColor3f(0.0f, 1.0f, 1.0f);
            glNormal3f(PN.x, PN.y, PN.z);
            glVertex3f(P1.x, P1.y, P1.z);
            glVertex3f(P2.x, P2.y, P2.z);
            glVertex3f(P3.x, P3.y, P3.z);
        glEnd();
    }
}

// **********************************************************************
// void DesenhaLadrilho(int corBorda, int corDentro)
// Desenha uma célula do piso.
// Eh possivel definir a cor da borda e do interior do piso
// O ladrilho tem largula 1, centro no (0,0,0) e está sobre o plano XZ
// **********************************************************************
void DesenhaLadrilho(int corBorda, int idTextura, Ponto inicioTextura, Ponto finalTextura, bool ativaBorda)
{
    //defineCor(corDentro);// desenha QUAD preenchido
    glEnable(GL_TEXTURE_2D);
    glColor3f(1,1,1);
    glBindTexture(GL_TEXTURE_2D, Tex[idTextura]);

    glBegin ( GL_QUADS );
        glNormal3f(0,1,0);
        glTexCoord2d(inicioTextura.x, inicioTextura.z);
        glVertex3f(-0.5f,  0.0f, -0.5f);
        glTexCoord2d(inicioTextura.x, finalTextura.z);
        glVertex3f(-0.5f,  0.0f,  0.5f);
        glTexCoord2d(finalTextura.x, finalTextura.z);
        glVertex3f( 0.5f,  0.0f,  0.5f);
        glTexCoord2d(finalTextura.x, inicioTextura.z);
        glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    //defineCor(corBorda);
    glColor3f(0,1,0);

    if (ativaBorda) {
        glBegin ( GL_LINE_STRIP );
        glNormal3f(0,1,0);
        glVertex3f(-0.5f,  0.0f, -0.5f);
        glVertex3f(-0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();
    }

}

// **********************************************************************
//
//
// **********************************************************************
void AddPontos(int novosPontos, string texto)
{
    pontos += novosPontos;
    cout << texto << endl;
    cout << "Total de pontos: " << pontos << endl;
}

void DesenhaPiso()
{
    srand(100); // usa uma semente fixa para gerar sempre as mesma cores no piso
    glPushMatrix();
    glTranslated(CantoEsquerdo.x, CantoEsquerdo.y, CantoEsquerdo.z);
    float proporcaoX = 1.0f / 51.0f;
    float proporcaoZ = 1.0f / 26.0f;

    for(int x=0; x<50;x++)
    {
        glPushMatrix();
        for(int z=0; z<25;z++)
        {
            DesenhaLadrilho(MediumGoldenrod, 1, Ponto(x * proporcaoX, 0, z * proporcaoZ), Ponto((x + 1) * proporcaoX, 0, (z + 1) * proporcaoZ), false);
            glTranslated(0, 0, 1);
        }
        glPopMatrix();
        glTranslated(1, 0, 0);
    }
    glPopMatrix();
}

void DesenhaParede()
{
    glPushMatrix();
    glTranslated(CantoEsquerdo.x + 25, CantoEsquerdo.y, CantoEsquerdo.z);
    glRotated(90, 0, 0, 1);
    float proporcaoX = 1.0f / 16.0f;
    float proporcaoZ = 1.0f / 26.0f;

    for(int x=0; x<15;x++)
    {
        glPushMatrix();
        for(int z=0; z<25;z++)
        {
            if (parede[z][x]) {
                DesenhaLadrilho(MediumGoldenrod, 0, Ponto(x * proporcaoX, 0, z * proporcaoZ), Ponto((x + 1) * proporcaoX, 0, (z + 1) * proporcaoZ), false);
            }
            glTranslated(0, 0, 1);
        }
        glPopMatrix();
        glTranslated(1, 0, 0);
    }

    glPopMatrix();
}

void DesenhaVeiculo()
{
    glPushMatrix();
        glTranslatef(posBaseX, -0.5f, posBaseZ); // Pos VeÌculo
        glRotatef(rotBase, 0, 1, 0);
        glPushMatrix();
            glColor3f(0.0f, 1.0f, 1.0f);
            glScalef(3.0f, 1.0f, 2.0f);
            glutSolidCube(1);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(1.0f, 0.0f, 0.0f);
            glRotatef(rotBraco - 90.0f, 0, 0, 1);
            glScalef(0.5f, 2.5f, 0.5f);
            glColor3f(1.0f, 0.64f, 0.0f);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

void DesenhaTiro(Ponto ponto)
{
    glPushMatrix();
        glTranslatef(ponto.x, ponto.y, ponto.z);
        glColor3f(1.0f, 1.0f, 1.0f);
        glutSolidSphere(0.3, 10, 10);
    glPopMatrix();
}

void Atira()
{
    Ponto DirecaoDoCanhao = Ponto(1,0,0);
    DirecaoDoCanhao.rotacionaZ(rotBraco);
    DirecaoDoCanhao.rotacionaY(rotBase);

    Ponto OffsetCanhao = Ponto(1.0f, 0.0f, 0.0f);
    OffsetCanhao.rotacionaY(rotBase);

    Ponto PosicaoDoCanhao = Ponto(posBaseX, 0.5f, posBaseZ) + Ponto (0.0f, -1.0f, 0.0f) + OffsetCanhao;

    Ponto B = PosicaoDoCanhao + (DirecaoDoCanhao * forca);

    float Distancia = 2 * forca * cos(rotBraco * 3.14 / 180);

    Ponto tempC = Ponto (Distancia, 0.0f, 0.0f);
    tempC.rotacionaY(rotBase);

    Ponto C = PosicaoDoCanhao + tempC;



    B.imprime();
    cout << "Rot Canhao: " << rotBraco << " | Rot Veiculo: " << rotBase << " | FIRE!!!" << endl;

    if (testTiro)
    {
        pontosTiros.push_back(PosicaoDoCanhao);
        pontosTiros.push_back(B);
        pontosTiros.push_back(C);
    }
    else
    {
        BezierTiro* tiro = new BezierTiro(PosicaoDoCanhao, B, C);

        tiros.push_back(tiro);
    }

}

void DestroiParede(int y, int z)
{
    if (y <= 15 && y >= 0 && z <= 25 && z >= 0) {
        parede[z][y] = false;
    }
}

bool VerificaColisao(Ponto tiro)
{

    // Verifica colisao parede
    Ponto pontoParede = tiro - CantoEsquerdo - Ponto(25, 0, 0);
    if (abs(pontoParede.x) < 0.5) {
        int y = static_cast<int>(pontoParede.y);
        int z = static_cast<int>(pontoParede.z);

        if (y <= 15 && y >= 0 && z <= 25 && z >= 0)
        {
            if (parede[z][y]) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        DestroiParede(y + i, z + j);
                    }
                }
                AddPontos(5, "Voce ganhou 5 pontos por acertar a parede.");
                return true;
            }
        }
    }

    // Verifica colisao ch„o
    Ponto pontoChao = tiro - CantoEsquerdo;
    if (pontoChao.y <= 0.5) {
        if (pontoChao.x >= 0 && pontoChao.x <= 50 && pontoChao.z >= 0 && pontoChao.z <= 25) {

            AddPontos(-5, "Voce perdeu 5 pontos por acertar o chao.");
            return true;
        }
    }

    // Verifica Colisao Inimigos
    for (int i = 0; i < inimigos.size(); i++) {
        Objeto* objeto = inimigos[i];

        Ponto pMin = objeto->getPMin();
        Ponto pMax = objeto->getPMax();

        // pMin.imprime();
        // cout << endl;
        // pMax.imprime();
        // cout << endl << endl;

        if (pMin.x <= tiro.x && pMin.y <= tiro.y && pMin.z <= tiro.z &&
            pMax.x >= tiro.x && pMax.y >= tiro.y && pMax.z >= tiro.z ) {

            delete objeto;

            inimigos.erase(inimigos.begin() + i);

            AddPontos(10, "Voce ganhou 10 pontos por acertar um inimigo.");
            return true;
        }

    }

    return false;
}




// **********************************************************************
//  void DefineLuz(void)
// **********************************************************************
void DefineLuz(void)
{
  // Define cores para um objeto dourado
  GLfloat LuzAmbiente[]   = {0.4, 0.4, 0.4 } ;
  GLfloat LuzDifusa[]   = {0.7, 0.7, 0.7};
  GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9 };
  GLfloat PosicaoLuz0[]  = {0.0f, 3.0f, 5.0f };  // Posição da Luz
  GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

   // ****************  Fonte de Luz 0

 glEnable ( GL_COLOR_MATERIAL );

   // Habilita o uso de iluminação
  glEnable(GL_LIGHTING);

  // Ativa o uso da luz ambiente
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
  // Define os parametros da luz número Zero
  glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa  );
  glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular  );
  glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0 );
  glEnable(GL_LIGHT0);

  // Ativa o "Color Tracking"
  glEnable(GL_COLOR_MATERIAL);

  // Define a reflectancia do material
  glMaterialfv(GL_FRONT,GL_SPECULAR, Especularidade);

  // Define a concentraçãoo do brilho.
  // Quanto maior o valor do Segundo parametro, mais
  // concentrado será o brilho. (Valores válidos: de 0 a 128)
  glMateriali(GL_FRONT,GL_SHININESS,51);

}
// **********************************************************************

void MygluPerspective(float fieldOfView, float aspect, float zNear, float zFar )
{
    //https://stackoverflow.com/questions/2417697/gluperspective-was-removed-in-opengl-3-1-any-replacements/2417756#2417756
    // The following code is a fancy bit of math that is equivilant to calling:
    // gluPerspective( fieldOfView/2.0f, width/height , 0.1f, 255.0f )
    // We do it this way simply to avoid requiring glu.h
    //GLfloat zNear = 0.1f;
    //GLfloat zFar = 255.0f;
    //GLfloat aspect = float(width)/float(height);
    GLfloat fH = tan( float(fieldOfView / 360.0f * 3.14159f) ) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum( -fW, fW, -fH, fH, zNear, zFar );
}
// **********************************************************************
//  void PosicUser()
// **********************************************************************
void PosicUser()
{

    // Define os parâmetros da projeção Perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define o volume de visualização sempre a partir da posicao do
    // observador
    if (ModoDeProjecao == 0)
        glOrtho(-10, 10, -10, 10, 0, 7); // Projecao paralela Orthografica
    else MygluPerspective(90,AspectRatio,0.01,50); // Projecao perspectiva

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    switch (cam) {
        case 2:
            gluLookAt(25, 30, 10,   // Posição do Observador
                      23, 0, 10,     // Posição do Alvo
                      0, 0, 1);
            break;
        case 3:
            gluLookAt(-2, 5, 15,   // Posição do Observador
                      -5, 0, 0,     // Posição do Alvo
                      0.0f,1.0f,0.0f);
            break;

        default:
            gluLookAt(-15, 10, 15,   // Posição do Observador
                  0,0,0,     // Posição do Alvo
                  0.0f,1.0f,0.0f);
            break;
    }


}
// **********************************************************************
//  void reshape( int w, int h )
//		trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{

	// Evita divisão por zero, no caso de uam janela com largura 0.
	if(h == 0) h = 1;
    // Ajusta a relação entre largura e altura para evitar distorção na imagem.
    // Veja função "PosicUser".
	AspectRatio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Seta a viewport para ocupar toda a janela
    glViewport(0, 0, w, h);
    //cout << "Largura" << w << endl;

	PosicUser();

}

// **********************************************************************
//  void display( void )
// **********************************************************************
float PosicaoZ = -30;
void display( void )
{

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	DefineLuz();

	PosicUser();

    DesenhaPiso();
    DesenhaParede();
    DesenhaVeiculo();


    if (testTiro)
    {
        for (int i = 0; i < pontosTiros.size(); i += 3) {
            Ponto a = pontosTiros[i];
            Ponto b = pontosTiros[i + 1];
            Ponto c = pontosTiros[i + 2];

            DesenhaTiro(a);
            DesenhaTiro(b);
            DesenhaTiro(c);

            glBegin ( GL_LINE_STRIP );
                glNormal3f(0,1,0);
                glVertex3f(a.x,  a.y, a.z);
                glVertex3f(b.x,  b.y, b.z);
                glVertex3f(c.x,  c.y, c.z);
            glEnd();

        }
    }
    else
    {
        for (int i = 0; i < tiros.size(); i++)
        {
            BezierTiro* tiro = tiros[i];
            if (tiro->isValido())
            {
                DesenhaTiro(tiro->proxPonto(0.0f));
            }
        }
    }

    for (int i = 0; i < inimigos.size(); i++) {
        Objeto* obj = inimigos[i];

        glPushMatrix();
            // glScalef(0.25f, 0.25f, 0.25f);
            glTranslated(obj->getPosicao().x, obj->getPosicao().y, obj->getPosicao().z);
            DesenhaObjeto(obj->getObjeto(), true);
        glPopMatrix();
    }



	glutSwapBuffers();
}


// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
	switch ( key )
	{
    case 27:        // Termina o programa qdo
      exit ( 0 );   // a tecla ESC for pressionada
      break;
    case '1':
        cam = 1;
        break;
    case '2':
        cam = 2;
        break;
    case '3':
        cam = 3;
        break;
    case 'w':
        posBaseX += cos(rotBase * 0.017453f) * 0.11f;
        posBaseZ -= sin(rotBase * 0.017453f) * 0.11f;
        break;
    case 's':
        posBaseX -= cos(rotBase * 0.017453f) * 0.11f;
        posBaseZ += sin(rotBase * 0.017453f) * 0.11f;
        break;
    case 'a':
        rotBase += 3.0f;
        break;
    case 'd':
        rotBase -= 3.0f;
        break;
    case ' ':
        Atira();
        break;
    case 'c':
        while (!pontosTiros.empty())
        {
                pontosTiros.pop_back();
        }
        break;
    case 'p':
            ModoDeProjecao = !ModoDeProjecao;
            glutPostRedisplay();
            break;
    case 'e':
            ModoDeExibicao = !ModoDeExibicao;
            init();
            glutPostRedisplay();
            break;
    default:
            cout << key;
    break;
  }
}

// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:
		    if (rotBraco + 3.0f >= 90.0f)
                rotBraco = 90.0f;
            else
                rotBraco += 3.0f;
			break;
	    case GLUT_KEY_DOWN:
            if (rotBraco - 3.0f <= 0.0f)
                rotBraco = 0.0f;
            else
                rotBraco -= 3.0f;
			break;
        case GLUT_KEY_LEFT:
            if (forca - 0.5f <= 0.0f)
                forca = 0.0f;
            else
                forca -= 0.5f;
            break;
            cout << "Forca em " << forca << endl;
        case GLUT_KEY_RIGHT:
            forca += 0.5f;
            cout << "Forca em " << forca << endl;
            break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int main ( int argc, char** argv )
{
	glutInit            ( &argc, argv );
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
	glutInitWindowPosition (0,0);
	glutInitWindowSize  ( 700, 700 );
	glutCreateWindow    ( "Computacao Grafica - Exemplo Basico 3D" );

	init ();
    //system("pwd");

	glutDisplayFunc ( display );
	glutReshapeFunc ( reshape );
	glutKeyboardFunc ( keyboard );
	glutSpecialFunc ( arrow_keys );
	glutIdleFunc ( animate );

	glutMainLoop ( );
	return 0;
}



