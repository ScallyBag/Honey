### Overview

SugaR is a free UCI chess engine derived from Stockfish. It is
not a complete chess program and requires some UCI-compatible GUI
(e.g. XBoard with PolyGlot, eboard, Arena, Sigma Chess, Shredder, Chess
Partner, Aquarium or Fritz) in order to be used comfortably. Read the
documentation for your GUI of choice for information about how to use
SugaR with it.

This version of SugaR supports up to 128 cores. The engine defaults
to one search thread, so it is therefore recommended to inspect the value of
the *Threads* UCI parameter, and to make sure it equals the number of CPU
cores on your computer.

This version of SugaR has support for Syzygybases.


### Files

This distribution of SugaR consists of the following files:

  * Readme.md, the file you are currently reading.

  * Copying.txt, a text file containing the GNU General Public License.

  * source, a subdirectory containing the full source code, including a Makefile
    that can be used to compile SugaR on Unix-like systems.

## Uci options

## Dynamic Strategy 
_Boolean, Default: False_

To be used as additional support in the analysis of particularly complex positions.
With the increase of the score, that is how much the motor is in advantage or fundamentally closer to the checkmate.
Or In all favorable pressing situations; The advanced Pawns are penalized and the King gains more importance because
we must pay attention to the compactness and the other way around.

## Uci options	### NN section (Experimental Neural Networks inspired technics)
Experimental, MonteCarloTreeSearch, if activated, the engine's behaviour is similar to AlphaZero concepts.
Idea are implemented, integrated on SugaR:
	
### NN MCTS Self-Learning
_Boolean, Default: True_

- [https://github.com/Kellykinyama12/Stockfish] (montecarlo by Kelly Kinyama) only when true. This creates three files for machine learning purposes:
###	-experience.bin
When no more than 40 moves are played, there are non more than 6 pieces on the chessboard and at a not low depth in analysis
###	-pawngame.bin
When there are no more than 2 pieces and the game's phase is not the ending
###	-openings.bin
In the form <positionKey>.bin (>=1) at the initial stage of game with memorized the move played, the depth and the score.
In this mode, the engine is not less strong than Stockfish in a match play without learning, but a lot better in analysis mode and to solve hard positions.

When activated, it loads these files in memory and therefore it can use Search Statistics
 (Principal Variation, History Heuristics, Transposition Table, Refutation Table and Killer Moves) to play better if the same game is encountered.
It persists the following information on the Hard Disk:

- _best move_
- _board signature (hash key)_
- _best move depth_
- _best move score_

With learning, the engine became stronger and stronger.
The algorithm builds a decision tree of moves and contains the statistics similar to Monte Carlo Tree Search. It makes a decision depending on what information is in the Decision Tree, so both Best Search First and later Depth First Search.

#### NN Perceptron Search

_Boolean, Default: False_
- [https://github.com/Stefano80/Stockfish/compare/82ff04b992a53c757519a6ff61576ebd267c0cee...f013d90c669940e68fd707e2197fe655e35c04ed]
( perceptron by Stefano Cardanobile) for Late Move Reductions search as training signal

#### MCTS Search
_Boolean, Default: False_
- [https://github.com/Stefano80/Stockfish/compare/badb2ac...86fdeac]
( Montecarlo by Stefano Cardanobile and Jörg Oster) in main search function to an upper node.

### Syzygybases

**Configuration**

Syzygybases are configured using the UCI options "SyzygyPath",
"SyzygyProbeDepth", "Syzygy50MoveRule" and "SyzygyProbeLimit".

The option "SyzygyPath" should be set to the directory or directories that
contain the .rtbw and .rtbz files. Multiple directories should be
separated by ";" on Windows and by ":" on Unix-based operating systems.
**Do not use spaces around the ";" or ":".**

Example: `C:\tablebases\wdl345;C:\tablebases\wdl6;D:\tablebases\dtz345;D:\tablebases\dtz6`

It is recommended to store .rtbw files on an SSD. There is no loss in
storing the .rtbz files on a regular HD.

Increasing the "SyzygyProbeDepth" option lets the engine probe less
aggressively. Set this option to a higher value if you experience too much
slowdown (in terms of nps) due to TB probing.

Set the "Syzygy50MoveRule" option to false if you want tablebase positions
that are drawn by the 50-move rule to count as win or loss. This may be useful
for correspondence games (because of tablebase adjudication).

The "SyzygyProbeLimit" option should normally be left at its default value.

**What to expect**
If the engine is searching a position that is not in the tablebases (e.g.
a position with 8 pieces), it will access the tablebases during the search.
If the engine reports a very large score (typically 123.xx), this means
that it has found a winning line into a tablebase position.

If the engine is given a position to search that is in the tablebases, it
will use the tablebases at the beginning of the search to preselect all
good moves, i.e. all moves that preserve the win or preserve the draw while
taking into account the 50-move rule.
It will then perform a search only on those moves. **The engine will not move
immediately**, unless there is only a single good move. **The engine likely
will not report a mate score even if the position is known to be won.**

It is therefore clear that behaviour is not identical to what one might
be used to with Nalimov tablebases. There are technical reasons for this
difference, the main technical reason being that Nalimov tablebases use the
DTM metric (distance-to-mate), while Syzygybases use a variation of the
DTZ metric (distance-to-zero, zero meaning any move that resets the 50-move
counter). This special metric is one of the reasons that Syzygybases are
more compact than Nalimov tablebases, while still storing all information
needed for optimal play and in addition being able to take into account
the 50-move rule.


### Compiling it yourself

On Unix-like systems, it should be possible to compile SugaR
directly from the source code with the included Makefile.

SugaR has support for 32 or 64-bit CPUs, the hardware POPCNT
instruction, big-endian machines such as Power PC, and other platforms.

On Windows-like systems, it should be possible to compile SugaR
directly from the source code with the included Sugar.sln with Visual Studio 15.3 Community 
from GUI or with command scenario using Visual Studio 15.3 Community Commands Shell.

In general it is recommended to run `make help` to see a list of make
targets with corresponding descriptions. When not using the Makefile to
compile you need to manually
set/unset some switches in the compiler command line or use MSVC solution and project files provided; see file *types.h*
for a quick reference.


### Terms of use

SugaR is free, and distributed under the **GNU General Public License**
(GPL). Essentially, this means that you are free to do almost exactly
what you want with the program, including distributing it among your
friends, making it available for download from your web site, selling
it (either by itself or as part of some bigger software package), or
using it as the starting point for a software project of your own.

The only real limitation is that whenever you distribute SugaR in
some way, you must always include the full source code, or a pointer
to where the source code can be found. If you make any changes to the
source code, these changes must also be made available under the GPL.

For full details, read the copy of the GPL found in the file named
*Copying.txt*.
