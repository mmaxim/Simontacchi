//Mike Maxim
//Wrapper for a variation

#ifndef __VARIATION_H__
#define __VARIATION_H__

#include <globals.h>
#include <move.h>

// Wrapper for storing and displaying the PV
class Variation {
public:

	Variation();
	Variation(const Variation&);
	Variation& operator = (const Variation&);

	string toString();

	int m_length;
	bool m_ht;
	move_t m_moves[30];

private:

	void copy(const Variation&);
};

#endif
