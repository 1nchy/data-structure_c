#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/*
根结点至少有两棵子树
树中每个结点最多有 MAX_DEGREE 棵子树
除根结点外的所有非叶结点至少有 MIN_DEGREE 棵子树
for each internal node with k children must have k keywords
the value of index equals the largest key in the corresponding subtree
所有叶结点到根结点的高度相同
叶结点之间按关键字大小顺序用指针相连
*/

#define SIZE 100

#define MAX_DEGREE 3
// #define MIN_DEGREE ((int)ceil((float)MAX_DEGREE/(2)))
#define MIN_DEGREE (MAX_DEGREE+1)/2
// ((int)ceil((float)4/2)) == 2

typedef int KeyType;

typedef enum NodeType {
    INTERNAL, LEAF
} NodeType;

typedef struct MBxnode {
    int keynum;
    KeyType key[MAX_DEGREE+1]; // key[0] stores data
    struct MBxnode *parent;
    struct MBxnode *children[MAX_DEGREE+1];
    NodeType type;
    struct MBxnode *prev;
    struct MBxnode *next;
} MBxnode, *MBxtree;

void ConstructMBxtree(MBxtree *root) {
    *root = NULL;
}

// find and return the largest index of key which \le k in *mbxt 
static int _Search(MBxnode *mbxt, KeyType k) {
    int i = 0;
    while (i<mbxt->keynum && mbxt->key[i]<=k) i++;
    return i-1;
}
/*
find k in the MBxtree root
if success, put the node's pointer into np and index into pos, return true
otherwise, put the pointer of the node whose range contains k into np and index into ps, return false
*/
static bool Search(MBxtree root, KeyType k, MBxnode **np, int *pos) {
    MBxnode *p = root, *fp = NULL;
    bool found = false;
    int i = 0;
    while (p != NULL && !found) {
        i = _Search(p, k);
        if (i >= 0 && p->key[i] == k && p->type == LEAF) {
            found = true;
        }
        else if (i == p->keynum-1 || (i >= 0 && p->key[i] == k)) {
            fp = p;
            p = p->children[i];
        }
        else { // k is larger than any key in p
            fp = p;
            p = p->children[i+1];
        }
    }
    *pos = i;
    if (found) {
        *np = p;
        return true;
    }
    else {
        *np = fp;
        return false;
    }
}

int Locate(MBxnode *fp, MBxnode *p) {
    for (int i = 0; i < fp->keynum; ++i) {
        if (fp->children[i] == p) {
            return i;
        }
    }
    return -1;
}

// maintain the index
static MBxnode* MaintainIndex(MBxnode *p) {
    MBxnode *fp = p->parent;
    while (fp != NULL) {
        int i = Locate(fp, p);
        // i must \ge 0
        if (i < 0 || fp->key[i] == p->key[p->keynum-1]) {
            break;
        }
        else {
            fp->key[i] = p->key[p->keynum-1];
        }
        p = fp;
        fp = fp->parent;
    }
    return fp;
}

static MBxnode* Split(MBxnode *old) {
    int n = MAX_DEGREE-MIN_DEGREE;
    MBxnode *new = (MBxnode*)malloc(sizeof(MBxnode));
    if (new == NULL) {
        return NULL;
    }
    memset(new, 0, sizeof(MBxnode));
    new->keynum = n+1;
    new->parent = old->parent;
    for (int i = 0; i <= n; ++i) {
        new->key[i] = old->key[MIN_DEGREE+i];
        new->children[i] = old->children[MIN_DEGREE+i];
        if (new->children[i] != NULL) {
            new->children[i]->parent = new;
        }
        old->children[MIN_DEGREE+i] = NULL;
    }
    old->keynum = MIN_DEGREE;
    new->type = old->type;

    // if old is a leaf node, we need to maintain the linked leaf list
    if (old->type == LEAF) {
        new->next = old->next;
        if (old->next != NULL) {
            new->next->prev = new;
        }
        new->prev = old;
        old->next = new;
    }

    return new;
}
/*
insert k into node mbxt
mbxt[ipos+1] = k; mbxt->children[ipos+1] = rp;
*/
static void Insert(MBxnode *mbxt, int ipos, KeyType k, MBxnode *rp) {
    for (int i = mbxt->keynum; i > ipos+1; --i) {
        mbxt->key[i] = mbxt->key[i-1];
        mbxt->children[i] = mbxt->children[i-1];
    }
    mbxt->key[ipos+1] = k;
    // if rp == NULL, mbxt->type == LEAF
    // else, rp is a node by spliting, so ipos can't be larger than mbxt->keynum-1
    if (rp != NULL) {
        mbxt->children[ipos+2] = rp;
    }
    ++(mbxt->keynum);
}

