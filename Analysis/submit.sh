#!/bin/sh


#create new subdir (analysis_timestamp) , copy source there and make dir inside (output).
# add script there to get plots, merge etc as independent as possible from rest (will need container classes at least)


# define all syst here in an array
# qsub stuff
# check in background whether files are done (sleep 30)
# if all done go to dir (if not yet there) analysis_timestamp and merge stuff to have all syst there

# may want to have an additional script for pdf variations (to get all of them) needs level of communication between tree and this script...? how many variations..? or just do 40 and skip non existing, mark file in output and ignore in the following
