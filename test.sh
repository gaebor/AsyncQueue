n=32
thread=0
thread_type=0

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

for m in ${n[*]}
do
    for chunk in `seq $min $m`
    do
        echo -n "chunk: $chunk n: $m, "
        if [ ! -f test.c$chunk.n$m.txt ]
        then 
            env time -f "%esec" ./event -c $chunk -n $m --type 0 2>&1 > test.c$chunk.n$m.txt
        else
            echo
        fi
    done
    echo 
done

nmax=`echo "${n[*]}" | tr ' ' '\n' | sort -nr | head -n1`

echo -n "n/c"
for chunk in `seq $min $((nmax-thread))`
do
    echo -n "	$chunk"
done
echo

for m in ${n[*]}
do
    for chunk in `seq $min $((m-thread))`
    do
        echo -n "$m"
        result="`env time -f "%e" ./event -c $chunk -n $m --type $thread_type -t $thread 2>&1 > test.txt`"
        errorcode=$?
        if [ $errorcode -ne 0 ]
        then
            result=FAIL
        else
            difference="`diff -q test.txt test.c$chunk.n$m.txt`"
            if [ -n "$difference" ]
            then
                result=DIFF
            fi
        fi
        echo -n "	$result"
    done
    echo 
done
