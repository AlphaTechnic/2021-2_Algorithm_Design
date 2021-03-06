#define _CRT_SECURE_NO_WARNINGS
#include <algorithm>

using namespace std;


void Edit_Distance(
    // inputs
    char* SS,      // Source string array
    char* TS,      // Edited string array
    int ins_cost, int del_cost, int sub_cost,
    // insertion, deletion, substitution cost
    // outputs
    int** Table,    // Cost Table Calculated (2-d array (|SS|+1) x (|TS|+1) will be set)
    char** SR,      // Source string with insertion to T marks '*'
    char** OP,      // Operation sequence('i', 'd', 's', or '.' (nop))
    char** TR,       // Edited string with deletion form S mark '*'
    // NOTE: ST, OP, TR must be allocated inside the function as needed(minimum as possible)
    int* Mem_Allocated
);


// 요구사항의 요구에 맞게 parent를 선택하는 함수
int get_parent(int case1, int case2, int case3, int ins_cost, int del_cost) {

    enum eType {
        Insertion, Deletion, Substitution,
    };

    if (case1 == case2 && case2 <= case3) {
        // 여러 parent를 택할 수 있을 때, ins_cost가 작은 경우 => Insertion을 택한다.
        return (ins_cost <= del_cost) ? Insertion : Deletion;
    }
    if (case1 <= case2 && case1 <= case3) {
        return Insertion;
    }
    if (case2 <= case1 && case2 <= case3) {
        return Deletion;
    }
    if (case3 <= case1 && case3 <= case2) {
        return Substitution;
    }
    return Substitution;
}


void Edit_Distance(
    // inputs
    char* SS,  // Source string array
    char* TS,  // Edited string array
    int ins_cost,  // insertion cost
    int del_cost,  // deletion cost
    int sub_cost,  // substitution cost

    // outputs
    int** Table,  // Cost Table Calculated (2-d array (|SS|+1) * (|TS|+1) will be set)
    char** SR,  // Source string('*' implies "insertion" of a char to T)
    char** OP,  // Operation sequence('i', 'd', 's', or '.'(nop))
    char** TR,  // Edited string('*' implies "deletion" of a char from S)

    int* Mem_Allocated
) {
    // edge case 정의 r = 0 혹은 c = 0
    for (int r = 0; r <= (int)strlen(SS); r++) {
        Table[r][0] = r * del_cost;
    }
    for (int c = 0; c <= (int)strlen(TS); c++) {
        Table[0][c] = c * ins_cost;
    }

    // fill table using bottom up fashion
    const int sub_cost_save = sub_cost;
    for (int r = 1; r <= (int)strlen(SS); r++) {
        for (int c = 1; c <= (int)strlen(TS); c++) {
            int case1 = Table[r][c - 1] + ins_cost;  // insert인 경우
            int case2 = Table[r - 1][c] + del_cost;  // delete인 경우

            // no operation은 sub_cost가 0짜리인 substitution이라는 접근
            int sub_cost = (SS[r - 1] == TS[c - 1]) ? 0 : sub_cost_save;
            int case3 = Table[r - 1][c - 1] + sub_cost;  // substitute인 경우

            // case1, case2, case3 중 minimum
            Table[r][c] = min(case1, case2);
            Table[r][c] = min(Table[r][c], case3);
        }
    }

    // back tracing
    const int MAX = (1 << 30);
    int r = strlen(SS);
    int c = strlen(TS);
    int LENGTH = 0;
    while (!(r == 0 && c == 0)) {
        // no operation은 sub_cost가 0짜리인 substitution이라는 접근
        sub_cost = (SS[r - 1] == TS[c - 1]) ? 0 : sub_cost_save;

        int case1 = (c - 1 >= 0) ? Table[r][c - 1] + ins_cost : MAX;  // insertion case
        int case2 = (r - 1 >= 0) ? Table[r - 1][c] + del_cost : MAX;  // deletion case
        int case3 = (r - 1 >= 0 && c - 1 >= 0) ? Table[r - 1][c - 1] + sub_cost : MAX;  // deletion case

        enum eType {
            Insertion, Deletion, Substitution,
        };

        eType op = (eType)get_parent(case1, case2, case3, ins_cost, del_cost);
        switch (op) {
        case Insertion:
            c--;
            break;
        case Deletion:
            r--;
            break;
        case Substitution:
            r--;
            c--;
            break;
        default:
            /* invalid entry! */
            printf("invalide entry!\n");
        }
        LENGTH++;
    }
    LENGTH++;  // for null character

    // memory allocation
    *OP = new char[LENGTH];
    *SR = new char[LENGTH];
    *TR = new char[LENGTH];
    *Mem_Allocated = 3 * LENGTH * (int)sizeof(char);


    // get editing sequence
    r = strlen(SS);
    c = strlen(TS);
    int idx = LENGTH - 2;
    while (!(r == 0 && c == 0)) {
        // no operation은 sub_cost가 0짜리인 substitution이라는 접근
        int sub_cost = (SS[r - 1] == TS[c - 1]) ? 0 : sub_cost_save;

        int case1 = (c - 1 >= 0) ? Table[r][c - 1] + ins_cost : MAX;  // insertion case
        int case2 = (r - 1 >= 0) ? Table[r - 1][c] + del_cost : MAX;  // deletion case
        int case3 = (r - 1 >= 0 && c - 1 >= 0) ? Table[r - 1][c - 1] + sub_cost : MAX;  // deletion case

        enum eType {
            Insertion, Deletion, Substitution,
        };

        eType op = (eType)get_parent(case1, case2, case3, ins_cost, del_cost);
        switch (op) {
        case Insertion:
            (*SR)[idx] = '*';
            (*OP)[idx] = 'i';
            (*TR)[idx] = TS[c - 1];
            idx--;
            c--;
            break;
        case Deletion:
            (*SR)[idx] = SS[r - 1];
            (*OP)[idx] = 'd';
            (*TR)[idx] = '*';
            idx--;
            r--;
            break;
        case Substitution:
            (*SR)[idx] = SS[r - 1];
            (*OP)[idx] = (sub_cost == 0) ? '.' : 's';
            (*TR)[idx] = TS[c - 1];
            idx--;
            r--;
            c--;
            break;
        default:
            /* invalid entry */
            printf("invalid entry!\n");
        }
    }
    (*SR)[LENGTH - 1] = (*OP)[LENGTH - 1] = (*TR)[LENGTH - 1] = '\0';
}
