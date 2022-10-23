gcc $1.c -o $1 -lpthread

for x in {5..50}
do
for ((i=0; i<$x; ++i )) ; 
do
for ((j=0; j<$x; ++j )) ; 
do
start=`date +%s%N`
./$1 $x $i $j > out1
echo $x, $i, $j
end=`date +%s%N`
echo Execution time was `expr $end - $start` nanoseconds. 
done
done
done
