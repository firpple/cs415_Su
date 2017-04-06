gcc generator.c -o RNG

for ((k = 100; k <= 10000; k *= 10))
do

	echo "made this file $k"

	./RNG "$k" > "unsort_N$k"

done

for ((k = 100000; k <= 300000; k += 50000))
do

	echo "made this file $k"

	./RNG "$k" > "unsort_N$k"

done

rm RNG
