#!/bin/bash

# Testing script for Maze Challenge
# Author: We_free (Christopher Sykes, Ben Matejka, Jacob Werzinsky, Sebastian Saker)
# Date: Mar 6, 2020
#
# NOTE: We assume that 
#
# USAGE: bash -v testing.sh

if ( ! test -x AMStartup )
    then 
        echo 1&>2 "The querier doesn't exist and/or is not executable"
        exit 1
    else

    #####################################
    ##### These tests should fail #######
    printf "\nThese tests should fail: \n"

    # testing eronoeus command line arguments
    printf "\nEroneous command line arguments:\n"
    printf "no options passed:\n"
    ./AMStartup
    printf "no options passed:\n"
    ./AMStartup hello this shouldnt work
    printf "incorrect options but correct parameters:\n"
    ./AMStartup -f 2 -g 3 -l flume.cs.dartmouth.edu
    printf "invalid nAvatars:\n"
    ./AMStartup -n 11 -d 3 -h flume.cs.dartmouth.edu
    printf "invalid Difficulty:\n"
    ./AMStartup -n 3 -d 11 -h flume.cs.dartmouth.edu
    printf "invalid Hostname:\n"
    ./AMStartup -n 2 -d 3 -h error.cs.dartmouth.edu


    ######################################
    ###### These tests should pass #######
    printf "\nThese tests should pass: \n"

    printf "Testing with one avatar should work...\n"
    ./AMStartup -n 1 -d 0 -h flume.cs.dartmouth.edu
    ./AMStartup -n 1 -d 1 -h flume.cs.dartmouth.edu
    ./AMStartup -n 1 -d 2 -h flume.cs.dartmouth.edu
    ./AMStartup -n 1 -d 3 -h flume.cs.dartmouth.edu

    printf "\nTesting with 2 avatars at all difficulties: \n"
    ./AMStartup -n 2 -d 0 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 1 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 2 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 3 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 4 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 5 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 6 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 7 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 8 -h flume.cs.dartmouth.edu
    ./AMStartup -n 2 -d 9 -h flume.cs.dartmouth.edu

    printf "\nTesting with 3 avatars at all difficulties: \n"
    ./AMStartup -n 3 -d 0 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 1 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 2 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 3 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 4 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 5 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 6 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 7 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 8 -h flume.cs.dartmouth.edu
    ./AMStartup -n 3 -d 9 -h flume.cs.dartmouth.edu

     printf "\nTesting with 4 avatars at all difficulties: \n"
    ./AMStartup -n 4 -d 0 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 1 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 2 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 3 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 4 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 5 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 6 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 7 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 8 -h flume.cs.dartmouth.edu
    ./AMStartup -n 4 -d 9 -h flume.cs.dartmouth.edu
