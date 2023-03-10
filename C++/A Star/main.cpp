#include <windows.h>
#include <gl/glut.h>
#include <iostream>
#include <stdlib.h>
#include "Monde.h"
#include <vector>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* +++ Prototypes des fonctions pour l'affichage 2D +++ */
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void init2D();
void reshape2D(int,int);
void display2D();
void keyboard2D(unsigned char,int,int);
void specialKeyboard2D(int,int,int);
void mouse2D(int,int,int,int);
void simule2D();

/* ++++++++++++++++++++++++++++++++++++ */
/* +++ Variables globales générales +++ */
/* ++++++++++++++++++++++++++++++++++++ */
int      width;                        // Largeur courante de la fenetre de visualisation
int      height;                       // Hauteur courante de la fenetre de visualisation
int      mainWindow;                   // Id OpenGL de la fenetre generale de visualisation
GLenum   errCodeOpenGL;                // Code d'erreur OpenGL
Monde*   monde;                        // le "monde"
int      phase;                        // 0 : remplissage aléatoire avec 'INITIAL_OBSTACLE_NUMBER' cases 'obstacle'
                                       // 1 : ajout/suppression de cases 'obstacle'
                                       // 2 : choix de la case 'départ'
                                       // 3 : choix de la case 'arrivée'
                                       // 4 : recherche du chemin
                                       // 5 : fin
float tailleTerrain=500.f;
int nbrCases=25;
float pourcentageObstacle=0.2f;
int choixDeplacement=1;
const double CASE_WIDTH = (double) 500 / (double) nbrCases;    // Taille d'une case (en pixels)

/* +++++++++++++++++++++++++++ */
/* +++ Fonction principale +++ */
/* +++++++++++++++++++++++++++ */
int main(int argc, char** argv){

/* ======================================== */
/* === Création de la fenêtre graphique === */
/* ======================================== */
   glutInit(&argc,argv);
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
   width = 500;
   height = 500;
   glutInitWindowSize(width,height);
   glutInitWindowPosition(1,1);
   mainWindow = glutCreateWindow("LABYTINTHE V1.0");
   init2D();
   glutReshapeFunc(reshape2D);
   glutDisplayFunc(display2D);
   glutKeyboardFunc(keyboard2D);
   glutSpecialFunc(specialKeyboard2D);
   glutIdleFunc(simule2D);
   glutMouseFunc(mouse2D);
   glutMainLoop();

/* ================================ */
/* === Fin normale du programme === */
/* ================================ */
   return 1;
}

/* +++++++++++++++++++++++ */
/* +++ Fonction init2D +++ */
/* +++++++++++++++++++++++ */
void init2D(){
/* ======================================================== */
/* === Spécification de la couleur de vidage de l'écran === */
/* ======================================================== */
   glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );

/* =============================== */
/* === Construction du "monde" === */
/* =============================== */
   
    monde = new Monde(tailleTerrain, nbrCases, pourcentageObstacle, choixDeplacement);

/* ====================================== */
/* === Activation des fonctionnalités === */
/* ====================================== */
   glEnable(GL_DEPTH_TEST);

/* ============================== */
/* === Autres initialisations === */
/* ============================== */
   phase = 2;
   std::cout << "phase = 0\n";
}

/* ++++++++++++++++++++++++++ */
/* +++ Fonction reshape2D +++ */
/* ++++++++++++++++++++++++++ */
void reshape2D(int w, int h){
/* ================================================== */
/* === La fenêtre ne peut pas être redimensionnée === */
/* ================================================== */
   glutReshapeWindow(500,500);
}

