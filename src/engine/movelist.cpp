//Mike Maxim
//Fast array-based access list

#include <movelist.h>

MoveList::MoveList() : m_length(0) {

}

move_t MoveList::get(int index) const {
	return m_movelist[index];
}

void MoveList::insert(move_t move) {
	m_movelist[m_length++] = move;
}

void MoveList::join(const MoveList& list) {
	int i;
	for (i = 0; i < list.m_length; i++)
		insert(list.get(i));
}

void MoveList::clear() {
	m_length = 0;
}

int MoveList::size() const {
	return m_length;
}

void MoveList::sort(int* weights) {
	
	int i,j;
	int val;
	move_t* ptr;
	int wval;

	// Insertion sort in ascending order
	for (i = 0, ptr = m_movelist; i < m_length; i++, ptr++) {
		val = *ptr; wval = weights[i];
		for (j = i-1; j >= 0; j--) {
			m_movelist[j+1] = m_movelist[j];
			weights[j+1] = weights[j];
			if (weights[j] > wval)
				break;
		}
		m_movelist[j+1] = val;
		weights[j+1] = wval;
	}
}

bool MoveList::exists(move_t move) {
	int i;
	for (i = 0; i < m_length; i++)
		if (MOVE_ISEQUAL(m_movelist[i],move))
			return true;
	return false;
}
