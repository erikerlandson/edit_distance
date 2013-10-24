/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <sstream>

// get the edit_alignment() function
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>
//using boost::algorithm::sequence_alignment::edit_alignment;
//using boost::algorithm::sequence_alignment::edit_opcode;

int main(int argc, char** argv) {
    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

#if 0
    // collect the sequence of edit operations into an output iterator.
    // return value is a pair (O, D), where O is the value of the output iterator
    // after output of the edit operation sequence, and D is the corresponding
    // edit distance
    std::stringstream ss;
    unsigned dist = edit_alignment(str1, str2, std::ostream_iterator<edit_opcode>(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";

    // the edit_opcode type can take on values: ins_op, del_op, sub_op, eql_op,
    // encoding insertion, deletion, substitution and equality.  Equality encodes a
    // substitution where the subsitution cost was zero.  Note, a zero subsitution
    // cost does *not* always imply that two elements are identical, although that
    // is often the case.
#endif

    return 0;
}
