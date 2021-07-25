#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef int ElemType;

//非零元素结点结构
typedef struct OLNode {
    int row,col;
    ElemType value;
    struct OLNode *right,*down;
} OLNode, *OLink;
//十字链表结构
typedef struct {
    OLink *rowhead,*colhead;
    int rows,cols,nums;
} CrossList, *PCrossList;

/*
实现十字链表的普通初始化
L 为待初始化的十字链表, row 表示十字链表的行数, col 表示十字链表的列数
成功返回 true, 失败返回 false
*/
bool InitCrossList(PCrossList L, int row, int col) {
    L->colhead = (OLink*)malloc(row * sizeof(OLink));
    if (L->colhead == NULL) {
        return false;
    }
    for (int i = 0; i < row; i++) {
        L->colhead[i] = NULL;
    }
    L->rowhead = (OLink*)malloc(col * sizeof(OLink));
    if (L->rowhead == NULL) {
        free(L->colhead);
        return false;
    }
    for (int j = 0; j < col; j++) {
        L->rowhead[j] = NULL;
    }
    L->rows = row;
    L->cols = col;
    L->nums = 0;
    return true;
}
/*
向初始化后的十字链表中添加结点, 将数组 A 中非0元素保存到十字链表中
A 为 ElemType 类型数组中第一个元素的地址，元素的个数为 m×n 个，按行优先存储
即 A[0] 为十字链表第1行第1列的元素, A[1] 为第1行第2列的元素, A[n] 为第2行第1列的元素, A[n+1] 为第2行第2个元素
返回添加的非 0 元素的个数,若添加失败则返回 -1 
*/
int AddCrossList_Array(PCrossList L, const ElemType A[]) {
    if (L->nums != 0) {
        return -1;
    }
    OLNode *prow = NULL;    //只能访问right项
    OLNode *pcol = NULL;    //只能访问down项
    OLNode *p = NULL;
    int m = L->rows;
    int n = L->cols;

    //按行优先的顺序，将各个非零数据添加到链表中
    for (int i = 0; i < m; i++) {
        //填充第 j+1 行的数据
        for (int j = 0; j < n; j++) {
            if (A[i*n+j] != 0) {
                //首先分配相应内存，并将各项数据填入，注意行数与列数分别为j和i
                p = (OLNode*)malloc(sizeof(OLNode));
                p->row = i;
                p->col = j;
                p->down = NULL;
                p->right = NULL;
                p->value = A[i*n+j];
                //将该项与其同行同列的结点连接，并且L->nums加一
                if (L->rowhead[i] == NULL) {
                    L->rowhead[i] = p;
                }
                else {
                    prow = L->rowhead[i];
                    while (prow->right != NULL) {
                        prow = prow->right;
                    }
                    prow->right = p;
                }
                if (L->colhead[j] == NULL) {
                    L->colhead[j] = p;
                }
                else {
                    pcol = L->colhead[j];
                    while (pcol->down != NULL) {
                        pcol = pcol->down;
                    }
                    pcol->down = p;
                }
                L->nums++;
            }
        }
    }
    return L->nums;
}
/*
向初始化后的十字链表中添加结点, 将二维数组 A 中非0元素保存到十字链表中
A 为 ElemType 类型数组中第一个元素的地址，元素的个数为 m×n 个
即 A[0][0] 为十字链表第1行第1列的元素, A[0][1] 为第1行第2列的元素, A[1][0] 为第2行第1列的元素
返回添加的非 0 元素的个数,若添加失败则返回 -1 
*/
int AddCrossList_Matrix(PCrossList L, const ElemType A[L->rows][L->cols]) {
    if (L->nums != 0) {
        return -1;
    }
    OLNode *prow = NULL;    //只能访问right项
    OLNode *pcol = NULL;    //只能访问down项
    OLNode *p = NULL;
    int m = L->rows;
    int n = L->cols;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (A[i][j] != 0) {
                p = (OLNode*)malloc(sizeof(OLNode));
                p->row = i;
                p->col = j;
                p->down = NULL;
                p->right = NULL;
                p->value = A[i][j];
                if (L->rowhead[i] == NULL) {
                    L->rowhead[i] = p;
                }
                else {
                    prow = L->rowhead[i];
                    while (prow->right != NULL) {
                        prow = prow->right;
                    }
                    prow->right = p;
                }
                if (L->colhead[j] == NULL) {
                    L->colhead[j] = p;
                }
                else {
                    pcol = L->colhead[j];
                    while (pcol->down != NULL) {
                        pcol = pcol->down;
                    }
                    pcol->down = p;
                }
                L->nums++;
            }
        }
    }
}
/*
向十字链表 L 中的位置 (m,n) 加入元素 k
成功返回 1 ,替换返回 0, 失败返回 -1
*/
int AddCrossList(PCrossList L, ElemType k, int m, int n) {
    if (k == 0 || m < 0 || m >= L->rows || n < 0 || n >= L->cols) {
        return -1;
    }
    OLNode *prow = L->rowhead[m];    //只能访问right项
    OLNode *pcol = L->colhead[n];    //只能访问down项
    OLNode *s = NULL;
    s = prow;
    while (s != NULL) {
        if (s->col == n) {
            break;
        }
        s = s->right;
    }
    if (s != NULL) {
        s->value = k;
        return 0;
    }
    //添加新结点
    s = (OLNode*)malloc(sizeof(OLNode));
    s->value = k;
    s->row = m;
    s->col = n;
    s->right = s->down = NULL;
    if (L->rowhead[m] == NULL) {
        L->rowhead[m] = s;
    }
    else if (L->rowhead[m]->right->col > n) {
        s->right = L->rowhead[m]->right;
        L->rowhead[m] = s;
    }
    else {
        while (prow->right != NULL && prow->right->col < n) {
            prow = prow->right;
        }
        s->right = prow->right;
        prow->right = s;
    }
    if (L->colhead[n] == NULL) {
        L->colhead[n] = s;
    }
    else if (L->colhead[n]->down->row > m) {
        s->down = L->colhead[n]->down;
        L->colhead[n] = s;
    }
    else {
        while (pcol->down != NULL && pcol->down->row < m) {
            pcol = pcol->down;
        }
        s->down = pcol->down;
        pcol->down = s;
    }
    L->nums++;
    return 1;
}
/*
在十字链表 L 中删除所有元素 k
返回删除结点的个数
*/
int DeleteAllCrossList(PCrossList L, ElemType k) {
    int i = 0, count = 0;
    OLNode *prow = NULL;    //只能访问right项
    OLNode *pcol = NULL;    //只能访问down项
    OLNode *p = NULL;

    while (i < L->rows) {
        p = L->rowhead[i];
        while (p != NULL && p->value != k) {
            p = p->right;
        }
        if (p == NULL) {
            i++;
            continue;
        }
        prow = L->rowhead[p->row];
        pcol = L->colhead[p->col];
        if (L->rowhead[p->row] == p) {
            L->rowhead[p->row] = p->right;
        }
        else {
            while (prow->right != p) {
                prow = prow->right;
            }
            prow->right = p->right;
        }
        if (L->colhead[p->col] == p) {
            L->colhead[p->col] = p->down;
        }
        else {
            while (pcol->down != p) {
                pcol = pcol->down;
            }
            pcol->down = p->down;
        }
        L->nums--;
        free(p);
        count++;
    }
    return count;
}
/*
在十字链表 L 中删除位于 (m,n) 的元素
成功返回 1 ,失败返回 0 ,位置输入错误返回 -1
*/
int DeleteCrossList(PCrossList L, int m, int n) {
    if (m < 0 || m >= L->rows || n < 0 || n >= L->cols) {
        return -1;
    }
    OLNode *prow = L->rowhead[m];    //只能访问right项
    OLNode *pcol = L->colhead[n];    //只能访问down项
    if (prow == NULL || pcol == NULL) {
        return 0;
    }
    OLNode *p = NULL;
    p = prow;
    while (p != NULL) {
        if (p->col == n) {
            break;
        }
        p = p->right;
    }
    if (p == NULL) {
        return 0;
    }
    //p指向了要删除的结点
    if (L->rowhead[m] == p) {
        L->rowhead[m] = p->right;
    }
    else {
        while (prow->right != p) {
            prow = prow->right;
        }
        prow->right = p->right;
    }
    if (L->colhead[n] == p) {
        L->colhead[n] = p->down;
    }
    else {
        while (pcol->down != p) {
            pcol = pcol->down;
        }
        pcol->down = p->down;
    }
    L->nums--;
    free(p);
    return 1;
}
/*
销毁十字链表
*/
int DestoryCrossList(PCrossList *L) {
    OLNode *p, *pre;
    for (int i = 0; i < (*L)->rows; i++) {
        p = (*L)->rowhead[i];
        while (p != NULL) {
            pre = p;
            p = p->right;
            free(pre);
        }
    }
    free((*L)->colhead);
    free((*L)->rowhead);
    (*L)->cols = (*L)->rows = (*L)->nums = 0;
    *L = NULL;
    return 0;
}
/*
打印十字链表
*/
void PrintCrossList(const PCrossList L) {
    printf("#CrossList\n");
    if (L == NULL || L->nums == 0) {
        return;
    }
    printf("  |");
    for (int i = 0; i < L->cols; i++) {
        printf("%5d", i);
    }
    printf("\n");
    for (int i = 0; i < L->cols; i++) {
        printf("------");
    }
    printf("\n");
    OLNode *p = NULL;
    for (int i = 0; i < L->rows; i++) {
        printf("%2d|", i);
        p = L->rowhead[i];
        for (int j = 0; j < L->cols; j++) {
            if (p != NULL && p->col == j) {
                printf("%5d", p->value);
                p = p->right;
            }
            else {
                printf("    *");
            }
        }
        printf("\n");
    }
}

int main(void) {
    CrossList L;
    PCrossList pL = &L;
    InitCrossList(&L, 4, 5);
    int A[20] = {1,0,0,5,0,0,2,0,0,0,0,0,0,0,3,2,0,0,0,1};
    AddCrossList_Array(&L, A);
    AddCrossList(&L, 6, 2, 4);
    PrintCrossList(&L);
    DeleteAllCrossList(&L, 2);
    PrintCrossList(&L);
    DestoryCrossList(&pL);
    PrintCrossList(&L);
    return 0;
}