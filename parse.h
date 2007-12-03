/*

Mafia Voteminder
Copyright (C) 2007 Ben Wilhelm

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, Version 3 only.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

I can be contacted at zorba-foreman@pavlovian.net

*/

#ifndef DNET_PARSE
#define DNET_PARSE

#include <map>
#include <string>
#include <vector>

using namespace std;

vector<string> tokenize(const string &in, const string &kar);
vector<int> sti(const vector<string> &foo);
vector<float> stf(const vector<string> &foo);
  
class kvData {
public:
  
  string category;
  map<string, string> kv;

  string debugOutput() const;

  string consume(const string &key);
  const string &read(const string &key) const;  // guarantees that it exists otherwise kablooey
  string saferead(const string &key) const;
  bool isDone() const;
  void shouldBeDone() const;

};

istream &getLineStripped(istream &ifs, string *out, int *line = NULL);
istream &getkvData(istream &ifs, kvData *out, int *line = NULL, int *endline = NULL);

string stringFromKvData(const kvData &kvd);

char fromHex(char in);

#endif
