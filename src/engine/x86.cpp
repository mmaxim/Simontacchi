//Mike Maxim
//Assembler routines to help bitboards

#include <board.h>

// Locate first set bit in the bitboard (bit 1 is MSB)
int NO_PROLOG Board::bb_firstset(bitboard word) {
  #ifdef WIN32

    __asm {
		cmp		dword ptr[8+esp], 1h
        sbb		eax, eax
		mov		edx, dword ptr[8+esp+eax*4]
        bsr     ecx, edx
        jz      l4
		not		eax
        and     eax, 20h
		add		eax, ecx
        ret
l4:     mov		eax, 40h
        ret
	}
#else
   return __builtin_clzll(word); 
  #endif
}

// Locate the last set bit in the bitboard (bit 1 is MSB)
int NO_PROLOG Board::bb_lastset(bitboard word) {
  #ifdef WIN32
	__asm { 
		bsf		eax, dword ptr[4+esp]
        jz		l5
        ret
l5:     bsf		edx, dword ptr[8+esp]
        jz		l6
        mov		eax, 20h
        add		eax, edx
        ret
l6:     mov		eax, 40h
        ret
	}
#else
    return __builtin_ctzll(word);
  #endif
}

//Count the number of set bits in the bitboard
int NO_PROLOG Board::bb_count(bitboard word) {
  #ifdef WIN32
  __asm {
		mov		ecx,dword ptr[4+esp]
		xor		eax,eax
		test	ecx,ecx
        jz      l1
l0:
		lea		edx,dword ptr[ecx-1]
		inc		eax
		and		ecx,edx
        jnz     l0
l1:
		mov		ecx,dword ptr[8+esp]
		test	ecx,ecx
        jz      l3
l2:
		lea		edx,dword ptr[ecx-1]
        inc     eax
		and		ecx,edx
        jnz     l2
l3:
        ret
	}
#else
  return __builtin_popcountll(word);
  #endif


  
}
