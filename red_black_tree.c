#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

typedef enum {
    RED,
    BLACK
} Color;
typedef struct node {
    int val;
    Color color;
    struct node *left;
    struct node *right;
    struct node *parent;
} RBnode, *RBtree;

#define SIZE 50

/*
Root nodes must be black.
The two child nodes of red node must be black.
The path from any node to each leaf node contains the same number of black nodes.
*/

// black node or null node ?
bool isblacknul(RBnode *p) {
    if (p == NULL || p->color == BLACK) {
        return true;
    }
    else {
        return false;
    }
}
// red node ?
bool isred(RBnode *p) {
    if (p != NULL && p->color == RED) {
        return true;
    }
    else {
        return false;
    }
}
// set color
void coloring(RBnode *p, Color c) {
    if (p != NULL) {
        p->color = c;
    }
}
//对以root结点为根的红黑树的结点a进行一次左旋,返回旋转子树的根结点
static RBnode* LeftRotate(RBtree *root, RBnode *a) {
    RBnode *b = a->right;
    RBnode *fa = a->parent;
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
    // RBnode *p = a;
    return b;
}
//对以root结点为根的红黑树的结点a进行一次右旋,返回旋转子树的根结点
static RBnode* RightRotate(RBtree *root, RBnode *a) {
    RBnode *b = a->left;
    RBnode *fa = a->parent;
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
    return b;
}
static RBnode* Discolor(RBnode *a) {
    if (a->color == RED) {
        a->color = BLACK;
    }
    else {
        a->color = RED;
    }
    return a;
}

void ConstructRBtree(RBtree *root) {
    *root = NULL;
}
// Node has been inserted into the RBtree
static RBnode* InsertedCope(RBtree *root, RBnode *p) {
    RBnode *fp = p->parent;
    if (fp == NULL) {
        Discolor(p);
        return *root;
    }
    if (fp->color == BLACK) {
        return p;
    }
    // fp->color == RED
    // which means that fp can't be a node without parent
    RBnode *gp = fp->parent; // grandparent node
    // gp != NULL && gp->color == BLACK
    RBnode *up = (fp==gp->left?gp->right:gp->left); // uncle node
    if (up != NULL && up->color == RED) {
        Discolor(fp);
        Discolor(up);
        Discolor(gp);
        InsertedCope(root, gp);
    }
    else {
        //up == NULL || up is a leaf node
        if (fp == gp->left) {
            if (p == fp->left) {
                Discolor(fp);
                Discolor(gp);
                RightRotate(root, gp);
            }
            else {
                 p = fp;
                fp = LeftRotate(root, fp);
                // InsertedCope(root, p);
                Discolor(fp);
                Discolor(gp);
                RightRotate(root, gp);
            }
        }
        else {
            if (p == fp->right) {
                Discolor(fp);
                Discolor(gp);
                LeftRotate(root, gp);
            }
            else {
                p = fp;
                fp = RightRotate(root, fp);
                // InsertedCope(root, p);
                Discolor(fp);
                Discolor(gp);
                LeftRotate(root, gp);
            }
        }
    }
    return p;
}
/*
向根为 root 的红黑树插入新元素 val
成功后返回新红黑树根结点,失败返回NULL
*/
RBtree InsertRBtree(RBtree *root, int val) {
    RBnode *s = (RBnode*)malloc(sizeof(RBnode));
    if (s == NULL) {
        return NULL;
    }
    s->val = val;
    s->left = s->right = s->parent = NULL;
    s->color = RED;

    if (*root == NULL) {
        s->color = BLACK;
        *root = s;
        return *root;
    }

    RBnode *p = *root;
    RBnode *fp = NULL;
    while (p != NULL) {
        fp = p;
        if (val < p->val) {
            p = p->left;
        }
        else if (val > p->val) {
            p = p->right;
        }
        else {
            free(s);
            return NULL;
        }
    }
    if (val < fp->val) {
        fp->left = s;
    }
    else {
        fp->right = s;
    }
    s->parent = fp;

    InsertedCope(root, s);

    return *root;
}

