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

void express(int a[], int express_node[], int num)
{
	int i, k, l;

	for (i = 1; i < num; i++)
	{
		k = a[i-1];
		l = a[i];

		express_node[l] = k;

	}

	return;

}


int main(void)
{

	int a[] ={2,4,1,0,3};
	int express_node[] = {-1,-1,-1,-1,-1};

	int num;

	num = sizeof(a)/sizeof(a[0]);

	express(a, express_node, num);
	print_array(express_node, num);

	return 0;

}