
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#define MAX_SIZE	1000

main(int argc, char* argv[]){
	int i;
	int number_of_points;
	long random_num;

	//scanf("%d",&number_of_points);
	number_of_points =  atoi(argv[1]);

	printf("%d\n",number_of_points);
	for(i=0;i<number_of_points;i++){
		random_num = random();
		printf("%ld\n", (random_num % MAX_SIZE));
	}	
}
