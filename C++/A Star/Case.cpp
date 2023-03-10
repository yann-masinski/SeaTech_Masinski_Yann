#include "Case.h"
#include <stdlib.h>
#include <cstdlib>
#include <cmath>
#include <windows.h>
#include <gl/gl.h>
#include <vector>

Case::Case(int x, int y, float xcentre, float ycentre, float xorigine, float yorigine, bool obs){ //Ceci est le constructeur de case
	this->centre=NULL;
	this->origine=NULL;
	this->parent = NULL;

	this->xPosition=x;
	this->yPosition=y;

	this->centre= new Point2D(xcentre,ycentre);
	this->origine= new Point2D(xorigine,yorigine);

	this->obstacle= obs;
	this->coutTraverse=	rand() % 20 ; //valeur entre 0 et 100
	this->f=0;
	this->g=0;
	this->h=0;
	
	if(obs){
		this->type=TYPE_OBSTACLE;
	}
	else
	{
		this->type=TYPE_LIBRE;
	}

	
};

Case::~Case(){					//destructeur Case
	if(this->centre!=NULL){
		delete this->centre;
	}
	if(this->origine!=NULL){
		delete this->origine;
	}
}

void Case::calculDistance(Case* arrive,int choixDeplacement){	//Calcul de la distance eurétique h
	float dx=(float)abs(this->xPosition - arrive->xPosition);
	float dy=(float)abs(this->yPosition - arrive->yPosition);
	float distance=0;

	switch (choixDeplacement){

	case CROIX:
		distance=dx+dy;
		this->h=distance;

	case DIAG:
		float a = abs(arrive->yPosition - this->yPosition);
        float b = abs(arrive->xPosition - this->xPosition);
        float racine2 = (float)sqrt(2.0);

        if (a < b) {
        float hypp = 2 * a / racine2;
        float opp = racine2 * hypp / 2;
        distance = hypp + (b - opp);
        } 
		else {
        float hypp = 2 * b / racine2;
        float opp = racine2 * hypp / 2;
        distance = hypp + (a - opp);
        }
		this->h=distance;
	}
}

void Case::draw(float tailleCaseSur2){
/* --------------------------------------------- */
/* --- Dessin du contour de la case en blanc --- */
/* --------------------------------------------- */
   glBegin(GL_LINE_LOOP);
      glColor4d(1.0,1.0,1.0,1.0);
	  glVertex2d( this->centre->x-tailleCaseSur2, this->centre->y-tailleCaseSur2 );
      glVertex2d( this->centre->x+tailleCaseSur2 , this->centre->y-tailleCaseSur2 );
      glVertex2d( this->centre->x+tailleCaseSur2 , this->centre->y+tailleCaseSur2 );
      glVertex2d( this->centre->x-tailleCaseSur2, this->centre->y+tailleCaseSur2);
   glEnd();

/* ---------------------------------------------------- */
/* --- Coloriage éventuel de l'intérieur de la case --- */
/* ---------------------------------------------------- */
   switch (this->type){
/* :::::::::::::::::::::::::::::::::::::::::::::::::::: */
/* ::: Coloriage en bleu si c'est une case obstacle ::: */
/* :::::::::::::::::::::::::::::::::::::::::::::::::::: */
   case TYPE_OBSTACLE:
      glBegin(GL_QUADS);
         glColor4d(0.0,0.0,1.0,1.0);
		 glVertex2d( this->centre->x-tailleCaseSur2+ 1.0, this->centre->y-tailleCaseSur2 + 1.0);
		 glVertex2d( this->centre->x+tailleCaseSur2 - 1.0, this->centre->y-tailleCaseSur2+ 1.0 );
		 glVertex2d( this->centre->x+tailleCaseSur2 - 1.0, this->centre->y+tailleCaseSur2- 1.0 );
		  glVertex2d( this->centre->x-tailleCaseSur2+ 1.0, this->centre->y+tailleCaseSur2- 1.0);
      glEnd();
      break;
/* :::::::::::::::::::::::::::::::::::::::::::::::::: */
/* ::: Coloriage en rouge si c'est la case départ ::: */
/* :::::::::::::::::::::::::::::::::::::::::::::::::: */
   case TYPE_DEPART:
      glBegin(GL_QUADS);
         glColor4d(1.0,0.0,0.0,1.0);
         glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y-tailleCaseSur2 + 3.0);
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y-tailleCaseSur2+ 3.0 );
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y+tailleCaseSur2- 3.0 );
		  glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y+tailleCaseSur2- 3.0);
      glEnd();
      break;
/* :::::::::::::::::::::::::::::::::::::::::::::::::: */
/* ::: Coloriage en vert si c'est la case arrivée ::: */
/* :::::::::::::::::::::::::::::::::::::::::::::::::: */
   case TYPE_ARRIVEE:
      glBegin(GL_QUADS);
         glColor4d(0.0,1.0,0.0,1.0);
            glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y-tailleCaseSur2 + 3.0);
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y-tailleCaseSur2+ 3.0 );
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y+tailleCaseSur2- 3.0 );
		  glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y+tailleCaseSur2- 3.0);
      glEnd();
      break;
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::: */
/* ::: Coloriage en cyan si c'est une case du chemin ::: */
/* ::::::::::::::::::::::::::::::::::::::::::::::::::::: */
   case TYPE_CHEMIN:
      glBegin(GL_QUADS);
         glColor4d(1.0,0.0,1.0,1.0);
            glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y-tailleCaseSur2 + 3.0);
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y-tailleCaseSur2+ 3.0 );
		 glVertex2d( this->centre->x+tailleCaseSur2 - 3.0, this->centre->y+tailleCaseSur2- 3.0 );
		  glVertex2d( this->centre->x-tailleCaseSur2+ 3.0, this->centre->y+tailleCaseSur2- 3.0);
      glEnd();
      break;
   }
}


void Case::setTypeArrive(){
	this->type=TYPE_ARRIVEE;
}

void Case::setTypeDepart(){
	this->type=TYPE_DEPART;
}

void Case::setTypeChemin() {
	this->type = TYPE_CHEMIN;
}

void Case::setF(float f){
	this->f=f;
}

void Case::setG(float g){
	this->g=g;
}

float Case::GetH(){
	return this->h;
}

float Case::GetF(){
	return this->f;
}

float Case::GetG(){
	return this->g;
}

int Case::GetX(){
	return this->xPosition;
}

int Case::GetY(){
	return this->yPosition;
}

bool Case::IsAnObstacle(){
	return obstacle;
}

int Case::CoutTraverse(){
	return this->coutTraverse;
}

Case* Case::GetParent(){
	return this->parent;
}

void Case::SetParent(Case* parent) {
	this->parent = parent;
}