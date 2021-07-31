#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 50

typedef struct node {
    int val;
    struct node *left;
    struct node *right;
    struct node *parent;
    int height;//avl树中结点的高度
    int bf;//avl树结点的平衡因子
} Avlnode, *Avltree;

static int Max(int a, int b) {return a>b?a:b;}
//计算avl树中该结点的高度
static int GetHeight(Avlnode *p) {
    if (p == NULL) {
        return 0;
    }
    if (p->left != NULL && p->right != NULL) {
        return Max(p->left->height, p->right->height)+1;
    }
    else if (p->left == NULL && p->right == NULL) {
        return 1;
    }
    else if (p->left == NULL) {
        return p->right->height+1;
    }
    else {
        return p->left->height+1;
    }
}
//对以root结点为根的avl树的结点a进行一次左旋,返回旋转子树的根结点
static Avlnode *LeftRotate(Avltree *root, Avlnode *a) {
    Avlnode *b = a->right;
    Avlnode *fa = a->parent;
    a->right = b->left;
    if (b->left != NULL) {
        b->left->parent = a;
    }
    b->left = a;
    a->parent = b;
    if (fa == NULL) {
        *root = b;
    }
    else if (a == fa->left) {
        fa->left = b;
    }
    else {
        fa->right = b;
    }
    b->parent = fa;
    Avlnode *p = a;
    while (p != NULL) {
        p->height = GetHeight(p);
        p->bf = GetHeight(p->left)-GetHeight(p->right);
        p = p->parent;
    }
    return b;
}
//对以root结点为根的avl树的结点a进行一次右旋,返回旋转子树的根结点
static Avlnode *RightRotate(Avltree *root, Avlnode *a) {
    Avlnode *b = a->left;
    Avlnode *fa = a->parent;
    a->left = b->right;
    if (b->right != NULL) {
        b->right->parent = a;
    }
    b->right = a;
    a->parent = b;
    if (fa == NULL) {
        *root = b;
    }
    else if (a == fa->left) {
        fa->left = b;
    }
    else {
        fa->right = b;
    }
    b->parent = fa;
    Avlnode *p = a;
    while (p != NULL) {
        p->height = GetHeight(p);
        p->bf = GetHeight(p->left)-GetHeight(p->right);
        p = p->parent;
    }
    return b;
}
//********无论是哪种调整方式,旋转子树的高度都会减一********
/*
创建平衡二叉树
*/
void ConstructAvltree(Avltree *root) {
    *root = NULL;
}
/*
向根为 root 的平衡二叉树插入新元素 val
成功后返回新平衡二叉树根结点,失败返回NULL
*/
Avltree InsertAvltree(Avltree *root, int val) {
    Avlnode *s = (Avlnode*)malloc(sizeof(Avlnode));
    if (s == NULL) {
        return *root;
    }
    s->val = val;
    s->left = s->right = s->parent = NULL;
    s->height = 1;
    s->bf = 0;

    if (*root == NULL) {
        *root = s;
        return *root;
    }
    //查找s的插入位置fp
    //记录距s插入位置最近且平衡因子不为零的结点
    Avlnode *A = *root;//可能的失衡结点
    Avlnode *p = *root;//用于查找插入位置的指针
    Avlnode *FA = NULL;//结点A的父亲结点
    Avlnode *fp = NULL;//结点p的父亲结点(指向p指针前面一个结点)
    while (p != NULL) {
        if (p->bf != 0/*GetHeight(p->left) != GetHeight(p->right)*/) {
            A = p; FA = fp;
        }
        fp = p;
        if (val < p->val) {
            p = p->left;
        }
        else if (val > p->val) {
            p = p->right;
        }
        else {//插入结点已存在
            free(s);
            return NULL;
        }
    }
    //插入s
    if (val < fp->val) {
        fp->left = s;
    }
    else {
        fp->right = s;
    }
    s->parent = fp;
    //确定结点B,并修改A的平衡因子
    Avlnode *B = NULL;
    if (val < A->val) {
        B = A->left;
        A->bf++;
    }
    else {
        B = A->right;
        A->bf--;
    }
    //修改B到s路径上各结点的平衡因子(初值均为0)及结点高度
    p = B;
    while (p != s) {
        if (val < p->val) {
            p->bf++;
            p->height = GetHeight(p);
            p = p->left;
        }
        else {
            p->bf--;
            p->height = GetHeight(p);
            p = p->right;
        }
    }
    //修改fp到root路径上各结点的高度
    p = fp;
    while (p != NULL) {
        p->height = GetHeight(p);
        p = p->parent;
    }
    //判断失衡类型并分类处理
    if (A->bf == 2 && B->bf == 1/*GetHeight(A->left)-GetHeight(A->right)==2&&GetHeight(B->left)-GetHeight(B->right)==1*/) {
        RightRotate(root, A);
    }
    else if (A->bf == 2 && B->bf == -1/*GetHeight(A->left)-GetHeight(A->right)==2&&GetHeight(B->left)-GetHeight(B->right)==-1*/) {
        LeftRotate(root, B);
        RightRotate(root, A);
    }
    else if (A->bf == -2 && B->bf == 1/*GetHeight(A->left)-GetHeight(A->right)==-2&&GetHeight(B->left)-GetHeight(B->right)==1*/) {
        RightRotate(root, B);
        LeftRotate(root, A);
    }
    else if (A->bf == -2 && B->bf == -1/*(A->left)-GetHeight(A->right)==-2&&GetHeight(B->left)-GetHeight(B->right)==-1*/) {
        LeftRotate(root, A);
    }
    return *root;
}

