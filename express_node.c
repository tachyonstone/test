#include <stdio.h>


void print_array(int array[], int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		printf("%d ", array[i]);
	}
	printf("\n");

	return;

}

void express(int a[][2], int express_node[], int num)
{
	int i, k;

	for (i = 0; i < num; i++)
	{
		k = a[i][1];
		express_node[k] = a[i][0];

	}

	return;

}


int main(void)
{

	int a[][2] ={{3,1}, {1,4}, {4,0}, {0,5}, {5,2}};
	int express_node[] = {-1,-1,-1,-1,-1};

	int num;

	num = sizeof(a)/sizeof(a[0]);

	express(a, express_node, num);
	print_array(express_node, num);

	return 0;

}