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

#include <algorithm>
#include <cassert>
#include <ostream>
#include <sstream>

#include "evaluate.h"
#include "misc.h"
#include "polybook.h"

#include "search.h"
#include "thread.h"
#include "tt.h"
#include "uci.h"
#include "syzygy/tbprobe.h"

using std::string;

namespace Stockfish {

UCI::OptionsMap Options; // Global object

namespace UCI {


/// 'On change' actions, triggered by an option's value change
void on_clear_hash(const Option&) { Search::clear(); }
void on_hash_size(const Option& o) { TT.resize(size_t(o)); }
void on_logger(const Option& o) { start_logger(o); }
void on_threads(const Option& o) { Threads.set(size_t(o)); }
//void on_eval_perturb(const Option& o) { Eval::NNUE::RandomEvalPerturb = o; }
void on_tb_path(const Option& o) { Tablebases::init(o); }
void on_use_NNUE(const Option& ) { Eval::NNUE::init(); }
void on_eval_file(const Option& ) { Eval::NNUE::init(); }

void on_book_file1(const Option& o) { polybook1.init(o); }
void on_book_file2(const Option& o) { polybook2.init(o); }
void on_book_file3(const Option& o) { polybook3.init(o); }
void on_book_file4(const Option& o) { polybook4.init(o); }

void on_best_book_move1(const Option& o) { polybook1.set_best_book_move(o); }
void on_best_book_move2(const Option& o) { polybook2.set_best_book_move(o); }
void on_best_book_move3(const Option& o) { polybook3.set_best_book_move(o); }
void on_best_book_move4(const Option& o) { polybook4.set_best_book_move(o); }

void on_book_depth1(const Option& o) { polybook1.set_book_depth(o); }
void on_book_depth2(const Option& o) { polybook2.set_book_depth(o); }
void on_book_depth3(const Option& o) { polybook3.set_book_depth(o); }
void on_book_depth4(const Option& o) { polybook4.set_book_depth(o); }

/// Our case insensitive less() function as required by UCI protocol
bool CaseInsensitiveLess::operator() (const string& s1, const string& s2) const {

  return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(),
         [](char c1, char c2) { return tolower(c1) < tolower(c2); });
}


/// UCI::init() initializes the UCI options to their hard-coded default values

void init(OptionsMap& o) {

  constexpr int MaxHashMB = Is64Bit ? 33554432 : 2048;

  o["Debug Log File"]        << Option("", on_logger);

  o["Best_Move_1"] 	         << Option(false, on_best_book_move1);
  o["Best_Move_2"] 	         << Option(false, on_best_book_move2);
  o["Best_Move_3"]           << Option(false, on_best_book_move3);
  o["Best_Move_4"]           << Option(false, on_best_book_move4);
  o["Book_Depth_1"] 	       << Option(127, 1, 127, on_book_depth1);
  o["Book_Depth_2"] 	       << Option(127, 1, 127, on_book_depth2);
  o["Book_Depth_3"]          << Option(127, 1, 127, on_book_depth3);
  o["Book_Depth_4"]          << Option(127, 1, 127, on_book_depth4);
  o["Book_File_1"] 	         << Option("", on_book_file1);
  o["Book_File_2"] 	         << Option("", on_book_file2);
  o["Book_File_3"]           << Option("", on_book_file3);
  o["Book_File_4"]           << Option("", on_book_file4);
  o["Use_Book_1"] 	         << Option(false);
  o["Use_Book_2"] 	         << Option(false);
  o["Use_Book_3"] 	         << Option(false);
  o["Use_Book_4"]            << Option(false);

  o["Analysis Contempt"]     << Option("Both var Off var White var Black var Both", "Both");
  o["Bench_KNPS"]            << Option (2000, 100, 6000);//used for UCI Play By Elo
  o["Clear Hash"]            << Option(on_clear_hash);
  o["Contempt"]              << Option(24, -100, 100);
  o["Hash"]                  << Option(16, 1, MaxHashMB, on_hash_size);
  o["Minimal_Output"]        << Option(false);
  o["Move Overhead"]         << Option(10, 0, 5000);
  o["MultiPV"]               << Option(1, 1, 256);
  o["Ponder"]                << Option(false);
  o["Threads"]               << Option(1, 1, 512, on_threads);

  // A separate weaker play level from the predefined levels below. The difference
  // between both of the methods and the "skill level" is that the engine is only weakened
  // by the reduction in nodes searched, thus reducing the move horizon visibility naturally
  o["Engine_Level"]           << Option("None var World_Champion var Super_GM "
                                        "var GM var Deep_Thought var SIM var Cray_Blitz "
                                        "var IM var Master var Expert var Class_A "
                                        "var Class_B var Class_C var Class_D var Class_E "
                                        "var None", "None");


  o["nodestime"]             << Option(0, 0, 10000);
  o["Search_Depth"]          << Option(0, 0, 30);
  o["Search_Nodes"]          << Option(0, 0, 10000000);
  o["Slow Mover"]            << Option(100, 10, 1000);
  //o["Slow Play"]             << Option(false);
  o["Tactical_Depth"]        << Option(5, 0, 16);
  o["Adaptive_Play"]         << Option(false);
  o["UCI_AnalyseMode"]       << Option(false);
  o["UCI_Chess960"]          << Option(false);
  o["UCI_Elo"]               << Option(1000, 1000, 3000);
  o["UCI_LimitStrength"]     << Option(false);
  o["UCI_ShowWDL"]           << Option(false);
  o["Tactical"]              << Option(0, 0, 8);
  o["Variety"]               << Option(false);
  //o["SyzygyPath"]          << Option("<4-men>", on_tb_path);
  //o["SyzygyPath"]          << Option("<empty>", on_tb_path);
  o["Syzygy50MoveRule"]      << Option(true);
  o["SyzygyPath"]            << Option("c:\\syzygy", on_tb_path);
  o["SyzygyProbeDepth"]      << Option(1, 1, 100);
  o["SyzygyProbeLimit"]      << Option(7, 0, 7);

  o["EvalFile"]              << Option(EvalFileDefaultName, on_eval_file);
  o["UseNN"]                 << Option("pure var true var false var pure", "pure", on_use_NNUE);
}


/// operator<<() is used to print all the options default values in chronological
/// insertion order (the idx field) and in the format defined by the UCI protocol.

std::ostream& operator<<(std::ostream& os, const OptionsMap& om) {

  for (size_t idx = 0; idx < om.size(); ++idx)
      for (const auto& it : om)
          if (it.second.idx == idx)
          {
              const Option& o = it.second;
              os << "\noption name " << it.first << " type " << o.type;

              if (o.type == "string" || o.type == "check" || o.type == "combo")
                  os << " default " << o.defaultValue;

              if (o.type == "spin")
                  os << " default " << int(stof(o.defaultValue))
                     << " min "     << o.min
                     << " max "     << o.max;

              break;
          }

  return os;
}


/// Option class constructors and conversion operators

Option::Option(const char* v, OnChange f) : type("string"), min(0), max(0), on_change(f)
{ defaultValue = currentValue = v; }

Option::Option(bool v, OnChange f) : type("check"), min(0), max(0), on_change(f)
{ defaultValue = currentValue = (v ? "true" : "false"); }

Option::Option(OnChange f) : type("button"), min(0), max(0), on_change(f)
{}

Option::Option(double v, int minv, int maxv, OnChange f) : type("spin"), min(minv), max(maxv), on_change(f)
{ defaultValue = currentValue = std::to_string(v); }

Option::Option(const char* v, const char* cur, OnChange f) : type("combo"), min(0), max(0), on_change(f)
{ defaultValue = v; currentValue = cur; }

Option::operator double() const {
  assert(type == "check" || type == "spin");
  return (type == "spin" ? stof(currentValue) : currentValue == "true");
}

Option::operator std::string() const {
  assert(type == "string");
  return currentValue;
}

bool Option::operator==(const char* s) const {
  assert(type == "combo");
  return   !CaseInsensitiveLess()(currentValue, s)
        && !CaseInsensitiveLess()(s, currentValue);
}


/// operator<<() inits options and assigns idx in the correct printing order

void Option::operator<<(const Option& o) {

  static size_t insert_order = 0;

  *this = o;
  idx = insert_order++;
}


/// operator=() updates currentValue and triggers on_change() action. It's up to
/// the GUI to check for option's limits, but we could receive the new value
/// from the user by console window, so let's check the bounds anyway.

Option& Option::operator=(const string& v) {

  assert(!type.empty());

  if (   (type != "button" && v.empty())
      || (type == "check" && v != "true" && v != "false")
      || (type == "spin" && (stof(v) < min || stof(v) > max)))
      return *this;

  if (type == "combo")
  {
      OptionsMap comboMap; // To have case insensitive compare
      string token;
      std::istringstream ss(defaultValue);
      while (ss >> token)
          comboMap[token] << Option();
      if (!comboMap.count(v) || v == "var")
          return *this;
  }

  if (type != "button")
      currentValue = v;

  if (on_change)
      on_change(*this);

  return *this;
}

} // namespace UCI

} // namespace Stockfish
