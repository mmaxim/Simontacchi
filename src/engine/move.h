//Mike Maxim
//Move wrapper class implemented as a single word

#ifndef __MOVE_H__
#define __MOVE_H__

#include <globals.h>

// Each key part of information is stored in a byte of the 32-bit word
#define MOVE_GETFROM(b)							(0xff & (b))
#define MOVE_GETTO(b)							(0xff & ((b) >> 8))
#define MOVE_GETPIECE(b)						(0xff & ((b) >> 16))
#define MOVE_GETPROMOTION(b)					(0xff & ((b) >> 24))

// Expression forms of the assignment statements below
#define MOVE_SETFROMEXP(b,f)					(((b) & ~0xff) | (f))
#define MOVE_SETTOEXP(b,f)						(((b) & ~(0xff<<8)) | ((f) << 8))
#define MOVE_SETPIECEEXP(b,f)					(((b) & ~(0xff<<16)) | ((f) << 16))
#define MOVE_SETPROMOTIONEXP(b,f)				(((b) & ~(0xff<<24)) | ((f) << 24))

// Assignment statement form of setting moves
#define MOVE_SETFROM(b,f)						(b = MOVE_SETFROMEXP((b),(f)))
#define MOVE_SETTO(b,f)							(b = MOVE_SETTOEXP((b),(f)))
#define MOVE_SETPIECE(b,f)						(b = MOVE_SETPIECEEXP((b),(f)))
#define MOVE_SETPROMOTION(b,f)					(b = MOVE_SETPROMOTIONEXP((b),(f)))
#define MOVE_SETCASTLE(m)						(m = MOVE_SETFROMEXP(MOVE_SETTOEXP((m),64),64))
#define MOVE_SETLONGCASTLE(m)					(m = MOVE_SETFROMEXP(MOVE_SETTOEXP((m),65),65))

// Fast forming of a move
#define MOVE_FORM_MOVEFULL(b,f,t,p,prom)		(MOVE_SETFROM(MOVE_SETTO(MOVE_SETPIECE(\
												 MOVE_SETPROMOTION((b),(prom)),(p)),(t)),(f)))

// Expression forming
#define MOVE_FORM_MOVEFULLEXP(b,f,t,p,prom)		(MOVE_SETFROMEXP(MOVE_SETTOEXP(MOVE_SETPIECEEXP(\
												 MOVE_SETPROMOTIONEXP((b),(prom)),(p)),(t)),(f)))

#define MOVE_FORM_MOVEFTP(b,f,t,p)				(MOVE_SETFROM(MOVE_SETTO(MOVE_SETPIECE((b),(p)),(t)),(f)))
#define MOVE_FORM_MOVEFTPEXP(b,f,t,p)			(MOVE_SETFROMEXP(MOVE_SETTOEXP(MOVE_SETPIECEEXP((b),(p)),(t)),(f)))

//Status 
#define MOVE_ISBLANKMOVE(m)						(MOVE_GETFROM((m)) == 0 && MOVE_GETTO((m)) == 0)
#define MOVE_ISEQUAL(m1,m2)						(((m1) & 0xffff) == ((m2) & 0xffff))
#define MOVE_ISCASTLE(m)						(MOVE_GETFROM((m)) == 64)
#define MOVE_ISLONGCASTLE(m)					(MOVE_GETFROM((m)) == 65)
#define MOVE_ISPROMOTION(m)						(MOVE_GETPROMOTION((m)) > 0)

//Empty
#define MOVE_EMPTY								(0)

typedef	unsigned int							m_storage;
typedef unsigned int							move_t;


#endif