/* ++++++++++++++++++++++++++ */
/* +++ Fonction display2D +++ */
/* ++++++++++++++++++++++++++ */
void display2D(){
/* =============================================================== */
/* === Vidage du tampon chromatique et du tampon de profondeur === */
/* =============================================================== */
   glutSetWindow(mainWindow);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/* ========================================================= */
/* === Matrice de projection de coordonnees 2D a l'écran === */
/* ========================================================= */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D((GLdouble)0.0,(GLdouble)width,(GLdouble)0.0,(GLdouble)height);

/* ================================ */
/* === Matrice de visualisation === */
/* ================================ */
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
/* ========================= */
/* === Dessin du "monde" === */
/* ========================= */
   monde->draw();

/* ============================================================= */
/* === On vérifie qu'il n'y a pas eu d'erreur OpenGL générée === */
/* === depuis le dernier appel de la fonction de display2D   === */
/* ============================================================= */
   if ((errCodeOpenGL = glGetError()) != GL_NO_ERROR){
      std::cerr << "ERROR: Probleme lors du rendu visuel 2D OpenGL : " << (char*)gluErrorString(errCodeOpenGL) << std::endl;
      exit(0);
   }

/* ========================================================== */
/* === Echange des tampons et déclenchement du traitement === */
/* ========================================================== */
   glutSwapBuffers();
   glFlush();
}

/* +++++++++++++++++++++++++++ */
/* +++ Fonction keyboard2D +++ */
/* +++++++++++++++++++++++++++ */
void keyboard2D(unsigned char key, int x, int y){
/* ========================= */
/* === Peigne des appels === */
/* ========================= */
   glutSetWindow(mainWindow);
   switch (key){

/* ---------------------------------------- */
/* --- 'q' : pour sortir définitivement --- */
/* ---------------------------------------- */
   case 'q':
      delete monde;
      exit(0);
      break;

/* ------------------------------ */
/* --- 'r' : pour recommencer --- */
/* ------------------------------ */
   case 'r':
      if (phase == 5){
         delete monde;
		 monde = new Monde(tailleTerrain, nbrCases, pourcentageObstacle, choixDeplacement);
         phase = 2;
         std::cout << "phase = 2\n";
         display2D();
      }
      break;
   }
}

/* ++++++++++++++++++++++++++++++++++ */
/* +++ Fonction specialKeyboard2D +++ */
/* ++++++++++++++++++++++++++++++++++ */
void specialKeyboard2D(int specialKey, int x, int y){
   //A FAIRE...
}

/* ++++++++++++++++++++++++ */
/* +++ Fonction mouse2D +++ */
/* ++++++++++++++++++++++++ */
void mouse2D(int button, int state, int x, int y){
/* --------------------------------------------------------- */
/* --- Détermination de la case sur laquelle on a cliqué --- */
/* --------------------------------------------------------- */
   int i = (int) (x / CASE_WIDTH);
   int j = nbrCases - 1 - (int) (y / CASE_WIDTH);

/* ----------------------------------------------- */
/* --- Action entreprise en cas de clic gauche --- */
/* ----------------------------------------------- */
   if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
      switch (phase){

/* ::::::::::::::::::::::::::::::::: */
/* ::: choix de la case 'départ' ::: */
/* ::::::::::::::::::::::::::::::::: */
      case 2:
         monde->setDepart(i,j);
         phase = 3;
         std::cout << "phase = 3\n";
         display2D();
         break;

/* :::::::::::::::::::::::::::::::::: */
/* ::: choix de la case 'arrivée' ::: */
/* :::::::::::::::::::::::::::::::::: */
      case 3:
		 monde->setArrive(i,j);
         phase = 4;
         std::cout << "phase = 4\n";
         display2D();
         break;
      }
   }
}

/* +++++++++++++++++++++++++ */
/* +++ Fonction simule2D +++ */
/* +++++++++++++++++++++++++ */
void simule2D(){
   if (phase == 4){
	   monde->algoastar();
       if (monde->GetPathFound() == true) {
           std::vector<Case*> path = monde->GetPath();
       }
       else {
           std::cout << "pas de chemin";
       }
      phase = 5;
      std::cout << "phase = 5\n";
   }
   display2D();
}
