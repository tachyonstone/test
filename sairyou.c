#include <stdio.h>

#define N 16

void print_array(int array[], int array_num)
{
	int i;

	for (i = 0; i < array_num; i++)
	{
		printf("%d, ", array[i]);
	}
	printf("\n");

	return;

}

int main(void)
{
	int i = 0;
	int j,k;
	int tree[][2] = {{0,1}, {0,2}, {1,3}, {1,4}, {2,5}, {2,6}};
	int open[N];
	int dist[] = {3,5,2,6,1,0,4};
	int arc_num = sizeof(tree)/sizeof(tree[0]);
	int mindex = 0;
	int size = 0;
	int target = 2;
	int top;
	int tmp;

	open[size++] = tree[0][0];


	while (i < N)
	{

		print_array(open, size);
		

		top = open[--size];
		if(top == target){
			printf("OK\n");
			return 0;
		}


		/*yamanobori*/
		//size = 0;

		for (j = 0; j < arc_num; j++)
		{

			for (k = 0; k < size; k++)
			{
				if(dist[open[mindex]]> dist[open[k]]){
					mindex = k;
				}

			}

			tmp = open[mindex];
			open[mindex] = open[size-1];
			open[size-1] = tmp;

			if (tree[j][0] == top)
			{
				open[size++] = tree[j][1];
			}
		}

		i++;

	}

	printf("NG\n");

	return 0;

}