###################################################################
#                                                                 #
#    MAXFLOW - software for computing mincut/maxflow in a graph   #
#                        Version 3.01                             #
#    http://www.cs.ucl.ac.uk/staff/V.Kolmogorov/software.html     #
#                                                                 #
#    Yuri Boykov (yuri@csd.uwo.ca)                                #
#    Vladimir Kolmogorov (v.kolmogorov@cs.ucl.ac.uk)              #
#    2001-2006                                                    #
#                                                                 #
###################################################################

1. Introduction.

This software library implements the maxflow algorithm described in

	"An Experimental Comparison of Min-Cut/Max-Flow Algorithms for Energy Minimization in Vision."
	Yuri Boykov and Vladimir Kolmogorov.
	In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), 
	September 2004

This algorithm was developed by Yuri Boykov and Vladimir Kolmogorov
at Siemens Corporate Research. To make it available for public use,
it was later reimplemented by Vladimir Kolmogorov based on open publications.

If you use this software for research purposes, you should cite
the aforementioned paper in any resulting publication.

----------------------------------------------------------------------

REUSING TREES:

Starting with version 3.0, there is a also an option of reusing search
trees from one maxflow computation to the next, as described in

	"Efficiently Solving Dynamic Markov Random Fields Using Graph Cuts."
	Pushmeet Kohli and Philip