static Avlnode* SubstituteNode(Avlnode *s) {
    if (s->left == NULL && s->right == NULL) {
        return s;
    }
    else if (s->left == NULL) {
        // right sub tree's height can't be larger than 1
        s->val = s->right->val;
        return s->right;
        // return SubstituteNode(s->right);
    }
    else if (s->right == NULL) {
        s->val = s->left->val;
        return s->left;
    }
    else if (s->bf == 1) {
        Avlnode *p = s->left;
        Avlnode *fp = s;
        while (p != NULL) {
            fp = p;
            p = p->right;
        }
        // swap s and fp
        s->val = fp->val;
        return SubstituteNode(fp);
    }
    else {
        Avlnode *p = s->right;
        Avlnode *fp = s;
        while (p != NULL) {
            fp = p;
            p = p->left;
        }
        // swap s and fp
        s->val = fp->val;
        return SubstituteNode(fp);
    }
}
/*
在根为 root 的平衡二叉树中删除元素 val
成功后返回新平衡二叉树根节点,失败返回NULL
*/
Avltree DeleteAvltree(Avltree *root, int val) {
    if (*root == NULL) {
        return NULL;
    }
    Avlnode *p = *root;//用于查找删除位置的指针
    Avlnode *s = NULL;
    Avlnode *fs = NULL;//被删除结点的父结点指针
    while (p != NULL) {
        if (val < p->val) {
            p = p->left;
        }
        else if (val > p->val) {
            p = p->right;
        }
        else {
            break;
        }
    }
    if (p == NULL) {
        return NULL;
    }
    if (p == *root && (*root)->left == NULL && (*root)->right == NULL) {
        free(p);
        *root = NULL;
        return *root;
    }
    /*****
     * 分三种情况讨论
     * 该结点无孩子，则直接删除该结点
     * 该结点有且只有一个孩子，则连接之后再删除结点
     * 该结点既有左孩子又有右孩子，则再分为以下几种情况
     *    该结点左子树高度大于右子树，让左子树的最右结点代替当前结点
     *    该结点右子树高度大于左子树，让右子树的最左结点代替当前结点
     * 最后在层层回溯，逐层调整
    */
    p = SubstituteNode(p);
    Avlnode *fp = p->parent;
    if (p == fp->left) {
        fp->left = NULL;
    }
    else {
        fp->right = NULL;
    }
    free(p);
    //此时fs为被删除结点的父结点
    //层层回溯修改结点高度
    s = fp;
    while (s != NULL) {
        s->height = GetHeight(s);
        s->bf = GetHeight(s->left) - GetHeight(s->right);
        s = s->parent;
    }
    //层层回溯调整平衡
    s = fp;
    while (s != NULL) {
        if (s->bf == 2) { /*GetHeight(s->left) - GetHeight(s->right) == 2*/
            p = s->left;
            if (p->bf >= 0) { /*GetHeight(p->left) >= GetHeight(p->right)*/
                s = RightRotate(root, s);
            }
            else {
                LeftRotate(root, p);
                s = RightRotate(root, s);
            }
        }
        else if (s->bf == -2) { /*GetHeight(s->left) - GetHeight(s->right) == -2*/
            p = s->right;
            if (p->bf <= 0) { /*GetHeight(p->right) >= GetHeight(p->left)*/
                s = LeftRotate(root, s);
            }
            else {
                RightRotate(root, p);
                s = LeftRotate(root, s);
            }
        }
        /*
        必须逐层循环直至根结点的原因:(纯数字意为高为x的任意平衡子树)
        node 20 ,height 5
          node 10 ,height 4
            node 5 ,height 3
              node 2 ,height 1
              node 7 ,height 2
                node 8 ,height 1
            node 15 ,height 3
              2
              3
          node 30 ,height 4
            4
            5
        上面这个平衡树删去结点8,每一层都需要调整

        node 20 ,height 6
          node 10 ,height 4
            node 5 ,height 3
              node 3 ,height 2
                node 2 ,height 1
              node 7 ,height 1
            2
          5
        上面这个平衡树删除结点7,调整一层后有一层不调整再后一层要调整
        */
        s = s->parent;
    }
    return *root;
}
/*
销毁根为 root 的平衡二叉树
成功返回 true, 失败返回 false
*/
bool DestoryAvltree(Avltree *root) {
    if (root == NULL || *root == NULL) {
        return false;
    }
    DestoryAvltree(&((*root)->left));
    DestoryAvltree(&((*root)->right));
    free(*root);
    *root = NULL;
    return true;
}
/*
在根为 root 的平衡二叉树中查找元素 val
成功返回寻找的二叉树结点,失败返回 NULL
*/
Avlnode *SearchAvltree(Avltree root, int val) {
    if (root == NULL) {
        return NULL;
    }
    else if (val < root->val) {
        return SearchAvltree(root->left, val);
    }
    else if (val > root->val) {
        return SearchAvltree(root->right, val);
    }
    else {
        return root;
    }
}
/*
打印平衡二叉树辅助函数
height 是原二叉树的高度, root 是当前即将打印的(子)二叉树的根结点
*/
static void PrintAvltreeInRank(Avltree root, int height) {
    if (root == NULL) {
        return;
    }
    printf("%*s", 4*height, "");
    printf("node: (%d)\n", root->val);
    PrintAvltreeInRank(root->right, height+1);
    if ((root->left == NULL) ^ (root->right == NULL)) {
        printf("%*s--\n", 4*(height+1), "");
    }
    PrintAvltreeInRank(root->left, height+1);
}
/*
打印平衡二叉树
root 是二叉树的根结点
*/
void PrintAvltree(Avltree root) {
    printf("# Avltree\n");
    PrintAvltreeInRank(root, 0);
}


