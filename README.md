#Simontacchi Chess Engine

Simontacchi is an attempt by Mike Maxim to forge a new XBoard/Winboard/UCI chess
engine to compete with the top commercial and amateur programs currently
available. Simontacchi is a very tactically oriented engine, it will not
sacrifice material for better position, although it will find deep combinations
which will yield material advantage. Simontacchi has been playing on the
Internet Chess Club for about 3 months and is currently rated ~2400 blitz.

The entire Simontacchi package is divided into two parts: the engine and the
controller. The engine is the actual code that comes up with moves. It
implements the search routine and all the features listed below. It is not
responsible for things like clock management, pondering, or basic protocol
requirements.

#Engine Features

* Aspiration Principle Variation Search (an optimized version of the traditional alpha-beta algorithm)
* Iterative Deepening
* Rotated Bitboard based move generation and evaluation
* Transposition Tables
* Always-Replace Main Table
* Pawn Evaluation Table
* Evaluation Cache
* Null-move forward pruning
* History and Killer Heuristics
* Lazy Evaluation
* Futility Pruning, Razoring
* Quiescence Search guided by the MVV/LVA heuristic with futility pruning
* Repetition Detection 
* Search Extensions (Check, Single Move, Recapture, Passed Pawn push (not yet
implemented))
* Static Evaluation
    * Pawn Structure
    * King Safety
    * King Attack
    * Piece Placement
    * Castling Bonus
    * Queen management in the opening

The control portion of Simontacchi is what implements the protocol
communicating with the main engine. There are two controllers, the
WinboardController and the UCIController. The WinboardController is the layer
that sits between the main engine and the Winboard protocol and is in effect in
console mode as well. Below is a list of what this means.

#WinboardController Features:

* Pondering Support (puts engine into infinite thought on ponder move)
* Clock Management
* Analysis Mode
* Book
    * Book Creation - The utility built in to Simontacchi that allows a user to build
their own custom book.
    * Book View - Allows a user to view the book moves at the current position.
* Full implementation of the XBoard/Winboard protocol.

The UCIController interfaces Simontacchi to UCI compatible GUIs. There are
still some aspects of the UCI protocol not yet implemented, but for the most
part everything is there.
