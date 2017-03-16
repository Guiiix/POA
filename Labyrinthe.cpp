//
// ================================
// Labyrinthe.cpp
// ===============================
//
// Contient l'ensemble des méthodes gérant le labyrinthe, à savoir :
// - Gestion des images (fichiers et emplacements)
// - Création des gardiens, caisses
// - Placement du trésor
// - Création des murs du labyrinthe
//

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


// Constructeur de classe
//
// @filename : nom du fichier contenant la carte du labyrinthe
//
// Initialise les différents membres de classe
// Créer les modèles pour les gardiens
// Initialise par défaut les fichiers images à la texture d'un mur classique
// Lance l'initialisation du labyrinthe

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

	for (int i = 0; i < 26; i++) sprintf (this->_picts_files[i], "%s/%s", texture_dir, "brickwall.jpg");

	this->_parse_map(filename);
}

// Lancement du parsing
// =========================================================================
// @filename : chaine de caractères représentant le nom du fichier à parser
//
// Lance les 3 tours de boucle pour la génération du labyrinthe
// Alloue l'espace mémoire nécessaire aux différentes structures de données

void Labyrinthe::_parse_map(char* filename)
{
	ifstream file (filename);
	
	if (file.is_open())
	{
		streampos lab_offset;

		lab_offset = this->_explore_and_configure(file);

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

	else cout << "Le fichier " << filename << " n'existe pas." << endl;

}


// Définition des fichiers images et taille du labyrinthe
// ====================================================================
// @file : fichier labyrinthe, pointeur placé au début du fichier
// @return : retourne la position du début de déclération du labyrinthe
//           dans le fichier
//
// Fait une premiere exploration du fichier labyrinthe.
// Commence par configurer les fichiers images dans le tableau
// _n_picts_files
// Compte ensuite le nombre d'élements du labyrinthe (caisses, gardiens,
// largeur, longueur)

streampos Labyrinthe::_explore_and_configure(ifstream &file)
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
				char tmp[128];
				int j = 0;
				bool begin_name_declaration = false;

				for (unsigned int i = 1; i < line.length(); i++)
				{
					if (line[i] == ' ' || line[i] == '\t')
					{
						if (begin_name_declaration) break;
						else continue;
					}

					begin_name_declaration = true;
					tmp[j] = line[i];
					j++;
				}

				tmp[j] = '\0';
				sprintf(this->_picts_files[(int)(first-'a')], "%s/%s", texture_dir, tmp);
			}

			// La définition du labyrinthe commence forcément par un angle
			else if (first == '+') 
			{
				beign_lab = true;

				// Récupération de la position à laquelle la déclaration du labyrinthe commence
				lab_offset = line_offset;
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
	return lab_offset;
}


// Détermine si une ligne est vide ou non
// ================================================================
// @str : chaine de caractères à tester
//
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


// Définition des divers éléments de la carte et _data
// ================================================================================
// @file : fichier labyrinthe, pointeur placé après la déclaration des images
//
// 2e tour de boucle...
// Compte le nombre de murs par le système décrit dans la fonction _create_walls
// Permet de créer les affiches à leur emplacement indiqué ainsi qu'avec la texture
// précédemmenbt définie
// Créer les caisses, leschasseur, les gardiens et le trésor

