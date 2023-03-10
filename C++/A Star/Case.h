#ifndef CASE_HPP
#define CASE_HPP

#include <vector>
#include "Point2D.h"
#define CROIX 1
#define DIAG 2

#define TYPE_LIBRE    0
#define TYPE_DEPART   1
#define TYPE_ARRIVEE  2
#define TYPE_OBSTACLE 3
#define TYPE_CHEMIN   4

class Case{
private:
  int coutTraverse;
  Point2D* centre;
  Point2D* origine;
  int xPosition;
  int yPosition;
  Case* parent;
  bool obstacle;
  float h;
  float g;
  float f;
  int type;
  std::vector<Case*> voisin;




public:
	Case(int,int,float,float,float,float,bool);
	~Case();
	void calculDistance(Case*,int);
	void draw(float);
	void setTypeArrive();
	void setTypeDepart();
	void setTypeChemin();
	void setF(float);
	void setG(float);
	float GetH();
	float GetF();
	float GetG();
	int  GetX();
	int GetY();
	bool IsAnObstacle();
	int  CoutTraverse();
	Case* GetParent();
	void SetParent(Case*);
	
};

#endif