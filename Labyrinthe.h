#ifndef LABYRINTHE_H
#define LABYRINTHE_H

#include "Environnement.h"
#include <string>

#define	LAB_WIDTH	80
#define	LAB_HEIGHT	25

class Labyrinthe : public Environnement {
private:
	char**	_data;
	int		_nlines;
	int		_nrows;

	bool	_parse_map(char*);
	bool	_is_empty_line(std::string);
	char	_get_first_char(std::string);
	void	_fill_data(std::ifstream&);
	void	_create_walls(std::ifstream&);
	void	_debug(void);

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
