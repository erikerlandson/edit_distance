edit_distance
=============

A C++ Boost implementation of edit distance functions.

The edit_distance library provides two functions:
* edit_distance(seq1, seq2) returns the edit distance (aka string distance, aka Levenshtein distance) between two sequences, which may be any C++ range.
* edit_alignment(seq1, seq2, out) returns the edit distance, and calls special handler methods from 'out' on the sequence of edit operations.

These functions are based on a variation of the Dijkstra Single Source Shortest Path algorithm, optimized for the particular structure of an edit graph.  They are efficient on long sequences with localized areas of differences (e.g. computing the diff of two similar files)

A user specified edit operation cost function can be provided:

    // provide a customized cost for insertion, deletion and substitution/equal 
    int d = edit_distance(s1, s2, _cost=my_cost_type());

Sequences may be any kind of C++ range object, and may be mixed:

    // Any forward range can be provided to these functions.  Sequence element types are not required to be exactly equal, as long as they are compatible.
    int d = edit_distance(my_vector, my_list | boost::adaptors::reversed, _cost=my_cost());
    
If a beam radius is provided, then edit operations will be restricted to within that radius of 'diagonal':

    // apply a beam radius check to limit exploration of edit paths
    int d = edit_alignment(s1, s2, output_handler, _edit_beam=100);

An edit cost beam may also be applied optionaly, which prunes exploration of edit paths based on a cost window.

    // apply a cost beam to limit exploration of "less promising" edit paths
    int d = edit_distance(s1, s2, _cost_beam=10);