MBxtree InsertMBxtree(MBxtree *root, KeyType k) {
    if (*root == NULL) {
        *root = (MBxtree)malloc(sizeof(MBxnode));
        if (*root == NULL) {
            return NULL;
        }
        memset(*root, 0, sizeof(MBxnode));
        (*root)->key[0] = k;
        (*root)->keynum = 1;
        (*root)->parent = NULL;
        (*root)->children[0] = NULL;
        // (*root)->type = INTERNAL;
        (*root)->type = LEAF;
        return *root;
    }
    int i = 0;
    KeyType x = k;
    MBxnode *q = NULL, *aq = NULL;
    if (Search(*root, k, &q, &i)) {
        return NULL;
    }
    bool finished = false;
    while (q != NULL && !finished) {
        Insert(q, i, x, aq);
        if (q->keynum <= MAX_DEGREE) {
            // maintain the index
            MaintainIndex(q);

            finished = true;
            break;
        }
        aq = Split(q);
        x = q->key[MIN_DEGREE-1];
        q = q->parent;
        if (q != NULL) {
            i = _Search(q, x);
        }
    }
    if (!finished) { // split the root node
        MBxnode *new = (MBxnode*)malloc(sizeof(MBxnode));
        memset(new, 0, sizeof(MBxnode));
        new->keynum = 2;
        new->type = INTERNAL;
        new->parent = NULL;
        new->key[0] = x;
        new->children[0] = *root;
        (*root)->parent = new;
        new->key[1] = aq->key[aq->keynum-1];
        new->children[1] = aq;
        aq->parent = new;
        // maintain the linked leaf list
        if (aq->type == LEAF) {
            (*root)->prev = NULL;
            (*root)->next = aq;
            aq->prev = (*root);
            aq->next = NULL;
        }
        *root = new;
    }

    return *root;
}


