#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

#define NONE -1

typedef struct elm_edge {
	int  vf, vr;  // e = (vf, vr) where vf, vr are vertex array indices
	int  cost;    // edge cost
	bool flag;    // true if the edge is in the SP spanning tree, false otherwise 
	int  fp, rp;  // adj list ptr of forward and reverse (-1 if none)
} edge;

typedef struct elm_vertex {
	int  f_hd, r_hd;	// adj list header(-1 if none)
	int  distance;		// distance from src to this vertex
	int  heap_idx;		// minHeap index
	bool inS;			// true if SP length found(true when move from V-S to S) 
} vertex;


class MinHeap {
private:
	int* heap;  // 배열을 이용한 heap 구현
	int sz; // size
	vertex* vertex_arr;  // array of vertices

public:
	MinHeap(int* arr_for_heap, vertex* V) {
		heap = arr_for_heap;
		sz = 0;
		vertex_arr = V;
	}

	void heappush(const int val);
	int heappop(void);
	void heapify(int idx);
};

void MinHeap::heappush(const int val) {
	heap[++sz] = val;
	heapify(sz);
}

int MinHeap::heappop(void) {
	int mnv = heap[1];
	heap[1] = heap[sz];
	heap[sz--] = NONE;

	// heapify
	int idx = 1;
	int to_swap;
	while (true) {
		to_swap = idx;
		// left chd와 비교
		if (2 * idx <= sz && vertex_arr[heap[idx]].distance > vertex_arr[heap[2 * idx]].distance) {
			to_swap = 2 * idx;
		}
		// right chd와 비교
		if (2 * idx + 1 <= sz && vertex_arr[heap[to_swap]].distance > vertex_arr[heap[2 * idx + 1]].distance) {
			to_swap = 2 * idx + 1;
		}
		// to_swap이 갱신되지 않았다면, break
		if (to_swap == idx) {
			break;
		}

		// swap
		std::swap(heap[idx], heap[to_swap]);
		vertex_arr[heap[idx]].heap_idx = idx;
		idx = to_swap;
	}
	vertex_arr[heap[idx]].heap_idx = idx;

	return mnv;
}

void MinHeap::heapify(int idx) {
	while (idx > 1 && vertex_arr[heap[idx]].distance < vertex_arr[heap[idx >> 1]].distance) {
		std::swap(heap[idx], heap[idx >> 1]);
		vertex_arr[heap[idx]].heap_idx = idx;
		idx >>= 1;
	}
	vertex_arr[heap[idx]].heap_idx = idx;
}


static int back_trace(const int src, const int Vnum, const vertex* V, edge* E) {
	int cumulative_dist = 0;

	int max_dist;
	int edge_of_max = -1;
	for (int v = 0; v < Vnum; v++) {
		if (v == src) continue;

		// front 에 걸려있는 edge들 탐색
		max_dist = INT_MIN;
		int front_e = V[v].f_hd;
		while (front_e != NONE) {
			const int& nxt_nd = E[front_e].vr;
			if (V[v].distance == V[nxt_nd].distance + E[front_e].cost && V[nxt_nd].distance > max_dist) {
				max_dist = V[nxt_nd].distance;
				edge_of_max = front_e;
			}
			front_e = E[front_e].fp;
		}

		// rear 에 걸려있는 edge들 탐색
		int rear_e = V[v].r_hd;
		while (rear_e != NONE) {
			const int& nxt_nd = E[rear_e].vf;
			if (V[v].distance == V[nxt_nd].distance + E[rear_e].cost && V[nxt_nd].distance > max_dist) {
				max_dist = V[nxt_nd].distance;
				edge_of_max = rear_e;
			}
			rear_e = E[rear_e].rp;
		}

		E[edge_of_max].flag = true;
		cumulative_dist += E[edge_of_max].cost;
	}
	return cumulative_dist;
}


