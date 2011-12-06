#ifndef FILEMATCH_H
#define FILEMATCH_H

namespace lhack {

string Search(const string& fsroot, vector<string>& filters,
              const string& target, float alpha);

};

#endif // FILEMATCH_H