// find the substituted node of s
static RBnode* SubstituteNode(RBnode *s) {
    if (s->left == NULL && s->right == NULL) {
        return s;
    }
    else if (s->left == NULL) {
        // swap s and s->left
        s->val = s->right->val;
        return SubstituteNode(s->right);
    }
    else if (s->right == NULL) {
        // swap s and s->right
        s->val = s->left->val;
        return SubstituteNode(s->left);
    }
    else {
        RBnode *p = s->right;
        RBnode *fp = s;
        while (p != NULL) {
            fp = p;
            p = p->left;
        }
        // swap s and fp
        s->val = fp->val;
        return SubstituteNode(fp);
    }
}
// Node has been substituted to a leaf node
static RBnode* DeleteCope(RBtree *root, RBnode *p, int h) {
    if (p == NULL) {
        return NULL;
    }
    RBnode *fp = p->parent;
    if (fp == NULL) {
        return NULL;
    }
    if (isred(p)) {
        if (p == fp->left) {
            fp->left = NULL;
        }
        else {
            fp->right = NULL;
        }
        return p;
    }
    // substituted node is black
    // p is black, so its parent node must have 2 child nodes
    if (p == fp->left) {
        RBnode *bp = fp->right;
        if (isred(bp)) { // bp->both->color == BLACK
            coloring(bp, BLACK);
            coloring(fp, RED);
            LeftRotate(root, fp);
            DeleteCope(root, p, 1);
        }
        else { // bp->color == BLACK
            if (isblacknul(bp->left) && isblacknul(bp->right)) {
                // need to recurse
                if (fp->color == RED) {
                    coloring(bp, RED);
                    coloring(fp, BLACK);
                    // LeftRotate(root, fp);
                }
                else {
                    coloring(bp, RED);
                    DeleteCope(root, fp, 1);
                }
            }
            else if (isred(bp->right)) {
                coloring(bp, fp->color);
                coloring(fp, BLACK);
                coloring(bp->right, BLACK);
                LeftRotate(root, fp);
            }
            else { //
                coloring(bp, RED);
                coloring(bp->left, BLACK);
                bp = RightRotate(root, bp);
                coloring(bp, fp->color);
                coloring(fp, BLACK);
                coloring(bp->right, BLACK);
                LeftRotate(root, fp);
            }
        }
        if (h == 0) {
            p->parent->left = NULL;
        }
    }
    else { // p == fp->right
        RBnode *bp = fp->left;
        if (isred(bp)) {
            coloring(bp, BLACK);
            coloring(fp, RED);
            RightRotate(root, fp);
            DeleteCope(root, p, 1);
        }
        else {
            if (isblacknul(bp->left) && isblacknul(bp->right)) {
                if (fp->color == RED) {
                    coloring(bp, RED);
                    coloring(fp, BLACK);
                    // RightRotate(root, fp);
                }
                else {
                    bp->color = RED;
                    DeleteCope(root, fp, 1);
                }
            }
            else if (isred(bp->left)) {
                coloring(bp, fp->color);
                coloring(fp, BLACK);
                coloring(bp->left, BLACK);
                RightRotate(root, fp);   
            }
            else { //
                coloring(bp, RED);
                coloring(bp->right, BLACK);
                bp = LeftRotate(root, bp);
                coloring(bp, fp->color);
                coloring(fp, BLACK);
                coloring(bp->left, BLACK);
                RightRotate(root, fp);
            }
        }
        if (h == 0) {
            p->parent->right = NULL;
        }
    } 

    return p;
}
/*
在根为 root 的红黑树中删除元素 val
成功后返回新红黑树根节点,失败返回NULL
*/
RBtree DeleteRBtree(RBtree *root, int val) {
    if (*root == NULL) {
        return NULL;
    }
    RBnode *s = *root; // delete node
    RBnode *fs = NULL; // delete node's parent
    while (s != NULL) {
        fs = s;
        if (val < s->val) {
            s = s->left;
        }
        else if (val > s->val) {
            s = s->right;
        }
        else {
            break;
        }
    }
    if (s == NULL) {
        return NULL;
    }
    if (s == *root && (*root)->left == NULL && (*root)->right == NULL) {
        free(s);
        *root = NULL;
        return *root;
    }

    RBnode *p = SubstituteNode(s); // substituted node
    // printf("== (%d)'s %d ==\n", val, p->val);

    free(DeleteCope(root, p, 0));

    return *root;
}

