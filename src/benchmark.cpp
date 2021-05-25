/*
  Honey, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2021 The Stockfish developers (see AUTHORS file)

  Honey is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Honey is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <istream>
#include <vector>

#include "position.h"
#include "evaluate.h"
using namespace std;

namespace {

const vector<string> Defaults = {
  "setoption name UCI_Chess960 value false",
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 10",
  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 11",
  "4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 7 19",
  "rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14 moves d4e6",
  "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14 moves g2g4",
  "r3r1k1/2p2ppp/p1p1bn2/8/1q2P3/2NPQN2/PPP3PP/R4RK1 b - - 2 15",
  "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
  "r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",
  "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
  "2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
  "r1bq1r1k/b1p1npp1/p2p3p/1p6/3PP3/1B2NN2/PP3PPP/R2Q1RK1 w - - 1 16",
  "3r1rk1/p5pp/bpp1pp2/8/q1PP1P2/b3P3/P2NQRPP/1R2B1K1 b - - 6 22",
  "r1q2rk1/2p1bppp/2Pp4/p6b/Q1PNp3/4B3/PP1R1PPP/2K4R w - - 2 18",
  "4k2r/1pb2ppp/1p2p3/1R1p4/3P4/2r1PN2/P4PPP/1R4K1 b - - 3 22",
  "3q2k1/pb3p1p/4pbp1/2r5/PpN2N2/1P2P2P/5PP1/Q2R2K1 b - - 4 26",
  "6k1/6p1/6Pp/ppp5/3pn2P/1P3K2/1PP2P2/3N4 b - - 0 1",
  "3b4/5kp1/1p1p1p1p/pP1PpP1P/P1P1P3/3KN3/8/8 w - - 0 1",
  "2K5/p7/7P/5pR1/8/5k2/r7/8 w - - 0 1 moves g5g6 f3e3 g6g5 e3f3",
  "8/6pk/1p6/8/PP3p1p/5P2/4KP1q/3Q4 w - - 0 1",
  "7k/3p2pp/4q3/8/4Q3/5Kp1/P6b/8 w - - 0 1",
  "8/2p5/8/2kPKp1p/2p4P/2P5/3P4/8 w - - 0 1",
  "8/1p3pp1/7p/5P1P/2k3P1/8/2K2P2/8 w - - 0 1",
  "8/pp2r1k1/2p1p3/3pP2p/1P1P1P1P/P5KR/8/8 w - - 0 1",
  "8/3p4/p1bk3p/Pp6/1Kp1PpPp/2P2P1P/2P5/5B2 b - - 0 1",
  "5k2/7R/4P2p/5K2/p1r2P1p/8/8/8 b - - 0 1",
  "6k1/6p1/P6p/r1N5/5p2/7P/1b3PP1/4R1K1 w - - 0 1",
  "1r3k2/4q3/2Pp3b/3Bp3/2Q2p2/1p1P2P1/1P2KP2/3N4 w - - 0 1",
  "6k1/4pp1p/3p2p1/P1pPb3/R7/1r2P1PP/3B1P2/6K1 w - - 0 1",
  "8/3p3B/5p2/5P2/p7/PP5b/k7/6K1 w - - 0 1",
  "5rk1/q6p/2p3bR/1pPp1rP1/1P1Pp3/P3B1Q1/1K3P2/R7 w - - 93 90",
  "4rrk1/1p1nq3/p7/2p1P1pp/3P2bp/3Q1Bn1/PPPB4/1K2R1NR w - - 40 21",
  "r3k2r/3nnpbp/q2pp1p1/p7/Pp1PPPP1/4BNN1/1P5P/R2Q1RK1 w kq - 0 16",
  "3Qb1k1/1r2ppb1/pN1n2q1/Pp1Pp1Pr/4P2p/4BP2/4B1R1/1R5K b - - 11 40",
  "4k3/3q1r2/1N2r1b1/3ppN2/2nPP3/1B1R2n1/2R1Q3/3K4 w - - 5 1",

  // 5-man positions
  "8/8/8/8/5kp1/P7/8/1K1N4 w - - 0 1",     // Kc2 - mate
  "8/8/8/5N2/8/p7/8/2NK3k w - - 0 1",      // Na2 - mate
  "8/3k4/8/8/8/4B3/4KB2/2B5 w - - 0 1",    // draw

  // 6-man positions
  "8/8/1P6/5pr1/8/4R3/7k/2K5 w - - 0 1",   // Re5 - mate
  "8/2p4P/8/kr6/6R1/8/8/1K6 w - - 0 1",    // Ka2 - mate
  "8/8/3P3k/8/1p6/8/1P6/1K3n2 b - - 0 1",  // Nd2 - draw

  // 7-man positions
  "8/R7/2q5/8/6k1/8/1P5p/K6R w - - 0 124", // Draw

  // Mate and stalemate positions
  "6k1/3b3r/1p1p4/p1n2p2/1PPNpP1q/P3Q1p1/1R1RB1P1/5K2 b - - 0 1",
  "r2r1n2/pp2bk2/2p1p2p/3q4/3PN1QP/2P3R1/P4PP1/5RK1 w - - 0 1",
//  "8/8/8/8/8/6k1/6p1/6K1 w - -",
//  "7k/7P/6K1/8/3B4/8/8/8 b - -",

  // Chess 960
  "setoption name UCI_Chess960 value true",
  "bbqnnrkr/pppppppp/8/8/8/8/PPPPPPPP/BBQNNRKR w HFhf - 0 1 moves g2g3 d7d5 d2d4 c8h3 c1g5 e8d6 g5e7 f7f6",
  "setoption name UCI_Chess960 value false"

#if 1 // for compiling with extended bench
  ,
  //extended bench
  //http://talkchess.com/forum3/viewtopic.php?f=2&t=74992
  //Tough Tactical Test, the 35 hardest positions

   "8/4n3/8/2n5/kp1N2P1/8/8/3K4 b - - 0 1", // Mate
   "8/1p2KP2/1p4q1/1Pp5/2P5/N1Pp1k2/3P4/1N6 b - - 76 40", //draw
  "r1b1r1k1/p3nppp/2p1p3/q3P1B1/2P5/P1pB4/5PPP/1R1Q1RK1 w - - 0 1 bm Bxh7+; id TTT1.004",
  "2b2rk1/N1p3b1/p2p1n2/2PPp1q1/2B1Pn1p/PrN2P2/5RPB/R1Q4K b - - 0 1 bm N6h5; id TTT1.005",
  "r2b4/1r5k/2p1p1p1/1pPpPpPp/pQ1P1P1P/1bP3KB/8/1N6 b - - 0 1 bm a3; id TTT1.009",
  "1qrr1b2/6p1/ppkn1P2/3pP3/1PP5/2BP1pP1/BQ3P2/1RRN1n1K b - - 0 1 bm Be7; id TTT1.011",
  "b1r1r3/b1q2ppk/1p2pP1p/2Pp3Q/p2Nn3/3R4/1B4PP/5RK1 w - - 0 1 bm Bc1; id TTT1.016",
  "8/Q2qk1p1/p3p2p/3p4/1Pp5/P3PP1P/6P1/5K2 w - - 0 1 bm Qxd7+; id TTT1.019",
  "r1b3r1/b4p1k/P1p5/Pp2p1q1/4Pp1p/2PN3P/4QPPK/RR3B2 b - - 0 1 bm Bxh3; id TTT1.022",
  "3rr1bk/6np/2p1n3/1pPpNpPp/pP1QpP1P/P2PP3/2K5/B7 w - - 0 1 bm Ng6+; id TTT1.023",
  "1k5r/pbq2p2/4pP2/p1P3P1/5P1r/2p1Q3/P3B1P1/3R1RK1 w - - 0 1 bm Rb1; id TTT1.026",
  "3r1bk1/1Q1b1r2/1B3qn1/PN1Pp2n/4Pp1p/1P3P2/4B1PK/2R1N1R1 b - - 0 1 bm Qg5; id TTT1.029",
  "5r2/2p2r1k/1p1pNp2/3P1Pp1/PpP3Pp/1P5P/6K1/1R6 w - - 0 1 bm Re1; id TTT1.030",
  "r1b2rk1/1pqnbp1p/p3p1p1/4n1N1/3B4/PN1B2Q1/2P3PP/3R1R1K w - - 0 1 bm Nc5; id TTT1.034",
  "1rb2rk1/2q3pp/p2ppbP1/n1nB4/1p1NP2P/2N1B3/PPP1Q3/2KR2R1 w - - 0 1 bm Nf5; id TTT1.036",
  "8/r4b2/3p2k1/p1pPpR2/PpP1P3/1P3Pp1/6KP/8 w - - 0 1 bm Rxf7; id TTT1.041",
  "2k5/8/1p2q1p1/1P1p2r1/p1pP1p1p/P1P1pPP1/4P1BP/4N1RK w - - 0 1 bm Bh3; id TTT1.044",
  "1r1qr1k1/3bb1p1/3p1pPB/2p1pP2/2PnP3/p1Q2N1P/P2RB2K/6R1 w - - 0 1 bm Nxd4; id TTT1.046",
  "2b2rk1/3nbppp/1q2p3/n2pP3/p1pP1PN1/prP2N1P/2BB2PK/1R2QR2 w - - 0 1 bm Bxh7+; id TTT1.048",
  "k4r1r/Pp6/bP1bq1p1/3N2p1/R1pPppB1/4P1PP/5PK1/3QBR2 b - - 0 1 bm f3+; id TTT1.049",
  "1b2r1r1/5p1k/3pb1pB/2pnP1Q1/q2P1N2/p1P5/1p3PP1/1R2R1K1 w - - 0 1 bm Re4; id TTT1.050",
  "b4rk1/6np/3p1pp1/1r2p1PP/1p2P3/1PqN1P2/2P3RQ/1KBR4 b - - 0 1 bm Bxe4; id TTT1.053",
  "r4r1k/1p4pp/1p1p3q/p2Np1n1/P3Ppb1/2PP1N2/1P1RKPPP/R2Q4 b - - 0 1 bm Nxf3 Bxf3+; id TTT1.054",
  "rn3rk1/4bpp1/1qp4p/pp1nP3/2pP1B1N/P1N2BPb/1P5P/1R1Q1R1K w - - 0 1 bm Bxh6; id TTT1.055",
  "Q4nk1/3r1p2/1p4b1/pP2P1B1/7P/P1pp3P/7K/8 b - - 0 1 bm Be4; id TTT1.057",
  "r2q1rk1/2n1bpp1/b2p4/3Pp1PQ/1p6/p3B2P/PPPN1P2/2KR2R1 w - - 0 1 bm Rg4; id TTT1.060",
  "r4r1k/6p1/1pp1b1Bp/p5q1/8/2P2Q1P/1P3PP1/3RR1K1 w - - 0 1 bm Qxf8+; id TTT1.062",
  "5rqk/4R2p/5Pp1/1p4P1/4Q3/2p3P1/6K1/8 w - - 0 1 bm Qe5 Qc6 Qb7 Kg1; id TTT1.064",
  "1rr4k/1bq2pb1/p2ppNp1/2n1n1Pp/B3P2Q/2B2P1R/PPP4P/2KR4 w - - 0 1 bm Bxe5; id TTT1.067",
  "6k1/1p6/1Rb2p2/6p1/PKP2r2/2B5/1P6/8 w - - 0 1 bm Rxc6; id TTT1.074",
  "6k1/1b2rr1p/4R1pP/3p1pP1/p1pPpK2/PpP5/1P6/2B3N1 b - - 0 1 bm e3; id TTT1.079",
  "2rq1rk1/3bbpp1/3np3/2ppB1PQ/p1p2P2/1PP1P1N1/P6P/R4RK1 w - - 0 1 bm Rf3; id TTT1.080",
  "r1N2r2/2R2p2/5Rp1/p1P4p/2Kn1P1k/1P6/8/8 w - - 0 1 bm Kxd4; id TTT1.081",
  "n2r2k1/1p1r1ppp/3N4/2pQP3/P1Pb1P2/2N3PK/1P1P3P/R1B3q1 b - - 0 1 bm Rxd6; id TTT1.082",
  "2r3k1/1r3ppp/q1p1p3/2P1RP1P/b1pP2P1/P1n5/1P6/KBQ3R1 w - - 0 1 bm f6; id TTT1.087",
  "1k6/6Bq/b6p/2p2p2/1pPp3r/pP1Pb3/P1K3Q1/6Rr w - - 0 1 bm Be5+; id TTT1.088",
  "8/5pkp/1p6/p4p2/3P4/B5rP/1R6/7K w - - 0 1 bm Rg2; id TTT1.100",
  "8/3P3k/n2K3p/2p3n1/1b4N1/2p1p1P1/8/3B4 w - - 0 1 bm Ng6+; id Plaskett's Puzzle \
   https://en.wikipedia.org/wiki/Plaskett%27s_Puzzle",
  "rn3r1k/pn1p1ppq/bpp4p/7P/4N1Q1/6P1/PP3PB1/R1B1R1K1 w - - 3 21 bm Bg5; \
   Alpha Zero move vs Stockfish https://arxiv.org/pdf/1712.01815.pdf pg `7 2nd game",
  "2b1rk2/5p2/p1P5/2p2P2/2p5/7B/P7/2KR4 w - - 0 1 bm f6; Smyslov Study",
  "6R1/P2k4/r7/5N1P/r7/p7/7K/8 w - - 0 1 ; D. Djaja 1972 White to play, draw",
  "3B4/1r2p3/r2p1p2/bkp1P1p1/1p1P1PPp/p1P1K2P/PPB5/8 w - - 0 1;  W.E. Rudolph, La Stratégie 1912, draw",
  "8/1p6/1Pp2N1Q/p1Ppk2p/P3p3/3PPpPp/3K1P1P/1R6 w - - 0 1; J. Hašek, Národní Listy 1951, draw",
  "n2Bqk2/5p1p/Q4KP1/p7/8/8/8/8 w - - 0 1; M. Matous, 1.hm Szachy 1975, win",
  "8/8/8/5Bp1/7k/8/4pPKP/8 w - - 0 1; Vitaly Chekhover, Gatchinskaja Pravda, 1954, draw",
  "1R6/8/8/5bp1/4p2k/8/B1p2PKP/8 w - - 0 1 bm Rh8+; Vitaly Chekhover, Gatchinskaja Pravda, 1954, draw",
  "1R4bq/p1p3p1/2p3Pb/k1P3PR/2P4P/p1K5/P7/8 w - - 0 1; A. A. Troitzky, Magyar Sakkvilág 1931, win",
  "1R4bq/p1p3p1/2p3Pb/k1P4R/2P4P/p5P1/P7/1K6 w - - 0 1; A. A. Troitzky, version Pal Benko, 2007, win",
  "k7/n1p1p1K1/P1p1Pp2/5P2/3R3P/pB6/2p5/2r5 w - - 0 1; Sandipan Chanda, 2009, win",
  "7r/p3k3/2p5/1pPp4/3P4/PP4P1/3P1PB1/2K5 w - - 0 1; Vitaly Chekhover, Parna Ty Bull 1947, draw",
  "2br4/r2pp3/8/1p1p1kN1/pP1P4/2P3R1/PP3PP1/2K5 w - - 0 1; F. Simkovitch, L'Italia Scacchistica 1923, draw",
  "8/1p6/1p6/kPp2P1K/2P5/N1Pp4/q2P4/1N6 w - - 0 1; Noam Elkies, 1991, draw",
  "4K1k1/8/1p5p/1Pp3b1/8/1P3P2/P1B2P2/8 w - - 0 1 bm f4; 1st Prize, Ladislav Salai Jr, Šachová skladba 2011-12, win",
  "3r4/3r4/2p1k3/p1pbPp1p/Pp1p1PpP/1P1P2R1/2P4R/2K1Q3 w - - 0 1 bm Rxg4:  False Fortress"
#endif
};

} // namespace

namespace Stockfish {

/// setup_bench() builds a list of UCI commands to be run by bench. There
/// are five parameters: TT size in MB, number of search threads that
/// should be used, the limit value spent for each position, a file name
/// where to look for positions in FEN format, the type of the limit:
/// depth, perft, nodes and movetime (in millisecs), and evaluation type
/// mixed (default), classical, NNUE.
///
/// bench -> search default positions up to depth 13
/// bench 64 1 15 -> search default positions up to depth 15 (TT = 64MB)
/// bench 64 4 5000 current movetime -> search current position with 4 threads for 5 sec
/// bench 64 1 100000 default nodes -> search default positions for 100K nodes each
/// bench 16 1 5 default perft -> run a perft 5 on default positions

vector<string> setup_bench(const Position& current, istream& is) {

  vector<string> fens, list;
  string go, token;

  // Assign default values to missing arguments
  string ttSize    = (is >> token) ? token : "256";
  string threads   = (is >> token) ? token  : "1";
  string limit     = (is >> token) ? token  : "13";
  string limitNN  =  (is >> token) ? token  : "false";
  string limitEvF  =  (is >> token) ? token : EvalFileDefaultName;
  string fenFile   = (is >> token) ? token  : "default";
  string limitType = (is >> token) ? token  : "depth";

  go = limitType == "eval" ? "eval" : "go " + limitType + " " + limit + limitNN + limitEvF;

  if (fenFile == "default")
      fens = Defaults;

  else if (fenFile == "current")
      fens.push_back(current.fen());

  else
  {
      string fen;
      ifstream file(fenFile);

      if (!file.is_open())
      {
          cerr << "Unable to open file " << fenFile << endl;
          exit(EXIT_FAILURE);
      }

      while (getline(file, fen))
          if (!fen.empty())
              fens.push_back(fen);

      file.close();
  }

  list.emplace_back("setoption name Threads value " + threads);
  list.emplace_back("setoption name Hash value " + ttSize);
  list.emplace_back("setoption name UseNN value " + limitNN);
  list.emplace_back("setoption name EvalFile value " + limitEvF);
  list.emplace_back("ucinewgame");

  for (const string& fen : fens)
      if (fen.find("setoption") != string::npos)
          list.emplace_back(fen);
      else
      {
          list.emplace_back("position fen " + fen);
          list.emplace_back(go);
      }

  list.emplace_back("setoption name UseNN value true");

  return list;
}

} // namespace Stockfish