static MBxnode* Merge(MBxnode *fn, int u_pos, int v_pos) {
    // merge v into u
    MBxnode *u = fn->children[u_pos];
    MBxnode *v = fn->children[v_pos];
    // move key
    for (int i = u->keynum, j = 0; j < v->keynum; ++i, ++j) {
        u->key[i] = v->key[j];
    }
    // move children
    for (int i = u->keynum, j = 0; j < v->keynum; ++i, ++j) {
        u->children[i] = v->children[j];
        if (u->children[i] != NULL) {
            u->children[i]->parent = u;
        }
    }
    u->keynum += v->keynum;
    // maintain linked leaf list
    if (u->type == LEAF) {
        u->next = v->next;
        if (v->next != NULL) {
            u->next->prev = u;
        }
    }

    free(v);
    v = NULL;
    fn->children[v_pos] = NULL;

    // shrink fn
    for (int i = u_pos+1; i < fn->keynum; ++i) {
        fn->key[i-1] = fn->key[i];
        fn->children[i] = fn->children[i+1];
    }
    fn->children[fn->keynum-1] = NULL;
    --(fn->keynum);
    // maintain the index
    MaintainIndex(u);

    return fn;
}
static MBxnode* DeleteRecursively(MBxtree *root, MBxnode *s) {
    MBxnode *fs = s->parent;
    if (fs == NULL) {
        return NULL;
    }
    MBxnode *p = NULL;
    int s_pos = Locate(fs, s), p_pos = -1;
    // find a node whose keynum is larger than MIN_DEGREE from adjecent nodes
    if (s_pos != 0 && fs->children[s_pos-1]->keynum > MIN_DEGREE) {
        p_pos = s_pos - 1;
        p = fs->children[p_pos];
    }
    else if (s_pos != fs->keynum-1 && fs->children[s_pos+1]->keynum > MIN_DEGREE) {
        p_pos = s_pos + 1;
        p = fs->children[p_pos];
    }
    
    if (p_pos != -1) {
        // p has extra keys, so p give a key to s
        if (p_pos < s_pos) {
            for (int i = s->keynum; i > 0; --i) {
                s->key[i] = s->key[i-1];
            }
            s->key[0] = p->key[p->keynum-1];

            // if !leaf
            for (int i = s->keynum; i > 0; --i) {
                s->children[i] = s->children[i-1];
            }
            s->children[0] = p->children[p->keynum-1];
            if (s->children[0] != NULL) {
                s->children[0]->parent = s;
            }
            p->children[p->keynum] = NULL;

            ++(s->keynum);
            --(p->keynum);
            MaintainIndex(p);
        }
        else {
            s->key[s->keynum] = p->key[0];
            for (int i = 0; i < p->keynum; ++i) {
                p->key[i] = p->key[i+1];
            }
            
            // if !leaf
            s->children[s->keynum] = p->children[0];
            if (p->children[0] != NULL) {
                s->children[s->keynum]->parent = s;
            }
            for (int i = 0; i < p->keynum; ++i) {
                p->children[i] = p->children[i+1];
            }
            p->children[p->keynum-1] = NULL;
            ++(s->keynum);
            --(p->keynum);
            MaintainIndex(s);
        }
    }
    else {
        // merge adjacent child node
        if (s_pos != 0) {
            // merge s into p
            s = fs->children[s_pos-1];
            fs = Merge(fs, s_pos-1, s_pos);
        }
        else {
            fs = Merge(fs, s_pos, s_pos+1);
        }
        if (fs->parent == NULL) {
            if (fs->keynum == 1) {
                s->parent = NULL;
                free(fs);
                fs = NULL;
                *root = s;
                return s;
            }
            return fs;
        }
        else {
            if (fs->keynum < MIN_DEGREE) {
                DeleteRecursively(root, fs);
            }
            return fs;
        }
    }
    return fs;
}
MBxtree DeleteMBxtree(MBxtree *root, KeyType k) {
    if (*root == NULL) {
        return NULL;
    }
    int pos = 0;
    MBxnode *s = NULL;
    if (!Search(*root, k, &s, &pos)) {
        return NULL;
    }

    for (int i = pos; i < s->keynum; ++i) {
        s->key[i] = s->key[i+1];
    }
    --(s->keynum);
    // maintain the index
    MaintainIndex(s);

    if (s->keynum >= MIN_DEGREE) {
        return *root;
    }
    DeleteRecursively(root, s);
    return *root;
}
bool DestoryMBxtree(MBxtree *root) {
    if (root == NULL || *root == NULL) {
        return false;
    }
    for (int i = 0; i < (*root)->keynum; ++i) {
        DestoryMBxtree(&((*root)->children[i]));
    }
    free(*root);
    *root = NULL;
    return true;
}

static void PrintMBxtreeInRank(MBxtree root, int height) {
    if (root == NULL) {
        return ;
    }
    printf("%*s---\n", 4*height, "");
    for (int i = root->keynum-1; i >= 0; --i) {
        printf("%*snode(%d): (%d)\n", 4*height, "", i, root->key[i]);
        PrintMBxtreeInRank(root->children[i], height+1);
    }
    printf("%*s---\n", 4*height, "");
}
void PrintMBxtree(MBxtree root) {
    printf("# MBxtree\n");
    PrintMBxtreeInRank(root, 0);
}



int check(MBxtree root) {
    static MBxnode *lastleaf = NULL;
    if (root == NULL) {
        return 0;
    }
    if (root->parent != NULL && root->keynum < MIN_DEGREE) {
        return -1;
    }
    int kid0 = check(root->children[0]);
    if (kid0 < 0) {
        return kid0;
    }
    if (root->children[0] != NULL && root->key[0] != root->children[0]->key[root->children[0]->keynum-1]) {
        return -6;
    }
    for (int i = 1; i < root->keynum; ++i) {
        if (kid0 != check(root->children[i])) {
            return -2;
        }
        if (root->key[i-1] >= root->key[i]) {
            return -4;
        }
    }
    if (kid0 != 0 && root->type == LEAF) {
        return -8;
    }
    if (kid0 == 0 && root->type != LEAF) {
        return -12;
    }

    if (root->type == LEAF && root->parent != NULL) {
        // check if the leaf nodes have been linked in order
        if (kid0 != 0) {
            return -16;
        }
        if (root->prev == lastleaf) {
            if (lastleaf != NULL && lastleaf->key[lastleaf->keynum-1] >= root->key[0]) {
                return -24;
            }
            lastleaf = root;
        }
        else {
            return -32;
        }
    }

    if (root->parent == NULL) {
        lastleaf = NULL;
    }

    return kid0 + 1;
}

