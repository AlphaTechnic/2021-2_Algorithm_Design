#include "Graph_adj_list.h"
#include <queue>

extern void errorExit(const char* s);

SLList2 pool;
unsigned long UsedMemoryForArray = 0;

void Read_Graph_adj_list(int Vnum, vertex* V, int Enum, edge* E) {
	// 파일에서 그래프 정보를 읽어 V와 E 배열을 구성한다.
	// V[ ].name과 E[ ].name은 설정할 필요가 없으나 편의상 각 배열에서의
	//   index를 저장한다.
	// V[ ].S는 adjacent list header로 스택 헤더로도 사용된다.
	// 따라서 edge (i,j)를 읽으면 V[i].S에 edge index k를 push하고
	//  마찬가지로 V[j].S에 k를 push한다.
	// V[].flag과 E[].flag은 반드시 false로 설정해야 한다.

	// ***이 함수를 작성한다

	// V 배열 초기화
	for (int i = 0; i < Vnum; i++) {
		V[i].name = i;
		V[i].flag = false;
	}
	// E 배열 초기화
	for (int i = 0; i < Enum; i++) {
		E[i].name = i;
		E[i].flag = false;
	}

	int a, b, c;
	SLL* p;
	for (int i = 0; i < Enum; i++) {
		scanf_s("%d%d%d", &a, &b, &c);
		E[i].vf = a;
		E[i].vr = b;
		E[i].cost = c;

		p = pool.allocSLL();
		p->i = i;
		V[a].S.push(p);

		p = pool.allocSLL();
		p->i = i;
		V[b].S.push(p);
	}
}

void Free_Graph_adj_list(int Vnum, vertex* V) {
	// V 배열의 adjacency list의 원소를 pop()을 사용하여
	// 모두  pool로 되돌려 준다

	// ***이 함수를 작성한다
	for (int i = 0; i < Vnum; i++) {
		while (!V[i].S.empty()) {
			pool.freeSLL(V[i].S.pop());
		}
	}

}

int BFS_Tree_adj_list(
	int     src,   // source node index
	int     Vnum,  // number of vertices
	vertex* V,     // vertex structure array (starting index 0)
	int     Enum,  // number of edges
	edge* E     // edge structure array (starting index 0)
) {
	// BFS를 사용하여 BFS tree에 속하는 에지의 flag을 true로 설정한다.
	// BFS 시작 vertex는 입력 파일에서 지정된다(src).
	// V[]의 adjacent list 탐색은 .top() 멤버 함수를 사용하여
	//  adj list 헤더를 얻고 이를 따라가며 인접 vertex를 탐색한다.
	// BFS tree에 속한 에지의 cost 함을 return 한다
	//   (not unique but 모든 각 edge cost가 1이면 unique)
	// -- 함수의 모든 parameter가 사용될 필요는 없다.
	// -- BFS를 위한 queue가 필요하면 STL의 queue를 사용해도 좋다

	// ***이 함수를 작성한다
	std::queue<int> que;
	int tot = 0;

	// init BFS state
	que.push(src);
	V[src].flag = true;

	while (!que.empty()) {
		int cur = que.front(); que.pop();
		for (SLL* p = V[cur].S.top(); p != NULL; p = p->p) {
			int btwn_e = p->i;
			if (E[btwn_e].flag) continue;  // already visited edge
			int nxt_v = (E[btwn_e].vf != cur ? E[btwn_e].vf : E[btwn_e].vr);
			if (V[nxt_v].flag) continue; // already visited vertex

			V[nxt_v].flag = true;
			E[btwn_e].flag = true;
			tot += E[btwn_e].cost;
			que.push(nxt_v);
		}
	}
	return tot;
}

// SLL management functions
// 아래 함수들은 02_04_CSE3081_CPrg&DataStructure_추가자료.pdf를
//   참조하여 각 함수들을 채워 넣자.
// 스택만 필요하므로 큐의 멤버 함수는 아래 함수 리스트에 없다

// stack member fuctions
void sllStack2::push(SLL* p) {
	// insert p to the top of stack ST
	p->p = ST;
	ST = p;
}

SLL* sllStack2::pop(void) {
	// remove and return p at the top of ST

	// ***이 함수를 작성한다
	SLL* data = ST;
	ST = ST->p;
	return data;
}

SLL* sllStack2::top(void) {
	// return p at the top of ST
	return ST;
}

bool sllStack2::empty(void) {
	// return true if empty
	if (ST == NULL)
		return true;
	else
		return false;
}

// SLList2 member functions
SLL* SLList2::allocSLL(void) {		// allocate an SLL element
	SLL* p;

	// ***이 함수를 작성한다
	// SLL_pool이 비어있으므로, system으로부터 새로운 메모리를 할당받는다.
	if (SLL_pool == NULL) {
		p = (SLL*)malloc(sizeof(SLL));
		if (p == NULL) {
			errorExit("Meomory allocation error (allocSLL)");
		}
		UsedMemoryForSLLs += sizeof(SLL);
		p->i = NONE;
	}
	// SLL_pool 헤드에 있는 원소를 return
	else {
		p = SLL_pool;
		SLL_pool = p->p;
	}
	p->p = NULL;
	SLL_cnt++;

	return(p);
}

void SLList2::freeSLL(SLL* p) {		// free *p (move it to SLL_pool
	if (p->i == NONE) {
		errorExit("This is already freed(freeSLL).");
	}

	// ***이 함수를 작성한다
	p->i = NONE;
	p->p = SLL_pool;
	SLL_pool = p;
	SLL_cnt--;
}

void SLList2::freeSLL_pool(void) {	// clear SLL_pool
	SLL* p = SLL_pool;

	// ***이 함수를 작성한다
	while (p != NULL) {
		SLL_pool = p->p;
		free(p);
		UsedMemoryForSLLs -= sizeof(SLL);
		p = SLL_pool;
	}

	if (SLL_cnt != 0) {
		errorExit("Non-zero SLL_cnt after cleanup.");
	}
}