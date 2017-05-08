#include "Gardien.h"
#include "Mover.h"
#include "Labyrinthe.h"
#include <climits>
#include <time.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "Chasseur.h"
#include "Sound.h"

#define PI 3.14159265

using namespace std;


// Constructeur de classe
// ===========================================================================
// @l : pointeur vers une instance de la classe Labyrinthe
// @modele : image représentant le gardien
// 
// Initialise les différents membres de classe
// Place le gardien en mode patrouille par défaut

Gardien::Gardien(Labyrinthe* l, const char* modele) : Mover (120, 80, l, modele)
{
	_guard_fire = new Sound ("sons/hunter_fire.wav");
	_guard_hit = new Sound ("sons/hunter_hit.wav");
	_wall_hit = new Sound ("sons/hit_wall.wav");

	this->_mode = PAT;
	this->_protection_potential = 0.0;
	this->_moving_to_treasor = false;
	this->_protection_potential_sum = 0.0;
	this->_life = 100;
	_last_fire = std::clock();
}


// Mise à jour des gardiens
// ===========================================================================
// Méthode appelée de nombreuses fois par seconde permettant de mettre à jour
// différents paramètres du gardiens (position, angle de vue, mode d'action...)
// Ici, la méthode calcul le potential de protection du trésor associé au gardien
// et lance les méthodes associé à son mode d'action.

void Gardien::update()
{
	if (_mode != DEAD) 
	{
		this->_calc_pp();
		if (this->_hunter_right_here()) this->_mode = ATT;
		if (this->_mode == ATT)
		{
			if (this->_hunter_right_here()) this->_att_actions();
			else  this->_mode = PAT;
		} 

		if (this->_mode == DEF) this->_def_actions();
		if (this->_mode == PAT) this->_pat_actions();
	}
}


// Actions des gardiens défenseurs
// ===========================================================================
// Si le gardien est en cours de déplacement vers le trésor, la fonction
// s'assure qu'il suive le chemin calculé jusqu'à ce qu'il arrive à 
// destination ou que le potential de protection du trésor soit atteint.
// S'il est déjà à sa position de défense, il tourne en rond afin de détecter
// le chasseur dans toutes les directions.

void Gardien::_def_actions()
{
	unsigned int pos_x = this->_x / this->_l->scale;
	unsigned int pos_y = this->_y / this->_l->scale;
	
	if (this->_moving_to_treasor)
	{	
		// On s'arrête à côté du trésor au maximum
		if (this->_current_index_way_treasor == this->_way_to_treasor_len - 2)
		{
			this->_moving_to_treasor = false;
			this->_free_way_to_treasor();
		}
		
		else if (this->_protection_potential_sum > TREASOR_PROTECTION_DOWN_THREASHOLD && this->_current_index_way_treasor < TREASOR_DEF_MAX_DISTANCE)
		{
			this->_moving_to_treasor = false;
			this->_free_way_to_treasor();
		}

		else
		{
			this->move_to(this->_way_to_treasor[this->_current_index_way_treasor + 1][0] - pos_x, this->_way_to_treasor[this->_current_index_way_treasor + 1][1] - pos_y);
			
			if (pos_x == _way_to_treasor[_current_index_way_treasor+1][0] &&
				pos_y == _way_to_treasor[_current_index_way_treasor+1][1])
				this->_current_index_way_treasor++;
		}
	}

	else
	{
		this->_angle = (this->_angle + 1) % 360;
		if (_protection_potential_sum > TREASOR_PROTECTION_UP_THREASHOLD) this->_mode = PAT;
		if (_protection_potential_sum < TREASOR_PROTECTION_DOWN_THREASHOLD) this->_move_to_treasor();
	}
}


// Actions des gardiens attaquants
// ==========================================================================

void Gardien::_att_actions()
{
	Mover* hunter = _l->_guards[0];

	double x = hunter->_x - _x;
	double y = hunter->_y - _y;

	_angle = 180 * atan2(-x, y) / M_PI;
	fire(0);
}


