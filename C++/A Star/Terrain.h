#ifndef TERRAIN_HPP
#define TERRAIN_HPP
#include "Case.h"
#include <vector>

class Terrain{
private:
	float taille;
	int nbrCase;
	float tailleCase;
	Case*** tableau;			//** permet d'avoir une matrice donc Case*** permet d'avoir une matrice à 2 dimension de Case*
	float pourcentageObstacle;

public:
	Terrain(float, int, float);
	~Terrain();
	Case* getCase(int,int);
	void calculH(Case*,int);
	int getNbrCase();
	void draw();
	std::vector<Case*> calculVoisin(Case*,int);
};

#endif