#include <stdio.h>

#define MAX 64

void enqueue(int queue[], int *tail, int n)
{

	if(*tail < MAX)
		queue[(*tail)++] = n;

	return;

}

void dequeue(int *head, int *tail)
{

	if(*head < *tail)
		(*head)++;

	return;

}

void print_queue(int queue[], int tail, int head)
{
	int i;

	for (i = head; i < tail; i++)
	{
		printf("%d ", queue[i]);
	}
	printf("\n");

	return;

}

int main(void)
{
	int queue[MAX];
	int n, i;

	int head = 0;
	int tail = 0;


	n = 3;
	enqueue(queue, &tail, n);
	print_queue(queue, tail, head);

	n = 1;
	enqueue(queue, &tail, n);
	print_queue(queue, tail, head);

	n = 4;
	enqueue(queue, &tail, n);
	print_queue(queue, tail, head);

	dequeue(&head, &tail);
	print_queue(queue, tail, head);

	dequeue(&head, &tail);
	print_queue(queue, tail, head);

	
	return 0;
}

