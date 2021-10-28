#define _CRT_SECURE_NO_WARNINGS
using namespace std;
#include <time.h>
#include <stack>
#include "DBL.h"

//#define NO_PATH_OUT   // comment out this line for path output
void Error_Exit(const char* s);

typedef struct _Vertex {
	dblStack S;		// adj list contains edge index
	int degree;
} Vertex;

typedef struct _Edge {
	int v1, v2;
} Edge;

void graphGeneration(Vertex** V, Edge** E, int* VN, int* EN);
void adjListGenerate(Vertex* V, Edge* E, int VN, int EN);
void deallocGraph(Vertex* V, Edge* E, int VN);
int* Find_Euler(Vertex* V, Edge* E, int VN, int EN, int* flag, int* pathN);

DBList pool;	// DBL storage pool

int main() {
	int T, VN, EN;
	Vertex* V;
	Edge* E;
	int* path;	// Euler cycle or path
	int pathN;  // path length
	int  flag;	// 0: cycle, 1: path, 2: none
	clock_t start_time, finish_time;

	scanf("%d", &T);	// read # of tests
	for (int t = 1; t <= T; t++) {	// for each test
		graphGeneration(&V, &E, &VN, &EN);

		start_time = clock(); // set the start time

		path = Find_Euler(V, E, VN, EN, &flag, &pathN); // find an Euler path or cycle

		finish_time = clock(); // set finish time

		double cmpt = (((double)(finish_time - start_time)) / CLK_TCK) * 1000; // compute the time passed
		printf("Test= %d flag= %d VnumInCycle/Path)= %d ", t, flag, pathN);

		if (flag == 0)
			printf("Euler_cycle(exec_time= %.2f msec)\n", cmpt);
		else if (flag == 1)
			printf("Euler_path(exec_time= %.2f msec)\n", cmpt);
		else
			printf("not_Eulerian(exec_time= %.2f msec)\n", cmpt);

#ifndef NO_PATH_OUT
		if (flag != 2) {
			for (int i = 0; i < EN + 1; i++) {
				printf("%d\n", path[i]);
			}
		}
#endif
		if (flag != 2)
			delete[] path;
		deallocGraph(V, E, VN);
	}
	pool.freeDBL_pool();	// clear all the DBL elements

	return 0;
}

int* Find_Euler(Vertex* V, Edge* E, int VN, int EN, int* flag, int* pathN) {
	// input V, VN, E, EN
	// output through paramters
	//   *flag = 0 (Euler cycle), 1 (Euler path), 2 (not Eulerian)
	//   *pathN = size of path[] array
	// output by return
	//   *path = list of vertex ids found(Euler cycle or path)

	stack<int> ST;		// use STL stack as explained in the class
	int* path = NULL;

	// *** 이 부분을 작성하세요

	// Initialize state
	*pathN = 0;
	int odd_cnt = 0;
	for (int i = 0; i < VN; i++) {
		if (V[i].degree % 2 == 1) {
			odd_cnt++;
			ST.push(i);
		}
	}
	switch (odd_cnt) {
	case 0:  // Euler cycle
		*flag = 0;
		ST.push(0);
		break;
	case 2:  // Euler path
		*flag = 1;
		break;
	default:  // not Eulerian
		*flag = 2;
		return path;
	}

	*pathN = EN + 1;
	path = new int[*pathN + 5];

	int cnt = 0;
	while (!ST.empty()) {
		int cur = ST.top();
		if (V[cur].degree == 0) {
			path[cnt++] = cur;
			ST.pop();
			continue;
		}

		// cur에 인접한 임의의 에지 e = (cur, nxt)를 선택
		DBL* btwn_e = V[cur].S.top();
		DBL* btwn_e2 = btwn_e->twin;
		int nxt = (E[btwn_e->d].v1 != cur) ? E[btwn_e->d].v1 : E[btwn_e->d].v2;

		// 선택한 edge를 삭제
		V[cur].degree--;
		V[cur].S.remove(btwn_e);  // means pop()
		V[nxt].degree--;
		V[nxt].S.remove(btwn_e2);
		pool.freeDBL(btwn_e);
		pool.freeDBL(btwn_e->twin);

		// push nxt
		ST.push(nxt);
	}
	return path;
}

