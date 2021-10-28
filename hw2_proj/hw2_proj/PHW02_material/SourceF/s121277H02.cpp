#include "Graph_adj_array.h"

void Read_Graph_adj_array(int Vnum, vertex V[], int Enum, edge E[]) {
    // read graph information
    // V와 E의 name field는 각각 자신의 인덱스를 저장한다
    // V[].flag과 E[].flag은 반드시 false로 설정해야 한다.

    // ***이 함수를 작성한다

    // V 배열 초기화
    for (int i = 0; i < Vnum; i++) {
        V[i].name = i;
        V[i].f_hd = V[i].r_hd = NONE;
        V[i].flag = false;
    }
    // E 배열 초기화
    for (int i = 0; i < Enum; i++) {
        E[i].name = i;
        E[i].flag = false;
    }

    int a, b, c;
    for (int i = 0; i < Enum; i++) {
        scanf_s("%d%d%d", &a, &b, &c);
        E[i].vf = a;
        E[i].vr = b;
        E[i].cost = c;

        E[i].fp = V[a].f_hd;
        E[i].rp = V[b].r_hd;

        V[a].f_hd = i;
        V[b].r_hd = i;
    }
}

int DFS_Tree_adj_array(
    int     src,   // source node index
    int     Vnum,  // number of vertices
    vertex* V,     // vertex structure array (starting index 0)
    int     Enum,  // number of edges
    edge* E     // edge structure array (starting index 0) 
) {
    // DFS를 사용하여 DFS tree에 속하는 에지의 flag을 true로 설정한다.
    // DFS 시작 vertex는 입력 파일에서 지정된다(src).
    // DFS tree에 속한 에지의 cost 함을 return 한다(모든 각 edge cost가 1이면 unique)
    // recursive 함수로 작성해도 무방하다.

    // ***이 함수를 작성한다***

    int tot = 0;
    V[src].flag = true;

    // front edge를 따라 dfs 탐색
    for (int btwn_e = V[src].f_hd; btwn_e != NONE; btwn_e = E[btwn_e].fp) {
        int nxt = E[btwn_e].vr;  // between edge 기준 next vertex는 vr에 있다.
        if (V[nxt].flag) continue;

        E[btwn_e].flag = true;
        tot += E[btwn_e].cost + DFS_Tree_adj_array(nxt, Vnum, V, Enum, E);
    }

    // rear edge를 따라 dfs 탐색
    for (int btwn_e = V[src].r_hd; btwn_e != NONE; btwn_e = E[btwn_e].fp) {
        int nxt = E[btwn_e].vf;  // between edge 기준 next vertex는 vf에 있다.
        if (V[nxt].flag) continue;

        E[btwn_e].flag = true;
        tot += E[btwn_e].cost + DFS_Tree_adj_array(nxt, Vnum, V, Enum, E);
    }
    return tot;
}