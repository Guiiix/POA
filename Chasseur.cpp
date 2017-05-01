#include "Chasseur.h"
#include <iostream>
#include <cmath>

/*
 *	Tente un deplacement.
 */

using namespace std;

bool Chasseur::move_aux (double dx, double dy)
{
	update();
	if (EMPTY == _l -> data ((int)((_x + dx) / Environnement::scale),
							 (int)((_y + dy) / Environnement::scale)))
	{
		_x += dx;
		_y += dy;
		return true;
	}
	return false;
}

/*
 *	Constructeur.
 */

Chasseur::Chasseur (Labyrinthe* l) : Mover (100, 80, l, 0)
{
	_hunter_fire = new Sound ("sons/hunter_fire.wav");
	_hunter_hit = new Sound ("sons/hunter_hit.wav");
	if (_wall_hit == 0)
		_wall_hit = new Sound ("sons/hit_wall.wav");
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;
	// on bouge que dans le vide!
	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		message ("Woooshh ..... %d", (int) dist2);
		// il y a la place.

		bool hit = false;
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			cout << abs(this->_l->_guards[i]->_x - this->_x) << endl;
			cout << abs(this->_l->_guards[i]->_y - this->_y) << endl << endl;
			if ((abs(this->_l->_guards[i]->_x - _fb->get_x()) <= 5) && (abs(this->_l->_guards[i]->_y - _fb->get_y()) <= 3))
			{
				this->_l->_guards[i]->tomber();
				hit = true;
			}
		}

		if (!hit)
			return true;
	}
	// collision...
	// calculer la distance maximum en ligne droite.
	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	// faire exploser la boule de feu avec un bruit fonction de la distance.
	_wall_hit -> play (1. - dist2/dmax2);
	message ("Booom...");
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	message ("Woooshh...");
	_hunter_fire -> play ();
	_fb -> init (/* position initiale de la boule */ _x, _y, 10.,
				 /* angles de visée */ angle_vertical, _angle);
}

void Chasseur::update()
{
	int case_x = this->_x / this->_l->scale;
	int case_y = this->_y / this->_l->scale;
	cout << "Chasseur case_x : " << case_x << endl;
	cout << "Chasseur case_y : " << case_y << endl;
	cout << "treasor case x : " << (this->_l->_treasor)._x << endl;
	cout << "treasor case x : " << (this->_l->_treasor)._y << endl;
	if ( ( abs ( case_x - _l->_treasor._x ) <= 1 ) && ( abs ( case_y - _l->_treasor._y ) <= 1))
		partie_terminee(true);
}

