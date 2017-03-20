#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <string>

class Labyrinthe : public Environnement {
private:
	char**	_data;
	char**	_dist_mat;
	char	_picts_files[26][128]; // 26 affiches différentes max
	int		_nlines;
	int		_nrows;
	int		_nguards_models;
	std::string* _guards_models;

	void	_parse_map(char*);
	bool	_is_empty_line(std::string);
	char	_get_first_char(std::string);
	void	_fill_data(std::ifstream&);
	void	_create_walls(std::ifstream&);
	void	_debug(void);
	void	_stick_v_pict(int, unsigned int,int);
	void	_stick_h_pict(int, unsigned int,int);
	std::streampos _explore_and_configure(std::ifstream &file);

public:
	Labyrinthe (char*);
	int width () { return _nrows;}	// retourne la largeur du labyrinthe.
	int height () { return _nlines;}	// retourne la longueur du labyrinthe.
	char data (int i, int j)
	{
		return _data [i][j];
	}	// retourne la case (i, j).
};

#endif
