
#include "debug.h"
#include "parse.h"
#include "util.h"

#include <fstream>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

using namespace std;

using namespace boost::filesystem;

const string inpsuffix = ".input.txt";
const string otpsuffix = ".output.txt";

#define CONFIRM(x, y) do { if(__builtin_expect(!(x), 0)) { out << (y) << " on line " << line << endl; dbgout(y); return; } } while(0)
#define DBG(y) do { if(verbose) { out << (y) << endl; } dbgout(y); } while(0)

void dbgout(const string &x) {
  dprintf("%s\n", x.c_str());
}

string tolower(string line) {
  for(int i = 0; i < line.size(); i++)
    line[i] = tolower(line[i]);
  return line;
}

void process_file(const string &infname) {
  string outfname = infname.substr(0, infname.size() - inpsuffix.size()) + otpsuffix;
  
  ofstream out(outfname.c_str());
  ifstream inf(infname.c_str());
  
  int line = 0;
  
  CONFIRM(inf, "Couldn't open input file");
  
  bool verbose = false;
  int lynchthreshold = -1;
  bool autounvote = false;
  bool includeposthammervotes = false;
  
  string ld;
  
  while(getLineStripped(inf, &ld, &line)) {
    DBG(ld);
    
    vector<string> tok = tokenize(ld, "=");
    if(tok[0] == "verbose") {
      CONFIRM(tok.size() == 1, "'verbose' has unnecessary parameters");
      verbose = true;
    } else if(tok[0] == "autounvote") {
      CONFIRM(tok.size() == 1, "'autounvote' has unnecessary parameters");
      autounvote = true;
    } else if(tok[0] == "includeposthammervotes") {
      CONFIRM(tok.size() == 1, "'includeposthammervotes' has unnecessary parameters");
      includeposthammervotes = true;
    } else if(tok[0] == "lynchthreshold") {
      CONFIRM(tok.size() == 2, "'lynchthreshold' does not have precisely one parameter");
      lynchthreshold = atoi(tok[1].c_str());
    } else if(tok[0] == "PLAYERLIST") {
      CONFIRM(tok.size() == 1, "'PLAYERLIST' has unnecessary parameters");
      break;
    } else {
      CONFIRM(0, tok[0] + " is confusing");
    }
  }
  
  map<string, int> nicklookup;
  vector<string> nicks;
  
  {
    bool neednick = false;
    while(getLineStripped(inf, &ld, &line)) {
      DBG(ld);
      
      CHECK(ld.size());
      
      if(ld == "VOTELIST") {
        break;
      } else if(ld[0] != '(') {
        CONFIRM(!neednick, "Player " + nicks.back() + " needs a nickname");
        CONFIRM(count(nicks.begin(), nicks.end(), ld) == 0, "Duplicate playername");
        DBG("Adding player " + ld);
        nicks.push_back(ld);
        if(count(ld.begin(), ld.end(), ' ') == 0) {
          CONFIRM(nicklookup.count(tolower(ld)) == 0, "Playername declared that conflicts with alias");
          DBG("Adding default nickname");
          nicklookup[tolower(ld)] = nicks.size() - 1;
        } else {
          neednick = true;
        }
      } else {
        CONFIRM(nicks.size(), "Alias defined without creating a player");
        CONFIRM(count(ld.begin(), ld.end(), ' ') == 0, "Alias contains spaces");
        CONFIRM(ld[ld.size() - 1] == ')', "Open-parenthesis without matching close parenthesis");
        CONFIRM(nicklookup.count(tolower(ld.substr(1, ld.size() - 2))) == 0, "Duplicate nickname");
        DBG("Adding nickname " + ld + " for player " + nicks.back());
        nicklookup[tolower(ld.substr(1, ld.size() - 2))] = nicks.size() - 1;
        neednick = false;
      }
    }
    CONFIRM(!neednick, "Player " + nicks.back() + " needs a nickname");
    CONFIRM(!count(nicks.begin(), nicks.end(), "unvote"), "No, you can't have a player named 'unvote'");
    CONFIRM(!count(nicks.begin(), nicks.end(), "u"), "No, you can't have a player named 'u'");
  }
  
  if(lynchthreshold == -1)
    lynchthreshold = nicks.size() / 2 + 1;
  
  vector<vector<pair<bool, int> > > votehistory(nicks.size());
  map<int, int> currentvote;
  map<int, int> votecounts;
  bool hammered = false;
  vector<string> warnings;
  
  {
    while(getLineStripped(inf, &ld, &line)) {
      DBG(ld);
      
      CHECK(ld.size());
      
      vector<string> toki = tokenize(ld, " ");
      
      for(int i = 0; i < toki.size(); i++)
        toki[i] = tolower(toki[i]);
      
      bool voting;
      int user = -1;
      int target = -1;
      
      if(toki.size() == 1 || toki.size() == 2 && (toki[1] == "u" || toki[1] == "unvote")) {
        CONFIRM(nicklookup.count(toki[0]), "I have no idea who " + toki[0] + " is");
        voting = false;
        user = nicklookup[toki[0]];
      } else if(toki.size() == 2 || toki.size() == 3) {
        CONFIRM(nicklookup.count(toki[0]), "I have no idea who " + toki[0] + " is");
        CONFIRM(nicklookup.count(toki.back()), "I have no idea who " + toki[0] + " is");
        CONFIRM(toki.size() == 2 || toki[1] == "votes" || toki[1] == "vote" || toki[1] == "v", StringPrintf("'%s' is not a valid thing for %s to do to %s", toki[1].c_str(), toki[0].c_str(), toki[2].c_str()));
        voting = true;
        user = nicklookup[toki[0]];
        target = nicklookup[toki.back()];
      } else {
        CONFIRM(0, StringPrintf("%d is a completely invalid number of words for a vote", toki.size()));
      }
      
      DBG(StringPrintf("%d, %d, %d", voting, user, target));
      
      if(hammered) {
        if(voting) {
          warnings.push_back(StringPrintf("'%s' attempted to vote '%s' after hammer", nicks[user].c_str(), nicks[target].c_str()));
        } else {
          warnings.push_back(StringPrintf("'%s' attempted to unvote after hammer", nicks[user].c_str()));
        }
        
        if(!includeposthammervotes)
          continue;
      }
      
      if(voting) {
        
        if(currentvote.count(user)) {
          warnings.push_back(StringPrintf("'%s' failed to unvote '%s' before voting '%s'", nicks[user].c_str(), nicks[currentvote[user]].c_str(), nicks[target].c_str()));
          if(autounvote) {
            votehistory[currentvote[user]].push_back(make_pair(false, user));
            votecounts[currentvote[user]]--;
            currentvote.erase(user);
          } else {
            continue;
          }
        }
        
        votehistory[target].push_back(make_pair(true, user));
        currentvote[user] = target;
        votecounts[currentvote[user]]++;
        
        if(!hammered && votecounts[currentvote[user]] == lynchthreshold) {
          hammered = true;
          warnings.push_back(StringPrintf("'%s' hammered (by '%s')", nicks[target].c_str(), nicks[user].c_str()));
        }
      } else {
        if(!currentvote.count(user)) {
          warnings.push_back(StringPrintf("'%s' attempted to unvote without having anyone voted", nicks[user].c_str()));
        } else {
          votehistory[currentvote[user]].push_back(make_pair(false, user));
          votecounts[currentvote[user]]--;
          currentvote.erase(user);
        }
      }
      
      DBG("Done handling vote");
    }
  }
  
  
}

int main() {
  directory_iterator end_itr;
  for(directory_iterator itr("."); itr != end_itr; ++itr) {
    string fn = itr->leaf();
    
    if(fn.size() > inpsuffix.size() && fn.substr(fn.size() - inpsuffix.size(), inpsuffix.size()) == inpsuffix) {
      process_file(fn);
    }
  }
}
