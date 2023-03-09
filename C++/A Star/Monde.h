#ifndef MONDE_HPP
#define MONDE_HPP
#include "Case.h"
#include "Terrain.h"

class Monde{
private:
	Case* depart;
	Case* arrive;
	Terrain* terrain;
	int choixDeplacement;
	float pourcentageObstacle;
	bool pathFound;

public:
	Monde(float,int,float,int);
	~Monde();
	void setArrive(int,int);
	void setDepart(int,int);
	void algoastar();
	void draw();
	bool GetPathFound();
	std::vector<Case*> GetPath();

};

#endif