// Actions des gardiens patrouilleurs
// ===========================================================================
// Par défaut, un patrouilleur se déplace jusqu'à ce qu'il heurte un obstacle.
// Dans le cas d'une collision, celui-ci repart dans une direction aléatoire.
// Un patrouilleur vérifie également que le seuil de protection du trésor 
// est atteint. Si ce n'est pas le cas et que moins de la moitié de la population
// est déjà défenseur, celui-ci en devient un et se déplace vers le trésor.

void Gardien::_pat_actions()
{
	if (!this->move(1, 1)) 
		this->_angle += rand() % 360;

	if (_protection_potential_sum < TREASOR_PROTECTION_DOWN_THREASHOLD)
	{
		int number_of_defensers = 0;
		for (int i = 1; i < this->_l->_nguards; i++)
		{
			if (((Gardien *) (this->_l->_guards[i]))->_mode == DEF)
				number_of_defensers++;
		}
		if (number_of_defensers < (this->_l->_nguards / 2))
		{
			this->_mode = DEF;
			this->_move_to_treasor();
		}
	}
}


// Calcul du potentiel de protection
// ===========================================================================
// Cette méthode permet de calculer le potentiel de protection du trésor 
// associé à un gardien. Pour cela, nous récupérons tout d'abord la valeur de 
// la case la plus éloignée du trésor. Le rapport entre ce nombre et la distance
// du gardien au trésor permet d'obtenir le potentiel de protection.
// Enfin, la somme des potentiels de tous les gardien est calculée et mise à jour.

void Gardien::_calc_pp()
{
	int dmax = 0;
	float dguard = 0;

	for (int i = 0; i < this->_l->height(); i++)
		for (int j = 0; j < this->_l->width(); j++)
			dmax = ((int)(((Labyrinthe*)this->_l)->dist_mat(j,i)) > dmax) ? ((Labyrinthe*)this->_l)->dist_mat(j,i) : dmax;

	dguard = ((Labyrinthe*)this->_l)->dist_mat(this->_x / this->_l->scale, this->_y / this->_l->scale);

	this->_protection_potential = dmax / dguard + (rand() % 20);

	float protection_potential_sum = 0;

	for (int i = 1; i < this->_l->_nguards; i++) 
		protection_potential_sum += ((Gardien *) (this->_l->_guards[i]))->get_protection_potential();

	float max = dmax * (_l->_nguards - 1);
	_protection_potential_sum = (protection_potential_sum / max) * 100;
}


// Déplace le gardien
// ===========================================================================
// @dx : vitesse de déplacement horizontale
// @dy : vitesse de déplacement verticale
//
// Cette méthode permet de déplacer le gardien en tenant compte de son angle
// de vision.
// Renvoi faux en cas de collision

bool Gardien::move (double dx, double dy)
{
	dx *= sin(this->_angle * M_PI / 180);
	dy *= cos(this->_angle * M_PI / 180);

	double next_position_x = this->_x - dx;
	double next_position_y = this->_y + dy;
	
	int cell_x = next_position_x / this->_l->scale;
	int cell_y = next_position_y / this->_l->scale;

	// Si pas de collision
	if (!(int)((Labyrinthe*)this->_l)->data(cell_x, cell_y))
	{
		this->_x = next_position_x;
		this->_y = next_position_y;
		return true;
	}

	else return false;
}


// Déplace le gardien sur une case aux alentours
// ===========================================================================
// @x : décallage horizontal
// @y : décallage vertical
//
// Cette méthode permet de déplacer sur une des case se trouvant autour de lui
// de vision.
// Par exemple, pour déplacer le gardien sur la case à sa droite, on appellera 
// la fonction de la sorte : move_to ( 1 , 0 );
// Renvoi faux en cas de collision

bool Gardien::move_to(int x, int y)
{
	if (x < 0 && y < 0) this->_angle = 135;
	if (x < 0 && y ==  0)this->_angle = 90;
	if (x < 0 && y >  0) this->_angle = 45;
	if (x ==  0 && y < 0) this->_angle = 180;
	if (x ==  0 && y >  0) this->_angle = 0;
	if (x >  0 && y < 0) this->_angle = 225;
	if (x >  0 && y ==  0) this->_angle = 270;
	if (x >  0 && y >  0) this->_angle = 315;

	return this->move(1, 1);
}


