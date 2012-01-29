//Mike Maxim
//PGN loader class used during opening book creation. Reads only
//"plain" PGN files, no comments or glyphs supported yet. Also will replay
//games if asked.

#ifndef __PGNLOADER_H__
#define __PGNLOADER_H__

#include <globals.h>
#include <board.h>
#include <notation.h>
#include <game.h>
#include <bufferedreader.h>

class PGNLoader {
public:

	PGNLoader();

	void load(const string&);
	vector<Game>& get_games();

	int num_games();
	void play_game(int);
	
private:

	void read_gamefile(const string&, vector<Game>&);
	void parse_tag(const string&, Game&);

	vector<Game> m_games;
};

#endif
