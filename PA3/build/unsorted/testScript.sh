for ((k = 100; k < 1000000000; k *= 100))
do

	echo "made this file $k"

	./kiwi "$k" > "unsort_N$k"

done
