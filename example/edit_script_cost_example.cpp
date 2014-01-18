/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <ctype.h>

#include "edit_distance_common.hpp"


// get the edit_alignment() function
#include <boost/algorithm/sequence/edit_distance.hpp>
using boost::algorithm::sequence::edit_distance;
using boost::algorithm::sequence::unit_cost;
using namespace boost::algorithm::sequence::parameter;


// define a custom cost function where case changes cost less
struct cost_case_less {
    typedef float cost_type;     // edit costs may be fractional

    cost_type insertion(char c) const { return 1; }
    cost_type deletion(char c) const { return 1; }

    // changes in case cost less than other edit operations:
    cost_type substitution(char c, char d) const { 
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
    float dist = edit_distance(str1, str2, _script = out, _cost = cost_case_less(), _substitution=true);
    std::cout << "dist= " << dist << "   edit operations=  \"" << out.ss.str() << "\"\n";    

    // compare to the behavior with the default cost function:
    out.ss.str("");
    dist = edit_distance(str1, str2, _script = out, _substitution=true);
    std::cout << "dist= " << dist << "   edit operations=  \"" << out.ss.str() << "\"\n";    

    return 0;
}
