#include "Terrain.h"
#include <cstdlib>
#include "Case.h"
#include <iostream>

Terrain::Terrain(float longueurTerrain,  int nbrCase, float pourcentageObstacle){  
	this->taille=longueurTerrain;
	this->nbrCase=nbrCase;
	this->tailleCase=longueurTerrain/nbrCase;
	this->pourcentageObstacle = pourcentageObstacle;

	this->tableau= new Case**[nbrCase];
	for (int i=0;i<nbrCase;i++){
		this->tableau[i]= new Case*[nbrCase];
		for(int j=0; j<nbrCase;j++){
			bool obstacle;
			if(rand()%101<pourcentageObstacle){
				obstacle=true;
			}
			else{
				obstacle=false;
			}
			this->tableau[i][j]= new Case(i,j,(i+0.5f)*this->tailleCase,(j+0.5f)*this->tailleCase,i*this->tailleCase,j*this->tailleCase, obstacle);
		}
	}
}

Terrain::~Terrain(){
	if(this->tableau!=NULL){
		for(int i=0; i<this->nbrCase;i++){
			if(this->tableau[i]!=NULL){
				for(int j=0; j<this->nbrCase;j++){
					if (this->tableau[i][j]!=NULL){
						delete this->tableau[i][j];
					}
				}
				delete this->tableau[i];
			}
		}
		delete this->tableau;
	}
}

Case* Terrain::getCase(int i, int j){
	return this->tableau[i][j];
}

void Terrain::calculH(Case* arrive, int choixDeplacement){
	for(int i=0; i<this->nbrCase;i++){
		for(int j=0; j<this->nbrCase;j++){
			this->tableau[i][j]->calculDistance(arrive,choixDeplacement);
		}
	}
}

int Terrain::getNbrCase(){
	return this->nbrCase;
}

void Terrain::draw(){
	for (int i=0;i< this->nbrCase ;i++){
      for (int j=0;j<this->nbrCase;j++){
		  tableau[i][j]->draw(this->tailleCase*0.5);
      }
   }
}

std::vector<Case*> Terrain::calculVoisin(Case* n, int choixDeplacement){
	int x=n->GetX();
	int y=n->GetY();
	std::vector<Case*> voisin;
	switch (choixDeplacement){

	case CROIX:

		if (x>0){
			if(!this->tableau[x-1][y]->IsAnObstacle()){
				voisin.push_back(this->tableau[x-1][y]);
			}

		}
		if (y>0){
			if(!this->tableau[x][y-1]->IsAnObstacle()){
				voisin.push_back(this->tableau[x][y-1]);
			}
	
		}
		if (x<this->nbrCase-1){
			if(!this->tableau[x+1][y]->IsAnObstacle()){
				voisin.push_back(this->tableau[x+1][y]);
			}
	
		}
		if (y<this->nbrCase-1){
			if(!this->tableau[x][y+1]->IsAnObstacle()){
				voisin.push_back(this->tableau[x][y+1]);
			}
	
		}
		return voisin;
		break;
	case DIAG:
		std::cout<<"Y'a r";
		break;
	}
	
}