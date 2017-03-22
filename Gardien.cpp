#include "Gardien.h"
#include "Mover.h"
#include "Labyrinthe.h"
#include <climits>

#include <iostream>

using namespace std;

Gardien::Gardien(Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
{
	this->_mode = PAT;
	this->_protection_potential = 0.0;
	
}

void Gardien::update()
{
	//this->_calc_pp();
}

void Gardien::_calc_pp()
{
	for (int i = 0; i < this->_l->width(); i++)
	{
		for (int j = 0; j < this->_l->height(); j++)
		{
			if (((Labyrinthe*)this->_l)->dist_mat(i,j) == UINT_MAX)
				cout << "+";
			else
				cout << ((Labyrinthe*)this->_l)->dist_mat(i,j) % 10;
			cout << " ";
		}
		cout << endl;
	}

	cout << endl;
}