bool DestoryRBtree(RBtree *root) {
    if (root == NULL || *root == NULL) {
        return false;
    }
    DestoryRBtree(&((*root)->left));
    DestoryRBtree(&((*root)->right));
    free(*root);
    *root = NULL;
    return true;
}

static void PrintRBtreeInRank(RBtree root, int height) {
    if (root == NULL) {
        return;
    }
    printf("%*s", 4*height, "");
    printf(root->color==RED?"node: (%d)\n":"node: [%d]\n", root->val);
    PrintRBtreeInRank(root->right, height+1);
    if ((root->left == NULL) ^ (root->right == NULL)) {
        printf("%*s--\n", 4*(height+1), "");
    }
    PrintRBtreeInRank(root->left, height+1);
}
void PrintRBtree(RBtree root) {
    printf("# RBtree\n");
    PrintRBtreeInRank(root, 0);
}

/*
检测红黑树的合法性
主要检测一下方面：1. 红结点的孩子一定是黑结点 2. 从根结点到各null结点的路径上黑结点数量相等
*/
static int _check(RBtree root, int bn) {
    if (root == NULL) {
        return bn;
    }
    if (root->color == RED) {
        if ((root->left != NULL && root->left->color == RED) || (root->right != NULL && root->right->color == RED)) {
            return -1;
        }
    }
    int _bn = bn + root->color==BLACK ? 1 : 0;
    int l = _check(root->left, _bn);
    int r = _check(root->right, _bn);
    if (l == -1 || r == -1 || l != r) {
        return -1;
    }
    else {
        return l;
    }
}
int check(RBtree root) {
    return _check(root, 0);
}

void test(void) {
    RBtree root;
    ConstructRBtree(&root);

    static bool rinit = false;
    if (rinit == false) {
        srand(time(NULL));
        rinit = true;
    }

    int values[SIZE] = {0};
    int randlist[SIZE*2] = {0};
    int deletes[SIZE] = {0};
    for (int i = 0, j = 0; i < SIZE; ++i) {
        values[i] = rand()%(SIZE*10-1)+1;
        deletes[i] = values[i];
        randlist[i+j] = values[i];
        InsertRBtree(&root, values[i]);
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
            DeleteRBtree(&root, values[rd]);
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

    for (int i = SIZE-1; i > 0; --i) {
        int j = rand() % (i+1);
        int t = deletes[i];
        deletes[i] = deletes[j];
        deletes[j] = t;
    }

    // PrintRBtree(root);

    for (int i = 0; i < SIZE; ++i) {
        DeleteRBtree(&root, deletes[i]);
        // PrintRBtree(root);
        if (check(root) < 0) {
            printf("===== error =====\n");
            printf("insert list:\n");
            for (int j = 0; j < SIZE*2; ++j) {
                printf("%d ", randlist[j]);
            }
            printf("\ndelete list:\n");
            for (int j = 0; j <= i; ++j) {
                printf("%d ", deletes[j]);
            }
            printf("\n");
            break;
        }
    }

    // printf("insert list:\n");
    // for (int i = 0; i < SIZE*2; ++i) {
    //     printf("%d ", randlist[i]);
    // }
    // printf("\ndelete list:\n");
    // for (int i = 0; i < SIZE; ++i) {
    //     printf("%d ", deletes[i]);
    // }
    // printf("\n");

    DestoryRBtree(&root);
}

void demo() {
    RBtree root;
    ConstructRBtree(&root);

    int n;
    while (scanf("%d", &n) != -1) {
        if (n == 0) {
            break;
        }
        else if (n > 0) {
            InsertRBtree(&root, n);
        }
        else {
            DeleteRBtree(&root, -n);
        }
        PrintRBtree(root);
        if (check(root) < 0) {
            printf("===== error =====\n");
            break;
        }
    }

    DestoryRBtree(&root);
}

int main(void) {
    for (int i = 0; i < 100; ++i) test();
    // demo();
    return 0;
}