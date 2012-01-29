//Mike Maxim
//Represents a game which can be played back in engine interaction or primarily used
//during book generation.

#ifndef __GAME_H__
#define __GAME_H__

#include <globals.h>
#include <board.h>
#include <notation.h>
#include <stdio.h>

class Game {
public:

	Game();

	//Main
	void commit(const string&);
	void parse_game(const string&);
	void set_moves(const vector<move_t>&);

	//Book needs
	void reset();
	Board& get_position();
	bool next_position();
	move_t get_nextmove();

	// PGN replay needs
	void get_moves(vector<move_t>&);
	void replay_game();

	// PGN Tag Info
	void setEvent(const string&);
	string getEvent() const;
	void setName(int, const string&);
	string getName(int) const;
	void setElo(int, int);
	int getElo(int) const;
	void setECO(const string&);
	string getECO() const;
	void setResult(const string&);
	string getResult() const;

private:

	string filter_numbers(const string&);
	void write_tags(FILE*);

	vector<move_t> m_moves;
	string m_result,m_event,m_names[2],m_eco;
	int m_mindex,m_elos[2];
	Board m_board;
};

#endif