void Labyrinthe::_fill_data(ifstream &file)
{
	int nline;
	int defined_guards;
	int defined_boxes;
	int defined_picts;
	bool line_wall; // Booléen pour compte les murs horizontaux
	string line;

	// Tableau de booléens pour compter les murs verticaux
	bool tab_walls[this->width()];

	nline = defined_picts = defined_boxes = 0;
	defined_guards = 1; // On réserve le 0 pour le chasseur
	for (int i = 0; i < this->width(); i++) tab_walls[i] = false;
	
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
						// Il faut chercher à savoir si l'affiche est sur un mur
						// horizontal ou vertical
						// On suppose qu'il n'est pas possible d'avoir une affiche
						// dans un angle

						// Si l'affiche est à une des extrémités horizontale,
						// c'est qu'elle est forcément sur un mur vertical
						if (nrow == 0 || (int) nrow == this->width()-1) this->_stick_v_pict(defined_picts, nrow, nline);
						// Si l'image n'est pas sur une extrémité horizontale,
						// on peut tester si le mur est un mur horizontal
						else if (line[nrow-1] == '-' || line[nrow+1] == '-') this->_stick_h_pict(defined_picts, nrow, nline);
						// Sinon c'est donc une image verticale
						else this->_stick_v_pict(defined_picts, nrow, nline);

						this->_picts[defined_picts]._ntex = wall_texture(this->_picts_files[(int)(line[nrow] - 'a')]);
						defined_picts++;
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


// Permet de créer une affiche horizontale
// =============================================================================
// @defined_picts : index courant dans le tableau _picts
// @nrow : numéro de la colonne où est placée l'image sur la carte du labyrinthe
// @nline : numéro de la ligne où est placée l'image sur la carte du labyrinthe

void Labyrinthe::_stick_h_pict(int defined_picts, unsigned int nrow, int nline)
{
	this->_picts[defined_picts]._x1 = nrow-1;
	this->_picts[defined_picts]._x2 = nrow+1;
	this->_picts[defined_picts]._y1 = nline;
	this->_picts[defined_picts]._y2 = nline;
}


// Permet de créer une affiche verticale
// =============================================================================
// @defined_picts : index courant dans le tableau _picts
// @nrow : numéro de la colonne où est placée l'image sur la carte du labyrinthe
// @nline : numéro de la ligne où est placée l'image sur la carte du labyrinthe

void Labyrinthe::_stick_v_pict(int defined_picts, unsigned int nrow, int nline)
{
	this->_picts[defined_picts]._x1 = nrow;
	this->_picts[defined_picts]._x2 = nrow;
	this->_picts[defined_picts]._y1 = nline-1;
	this->_picts[defined_picts]._y2 = nline+1;
}


// Permet de générer les murs du labyrinthe
// =================================================================================
// @file : fichier labyrinthe, pointeur placé après la déclaration des images
// 
// 3e tour de boucle...
// Parcours le fichier du labyrinthe afin de générer les murs.
// Les booléens line_wall_b ainsi que tab_walls_b permettent de savoir si
// la ligne ou la colonne courant est placée sur un mur et, le cas échéant,
// repérer si le mur est vertical, horizontal et déduire une point d'arrivée.
// Les variables line_wall_index et tab_walls_index permettent de garder la position
// d'origine du mur (i.e. le dernier '+' rencontré)

void Labyrinthe::_create_walls(ifstream &file)
{
	int nline;
	int defined_walls;
	bool line_wall_b;
	int line_wall_index[2]; // position of last horizontal +
	bool tab_walls_b[this->width()];
	int tab_walls_index[this->width()][2]; // position of last vertical +
	string line;

	defined_walls = 0;
	nline = 0;
	bzero(line_wall_index, 2);
	for (int i = 0; i < this->width(); i++)
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


// Affiche les variables définies à des fins de debug
// ===================================================
// Variables affichées :
// Nombre de murs
// Nombre d'images
// Nombre de caisses
// Nombre de gardiens
// Longueur du labyrinthe
// Largeur du labyrinthe
// Echelle de l'environnement
// Chemin des fichiers images
// Matrice de collisions (_data)
// Position du trésor
// Positions des murs
// Positions des caisses
// Positions du chasseur et des gardiens

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

	for (int i = 0; i < 26; i++)
	{
		cout << (char)('a' + i) << " : " << this->_picts_files[i] << endl;
	}

	cout << "TREASOR: " << this->_treasor._x << ";" << this->_treasor._y << endl;
	for (int i = 0; i < this->_nwall; i++) cout << "WALL " << i + 1 << ": " 
		<< this->_walls[i]._x1 << ";" << this->_walls[i]._y1 << " " << this->_walls[i]._x2 << ";" << this->_walls[i]._y2 << endl;
	for (int i = 0; i < this->_nboxes; i++) cout << "BOXE " << i + 1 << ": " << this->_boxes[i]._x << ";" << this->_boxes[i]._y << endl;
	for (int i = 0; i < this->_nguards; i++) cout << "GUARD " << i + 1 << ": " << this->_guards[i]->_x << ";" << this->_guards[i]->_y << endl;
	cout << "END OF DEBUG" << endl;	
}