int SPT_Dijkstra(
	int src,	// source vertex index
	// graph structure array
	// 1. the adjacency list structure is the same as PHW02
	// 2. additional fields are added for Dijkstra's algorithm(see .h file)
	int Vnum, vertex* V,	// Vertex array size and the array
	int Enum, edge* E,		// Edge array size and the array

	int* minHeap	// array for min heap (array size = Vnum+1)
		// heap index range is 1 ~ (Vnum - 1) note: src must not in the initial heap
		// just arry is passed. must construct min-heap in this function

	// OUTPUT
	// 1. V[].distance : shortest path length from src to this vertex
	// 2. shortest path spanning tree : set E[].flag = true if the edge is in the tree
	// 3. return the sum of edge costs in the shortest path spanning tree.
	//    should be small as possible (think a way to make it small)
) {
	int treeCost = 0;
	// *** 이 함수를 작성하자 ***
	// 반드시 min-heap을 사용하여 O((n+m)logn) 알고리즘을 구현해야 한다(아니면 trivial한 프로그램임)
	// heap 연산 등 필요한 함수는 자유롭게 작성하여 추가한다.
	// 그러나 global 변수, dynamic array 등은 추가로 사용하지 않는다(실제로 필요 없다)


	// V[src] 초기화
	V[src].inS = true;
	V[src].distance = 0;

	// src와 인접한 vertex에 대해 distance inititialize
	int front_e = V[src].f_hd;
	while (front_e != NONE) {
		V[E[front_e].vr].distance = E[front_e].cost;
		front_e = E[front_e].fp;
	}
	int rear_e = V[src].r_hd;
	while (rear_e != NONE) {
		V[E[rear_e].vf].distance = E[rear_e].cost;
		rear_e = E[rear_e].rp;
	}

	// pq에 삽입
	MinHeap mh = MinHeap(minHeap, V);
	for (int v = 0; v < Vnum; v++) {
		if (!V[v].inS) {
			mh.heappush(v);
		}
	}

	// dijkstra iteration
	for (int i = 0; i < Vnum - 2; i++) {
		const int cur_nd = mh.heappop();
		V[cur_nd].inS = true;

		// front iteration
		front_e = V[cur_nd].f_hd;
		while (front_e != NONE) {
			const int& nxt_nd = E[front_e].vr;
			int new_dist = V[cur_nd].distance + E[front_e].cost;
			if (!V[nxt_nd].inS && new_dist < V[nxt_nd].distance) {
				// new_dist로 갱신하고 heapify
				V[nxt_nd].distance = new_dist;
				mh.heapify(V[nxt_nd].heap_idx);
			}
			front_e = E[front_e].fp;
		}

		// rear iteration
		rear_e = V[cur_nd].r_hd;
		while (rear_e != NONE) {
			const int& nxt_nd = E[rear_e].vf;
			int new_dist = V[cur_nd].distance + E[rear_e].cost;
			if (!V[nxt_nd].inS && new_dist < V[nxt_nd].distance) {
				// new_dist로 갱신하고 heapify
				V[nxt_nd].distance = new_dist;
				mh.heapify(V[nxt_nd].heap_idx);
			}
			rear_e = E[rear_e].rp;
		}
	}
	const int last_nd = mh.heappop();
	V[last_nd].inS = true;

	treeCost = back_trace(src, Vnum, V, E);

	return treeCost;
}


void Read_Graph(int Vnum, vertex* V, int Enum, edge* E) {
	// Graph 자료구조를 만드는 함수
	// *** 이 함수를 추가하자 ***
	// PHW02의 Read_Graph_adj_array()를 이 과제의 자료구조에 맞춰 살짝 수정하여 사용한다
	// 즉, Read_Graph_adj_array()에서 불필요한 내용을 제거한 후 사용.

	// vertex array V 초기화
	for (int i = 0; i < Vnum; i++) {
		for (int i = 0; i < Vnum; i++) {
			V[i].f_hd = V[i].r_hd = NONE;
			V[i].distance = INT_MAX;
			V[i].heap_idx = NONE;
			V[i].inS = false;
		}
	}

	// generate graph by user's input
	int v1, v2, cost;
	for (int idx = 0; idx < Enum; idx++) {
		scanf_s("%d%d%d", &v1, &v2, &cost);

		E[idx].vf = v1;
		E[idx].vr = v2;
		E[idx].cost = cost;
		E[idx].flag = false;

		E[idx].fp = V[v1].f_hd;
		E[idx].rp = V[v2].r_hd;

		V[v1].f_hd = V[v2].r_hd = idx;
	}
}

// the following functions are for testing if the submitted program is correct.
int  Tree_Check(int Vnum, vertex* V, int Enum, edge* E, int* visited);
bool SPT_test(int src, int Vnum, vertex* V, int Enum, edge* E, int* minHeap);
void Error_Exit(const char* s);

int main(void) {
	int		src;
	vertex* V;		int Vnum;
	edge* E;		int Enum;
	int* minHeap;
	int    Tree_cost;
	int    Tnum; 		// # of test cases
	clock_t start, finish;
	double cmpt;

	scanf_s("%d", &Tnum);		// read # of tests

	for (int t = 0; t < Tnum; t++) {
		scanf_s("%d %d %d", &Vnum, &Enum, &src);
		V = new vertex[Vnum];
		E = new edge[Enum];
		minHeap = new int[Vnum + 1];	// heap array allocation
		if (V == NULL || E == NULL || minHeap == NULL) {
			Error_Exit("Memory Allocation Error");
		}
		Read_Graph(Vnum, V, Enum, E);

		/**/start = clock();	// start timer

		Tree_cost = SPT_Dijkstra(src, Vnum, V, Enum, E, minHeap);	// code by students

		/**/finish = clock();	// stop timer
		cmpt = ((double)(finish - start)) / (double)CLK_TCK;

		// 아래 Tree_Check와 SPT_test 함수는 첨부한 SPT_test.obj 파일에 있다.
		// 이 테스트에서 오류로 인하여 프로그램이 정지하면 뭔가 잘못된 것이다(제출해도 0점)
		if (Tree_Check(Vnum, V, Enum, E, minHeap) == 0) {
			Error_Exit("   ERROR The result is not a spanning tree");
		}
		if (SPT_test(src, Vnum, V, Enum, E, minHeap) == false) {
			Error_Exit("** Something wrong in applying Dijkstra's");
		}
		if (t != 0)
			printf("\n");

		printf("**T%d (Dijkstra) (V = %d, E = %d, time = %.3f sec) Tree Cost = %d\n",
			t + 1, Vnum, Enum, cmpt, Tree_cost);

		delete[] minHeap; delete[] V; delete[] E;
	}
	return 0;
}

void Error_Exit(const char* s) {
	printf("%s\n", s);
	exit(-1);
}