void deallocGraph(Vertex* V, Edge* E, int VN) {
	DBL* p;

	// *** 여기에 adj list를 삭제하는 routine을 작성하세요
	for (int i = 0; i < VN; i++) {
		while (!V[i].S.empty()) {
			p = V[i].S.pop();
			pool.freeDBL(p);
		}
	}

	delete[] V;
	delete[] E;
}

void graphGeneration(Vertex** V, Edge** E, int* VN, int* EN) {
	scanf("%d %d", VN, EN);	// read # of vertices and edges
	//allocVandEarray(V, E, *VN, *EN);	// vertex and edge array allocation

	*V = new Vertex[*VN];
	*E = new Edge[*EN];
	if (*V == NULL || *E == NULL) {
		Error_Exit("Memory Allocation Error!");
	}

	for (int v = 0; v < *VN; v++) {
		(*V)[v].degree = 0;
	}
	for (int e = 0; e < *EN; e++) {
		scanf("%d %d", &((*E)[e].v1), &((*E)[e].v2));	// read edge information
		++((*V)[(*E)[e].v1].degree);
		++((*V)[(*E)[e].v2].degree);
	}
	adjListGenerate(*V, *E, *VN, *EN);	// create adj lust in G=(V,E)
}

void adjListGenerate(Vertex* V, Edge* E, int VN, int EN) {
	// Construct adjacent list in V array
	int v1, v2;
	DBL* adjE1, * adjE2;

	// *** 이 부분을 작성하세요
	for (int e = 0; e < EN; e++) {
		adjE1 = pool.allocDBL();
		adjE2 = pool.allocDBL();
		adjE1->d = e;
		adjE2->d = e;
		adjE1->twin = adjE2;
		adjE2->twin = adjE1;

		v1 = E[e].v1;
		v2 = E[e].v2;
		V[v1].S.push(adjE1);
		V[v2].S.push(adjE2);
	}
}

void Error_Exit(const char* s) {
	printf("%s", s);
	exit(-1);
}

DBL* DBList::allocDBL(void) {		// allocate a DBL element
	DBL* p;

	// *** 이 부분을 작성하세요
	if (DBL_pool == NULL) {  // pool이 비었다면, system 메모리에서 가져온다.
		p = new DBL;
		if (p == NULL) Error_Exit("Memory allocation error (Alloc_DBL)");

		p->d = NONE;
		UsedMemoryForDBLs += sizeof(DBL);
	}
	else {
		p = DBL_pool;
		DBL_pool = p->rp;
	}
	p->rp = p->lp = p->twin = NULL;
	++DBL_cnt;

	return(p);
}

void DBList::freeDBL(DBL* p) {
	// move p to pool

	if (p->d == NONE) {
		Error_Exit("This element is already freed(Free_DBL).");
	}

	// *** 이 부분을 작성하세요
	p->d = NONE;
	p->rp = DBL_pool;
	DBL_pool = p;

	--DBL_cnt;		// reduce # of active DBL elements by one
}

void DBList::freeDBL_pool(void) {
	DBL* p = DBL_pool;

	while (p != NULL) {
		DBL_pool = p->rp;	// get next pointer
		delete p;
		p = DBL_pool;
		UsedMemoryForDBLs -= sizeof(DBL);
	}
	if (DBL_cnt != 0) {
		Error_Exit("Non-zero DBL_cnt after cleanup.");
	}
}

void dblStack::push(DBL* p) {

	// *** 이 부분을 작성하세요
	if (ST != NULL) ST->lp = p;
	p->rp = ST;
	p->lp = NULL;
	ST = p;
}

DBL* dblStack::pop(void) {	// remove and return p in front of Q
	DBL* p;

	// *** 이 부분을 작성하세요
	p = ST;
	if (ST->rp == NULL) {
		ST = NULL;
	}
	else {
		ST = ST->rp;
		ST->lp = NULL;
	}

	return p;
}

void dblStack::remove(DBL* p) {	// delete p from ST

	// *** 이 부분을 작성하세요
	if (ST == p) {
		pop();
	}
	else {
		(p->lp)->rp = p->rp;
		if (p->rp != NULL) {
			(p->rp)->lp = p->lp;
		}
	}
}

DBL* dblStack::top(void) {
	return ST;
}

bool dblStack::empty(void) {
	if (ST == NULL)
		return true;
	else
		return false;
}