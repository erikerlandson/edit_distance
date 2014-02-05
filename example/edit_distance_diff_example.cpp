/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::cerr;
using std::cout;
using std::endl;

#include <boost/algorithm/sequence/edit_distance.hpp>
using boost::algorithm::sequence::edit_distance;
using namespace boost::algorithm::sequence::parameter;


void load_file_lines(char const* fname, vector<string>& lines) {
    std::ifstream f;
    f.open(fname, std::ifstream::in);
    if (!f.good()) {
        cerr << "error loading " << fname << endl;
        exit(1);
    }

    lines.clear();
    lines.reserve(10000);
    while (!f.eof()) {
        lines.push_back(string());
        std::getline(f, lines.back());
    }
}


struct diff_handler {
    struct hunk {
        hunk() : del(), ins(), del_pos(0), ins_pos(0) {}
        size_t del_pos;
        size_t ins_pos;
        vector<string> del;
        vector<string> ins;
    };

    vector<hunk> hunks;
    size_t src_pos;
    size_t dst_pos;
    bool needhunk;

    diff_handler() : hunks(), src_pos(0), dst_pos(0), needhunk(true) {}

    inline void newhunk() {
        hunks.push_back(hunk());
        hunks.back().del_pos = src_pos;
        hunks.back().ins_pos = dst_pos;
        needhunk = false;
    }

    inline void insertion(const string& s, size_t) {
        if (needhunk) newhunk();
        hunks.back().ins.push_back(s);
        ++dst_pos;
    }

    inline void deletion(const string& s, size_t) {
        if (needhunk) newhunk();
        hunks.back().del.push_back(s);
        ++src_pos;
    }

    inline void equality(const string&, const string&) {
        needhunk = true;
        ++src_pos;
        ++dst_pos;
    }

    void write_standard_format(std::ostream& ostrm) {
        for (vector<hunk>::iterator h(hunks.begin());  h != hunks.end();  ++h) {

            if (h->del.empty())  ostrm << h->del_pos;
            else {
                ostrm << h->del_pos + 1;
                if (h->del.size() > 1)  ostrm << "," << h->del_pos + h->del.size();
            }

            ostrm << ((h->del.empty()) ? "a" : ((h->ins.empty()) ? "d" : "c"));

            if (h->ins.empty())  ostrm << h->ins_pos;
            else {
                ostrm << h->ins_pos + 1;
                if (h->ins.size() > 1)  ostrm << "," << h->ins_pos + h->ins.size();
            }

            ostrm << endl;

            for (vector<string>::iterator s(h->del.begin()); s != h->del.end();  ++s)
                ostrm << "< " << *s << endl;

            if (!h->del.empty()  &&  !h->ins.empty())  ostrm << "---" << endl;

            for (vector<string>::iterator s(h->ins.begin()); s != h->ins.end();  ++s)
                ostrm << "> " << *s << endl;
        }
    }
};


int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "usage: " << argv[0] << " <src> <dst>" << endl;
        exit(1);
    }

    vector<string> src;
    vector<string> dst;

    load_file_lines(argv[1], src);
    load_file_lines(argv[2], dst);

    diff_handler handler;

    const size_t term_cost = 10000;
    int err=0;

    size_t dist = edit_distance(src, dst, _script=handler, _max_cost=term_cost);
    if (dist >= term_cost) {
        cerr << "WARNING: file difference exceeded " << term_cost << " lines, reported difference is not minimal!" << endl;
        err = 1;
    }

    handler.write_standard_format(cout);

    return err;
}
