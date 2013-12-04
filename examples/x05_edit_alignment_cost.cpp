/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <ctype.h>

#include "x_common.hpp"


// get the edit_alignment() function
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>
using boost::algorithm::sequence_alignment::edit_alignment;
using boost::algorithm::sequence_alignment::unit_cost;
using namespace boost::algorithm::sequence_alignment::parameter;


// define a custom cost function where case changes cost less
struct cost_case_less {
    typedef float cost_type;     // edit costs may be fractional

    cost_type cost_ins(char c) const { return 1; }
    cost_type cost_del(char c) const { return 1; }

    // changes in case cost less than other edit operations:
    cost_type cost_sub(char c, char d) const { 
        if (toupper(c) == toupper(d)) return 0.5;
        return 1;
    }
};


int main(int argc, char** argv) {
    char const* str1 = "Try to find XXX capitalized";
    char const* str2 = "xxx";

    // Match the substring 'xxx' against the larger string, with cheap case changes,
    // identifies the correct location of 'XXX' in the larger string
    stringstream_tuple_output<cost_case_less, char const*> out;
    float dist = edit_alignment(str1, str2, out, cost_case_less(), _allow_sub=true);
    std::cout << "dist= " << dist << "   edit operations=  \"" << out.ss.str() << "\"\n";    

    // compare to the behavior with the default cost function:
    out.ss.str("");
    dist = edit_alignment(str1, str2, out, _allow_sub=true);
    std::cout << "dist= " << dist << "   edit operations=  \"" << out.ss.str() << "\"\n";    

    return 0;
}
