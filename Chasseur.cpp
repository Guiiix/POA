#include "Chasseur.h"
#include "Labyrinthe.h"
#include "Gardien.h"
#include <iostream>
#include <cmath>
#include <string.h>
#include <stdlib.h>

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
	_life = 100;
	display_lifes();
}

/*
 *	Fait bouger la boule de feu (ceci est une exemple, � vous de traiter les collisions sp�cifiques...)
 */

bool Chasseur::process_fireball (float dx, float dy)
{
	// calculer la distance entre le chasseur et le lieu de l'explosion.
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		bool hit = false;
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			cout << abs(this->_l->_guards[i]->_x - this->_x) << endl;
			cout << abs(this->_l->_guards[i]->_y - this->_y) << endl << endl;
			if ((abs(this->_l->_guards[i]->_x - _fb->get_x()) <= 6) && (abs(this->_l->_guards[i]->_y - _fb->get_y()) <= 5))
			{
				((Gardien*)(this->_l->_guards[i]))->hit();
				hit = true;
			}
		}

		if (!hit)
			return true;
	}

	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	_wall_hit -> play (1. - dist2/dmax2);
	return false;
}

/*
 *	Tire sur un ennemi.
 */

void Chasseur::fire (int angle_vertical)
{
	_hunter_fire->play();
	_fb ->init(_x, _y, 10., angle_vertical, _angle);
}

void Chasseur::update()
{
	int case_x = this->_x / this->_l->scale;
	int case_y = this->_y / this->_l->scale;

	// On a atteint le tr�sor !
	if ( ( abs ( case_x - _l->_treasor._x ) <= 1 ) && ( abs ( case_y - _l->_treasor._y ) <= 1))
		partie_terminee(true);

	for (int i = 0; i < _l->_nboxes; i++)
	{
		// On a trouv� une caisse !
		if ( ( abs ( case_x - ((Labyrinthe*)_l)->boxes_status[i].box->_x ) <= 1 ) && ( abs ( case_y - ((Labyrinthe *)_l)->boxes_status[i].box->_y ) <= 1) &&  !((Labyrinthe *)_l)->boxes_status[i].opened )
		{
			((Labyrinthe *)_l) -> boxes_status[i].opened = true;
			heal();
		}
	}
}

void Chasseur::display_lifes()
{
	message("Vie restante : %d%c", _life, '%');
}

void Chasseur::loose_life()
{
	_life -= 20;
	if (_life < 0) _life = 0;
	display_lifes();
	if (_life == 0)
		partie_terminee(false);
}

void Chasseur::heal()
{
	_life = 100;
	display_lifes();
}

bool Chasseur::alive()
{
	if (_life > 0) return true;
	return false;
}
