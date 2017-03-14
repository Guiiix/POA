#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <iostream>
#include <fstream>

Sound*	Chasseur::_hunter_fire;	// bruit de l'arme du chasseur.
Sound*	Chasseur::_hunter_hit;	// cri du chasseur touché.
Sound*	Chasseur::_wall_hit;	// on a tapé un mur.

using namespace std;

Environnement* Environnement::init (char* filename)
{
	return new Labyrinthe (filename);
}


Labyrinthe::Labyrinthe (char* filename)
{
	this->_nwall = 0;
	this->_npicts = 0;
	this->_nboxes = 0;
	this->_nguards = 0;
	this->_nlines =  0;
	this->_nrows = 0;

	this->_parse_map(filename);
	
	cout << "N walls : " << this->_nwall << endl;
	cout << "N picts : " << this->_npicts << endl;
	cout << "N boxes : " << this->_nboxes << endl;
	cout << "N guards : " << this->_nguards << endl;
	return;
	// les 4 murs.
	static Wall walls [] = {
		{ 0, 0, LAB_WIDTH-1, 0, 0 },
		{ LAB_WIDTH-1, 0, LAB_WIDTH-1, LAB_HEIGHT-1, 0 },
		{ LAB_WIDTH-1, LAB_HEIGHT-1, 0, LAB_HEIGHT-1, 0 },
		{ 0, LAB_HEIGHT-1, 0, 0, 0 },
	};
	// une affiche.
	//  (attention: pour des raisons de rapport d'aspect, les affiches doivent faire 2 de long)
	static Wall affiche [] = {
		{ 4, 0, 6, 0, 0 },		// première affiche.
		{ 8, 0, 10, 0, 0 },		// autre affiche.
	};
	// 3 caisses.
	static Box	caisses [] = {
		{ 70, 12, 0 },
		{ 10, 5, 0 },
		{ 65, 22, 0 },
	};
	// juste un mur autour et les 3 caisses et le trésor dedans.
	for (int i = 0; i < LAB_WIDTH; ++i)
		for (int j = 0; j < LAB_HEIGHT; ++j) {
			if (i == 0 || i == LAB_WIDTH-1 || j == 0 || j == LAB_HEIGHT-1)
				_data [i][j] = 1;
			else
				_data [i][j] = EMPTY;
		}
	// indiquer qu'on ne marche pas sur une caisse.
	_data [caisses [0]._x][caisses [0]._y] = 1;
	_data [caisses [1]._x][caisses [1]._y] = 1;
	_data [caisses [2]._x][caisses [2]._y] = 1;
	// les 4 murs.
	_nwall = 4;
	_walls = walls;
	// deux affiches.
	_npicts = 2;
	_picts = affiche;
	// la deuxième à une texture différente.
	char	tmp [128];
	sprintf (tmp, "%s/%s", texture_dir, "voiture.jpg");
	_picts [1]._ntex = wall_texture (tmp);
	// 3 caisses.
	_nboxes = 3;
	_boxes = caisses;
	// le trésor.
	_treasor._x = 10;
	_treasor._y = 10;
	_data [_treasor._x][_treasor._y] = 1;	// indiquer qu'on ne marche pas sur le trésor.
	// le chasseur et les 4 gardiens.
	_nguards = 5;
	_guards = new Mover* [_nguards];
	_guards [0] = new Chasseur (this);
	_guards [1] = new Gardien (this, "drfreak");
	_guards [2] = new Gardien (this, "Marvin"); _guards [2] -> _x = 90.; _guards [2] -> _y = 70.;
	_guards [3] = new Gardien (this, "Potator"); _guards [3] -> _x = 60.; _guards [3] -> _y = 10.;
	_guards [4] = new Gardien (this, "garde"); _guards [4] -> _x = 130.; _guards [4] -> _y = 100.;
	// indiquer qu'on ne marche pas sur les gardiens.
	_data [(int)(_guards [1] -> _x / scale)][(int)(_guards [1] -> _y / scale)] = 1;
	_data [(int)(_guards [2] -> _x / scale)][(int)(_guards [2] -> _y / scale)] = 1;
	_data [(int)(_guards [3] -> _x / scale)][(int)(_guards [3] -> _y / scale)] = 1;
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;
}

bool Labyrinthe::_parse_map(char* filename)
{
	ifstream file (filename);
	
	if (file.is_open())
	{
		string line;
		char first;
		string labyrinthe = "";
		bool pics_confd = false;
		streampos lab_offset = 0;
		streampos line_offset;

		while (getline(file, line))
		{
			// On ne se soucie pas des lignes vides
			if (_is_empty_line(line)) continue;

			first = this->_get_first_char(line);

			// On passe les lignes commentées
			if (first == '#') continue;

			// La carte du labyrithe commence forcément par un angle si le fichier
			// est bien formé
			if (!pics_confd)
			{
				if (islower(first))
				{
					// Configuration des images
				}

				// La définition du labyrinthe commence forcément par un angle
				else if (first == '+') 
				{
					pics_confd = true;
					lab_offset = line_offset;
				}

				else
				{
					cout << "Fichier labyrinthe malformé" << endl;
					return false;
				}
			}

			if (pics_confd)
			{
				this->_nlines++;
				if (this->_nrows < (int)line.length()) this->_nrows = line.length();
			}
			
			cout << line << endl;
			line_offset = file.tellg();
		}

		file.clear();
		file.seekg(lab_offset);
		this->_create_conflicts_mat(file);
	}

	else
	{
		cout << "Le fichier " << filename << " n'existe pas." << endl;
		return false;
	}

	return true;
}

// Renvoi vrai si la ligne est vide ou ne contient que des espaces.
// Dans le cas contraire, renvoi faux.
bool Labyrinthe::_is_empty_line(string str)
{ 
	return (str.find_first_not_of(' ') == std::string::npos); 
}

// Récupère le premier caractère d'une chaine de caractères autre qu'un espace.
// Suppose que la chaine de passée en paramètre contient au moins un caractère
// autre qu'un espace
char Labyrinthe::_get_first_char(string str)
{
	int i = -1;
	while (++i < (int)str.length() && str[i] == ' ');
	return str[i];
}

void Labyrinthe::_check_line_objects(string line)
{
	for (int i = 0; i < (int)line.length(); i++)
	{
		switch(line[i])
		{
			case 'x':
				this->_nboxes++;
				break;
			case 'G':
				this->_nguards++;
				break;
			default:
				break;
		}
	}
}

void Labyrinthe::_create_conflicts_mat(ifstream &file)
{
	// Initialisation de la matrice de positions 
	this->_conflicts_mat = new char*[this->_nlines];
	for (int i = 0; i < this->_nlines; i++)
	{
		this->_conflicts_mat[i] = new char[this->_nrows];
		for (int j = 0; j < this->_nrows; j++)
		{
			this->_conflicts_mat[i][j] = 1;
		}
	}
}