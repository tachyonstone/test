#include <stdio.h>

#define N 10

void print_route(int prev[], int g)
{
  int tmp;

  printf("%d",g);
  tmp = prev[g];

  while(tmp >= 0){
    printf(" <- %d",tmp);
    tmp = prev[tmp];
  }

  printf("\n");

  return;
}

int length(int tree[][3], int arc_num, int s, int t)
{
  int i;

  for(i = 0; i < arc_num; i++){
    if(tree[i][0] == s && tree[i][1] == t){
      return tree[i][2];
    }
  }

  return -1;
}

int distance(int prev[], int node, int tree[][3], int arc_num)
{
  int tmp = node, d = 0;
  while(prev[tmp] != -1){
    d += length(tree, arc_num, prev[tmp], tmp);
    tmp = prev[tmp];
  }

  return d;
}


int main(void)
{
  int tree[][3] = {{0,1,1},{0,2,3},{1,2,1},{1,6,6},{2,3,6},{2,6,6},{2,7,3},
		   {3,4,5},{3,7,2},{3,8,4},{4,8,2},{5,1,1},{6,5,7},{6,7,2},
		   {7,8,1},{7,9,7},{8,9,5}};
  int arc_num = sizeof(tree)/sizeof(tree[0]);
  int open[N];
  int closed[N] = {0};
  int size = 0;
  int start = 0;
  int goal = 9;
  int top,i,mindex,tmp;
  int dist[N] = {100,7,4,6,4,2,3,4,2,0};
  int prev[N] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

  open[size++] = start;

  while(size > 0){

    mindex = 0;

    for(i = 1; i < size; i++){
      if (distance(prev, open[i], tree, arc_num) + dist[open[i]] < distance(prev, open[mindex], tree, arc_num) + dist[open[mindex]])
	mindex = i;
    }

    tmp = open[mindex];
    open[mindex] = open[size-1];
    open[size-1] = tmp;

    top = open[--size];
    printf("%d,",top);  //openから取り出されたノードの表示

    if(top == goal){
      printf("\n");
      print_route(prev, goal);  //見つかった経路の表示
      printf("\nOK\n");

      return 0;
    }

    for(i = 0; i < arc_num; i++){

      if(tree[i][0] == top){

	if(closed[tree[i][1]] == 0){
	  open[size++] = tree[i][1];
	  prev[tree[i][1]] = top;

	  closed[tree[i][1]] = 1;

	}else if(closed[tree[i][1]] == 1){
	  if(distance(prev, tree[i][1], tree, arc_num) > distance(prev, top, tree, arc_num)+length(tree, arc_num, top, tree[i][1]))
	    prev[tree[i][1]] = top;
	}

      }

    }//END of for

  }//END of while

  printf("\nNG\n");

  return 0;

}//EOF
