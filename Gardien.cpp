#include "Gardien.h"
#include "Mover.h"
#include "Labyrinthe.h"
#include <climits>
#include <time.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>

#define PI 3.14159265

using namespace std;

Gardien::Gardien(Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
{
	this->_mode = PAT;
	this->_protection_potential = 0.0;
	this->_moving_to_treasor = false;
	
}

void Gardien::update()
{
	cout << "--- Guard " << this << " ---" << endl;
	cout << this->_x << ";" << this->_y << endl;
	this->_calc_pp();
	if (this->_mode == DEF) this->_def_actions();
	if (this->_mode == PAT) this->_pat_actions();
	if (this->_mode == ATT) this->_att_actions();
	cout << "--- end ---" << endl << endl;
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
	cout << "Protection potential : " << this->_protection_potential << endl;
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

/*
 * Cette méthode contient l'ensemble des actions à effectuer lorsqu'un gardien est
 * défenseur.
 * On vérifie tout d'abord si le gardien est situé aux alentours du trésor. Si ce n'est pas
 * le cas, on le fait bouger jusqu'à une case l'avoisinant.
*/
void Gardien::_def_actions()
{
	unsigned int pos_x = this->_x / this->_l->scale;
	unsigned int pos_y = this->_y / this->_l->scale;

	if (this->_moving_to_treasor)
	{
		int i = 0;

		while (this->_way_to_treasor[i][0] != pos_x || this->_way_to_treasor[i][1] != pos_y) i++;
		if (this->_way_to_treasor[i+1][0] == _l->_treasor._x &&
			this->_way_to_treasor[i+1][1] == _l->_treasor._y)
			this->_moving_to_treasor = false;
		else
		{
			cout << "we are at " << pos_x << ";" << pos_y << endl;
			cout << "we're going to " <<  this->_way_to_treasor[i+1][0] << ";" << this->_way_to_treasor[i+1][1] << endl;
			this->move_to(this->_way_to_treasor[i+1][0] - pos_x, this->_way_to_treasor[i+1][1] - pos_y);
		}

	}
}

void Gardien::_att_actions()
{

}

void Gardien::_pat_actions()
{
	float protection_potential_sum = 0;

	for (int i = 1; i < this->_l->_nguards; i++) 
		protection_potential_sum += ((Gardien *) (this->_l->_guards[i]))->get_protection_potential();

	if (protection_potential_sum < TREASOR_PROTECTION_THREASHOLD)
	{
		int number_of_defensers = 0;
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			if (((Gardien *) (this->_l->_guards[i]))->_mode == DEF)
				number_of_defensers++;
		}
		if (number_of_defensers < (this->_l->_nguards / 2))
		{
			cout << "Becoming a defenser" << endl;
			this->_mode = DEF;
			this->_move_to_treasor();
		}
	}

	else
	{
		if (!this->move(1, 1)) 
			this->_angle += rand() % 360;
	} 
}

void Gardien::_move_to_treasor()
{
	bool** visited;
	
	this->_moving_to_treasor = true;
	visited = new bool*[this->_l->width()];

	for (int i = 0; i < this->_l->width(); i++)
	{
		visited[i] = new bool[this->_l->height()];
		for (int j = 0; j < this->_l->height(); j++)
			visited[i][j] = false;
	}
	cout << "-- _move_to_treasor -- ";
	cout << "initial x : " << this->_x / Environnement::scale << ", initial y : " << this->_y / Environnement::scale << endl;
	
	if (this->_find_way_to_treasor(this->_x / Environnement::scale, this->_y / Environnement::scale, visited, 1))
	{
		cout << endl << "!!! Found the way !!!" << endl;
		this->_way_to_treasor[0][0] = _x / Environnement::scale;
		this->_way_to_treasor[0][1] = _y / Environnement::scale;
		for (int i = 0; i < this->_way_to_treasor_len; i++)
		{
			cout << this->_way_to_treasor[i][0] << ";" << this->_way_to_treasor[i][1] << endl; 
		}
	}

	else
	{
		cout << "... Failed to found the way ..." << endl;
	}

}

bool Gardien::_find_way_to_treasor(int x, int y, bool** visited, int depth)
{
	cout << endl << "-------------------------------------------------" << endl;
	cout << "_find_way_to_treasor(" << x << ", " << y << ")" << endl;
	unsigned int next[8][3];
	unsigned int tmp[3];
	int index_min;
	int c = 0;
	
	visited[x][y] = true;

	cout << "Placement des points dans le tableau" << endl;

	// On place les points autour du gardien dans un tableau
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0) continue;

			next[c][0] = ((Labyrinthe*)_l)->dist_mat(x + i,y + j);
			next[c][1] = x + i;
			next[c][2] = y + j;

			c++;
		}
	}

	// On trie ensuite ce tableau
	for (int i = 0; i < 7; i++)
	{
		index_min = i;

		for (int j = i + 1; j < 8; j++)
			if (next[j][0] < next[index_min][0])
				index_min = j;


		if (index_min != i)
		{
			memcpy(tmp, next[index_min], 3 * sizeof(unsigned int));
			memcpy(next[index_min], next[i], 3 * sizeof(unsigned int));
			memcpy(next[i], tmp, 3 * sizeof(unsigned int));
		}
	}


	for (int i = 0; i < 8; i++)
	{
		cout << "Point " << next[i][0] << " ( " << next[i][1] << " ; " << next[i][2] << " )";

		// Il ne reste que les cases infranchissables
		if (next[i][0] == UINT_MAX)
		{
			cout << " --> wall." << endl;
			break;
		}

		if (visited[next[i][1]][next[i][2]])
		{
			cout << " --> visited." << endl;;
			continue;
		}

		// On a trouvé le trésor !
		if (next[i][0] == 0)
		{
			cout << " --> [TREASOR] at depth " << depth << endl;
			cout << next[i][0] << " (" << next[i][1] << ";" << next[i][2] << ")";
			this->_way_to_treasor_len = depth;
			this->_way_to_treasor = new unsigned int*[depth+1];
			for (int j = 0; j < depth+1; j++)
			{
				this->_way_to_treasor[j] = new unsigned int[2];
			}

			memcpy(this->_way_to_treasor[depth], (next[i]+1), 2 * sizeof(unsigned int));

			return true;
		}

		else
		{
			cout << " --> Recurse." << endl;
			if (_find_way_to_treasor(next[i][1], next[i][2], visited, depth+1))
			{
				cout << " <-- " << next[i][0] << "(" << next[i][1] << ";" << next[i][2] << ")";
				memcpy(this->_way_to_treasor[depth], (next[i] + 1), 2 * sizeof(unsigned int));
				return true;
			}
		}
	}

	cout << "FALSE" << endl;
	return false;
}

void Gardien::move_to(int x, int y)
{
	cout << "moving to " << x << ";" << y << endl; 
	// A revoir
	if (x < 0 && y < 0) this->_angle = 135;
	if (x < 0 && y ==  0) this->_angle = 90;
	if (x < 0 && y >  0) this->_angle = 45;
	if (x ==  0 && y < 0) this->_angle = 180;
	if (x ==  0 && y >  0) this->_angle = 0;
	if (x >  0 && y < 0) this->_angle = 225;
	if (x >  0 && y ==  0) this->_angle = 270;
	if (x >  0 && y >  0) this->_angle = 315;

	cout << "x : " << x << endl;
	cout << "y : " << y << endl;
	cout << "Angle : " << this->_angle << endl;

	this->move(1, 1);
}


