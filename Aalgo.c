#include <stdio.h>

#define N 15

void print_route(int prev[], int g)
{
	int tmp;

	printf("%d", g);
	tmp = prev[g];

	while(prev[tmp] != -1){
		if(tmp >= 0){
			printf(" <-%d", tmp);
			tmp = prev[tmp];
		}

	}
	printf("<-%d\n", tmp);
	printf("\n");

	return;
}

int distance(int prev[], int node, int tree[][3], int arc_num)
{
	int tmp = node;
	int d = 0;

	while(prev[tmp] != -1){
		d += length(tree, arc_num, prev[tmp], tmp);
		tmp = prev[tmp];
	}

	return d;

}

int length(int tree[][3], int arc_num, int s, int t)
{
	int i;
	for (i = 0; i < arc_num; i++)
	{
		if (tree[i][0] == s && tree[i][1] == t)
		{
			return tree[i][2];
		}
	}

	return -1;

}

int main(void)
{
	int tree[][3] = {{0,1,2}, {0,2,3}, {1,3,4}, {1,4,1}, {2,5,1}, {2,6,2},
					 {5,6,1}, {3,7,8}, {3,8,7}, {4,9,2}, {4,10,1}, {5,11,3}, {5,12,2}, {6,13,6}, {6,14,6}};
	int arc_num = sizeof(tree)/sizeof(tree[0]);
	int open[N];
	int closed[N] = {0};
	int size = 0;
	int start =0;
	int goal = 9;
	int top,i,mindex,tmp;
	int prev[N] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int dist[N] = {3,5,3,3,1,2,5,8,7,0,2,1,6,6,6};
	int k;
	int l=0;


	open[size++] = start;

	while(size > 0){

		mindex = 0;

		for (k = 1; k < size; k++)
			{
				if(dist[open[mindex]]> dist[open[k]]){
					mindex = k;
				}

			}

			tmp = open[mindex];
			open[mindex] = open[size-1];
			open[size-1] = tmp;

			top = open[--size];
			printf("%d ", top);


			if(top == goal){
				printf("\nOK\n");
				print_route(prev,goal);
				return 0;
			}

		for (i = 0; i < arc_num; i++)
		{
			if (tree[i][0] == top)
			{
				if (closed[tree[i][1]] == 0)
				{
					open[size++] = tree[i][1];
					prev[tree[i][1]] = top;

					closed[tree[i][1]] = 1;

				}else if(closed[tree[i][1]] == 1){
					if(distance(prev, i, tree, arc_num) > distance(prev, top, tree, arc_num))
						prev[tree[i][1]] = top;
				}

			}

		}

	}

	printf("\nNG\n");

	
	return 0;
}