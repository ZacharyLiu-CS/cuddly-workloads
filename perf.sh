#!/bin/bash

# Usage: sudo cpu.sh binary_name

binary_name=$1
frequency=400
time_duration=60
process_id=-1

flamegraph_path="./FlameGraph"
flamegraph_remote_url="git@github.com:brendangregg/FlameGraph.git"
is_running=0
if [ $process_id -eq -1 ];
then
        process_id=`ps -aux |grep ${binary_name}  |grep -v grep |awk '{print \$2}' | head -n 1`
        if [ -z "$process_id" ];
        then
                echo $binary_name "not running!"
        else
                is_running=1
                echo $binary_name "process id :"$process_id
        fi
fi

if [ ! -d $flamegraph_path ];
then
        mkdir -p $flamegraph_path
fi

if [ "`ls -A $flamegraph_path`" = "" ];
then
        git clone $flamegraph_remote_url
fi

if [ $is_running -eq 1 ]
then
        perf record -F ${frequency} -p ${process_id}  -g -- sleep ${time_duration}
        perf script | ${flamegraph_path}/stackcollapse-perf.pl | ${flamegraph_path}/flamegraph.pl > ${binary_name}_`date +%s`.svg
fi

