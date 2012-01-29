//Mike Maxim
//Variation stuff

#include <variation.h>

Variation::Variation() : m_length(0),m_ht(false) { 
	m_moves[0] = MOVE_EMPTY; 
}

Variation::Variation(const Variation& var) {
	copy(var);
}

Variation& Variation::operator =(const Variation& var) {
	if (&var != this) {
		copy(var);
	}
	return *this;
}

void Variation::copy(const Variation& var) {
	int i;
	m_length = var.m_length; m_ht = var.m_ht;
	for (i = 0; i < m_length; i++)
		m_moves[i] = var.m_moves[i];
}

string Variation::toString() {

	int i;
	string str="";
	for (i = 0; i < m_length; i++)
		str += itos(m_moves[i]) + " ";

	return str;
}