// Déplace le gardien vers le trésor
// ===========================================================================
// Cette méthode est appelée lorsqu'un gardien décide de se rapprocher du 
// trésor. Celle-ci crée les pré-reqis, à savoir le tableau de booléens pour 
// lister les cases déjà visitées ainsi que la position actuelle du joueur,
// pour l'algorithme de plus cours chemin appelé.

bool Gardien::_move_to_treasor()
{
	bool** visited;
	
	this->_moving_to_treasor = true;
	visited = new bool*[this->_l->width()];

	for (int i = 0; i < this->_l->width(); i++)
	{
		visited[i] = new bool[this->_l->height()];
		for (int j = 0; j < this->_l->height(); j++) visited[i][j] = false;
	}
	
	// Chemin trouvé !
	if (this->_find_way_to_treasor(this->_x / Environnement::scale, this->_y / Environnement::scale, visited, 1))
	{
		// On ajoute la position courante en tant que première case du chemin
		this->_way_to_treasor[0][0] = _x / Environnement::scale;
		this->_way_to_treasor[0][1] = _y / Environnement::scale;

		// puis l'on positionne le compteur d'index du chemin à 0.
		this->_current_index_way_treasor = 0;

		// Nous pouvons maintenant libérer la mémoire du tableau de booléens
		for (int i = 0; i < this->_l->width(); i++) delete[] visited[i];
		delete visited;

		return true;
	}

	// Aucun chemin trouvé...
	else return false;

}


// Cherche un chemin vers le trésor
// ===========================================================================
// @x : index x dans le labyrinthe
// @y : index y dans le labyrinthe
// @visited : tableau à deux dimensions de la taille du labyrinthe
//            contenant des booléens repérant les cases déjà explorées par
//            l'algorithme
// @depth : profondeur de la récursivité. Permettra de déterminer la longueur
//          du chemin
//
// Cette méthode exécute un alorithme de plus court chemin (type Dijkstra).
// Elle commence par placer dans un tableau temporaire les informations
// des cases présentes autour de la position @x @y. Plus précisément, elle
// stock la position x, y de la case et sa distance au trésor.
// Le tableau est ensuite trié en par distance au trésor.
//  

bool Gardien::_find_way_to_treasor(int x, int y, bool** visited, int depth)
{
	unsigned int next[8][3];
	unsigned int tmp[3];
	int index_min;
	int c = 0;
	
	// On définie la case passée en paramètres comme étant explorée.
	visited[x][y] = true;

	// Placement des cases autour de X;Y dans un tableau
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0) continue; // On exclue la position courante

			next[c][0] = ((Labyrinthe*)_l)->dist_mat(x + i,y + j);
			next[c][1] = x + i;
			next[c][2] = y + j;

			// On évite les diagonales en les considérant comme étant à une distance infinie
			if (abs(i) == abs(j)) next[c][0] = UINT_MAX; 

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
		if (next[i][0] == UINT_MAX) break; // Il ne reste que les cases infranchissables

		if (visited[next[i][1]][next[i][2]]) continue; // On ignore les cases déjà visitées

		// On a trouvé le trésor !
		if (next[i][0] == 0)
		{
			// Nous pouvons déterminer la distance au trésor par la profondeur de la récursivité
			// à laquelle nous ajoutons la case courante.
			this->_way_to_treasor_len = depth + 1;

			// Création du tableau traçant le chemin vers le trésor
			this->_way_to_treasor = new unsigned int*[depth+1];

			for (int j = 0; j < depth+1; j++)
			{
				this->_way_to_treasor[j] = new unsigned int[2];
			}

			memcpy(this->_way_to_treasor[depth], (next[i]+1), 2 * sizeof(unsigned int));

			return true;
		}

		// Si nous ne sommes toujours pas arrivé au trésor, nous ré-appelons la fonction avec la case trouvée en augmentant la profondeur
		else
		{
			if (_find_way_to_treasor(next[i][1], next[i][2], visited, depth + 1))
			{
				memcpy(this->_way_to_treasor[depth], (next[i] + 1), 2 * sizeof(unsigned int));
				return true;
			}
		}
	}

	return false;
}


