#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
// #include <math.h>

/*
树中每个结点最多有 MAX_DEGREE 棵子树
根结点至少有两棵子树
除根结点外的所有非叶结点至少有 MIN_DEGREE 棵子树
非根结点至少要保存 MIN_DEGREE-1 个关键字
所有叶结点到根结点的高度相等
*/

#define SIZE 100

#define MAX_DEGREE 3
// #define MIN_DEGREE ((int)ceil((float)MAX_DEGREE/(2)))
#define MIN_DEGREE (MAX_DEGREE+1)/2
// ((int)ceil((float)4/2)) == 2

typedef int KeyType;

//数组 key 与 children 定义长度为MAX_DEGREE+1是为了方便分裂操作
typedef struct MBnode {
    int keynum;  
    KeyType key[MAX_DEGREE+1]; // key[0]不存储数据
    struct MBnode *parent;
    struct MBnode *children[MAX_DEGREE+1];
} MBnode, *MBtree;


void ConstructMBtree(MBtree *root) {
    *root = NULL;
}
//在 mbt 指向的结点中, 寻找并返回小于等于 key 的最大关键字序号
static int _Search(MBnode *mbt, KeyType key) {
    int i = 1;
    while (i<=mbt->keynum && mbt->key[i]<=key) ++i;
    return i-1;
}
/*
在根为 root 的 B 树中查找关键字 k
如果查找成功, 则将所在结点地址放入 np, 将结点内位置序号放入 pos, 并返回 true
否则, 将 k 应该被插入的结点放入 np, 将结点内应插位置序号放入 pos, 并返回 false
*/
static bool Search(MBtree root, KeyType k, MBnode **np, int *pos) {
    MBnode *p = root, *fp = NULL;
    bool found = false;
    int i = 0;
    while (p != NULL && !found) {
        i = _Search(p, k);
        if (i > 0 && p->key[i] == k) {
            found = true;
        }
        else {
            fp = p;
            p = p->children[i];
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
//B 树结点分裂, 返回分裂出的新结点
static MBnode *Split(MBnode *old) {
    int n = MAX_DEGREE-MIN_DEGREE;
    MBnode *new = (MBnode*)malloc(sizeof(MBnode));
    if (new == NULL) {
        return NULL;
    }
    memset(new, 0, sizeof(MBnode));
    new->keynum = n;
    new->parent = old->parent;
    new->children[0] = old->children[MIN_DEGREE];
    if (new->children[0] != NULL) {
        new->children[0]->parent = new;
    }
    old->children[MIN_DEGREE] = NULL;
    for (int i = 1; i <= n; ++i) {
        new->key[i] = old->key[MIN_DEGREE+i];
        new->children[i] = old->children[MIN_DEGREE+i];
        if (new->children[i] != NULL) {
            new->children[i]->parent = new;
        }
        old->children[MIN_DEGREE+i] = NULL;
    }
    old->keynum = MIN_DEGREE-1;
    return new;
}
/*
B 树插入数据(辅助函数)
在 mbp->key[ipos+1] 处插入 key, 在 mbp->children[ipos+1] 处插入 rp
*/
static void Insert(MBnode *mbp, int ipos, KeyType k, MBnode *rp) {
    //腾一个空位
    for (int i = mbp->keynum; i >= ipos+1; --i) {
        mbp->key[i+1] = mbp->key[i];
        mbp->children[i+1] = mbp->children[i];
    }
    mbp->key[ipos+1] = k;
    mbp->children[ipos+1] = rp;
    mbp->keynum++;
}
/*
在根为 *root 的 B 树中插入关键字 k
*/
MBtree InsertMBtree(MBtree *root, KeyType k) {
    if (*root == NULL) {
        *root = (MBnode*)malloc(sizeof(MBnode));
        if (*root == NULL) {
            return NULL;
        }
        memset(*root, 0, sizeof(MBnode));
        (*root)->key[1] = k;
        (*root)->keynum = 1;
        (*root)->parent = NULL;
        (*root)->children[0] = NULL;
        (*root)->children[1] = NULL;
        return *root;
    }
    int i = 0;
    KeyType x = k;
    MBnode *q = NULL;
    MBnode *ap = NULL;
    if (Search(*root, k, &q, &i)) {
        return NULL;
    }
    bool finished = false;
    while (q != NULL && !finished) {
        Insert(q, i, x, ap);
        if (q->keynum < MAX_DEGREE) {
            finished = true;
            break;
        }
        ap = Split(q);
        x = q->key[MIN_DEGREE];
        q = q->parent;
        if (q != NULL) {
            i = _Search(q, x);
        }
    }
    if (!finished) { //根结点要分裂, 并产生新根
        MBnode *new = (MBnode*)malloc(sizeof(MBnode));
        if (new == NULL) {
            return NULL;
        }
        memset(new, 0, sizeof(MBnode));
        new->keynum = 1;
        new->parent = NULL;
        new->key[1] = x;
        new->children[0] = *root;
        (*root)->parent = new;
        new->children[1] = ap;
        ap->parent = new;
        *root = new;
    }
    return *root;
}

static MBnode* SubstitutedKey(MBnode *s, int pos, int *sk_pos) {
    if (s->children[0] == NULL) {
        *sk_pos = pos;
        return s;
    }
    MBnode *p = s->children[pos-1];
    MBnode *fp = NULL;
    while (p != NULL) {
        fp = p;
        p = p->children[p->keynum];
    }
    s->key[pos] = fp->key[fp->keynum];
    *sk_pos = fp->keynum;
    return fp;
}
// u_pos + 1 == v_pos in default
static MBnode* Merge(MBnode *fn, int u_pos, int v_pos) {
    // merge fn->key[v_pos] and v into u
    MBnode *u = fn->children[u_pos];
    MBnode *v = fn->children[v_pos];
    ++(u->keynum);
    u->key[u->keynum] = fn->key[v_pos];
    // move key
    for (int i = u->keynum+1, j = 1; j <= v->keynum; ++i, ++j) {
        u->key[i] = v->key[j];
    }
    // move children
    for (int i = u->keynum, j = 0; j <= v->keynum; ++i, ++j) {
        u->children[i] = v->children[j];
        if (u->children[i] != NULL) {
            u->children[i]->parent = u;
        }
    }

    u->keynum += v->keynum;
    free(v);

    // shrink fn
    for (int i = v_pos; i < fn->keynum; ++i) {
        fn->key[i] = fn->key[i+1];
        fn->children[i] = fn->children[i+1];
    }
    fn->children[fn->keynum] = NULL;
    --(fn->keynum);

    return fn;
}
static MBnode* DeleteRecursively(MBtree *root, MBnode *s) {
    // 在左右兄弟中寻找关键字数量大于 MIN_DEGREE-1 的兄弟结点, 进行父子换位法
    MBnode *fs = s->parent;
    if (fs == NULL) {
        return NULL;
    }
    MBnode *p = NULL;
    int s_pos = -1; // the index of s in fs->children
    int p_pos = -1; // the index of p in fs->children
    for (int i = 0; i <= fs->keynum; ++i) {
        p = fs->children[i];
        if (p == s) {
            s_pos = i;
            break;
        }
    }
    if (s_pos != 0 && fs->children[s_pos-1]->keynum > MIN_DEGREE-1) {
        p_pos = s_pos-1;
        p = fs->children[p_pos];
    }
    else if (s_pos != fs->keynum && fs->children[s_pos+1]->keynum > MIN_DEGREE-1) {
        p_pos = s_pos+1;
        p = fs->children[p_pos];
    }

    if (p_pos != -1) {
        // p has extra keys, so p give a key to fp, and fp give another key to s
        if (p_pos < s_pos) { // p_pos == s_pos-1
            for (int i = s->keynum; i > 0; --i) {
                s->key[i+1] = s->key[i];
            }
            s->key[1] = fs->key[s_pos];
            fs->key[s_pos] = p->key[p->keynum];

            // if !leaf
            for (int i = s->keynum; i >= 0; --i) {
                s->children[i+1] = s->children[i];
            }
            s->children[0] = p->children[p->keynum];
            if (s->children[0] != NULL) {
                s->children[0]->parent = s;
            }
            p->children[p->keynum] = NULL;

            ++(s->keynum);
            --(p->keynum);
        }
        else {  // p_pos == s_pos+1
            ++(s->keynum);
            s->key[s->keynum] = fs->key[p_pos];
            fs->key[p_pos] = p->key[1];
            for (int i = 1; i < p->keynum; ++i) {
                p->key[i] = p->key[i+1];
            }

            // if !leaf
            s->children[s->keynum] = p->children[0];
            if (s->children[s->keynum] != NULL) {
                s->children[s->keynum]->parent = s;
            }
            for (int i = 0; i < p->keynum; ++i) {
                p->children[i] = p->children[i+1];
            }
            p->children[p->keynum] = NULL;

            --(p->keynum);
        }
    }
    else {
        // merge adjacent child node
        if (s_pos != 0) {
            // merge s and fs->key[s_pos] into p
            s = fs->children[s_pos-1];
            fs = Merge(fs, s_pos-1, s_pos);
        }
        else {
            // merge fs->key[s_pos+1] and p into s
            fs = Merge(fs, s_pos, s_pos+1);
        }
        // check fs->keynum
        if (fs->parent == NULL) { // fs == *root
            if (fs->keynum == 0) {
                s->parent = NULL;
                free(fs);
                *root = s;
                return s;
            }
            return fs;
        }
        else {
            if (fs->keynum < MIN_DEGREE-1) {
                DeleteRecursively(root, fs);
            }
            return fs;
        }
        
    }
    return fs;
}
/*
max=5, min=3
    [   150     300    ]
    |         |         |
[50, 100][200, 250][350, 400, 450, 500]
delete: 50
          [    300    ]
        |               |
[100, 150, 200, 250][350, 400, 450, 500]
*/

/*
在根为 *root 的 B树中删除关键字 k
*/
MBtree DeleteMBtree(MBtree *root, KeyType k) {
    if (*root == NULL) {
        return NULL;
    }
    int pos = 0; // 大于零
    MBnode *s = NULL, *p = NULL, *fp = NULL;
    if (!Search(*root, k, &s, &pos)) {
        return NULL;
    }
    // 删除非最下层结点中的关键字, 可以转化为删除最下层结点中的关键字
    s = SubstitutedKey(s, pos, &pos);
    // 删除最下层的叶子结点 s 中下标为 pos 的关键字
    // 数据移位
    for (int i = pos; i < s->keynum; ++i) {
        s->key[i] = s->key[i+1];
    }
    --(s->keynum);
    if (s->keynum >= MIN_DEGREE-1) { // 删除 key 后的结点符合 B 树性质
        return *root;
    }
    //在左右兄弟中寻找关键字数量大于 MIN_DEGREE-1 的兄弟结点, 进行父子换位法
    DeleteRecursively(root, s);
    return *root;
}

MBnode* SearchInMBtree(MBtree root, KeyType k, int *pos) {
    MBnode *s;
    if (Search(root, k, &s, pos) == false) {
        *pos = -1;
        return NULL;
    }
    return s;
}


bool DestoryMBtree(MBtree *root) {
    if (root == NULL || *root == NULL) {
        return false;
    }
    for (int i = 0; i <= (*root)->keynum; ++i) {
        DestoryMBtree(&((*root)->children[i]));
    }
    free(*root);
    *root = NULL;
    return true;
}

static void PrintMBtreeInRank(MBtree root, int height) {
    if (root == NULL) {
        return;
    }
    printf("%*s---\n", 4*height, "");
    // printf("%*snode(0): (%d)\n", 4*height, "", root->key[0]);
    for (int i = root->keynum; i > 0; --i) {
        PrintMBtreeInRank(root->children[i], height+1);
        printf("%*snode(%d): (%d)\n", 4*height, "", i, root->key[i]);
    }
    PrintMBtreeInRank(root->children[0], height+1);
    printf("%*s---\n", 4*height, "");

}

void PrintMBtree(MBtree root) {
    printf("# MBtree\n");
    PrintMBtreeInRank(root, 0);
}


int check(MBtree root) {
    if (root == NULL) {
        return 0;
    }
    if (root->parent != NULL && root->keynum < MIN_DEGREE-1) {
        return -1;
    }
    int kid0 = check(root->children[0]);
    if (kid0 < 0) {
        return -1;
    }
    for (int i = 1; i <= root->keynum; ++i) {
        if (kid0 != check(root->children[i])) {
            return -1;
        }
    }
    return kid0 + 1;
}


void test(void) {
    MBtree root;
    ConstructMBtree(&root);

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
        // printf("%d ", values[i]); fflush(stdout);
        InsertMBtree(&root, values[i]);
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
            // printf("%d ", -values[rd]); fflush(stdout);
            DeleteMBtree(&root, values[rd]);
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

    // PrintMBtree(root);
    // printf("Delete list: ");
    for (int i = 0; i < SIZE; ++i) {
        // printf("%d ", deletes[i]); fflush(stdout);
        DeleteMBtree(&root, deletes[i]);
        // PrintMBtree(root);
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

    DestoryMBtree(&root);
}

void demo() {
    MBtree root;
    ConstructMBtree(&root);

    int n;
    while (scanf("%d", &n) != -1) {
        if (n == 0) {
            // break;
            PrintMBtree(root);
        }
        else if (n > 0) {
            InsertMBtree(&root, n);
        }
        else {
            DeleteMBtree(&root, -n);
        }
        // PrintMBtree(root);
        if (check(root) < 0) {
            printf("===== error =====\n");
            break;
        }
    }

    DestoryMBtree(&root);
}

int main(void) {
    for (int i = 0; i < 100; ++i) test();
    // test();
    // demo();
}