// 合法则返回子树高度，否则返回-1
static int check(Avltree root) {
    if (root == NULL) {
        return 0;
    }
    int l = check(root->left);
    int r = check(root->right);
    if (l == -1 || r == -1 || abs(l-r) > 1) {
        return -1;
    }
    else {
        return Max(l, r)+1;
    }
}

void test(void) {
    Avltree root;
    ConstructAvltree(&root);

    static bool rinit = false;
    if (rinit == false) {
        srand(time(NULL));
        rinit = true;
    }

    int values[SIZE] = {0};
    int deletes[SIZE] = {0};
    int randlist[SIZE*2] = {0};
    // printf("Insert list: ");
    for (int i = 0, j = 0; i < SIZE; ++i) {
        values[i] = rand()%(SIZE*10-1)+1;
        deletes[i] = values[i];
        randlist[i+j] = values[i];
        // printf("%d ", values[i]);
        InsertAvltree(&root, values[i]);
        if (check(root) < 0) {
            printf("===== error =====\n");
            printf("insert list:\n");
            for (int k = 0; k <= i+j; ++k) {
                printf("%d ", randlist[k]);
            }
            printf("\n");
            break;
        }
        int rd = rand() % (3);
        if (rd == 0) { // delete nodes randomly while inserting
            rd = rand() % (i+1);
            ++j;
            randlist[i+j] = -values[rd];
            DeleteAvltree(&root, values[rd]);
            if (check(root) < 0) {
                printf("===== error =====\n");
                printf("insert list:\n");
                for (int k = 0; k <= i+j; ++k) {
                    printf("%d ", randlist[k]);
                }
                printf("\n");
            }
        }
    }
    // printf("\n");

    for (int i = SIZE-1; i > 0; --i) {
        int j = rand() % (i+1);
        int t = deletes[i];
        deletes[i] = deletes[j];
        deletes[j] = t;
    }

    // PrintAvltree(root);
    // printf("Delete list: ");
    for (int i = 0; i < SIZE; ++i) {
        // printf("%d ", deletes[i]);
        // fflush(stdout);
        DeleteAvltree(&root, deletes[i]);
        // PrintAvltree(root);
        if (check(root) < 0) {
            printf("===== error =====\n");
            printf("insert list:\n");
            for (int j = 0; j < SIZE*2; ++j) {
                printf("%d ", values[j]);
            }
            printf("\ndelete list:\n");
            for (int j = 0; j <= i; ++j) {
                printf("%d ", deletes[j]);
            }
            printf("\n");
            break;
        }
    }
    // printf("\n");

    // printf("insert list:\n");
    // for (int i = 0; i < SIZE*2; ++i) {
    //     printf("%d ", randlist[i]);
    // }
    // printf("\ndelete list:\n");
    // for (int i = 0; i < SIZE; ++i) {
    //     printf("%d ", deletes[i]);
    // }
    // printf("\n");

    DestoryAvltree(&root);
}

void demo() {
    Avltree root;
    ConstructAvltree(&root);

    int n;
    while (scanf("%d", &n) != -1) {
        if (n == 0) {
            // break;
            PrintAvltree(root);
        }
        else if (n > 0) {
            InsertAvltree(&root, n);
        }
        else {
            DeleteAvltree(&root, -n);
        }
        // PrintAvltree(root);
        if (check(root) < 0) {
            printf("===== error =====\n");
            break;
        }
    }

    DestoryAvltree(&root);
}


int main(void) {
    for (int i = 0; i < 100; ++i) test();
    // test();
    // demo();
    return 0;
}
