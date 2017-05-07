#ifndef CHASSEUR_H
#define CHASSEUR_H

#include <stdio.h>
#include "Mover.h"
#include "Sound.h"

class Labyrinthe;

class Chasseur : public Mover {
private:
	int _life;
	// accepte ou non un deplacement.
	bool move_aux (double dx, double dy);
	void display_lifes(void);
	void heal(void);
public:
	static Sound*	_hunter_fire;	// bruit de l'arme du chasseur.
	static Sound*	_hunter_hit;	// cri du chasseur touché.
	static Sound*	_wall_hit;		// on a tapé un mur.

	Chasseur (Labyrinthe* l);
	// ne bouger que dans une case vide (on 'glisse' le long des obstacles)
	bool move (double dx, double dy) {
		return move_aux (dx, dy) || move_aux (dx, 0.0) || move_aux (0.0, dy);
	}
	// le chasseur ne pense pas!
	virtual void update (void);
	// fait bouger la boule de feu (ceci est une exemple, à vous de traiter les collisions spécifiques...)
	bool process_fireball (float dx, float dy);
	// tire sur un ennemi.
	void fire (int angle_vertical);
	void loose_life(void);
	bool alive(void);
};

#endif