// Libère la mémoire du chemin vers le trésor
// ===========================================================================

void Gardien::_free_way_to_treasor()
{
	for (int i = 0; i < _way_to_treasor_len; i++) delete[] _way_to_treasor[i];
	_way_to_treasor = NULL;
	_way_to_treasor_len = 0;
	_current_index_way_treasor = 0;
}


// Blesse un gardien
// ===========================================================================

void Gardien::hit()
{
	_life -= 20;
	if (this->_life <= 0) _die();
	else tomber();
}


// Fait mourir un gardien
// ===========================================================================

void Gardien::_die()
{
	rester_au_sol();
	_mode = DEAD;
}


// Repère si le chasseur est en face du gardien
// ===========================================================================

bool Gardien::_hunter_right_here()
{
	Mover* hunter = this->_l->_guards[0];
	int pos_x = this->_x / Environnement::scale;
	int pos_y = this->_y / Environnement::scale;
	int pos_x_hunter = hunter->_x / Environnement::scale;
	int pos_y_hunter = hunter->_y / Environnement::scale;

	// Calcul de la distance entre le gardien et le chasseur
	int dst = sqrt ( pow ( this->_x - hunter->_x, 2 ) + pow ( this->_y - hunter->_y, 2));

	// Calcul de la portée de vision du gardien
	double view = VIEW * this->_life;

	if (dst <= view) {
		//Calcul calcul de l'angle 
		float angle = atan2(-(pos_x_hunter - pos_x), pos_y_hunter - pos_y_hunter);

		float x = pos_x;
		float y = pos_y;
		
		int rx = pos_x;
		int ry = pos_y;

		float xv = -sin(angle);
		float yv = cos(angle);

		while (rx != pos_x_hunter || ry != pos_y_hunter) 
		{
			if (EMPTY != _l->data(rx, ry)){
				return false;
			}

			x += xv;
			y += yv;
			rx = round(x);
			ry = round(y);
		}
		return true;
	}
	return false;
}


// Repère si le chasseur est en face du gardien
// ===========================================================================
// @angle_vertical : permet d'ajuster l'angle vertical du tir
//
// Cette méthode est appelée lorsqu'un gardien détecte le chasseur.
// Un temps entre deux tirs est défini en tant que constante
// afin d'éviter de mitrailler le joueur.

void Gardien::fire (int angle_vertical)
{
	if ((std::clock() - _last_fire) / 1000000 > GAP_BT_FIRE)
	{
		if (((Chasseur*)(this->_l->_guards[0]))->alive())
		{
			_last_fire = std::clock();
			_guard_fire -> play ();
			_fb -> init (_x, _y, 10., angle_vertical, -_angle);
		}
	}
}


// Manipule la boule de feu
// ===========================================================================
// @dx, vitesse x
// @dy, vitesse y
//
// Permet de gérer les collisions de la boule de feu du gardien
// En cas de contact avec le chasseur, la boule de feu explose
// et la santé du chasseur se dégrade.

bool Gardien::process_fireball (float dx, float dy)
{
	float	x = (_x - _fb -> get_x ()) / Environnement::scale;
	float	y = (_y - _fb -> get_y ()) / Environnement::scale;
	float	dist2 = x*x + y*y;

	if (EMPTY == _l -> data ((int)((_fb -> get_x () + dx) / Environnement::scale),
							 (int)((_fb -> get_y () + dy) / Environnement::scale)))
	{
		bool hit = false;
		if ((abs(this->_l->_guards[0]->_x - _fb->get_x()) <= 5) && (abs(this->_l->_guards[0]->_y - _fb->get_y()) <= 5))
		{
			((Chasseur*)(this->_l->_guards[0]))->loose_life();
			hit = true;
		}

		if (!hit) return true;
	}

	float	dmax2 = (_l -> width ())*(_l -> width ()) + (_l -> height ())*(_l -> height ());
	_wall_hit -> play (1. - dist2/dmax2);

	return false;
}