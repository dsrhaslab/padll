#!/bin/bash

# padll_path='/home1/07853/rgmacedo/padll/build'
padll_path='path/to/padll/build'

iops=10000000
num_runs=1

# $1 = number of stages
# $2 = number of workflows
function Execute {
    echo "Executing bench.sh (stages = $1 ; workflows = $2)"
    echo ""

    export padll_workflows=$2
    for (( stage=1; stage<($1+1); stage++ )); do 
        LD_PRELOAD=$padll_path/libpadll.so $padll_path/padll_scalability_bench $num_runs $2 $iops $1  > /dev/null 2>&1 &
    	# echo "$?"
    done
    echo ""; echo "Results are placed at /tmp/padll-scalability-results/."; echo ""; 
}

"$@"
