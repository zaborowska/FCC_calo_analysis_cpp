#!/bin/sh -u
source /afs/cern.ch/exp/fcc/sw/0.8pre/setup.sh $1
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/install/lib/
