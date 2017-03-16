#include "Labyrinthe.h"
#include "Chasseur.h"
#include "Gardien.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <strings.h>
#include <time.h>

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
	srand(time(NULL));
	this->_nwall = 0;
	this->_npicts = 0;
	this->_nboxes = 0;
	this->_nguards = 0;
	this->_nlines =  0;
	this->_nrows = 0;

	this->_nguards_models = 4;
	this->_guards_models = new string[this->_nguards_models];
	this->_guards_models[0] = "drfreak";
	this->_guards_models[1] = "Marvin";
	this->_guards_models[2] = "Potator";
	this->_guards_models[3] = "garde";

	this->_parse_map(filename);
	this->_debug();
	

	/*return;
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
	_data [(int)(_guards [4] -> _x / scale)][(int)(_guards [4] -> _y / scale)] = 1;*/

}

bool Labyrinthe::_parse_map(char* filename)
{
	ifstream file (filename);
	
	if (file.is_open())
	{
		char first;
		bool beign_lab;
		string line;
		streampos lab_offset;
		streampos line_offset;
		
		beign_lab = false;
		lab_offset = 0;

		while (getline(file, line))
		{
			// On ne se soucie pas des lignes vides
			if (_is_empty_line(line))
			{
				// Récupération de la position de la ligne courant dans le fichier
				line_offset = file.tellg();
				continue;
			}

			first = this->_get_first_char(line);

			// On passe les lignes commentées
			if (first == '#') continue;

			// Partie supérieure du fichier, configuration des images
			if (!beign_lab)
			{
				if (islower(first))
				{
					// Configuration des images
				}

				// La définition du labyrinthe commence forcément par un angle
				else if (first == '+') 
				{
					beign_lab = true;

					// Récupération de la position à laquelle la déclaration du labyrinthe commence
					lab_offset = line_offset;
				}

				else
				{
					cout << "Fichier labyrinthe malformé" << endl;
					return false;
				}
			}

			// Partie inférieure du fichier, déclaration du labyrinthe
			if (beign_lab)
			{
				this->_nlines++;
				if (this->_nrows < (int)line.length()) this->_nrows = line.length();

				for (unsigned int i = 0; i < line.length(); i++)
				{
					if (islower(line[i]))
					{
						if (line[i] == 'x') this->_nboxes++;
						else this->_npicts++;
					}

					if (line[i] == 'G' || line[i] == 'C') this->_nguards++;
				}
			}
			
			line_offset = file.tellg();
		}

		// On replace de pointeur sur la première ligne du labyrinthe
		file.clear();
		file.seekg(lab_offset);

		// Allocation des tableaux
		this->_picts = new Wall[this->_npicts];
		this->_boxes = new Box[this->_nboxes];
		this->_guards = new Mover*[this->_nguards];
		this->_data = new char*[this->width()];
		for (int i = 0; i < this->width(); i++)
		{
			this->_data[i] = new char[this->height()];
			for (int j = 0; j < this->height(); j++)
				this->_data[i][j] = 1;
		}
		this->_fill_data(file);

		file.clear();
		file.seekg(lab_offset);
		this->_walls = new Wall[this->_nwall];

		this->_create_walls(file);
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

// 2e tour de boucle
// Compte le nombre de murs
// Rempli la matrice _data
// Créer les images, les caisses, les chasseurs et le trésor
void Labyrinthe::_fill_data(ifstream &file)
{
	int nline;
	int defined_guards;
	int defined_boxes;
	int defined_picts;
	bool line_wall; // Booléen pour compte les murs horizontaux
	string line;

	// Tableau de booléens pour compter les murs verticaux
	bool tab_walls[this->_nrows];

	nline = defined_picts = defined_boxes = 0;
	defined_guards = 1; // On réserve le 0 pour le chasseur
	for (int i = 0; i < this->_nrows; i++) tab_walls[i] = false;
	
	while (getline(file, line) && nline < this->height() /* Evite segfault si lignes vides en fin de fichier */)
	{
		line_wall = false;
		for (unsigned int nrow = 0; nrow < line.length(); nrow++)
		{
			// Gestion des murs et des caisses
			if (line[nrow] == '+')
			{
				if (line_wall) this->_nwall++; // On a un mur horizontal !
				if (tab_walls[nrow]) this->_nwall++; // On a un mur vertical
				line_wall = true;
				tab_walls[nrow] = true;
			}

			else
			{
				if (islower(line[nrow]))
				{
					if (line[nrow] == 'x')
					{
						// Placement des caisses
						this->_boxes[defined_boxes]._x = nrow;
						this->_boxes[defined_boxes]._y = nline;
						this->_boxes[defined_boxes]._ntex = 0;
						defined_boxes++;
					}

					else
					{
						// Créer les affiches ici ou 3e boucle ?
					}
				}

				else
				{
					// Si on a pas de |, c'est qu'il n'y a pas de mur vertical
					if (line[nrow] != '|') tab_walls[nrow] = false;
					// Si on a pas de -, c'est qu'il n'y a pas de mur horizontal
					if (line[nrow] != '-') line_wall = false;
				}
			}

			// Le reste
			switch (line[nrow])
			{
				case 'T': // Placement du trésor
					this->_treasor._x = nrow;
					this->_treasor._y = nline;
				break;

				case 'G': // Placement des gardiens
					this->_guards[defined_guards] = new Gardien (this, this->_guards_models[rand()%this->_nguards_models].c_str());
					this->_guards[defined_guards]->_x = nrow * this->scale;
					this->_guards[defined_guards]->_y = nline * this->scale;
					defined_guards++;
				break;

				case 'C': // Placement du chasseur
					this->_guards[0] = new Chasseur (this);
					this->_guards[0]->_x = nrow * this->scale;
					this->_guards[0]->_y = nline * this->scale;
				break;

				case ' ':
					this->_data[nrow][nline] = EMPTY;
				break;
			}
		}

		nline++;
	}
}

void Labyrinthe::_create_walls(ifstream &file)
{
	int nline;
	int defined_walls;
	bool line_wall_b;
	int line_wall_index[2]; // position of last horizontal +
	bool tab_walls_b[this->_nrows];
	int tab_walls_index[this->_nrows][2]; // position of last vertical +
	string line;

	defined_walls = 0;
	nline = 0;
	bzero(line_wall_index, 2);
	for (int i = 0; i < this->_nrows; i++)
	{
		tab_walls_b[i] = false;
		bzero(tab_walls_index[i], 2);
	}


	while (getline(file, line) && nline < this->height())
	{
		line_wall_b = false;

		for (unsigned int nrow = 0; nrow < line.length(); nrow++)
		{
			// Gestion des murs et des caisses
			if (line[nrow] == '+')
			{
				if (line_wall_b)
				{
					this->_walls[defined_walls]._x1 = line_wall_index[0];
					this->_walls[defined_walls]._y1 = line_wall_index[1];
					this->_walls[defined_walls]._x2 = nrow;
					this->_walls[defined_walls]._y2 = nline;
					this->_walls[defined_walls]._ntex = 0;
					defined_walls++;
				}

				if (tab_walls_b[nrow])
				{
					this->_walls[defined_walls]._x1 = tab_walls_index[nrow][0];
					this->_walls[defined_walls]._y1 = tab_walls_index[nrow][1];
					this->_walls[defined_walls]._x2 = nrow;
					this->_walls[defined_walls]._y2 = nline;
					this->_walls[defined_walls]._ntex = 0;
					defined_walls++;
				}

				line_wall_b = true;
				tab_walls_b[nrow] = true;

				line_wall_index[0] = nrow;
				line_wall_index[1] = nline;
				tab_walls_index[nrow][0] = nrow;
				tab_walls_index[nrow][1] = nline;
			}

			else
			{
				// créer les affiches ici ?

				if (line[nrow] != '|' && !islower(line[nrow]))
				{
					tab_walls_b[nrow] = false;
				}

				if (line[nrow] != '-' && !islower(line[nrow]))
				{
					line_wall_b = false;
				}
			}
		}

		nline++;
	}
}

void Labyrinthe::_debug()
{
	cout << "N walls : " << this->_nwall << endl;
	cout << "N picts : " << this->_npicts << endl;
	cout << "N boxes : " << this->_nboxes << endl;
	cout << "N guards : " << this->_nguards << endl;
	cout << "Width : " << this->width() << endl;
	cout << "Height : " << this->height() << endl;
	cout << "ENV scale : " << this->scale << endl;

	for (int i = 0; i < this->height(); i++)
	{
		for (int j = 0; j < this->width(); j++)
		{
			cout << (int) this->_data[j][i];
		}

		cout << endl;
	}

	cout << "TREASOR: " << this->_treasor._x << ";" << this->_treasor._y << endl;
	for (int i = 0; i < this->_nwall; i++) cout << "WALL " << i + 1 << ": " 
		<< this->_walls[i]._x1 << ";" << this->_walls[i]._y1 << " " << this->_walls[i]._x2 << ";" << this->_walls[i]._y2 << endl;
	for (int i = 0; i < this->_nboxes; i++) cout << "BOXE " << i + 1 << ": " << this->_boxes[i]._x << ";" << this->_boxes[i]._y << endl;
	for (int i = 0; i < this->_nguards; i++) cout << "GUARD " << i + 1 << ": " << this->_guards[i]->_x << ";" << this->_guards[i]->_y << endl;
	cout << "END OF DEBUG" << endl;
	
}