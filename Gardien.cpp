#include "Gardien.h"
#include "Mover.h"
#include "Labyrinthe.h"
#include <climits>
#include <time.h>
#include <iostream>
#include <cmath>

#define PI 3.14159265

using namespace std;

Gardien::Gardien(Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
{
	this->_mode = PAT;
	this->_protection_potential = 0.0;
	
}

void Gardien::update()
{
	//this->_calc_pp();
	if (!this->move(0.5,0.5))
		this->_angle+= rand() % 360;
}

void Gardien::_calc_pp()
{
	int dmax = 0;
	float dguard = 0;

	for (int i = 0; i < this->_l->height(); i++)
	{
		for (int j = 0; j < this->_l->width(); j++)
		{
			dmax = ((int)(((Labyrinthe*)this->_l)->dist_mat(j,i)) > dmax) ? ((Labyrinthe*)this->_l)->dist_mat(j,i) : dmax;
		}
	}

	dguard = ((Labyrinthe*)this->_l)->dist_mat(this->_x / this->_l->scale, this->_y / this->_l->scale);


	cout << "DMAX : " << dmax << endl;
	cout << "DGUARD : " << dguard << endl;
	this->_protection_potential = dmax/dguard;
	cout << "Protection potential : " << this->_protection_potential << endl << endl;
}

bool Gardien::move (double dx, double dy)
{
	dx *= sin(this->_angle * M_PI / 180);
	dy *= cos(this->_angle * M_PI / 180);

	double next_position_x = this->_x - dx;
	double next_position_y = this->_y + dy;
	
	int cell_x = next_position_x / this->_l->scale;
	int cell_y = next_position_y / this->_l->scale;

	cout << "CELL X : " << cell_x << endl;
	cout << "CELL Y : " << cell_y << endl;

	// Si pas de collision
	if (!(int)((Labyrinthe*)this->_l)->data(cell_x, cell_y))
	{
		this->_x = next_position_x;
		this->_y = next_position_y;
		return true;
	}

	else return false;
}
