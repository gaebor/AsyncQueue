#!/usr/bin/env bash

n=32
thread=0
thread_type=0

executable=./`hostname`/event

min=10

while [[ $# -gt 0 ]]
do
key="$1"
case $key in
    -n)
    n="$2"
    shift # past argument
    shift # past value
    ;;
    -m|--min)
    min="$2"
    shift # past argument
    shift # past value
    ;;

    -t|--thread)
    thread="$2"
    shift # past argument
    shift # past value
    ;;
    -T|--type)
    thread_type="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    n=("$@")
    break
    shift # past argument
    ;;
esac
done

baseline=()

for m in ${n[*]}
do
    for chunk in `seq $min $m`
    do
        echo -n "chunk: $chunk n: $m, "
        TIME=`env time -f "%e" $executable -c $chunk -n $m --type 0 2>&1 > test.c$chunk.n$m.txt`
        echo $TIME sec
    done
    echo 
    baseline+=("$TIME")
done

nmax=`echo "${n[*]}" | tr ' ' '\n' | sort -nr | head -n1`

echo -n "n\c"
for chunk in `seq $min $((nmax-thread))`
do
    echo -n "	$chunk"
done
echo

this_thread=$((2**thread))

len=${#n[@]}
for (( i=0; i<$len; i++ ))
do
    m=${n[$i]}
    reftime=${baseline[$i]}
    echo -n "$m"
    for chunk in `seq $min $((m-thread))`
    do
        result="`env time -f "%e" $executable -c $chunk -n $m --type $thread_type -t $this_thread 2>&1 > test.txt`"
        errorcode=$?
        if [ $errorcode -ne 0 ]
        then
            result=FAIL
        else
            difference="`diff -q test.txt test.c$chunk.n$m.txt`"
            if [ -n "$difference" ]
            then
                result=DIFF
            else
                result=`python -c "print('%.2f%%' % (100.0*$result*$this_thread/$reftime-100))"`
            fi
        fi
        echo -n "	$result"
    done
    echo 
done
