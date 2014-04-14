Note: I am currently doing development for edit_distance() on my fork of Boost.Algorithm:
* https://github.com/erikerlandson/algorithm/tree/edit_distance
* https://github.com/erikerlandson/algorithm/tree/order_np_alg
* http://erikerlandson.github.io/algorithm/libs/algorithm/doc/html/algorithm/Sequence.html


edit_distance
=============

A C++ Boost implementation of the edit distance between two sequences -- aka Levenshtein distance, aka string distance, aka string difference.

The edit_distance library provides a function edit_distance():
* edit_distance(seq1, seq2) returns the edit distance between two sequences, which may be any C++ range.
* edit_distance(seq1, seq2, _script = out) returns the edit distance, and calls special handler methods from object 'out' on the correspondiung "script" of edit operations.

These functions are based on a variation of the Dijkstra Single Source Shortest Path algorithm, optimized for the particular structure of an edit graph.  They are efficient on long sequences with localized areas of differences (e.g. computing the diff of two similar files).  Whenever conditions permit, an even faster specialized distance algorithm is automatically applied:
    An O(ND) Difference Algorithm and its Variations
    by Eugene W. Myers
    Dept of Computer Science, University of Arizona

A user specified edit operation cost function can be provided:

    // provide a customized cost for insertion, deletion and/or substitution
    int d = edit_distance(s1, s2, _cost=my_cost_type());

    // use a customized definition of element equality:
    int d = edit_distance(s1, s2, _equal=my_equal());

Sequences may be any kind of C++ range object, and may be mixed:

    // Any forward range can be provided to these functions.
    // Sequence element types are not required to be exactly equal, as long as they are compatible.
    int d = edit_distance(my_vector, my_list | boost::adaptors::reversed);
