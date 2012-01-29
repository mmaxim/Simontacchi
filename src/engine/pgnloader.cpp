//Mike Maxim
//Loads PGN game files

#include <pgnloader.h>

PGNLoader::PGNLoader() {


}

void PGNLoader::play_game(int index) {

	if (index < 0 || index >= num_games())
		return;
	
	m_games[index].replay_game();
}

void PGNLoader::load(const string& file) {
	m_games.clear();
	read_gamefile(file,m_games);
}

vector<Game>& PGNLoader::get_games() {
	return m_games;
}

int PGNLoader::num_games() {
	return (int)m_games.size();
}

void PGNLoader::read_gamefile(const string& file, vector<Game>& games) {

	BufferedReader reader;
	string curgame="",line;
	int gnum=0;
	Game ggame;

	reader.open(file);

	while (reader.hasMoreBytes()) {
		line = reader.readLine();
		if (line.length() == 0)
			continue;
		else if (line[0] == '[')
			parse_tag(line,ggame);
		else {
			curgame += line + " ";
			if (strstr(line.c_str(),"1-0") || strstr(line.c_str(),"0-1") || strstr(line.c_str(),"1/2-1/2")) {
				ggame.parse_game(curgame);
				games.push_back(ggame);
		//		ggame.commit("out.pgn");
				curgame = "";
				gnum++;
			}
		}
	}

	reader.close();
}

void PGNLoader::parse_tag(const string& tag, Game& game) {

	string rawtag,type,value;

	rawtag = tag.substr(1,tag.length()-1);
	StringTokenizer tok(rawtag,"\"");
	
	type = tok[0];
	value = tok[1];

	if (type == "Event ") 
		game.setEvent(value);
	else if (type == "White ")
		game.setName(BB_WHITE,value);
	else if (type == "Black ")
		game.setName(BB_BLACK,value);
	else if (type == "WhiteElo ")
		game.setElo(BB_WHITE,atoi(value.c_str()));
	else if (type == "BlackElo ")
		game.setElo(BB_BLACK,atoi(value.c_str()));
	else if (type == "ECO ")
		game.setECO(value);
}
