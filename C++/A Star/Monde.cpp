#include "Monde.h"
#include <cstdlib>
#include <vector>
#include <windows.h>
#include <gl/gl.h>

Monde::Monde(float tailleTerrain, int nbrCases, float pourcentageObstacle, int choixDeplacement){
	this->arrive=NULL;
	this->depart=NULL;
	this->terrain=NULL;

	this->choixDeplacement=choixDeplacement;
	this->pourcentageObstacle=pourcentageObstacle;
	this->pathFound=false;

	this->terrain=new Terrain(tailleTerrain,nbrCases,pourcentageObstacle);
}

Monde::~Monde(){
	if(this->terrain!=NULL){
		delete this->terrain;
	}
}

void Monde::setArrive(int iArrive, int jArrive){
	this->arrive=this->terrain->getCase(iArrive,jArrive);
	this->arrive->setTypeArrive();
}

void Monde::setDepart(int iDepart, int jDepart){
	this->depart=this->terrain->getCase(iDepart,jDepart);
	this->depart->setTypeDepart();
}

void Monde::draw(){
	this->terrain->draw();
}

bool Monde::GetPathFound()
{
	return this->pathFound;
}

std::vector<Case*> Monde::GetPath() {
	std::vector<Case*> PATH;
	Case* actuel = arrive->GetParent();
	while (actuel->GetParent() != NULL){
		PATH.push_back(actuel);
		actuel->setTypeChemin();
		actuel = actuel->GetParent();
	}
	return PATH;
}


void Monde::algoastar() {
	std::vector<Case*> OPEN;
	std::vector<Case*> CLOSED;
	OPEN.push_back(depart);
	this->terrain->calculH(arrive, choixDeplacement);
	this->depart->setG(0);
	this->depart->setF(this->depart->GetH());
	while(OPEN.size()!=0){
		Case* n=OPEN.back();
		OPEN.pop_back();
		if(n==arrive){
			this->pathFound=true;
			return;
		}
		CLOSED.push_back(n);
		std::vector<Case*> voisin = this->terrain->calculVoisin(n,choixDeplacement);
		for (int i = 0; i < voisin.size(); i++) {
			float G_tmp=voisin.back()->CoutTraverse()+voisin.back()->GetParent()->GetG();
			float F_tmp=G_tmp+voisin.back()->GetH();
			for (int j = 0; j < OPEN.size(); j++) {
				if (OPEN.at(j) == voisin.back()) {
					if (OPEN.at(j)->GetF() > voisin.back()->GetF()) {
						for (int k = 0; k < CLOSED.size(); k++) {
							if (CLOSED.at(k) == voisin.back()) {
								if (CLOSED.at(k)->GetF() > voisin.back()->GetF()) {
									voisin.back()->SetParent(n);
									voisin.back()->setG(G_tmp);
									voisin.back()->setF(F_tmp);
									OPEN.erase(OPEN.begin()+j-1);
									CLOSED.erase(CLOSED.begin() + k - 1);
									OPEN.push_back(voisin.back());
								}
							}
						}
					}
				}
			}
			OPEN.pop_back();
		 }
	}
}
