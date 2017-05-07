#ifndef GARDIEN_H
#define GARDIEN_H

#define TREASOR_PROTECTION_DOWN_THREASHOLD 30
#define TREASOR_PROTECTION_UP_THREASHOLD 50
#define TREASOR_DEF_MAX_DISTANCE 20
#define TREASOR_DEF_MIN_PP 30
#define VIEW 1
#define GAP_BT_FIRE 2

#include "Mover.h"
#include "Sound.h"
#include <ctime>

class Labyrinthe;

enum Mode {ATT, DEF, PAT, DEAD};

class Gardien : public Mover {

private:
	int _life;
	Mode _mode;
	float _protection_potential;
	float _protection_potential_sum;

	bool _moving_to_treasor;
	unsigned int** _way_to_treasor;
	int _way_to_treasor_len;
	int _current_index_way_treasor;

	void _calc_pp(void);
	void _att_actions(void);
	void _def_actions(void);
	void _pat_actions(void);

	bool _move_to_treasor(void);
	bool _find_way_to_treasor(int x, int y, bool** visited, int depth);
	void _free_way_to_treasor(void);
	void _die(void);
	bool _hunter_right_here(void);
	std::clock_t _last_fire;
public:
	Gardien (Labyrinthe* l, const char* modele);

	Sound*	_guard_fire;	// bruit de l'arme du gardien.
	Sound*	_guard_hit;		// cri du gardien touché.
	Sound*	_wall_hit;		// on a tapé un mur.

	// Getters
	float get_protection_potential(void) { return this->_protection_potential; };

	// mon gardien pense très mal!
	void update (void);
	// et ne bouge pas!
	bool move (double dx, double dy); 
	// ne sait pas tirer sur un ennemi.
	void fire (int angle_vertical);
	// quand a faire bouger la boule de feu...
	bool process_fireball (float dx, float dy);

	bool move_to (int x, int y);
	void hit(void);
};

#endif
