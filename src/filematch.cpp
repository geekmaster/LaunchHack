/*
*   Copyright 2011 Vassil Panayotov <vd.panayotov@gmail.com>
*
*   Licensed under the Apache License, Version 2.0 (the "License");
*   you may not use this file except in compliance with the License.
*   You may obtain a copy of the License at
*
*       http://www.apache.org/licenses/LICENSE-2.0
*
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*/

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <algorithm>
#include <tr1/unordered_map>
#include <list>
#include <cmath>
#include <iostream>
#include <fstream>

#include <fts.h>
#include <fnmatch.h>

namespace lhack {

using namespace std;

typedef unsigned int ngramid_t;

class Identity : public unary_function<ngramid_t, ngramid_t>
{
public:
    inline ngramid_t operator()(ngramid_t value) const {
        return value;
    }
};

typedef unsigned int pathid_t;

// <pathid, count_of_ngram_occurences_in_path(pathid)>
typedef pair<pathid_t, unsigned> index_atom_t;

// array recording all paths where the ngram was encountered
typedef vector<index_atom_t> ngram_invert_t;

typedef tr1::unordered_map<ngramid_t, ngram_invert_t, Identity> index_t;
typedef vector<string> fullpaths_t;
typedef vector<pair<ngramid_t, unsigned> > qfeat_t; // query features

void
PrintFeatures(const qfeat_t& feats)
{
    for (qfeat_t::const_iterator it = feats.begin(); it != feats.end(); it++) {
        ngramid_t id = it->first;
        char* idbytes = reinterpret_cast<char*>(&id);
        cout << (idbytes[0]? idbytes[0]: '_') << ",";
        cout << (idbytes[1]? idbytes[1]: '_') << ",";
        cout << (idbytes[2]? idbytes[2]: '_');
        cout << ": " << it->second << endl;
    }
}

void
PrintIndex(index_t& index)
{
    cout << "Index \n";
    for (index_t::iterator it = index.begin(); it != index.end(); it++) {
        cout << "ID: ";
        ngramid_t id = it->first;
        char* idbytes = reinterpret_cast<char*>(&id);
        cout << (idbytes[0]? idbytes[0]: '_') << ",";
        cout << (idbytes[1]? idbytes[1]: '_') << ",";
        cout << (idbytes[2]? idbytes[2]: '_');
        cout << "<" << id << "> [";
        ngram_invert_t& invidx = it->second;
        for (ngram_invert_t::iterator it2 = invidx.begin(); it2 != invidx.end(); it2++) {
            cout << '(' << it2->first << ':' << it2->second << "), ";
        }
        cout << ']' << endl;
    }
    cout << "/Index \n";
}

/**
 * Extracts the base name of a file from abs. path.
 * Assumes that the path contains at least one '/' and
 * at least one '.'
 */
inline bool
FindBasename(const string& path, int *fnbase, int *fnlen)
{
    int i;
    for (i = path.length() - 1; i >= 0; i--)
        if (path[i] == '.')
            break;

    if (i < 0)
        return false;

    int j = i;
    for (; j >= 0; j--)
        if (path[j] == '/')
            break;

    if (j < 0)
        return false;

    *fnbase = j + 1;
    *fnlen = i - j - 1;

    return true;
}

/**
 * Uses letter (or rather byte) tri-grams
 * @todo NOTE: THE CODE DEPENDS ON CPU ENDIANESS (assumes LE - check on ARM)
 */
inline void
MakeIndex(const string& path, unsigned pathid,
          int fnbase, int fnlen, index_t& ngindex)
{
    unsigned int ngram = 0;
    char* const ngbytes = reinterpret_cast<char*>(&ngram);

    for (int i = fnbase; i < (fnbase + fnlen); i++) {
        ngbytes[3] = path[i];
        ngram >>= 8;

        index_t::iterator it = ngindex.find(ngram);
        if (it == ngindex.end()) { // is this the first time n-gram of this type is inserted?
            ngram_invert_t invidx;
            invidx.reserve(20);
            invidx.push_back(make_pair(pathid, 1));
            ngindex.insert(make_pair(ngram, invidx));
        }
        else {
            ngram_invert_t& invidx = it->second;
            if (invidx.back().first == pathid)  // 2 or more instances of that 3-gram in this path?
                ++ invidx.back().second;
            else
                invidx.push_back(make_pair(pathid, 1));
        }
    }
}

// Returns a list of the full paths to all files under "root"
// 'filters' is an array of globs, used to select a subset of the files
// 'minlen' is the minimum file name length(w/o the extension)
void
IndexFiles(const string& root, const vector<string>& filters,
           fullpaths_t& out_fpaths, index_t& out_index,
           int minlen, int maxlen)
{
    char* const roots[] = {(char*) root.c_str(), 0};
    FTS *tree = fts_open(roots, FTS_NOCHDIR, 0);

    FTSENT *node;
    while ((node = fts_read(tree))) {
        if (node->fts_info & FTS_F) {
            int i = 0;
            for (; i < filters.size(); i++) {
                if (fnmatch(filters[i].c_str(), node->fts_name, FNM_CASEFOLD) == 0)
                    break;
            }
            if (i == filters.size())
                continue;

            string path(node->fts_path);
            int fnbase, fnlen;
            if (!FindBasename(path, &fnbase, &fnlen))
                continue;

            if (fnlen < minlen)
                continue;
            fnlen = min(fnlen, maxlen);
            out_fpaths.push_back(path);
            MakeIndex(path, out_fpaths.size() - 1, fnbase, fnlen, out_index);
        }
    }

    fts_close(tree);
}

void QueryFeats(const string& query, vector<pair<ngramid_t, unsigned> >& out_feats)
{
    vector<ngramid_t> ngids;
    ngramid_t ngramid = 0;
    char *ngbytes = reinterpret_cast<char*>(&ngramid);
    for(string::const_iterator it = query.begin(); it != query.end(); it++) {
        ngbytes[3] = *it;
        ngramid >>= 8;
        ngids.push_back(ngramid);
    }
    sort(ngids.begin(), ngids.end());
    ngramid_t prev_id = ngids[0];
    unsigned count = 0;
    for (vector<ngramid_t>::iterator it=ngids.begin(); it != ngids.end(); it++) {
        if (prev_id != *it) {
            out_feats.push_back(make_pair(prev_id, count));
            prev_id = *it;
            count = 1;
        }
        else {
            ++ count;
        }
    }
    out_feats.push_back(make_pair(prev_id, count));
}

template <typename T1, typename T2>
class CompareFirst : public binary_function<pair<T1, T2>, pair<T1, T2>, bool >
{
public:
    inline bool operator()(const pair<T1, T2>& left, const pair<T1, T2>& right)
    {
        return left.first < right.first;
    }
};

template <typename T1, typename T2>
class CompareSecond : public binary_function<pair<T1, T2>, pair<T1, T2>, bool >
{
public:
    inline bool operator()(const pair<T1, T2>& left, const pair<T1, T2>& right)
    {
        return left.second < right.second;
    }
};

/**
 * Returns the pathid for the best matching file name or -1 if no
 * result satisfies the similarity criteria(tau)
 */
int BestMatch(const string& query, int tau, index_t& ngindex)
{
    // Extract the query's features
    typedef vector<pair<ngramid_t, unsigned> > feat_vec_t;
    feat_vec_t feats; feats.reserve(query.size());
    QueryFeats(query, feats);

    // Order the features from the rarest to the more common
    typedef vector<pair<ngramid_t, unsigned> > freq_vec_t;
    freq_vec_t freqs; freqs.reserve(feats.size());
    vector<pair<unsigned, unsigned> > freq2feat;
    unsigned zeros = 0, p = 0;
    for (feat_vec_t::iterator it = feats.begin(); it != feats.end(); it++) {
        index_t::iterator ixit = ngindex.find(it->first);
        if (ixit == ngindex.end()) {
            ++ zeros;
            continue; // this query feature is not in the index
        }

        freqs.push_back(make_pair(it->first, ixit->second.size()));
        freq2feat.push_back(make_pair(p++, ixit->second.size()));
    }
    sort(freqs.begin(), freqs.end(), CompareSecond<ngramid_t, unsigned>());

    // The indices of the original features are permuted in the same way as in freqs
    sort(freq2feat.begin(), freq2feat.end(), CompareSecond<unsigned, unsigned>());

    // Make a short list of candidate file names
    vector<index_atom_t> candidates; candidates.reserve(150); // the initial size is just a guess
    int i = 0, f = 0;
    int signature_len = query.size() - tau - zeros + 1;
    if (signature_len < 1)
        return -1;
    while (f < signature_len) {
        ngram_invert_t& invidx = (ngindex.find(freqs[i].first))->second;
        for (ngram_invert_t::iterator invit = invidx.begin(); invit != invidx.end(); invit++) {
            unsigned featidx = freq2feat[i].first;
            candidates.push_back(make_pair(invit->first, min(invit->second, feats[featidx].second)));
        }
        f += feats[freq2feat[i].first].second;
        ++ i;
    }

    if (candidates.empty())
        return -1;

    //cout << "Candidate array size: " << candidates.size() << endl;

    // Fold the list on pathid and put the entries in a list
    sort(candidates.begin(), candidates.end(), CompareFirst<pathid_t, unsigned>());
    pathid_t prev = candidates[0].first;
    unsigned count = 0;
    list<index_atom_t> candlist;
    unsigned best_count = 0;
    pathid_t most_similar = 0;
    for (vector<index_atom_t>::iterator it = candidates.begin(); it != candidates.end(); it++) {
        if (it->first != prev) {
            candlist.push_back(make_pair(prev, count));
            prev = it->first;
            count = it->second;
        }
        else {
            count += it->second;
            if (count > best_count) {
                best_count = count;
                most_similar = it->first;
            }
        }
    }
    candlist.push_back(make_pair(prev, count));

    // cout << "Candidate list size: " << candlist.size() << endl;

    // For the rest of the features update the entries in the candidate list, while pruning those,
    // that don't have a chance of becoming the best candidate or fit within the similarity bounds
    int max_sim = query.size() - f - zeros;
    for (int j = i; j < freqs.size(); j++) {
        ngram_invert_t& invidx = (ngindex.find(freqs[j].first))->second;
        unsigned feat_count = feats[freq2feat[j].first].second;
        list<index_atom_t>::iterator ilist = candlist.begin();
        while ( ilist != candlist.end()) {
            // Check if the candidate isn't prommising
            //cout << "Current candidate: " << ilist->first << endl;
            unsigned cand_maxsim = ilist->second + max_sim;
            if (cand_maxsim < best_count || cand_maxsim < tau) {
                candlist.erase(ilist++);
                if (candlist.size() == 0)
                    return -1;
                else
                    continue;
            }

            // check if this candidate path has the current feature
            // The inverted index is sorted because of the way it is constructed
            ngram_invert_t::iterator lbit = lower_bound(
                        invidx.begin(), invidx.end(), *ilist,
                        CompareFirst<pathid_t, unsigned>());
            if (lbit == invidx.end() || lbit->first != ilist->first) {
                ++ ilist;
                continue;
            }

            ilist->second += min(lbit->second, feat_count);
            if (ilist->second > best_count) {
                best_count = ilist->second;
                most_similar = ilist->first;
            }

            ++ ilist;
        }

        max_sim -= feat_count;
    }

    if (best_count >= tau)
        return most_similar;
    else
        return -1;
}

string Search(const string& fsroot, vector<string>& filters,
              const string& target, float alpha)
{
    // overlap similarity (not cosine as in the paper)
    int tau = ceil(alpha * target.length());
    vector<string> fullpaths;
    fullpaths.reserve(1024);
    int minlen = tau, maxlen = target.length();
    index_t index(10111);
    IndexFiles(fsroot, filters, fullpaths, index, minlen, maxlen);
    int path = BestMatch(target, tau, index);
    if (path == -1) {
        return string();
    }
    else {
        return fullpaths[path];
    }
}

}; // namespace lhack
