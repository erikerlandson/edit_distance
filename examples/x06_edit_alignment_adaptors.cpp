/*******
edit_distance: STL and Boost compatible edit distance functions for C++

Copyright (c) 2013 Erik Erlandson

Author:  Erik Erlandson <erikerlandson@yahoo.com>

Distributed under the Boost Software License, Version 1.0.
See accompanying file LICENSE or copy at
http://www.boost.org/LICENSE_1_0.txt
*******/

#include <sstream>

#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>
using boost::tuple;

// get the edit_alignment() function
// define a printable default value for char
#define BOOST_CHAR_DEFAULT_OVERRIDE '_'
#include <boost/algorithm/sequence_alignment/edit_alignment.hpp>
#if 0
using boost::algorithm::sequence_alignment::edit_alignment;
using boost::algorithm::sequence_alignment::edit_opcode;

// adaptor can be used to obtain enhanced outputs
using boost::algorithm::sequence_alignment::acquire;

// flags to select types:
using boost::algorithm::sequence_alignment::costs;
using boost::algorithm::sequence_alignment::indexes;
using boost::algorithm::sequence_alignment::elements;
#endif

int main(int argc, char** argv) {
    char const* str1 = "Oh, hello world.";
    char const* str2 = "Hello world!!";

#if 0
    std::stringstream ss;
    ss << boost::tuples::set_delimiter(',');

    // edit_alignment() returns sequence of edit ops
    unsigned dist = edit_alignment(str1, str2, std::ostream_iterator<edit_opcode>(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";
    
    // The acquire<costs>() adaptor adds cost of each edit operation.  output-iterator value type must 
    // be assignable from tuple<edit_opcode, cost_type>
    ss.str("");
    dist = acquire<costs>(edit_alignment)(str1, str2, std::ostream_iterator<tuple<edit_opcode, unsigned> >(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";

    // Compose with acquire<elements>() to add sequence element values:
    // output-iterator type must be assignable from:
    // tuple<edit_opcode, cost_type, element_type (seq1), element_type (seq2)>
    ss.str("");
    dist = acquire<elements>(acquire<costs>(edit_alignment))(str1, str2, std::ostream_iterator<tuple<edit_opcode, unsigned, char, char> >(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";

    // Compose with acquire<indexes> to add sequence index values:
    // output-iterator type must be assignable from:
    // tuple<edit_opcode, cost_type, size_type (seq1), size_type (seq2)>
    ss.str("");
    dist = acquire<indexes>(acquire<costs>(edit_alignment))(str1, str2, std::ostream_iterator<tuple<edit_opcode, unsigned, unsigned, unsigned> >(ss, "")).second;
    std::cout << "dist= " << dist << "   edit operations= " << ss.str() << "\n";    
#endif

    return 0;
}