void test(void) {
    MBxtree root;
    ConstructMBxtree(&root);

    static bool rinit = false;
    if (rinit == false) {
        srand(time(NULL));
        rinit = true;
    }

    int values[SIZE] = {0};
    int deletes[SIZE] = {0};
    int randlist[SIZE*2] = {0};

    int c;

    // printf("insert list:\n");

    for (int i = 0, j = 0; i < SIZE; ++i) {
        values[i] = rand()%(SIZE*10-1)+1;
        deletes[i] = values[i];
        randlist[i+j] = values[i];
        // printf("%d ", values[i]); fflush(stdout);
        InsertMBxtree(&root, values[i]);
        c = check(root);
        if (c < 0) {
            printf("===== error (%d) =====\n", c);
            printf("insert list:\n");
            for (int k = 0; k <= i+j; ++k) {
                printf("%d ", randlist[k]);
            }
            printf("\n");
            fflush(stdout);
            DestoryMBxtree(&root);
            return;
        }
        int rd = rand() % (3);
        if (rd == 0) { // delete nodes randomly while inserting
            rd = rand() % (i+1);
            ++j;
            randlist[i+j] = -values[rd];
            // printf("%d ", -values[rd]); fflush(stdout);
            DeleteMBxtree(&root, values[rd]);
            c = check(root);
            if (c < 0) {
                printf("===== error (%d) =====\n", c);
                printf("insert list:\n");
                for (int k = 0; k <= i+j; ++k) {
                    printf("%d ", randlist[k]);
                }
                printf("\n");
                fflush(stdout);
                DestoryMBxtree(&root);
                return;
            }
        }
    }
    // printf("\n");

    // DestoryMBxtree(&root); return;

    for (int i = SIZE-1; i > 0; --i) {
        int j = rand() % (i+1);
        int t = deletes[i];
        deletes[i] = deletes[j];
        deletes[j] = t;
    }

    // PrintMBxtree(root);
    // printf("Delete list: ");
    for (int i = 0; i < SIZE; ++i) {
        // printf("%d ", deletes[i]); fflush(stdout);
        DeleteMBxtree(&root, deletes[i]);
        // PrintMBxtree(root);
        c = check(root);
        if (c < 0) {
            printf("===== error (%d) =====\n", c);
            printf("insert list:\n");
            for (int j = 0; j < SIZE*2; ++j) {
                printf("%d ", randlist[j]);
            }
            printf("\ndelete list:\n");
            for (int j = 0; j <= i; ++j) {
                printf("%d ", deletes[j]);
            }
            printf("\n");
            fflush(stdout);
            break;
        }
    }

    DestoryMBxtree(&root);
}

void demo() {
    MBxtree root;
    ConstructMBxtree(&root);

    int list[] = {0};

    int n;
    int c;
    for (int i = 0; list[i] != 0; ++i) {
        if (list[i] > 0) {
            InsertMBxtree(&root, list[i]);
        }
        else { // list[i] < 0
            DeleteMBxtree(&root, list[i]);
        }
        c = check(root);
        if (c < 0) {
            printf("===== error (%d) =====\n", c);
            break;
        }
    }
    while (scanf("%d", &n) != -1) {
        if (n == 0) {
            // break;
            PrintMBxtree(root);
        }
        else if (n > 0) {
            InsertMBxtree(&root, n);
        }
        else {
            DeleteMBxtree(&root, -n);
        }
        // PrintMBxtree(root);
        c = check(root);
        if (c < 0) {
            printf("===== error (%d) =====\n", c);
            break;
        }
    }

    DestoryMBxtree(&root);
}

int main(void) {
    for (int i = 0; i < 100; ++i) test();
    // test();
    // demo();

    return 0;
}

