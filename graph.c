//在邻接表存储结构上实现图的基本操作
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_VERTEX_NUM 5
#define MAX_ARC_INFO 65535  //最大权值不超过 MAX_ARC_INFO
#define MIN_ARC_INFO 0      //最小权值不低于 MIN_ARC_INFO
typedef char VertexType;
typedef int InfoPtr;    //权重
typedef enum {
    DG,//有向图
    UDG//无向图
} GraphType;
typedef struct ArcNode {
    int adjvex;
    InfoPtr info;
    struct ArcNode *nextarc;
} ArcNode;
typedef struct VNode {
    VertexType data;
    ArcNode *firstarc;
} VNode;
typedef struct {
    VNode vertex[MAX_VERTEX_NUM];
    int vexnum, arcnum;
    GraphType type;
} ListGraph;//允许插入权为0的边,但不允许插入自环
typedef struct{
    VertexType vertex[MAX_VERTEX_NUM]; //顶点
    int arcs[MAX_VERTEX_NUM][MAX_VERTEX_NUM]; //邻接矩阵
    int vexnum, arcnum;   //图的当前顶点数和弧数
    GraphType type;     //图的种类标志
} MatrixGraph;//允许插入权为0的边,但不允许插入自环
typedef struct {
    int start;
    VertexType startVertex;
    int end;
    VertexType endVertex;
    InfoPtr info;
} Edge;
//返回顶点 v 在vertex数组中的下标，如果v不存在，返回-1
int LocateVertex_List(ListGraph* G, VertexType v) {
    for (int i = 0; i < G->vexnum; i++) {
        if (G->vertex[i].data == v) {
            return i;
        }
    }
    return -1;
}
//图 G 初始化,确定图的类型(GraphType) UDG, DG
void InitGraph_List(ListGraph *G, GraphType e) {
    G->type = e;
    G->arcnum = 0;
    G->vexnum = 0;
}
//获得图 G 中某位置结点的数据
VertexType GetVertexData_List(ListGraph *G, int index) {
    return G->vertex[index].data;
}
/*
向图 G 中添加结点 v
成功返回 true, 失败返回 false
*/
bool InsertVertex_List(ListGraph *G, VertexType v) {
    if (G->vexnum == MAX_VERTEX_NUM) {
        return false;
    }
    int v_location = LocateVertex_List(G, v);
    if (v_location != -1) {
        return false;
    }
    G->vertex[G->vexnum].data = v;
    G->vertex[G->vexnum].firstarc = NULL;
    G->vexnum++;
    return true;
}
/*
向图 G 中添加边 (v,w) 或 <v,w>, 并赋以权重(大于等于零)
成功返回 true, 失败返回 false
*/
bool InsertArc_List(ListGraph *G, VertexType v, VertexType w, int info) {
    if (G->arcnum == MAX_VERTEX_NUM * MAX_VERTEX_NUM || info < MIN_ARC_INFO || info > MAX_ARC_INFO) {
        return false;
    }
    int v_location = LocateVertex_List(G, v);
    int w_location = LocateVertex_List(G, w);
    if (v_location == -1 || w_location == -1 || v_location == w_location) {
        return false;
    }
    ArcNode *p;
    p = G->vertex[v_location].firstarc;
    while (p != NULL) {
        if (p->adjvex == w_location) {
            return false;
        }
        p = p->nextarc;
    }
    if (G->type == UDG) {
        p = G->vertex[w_location].firstarc;
        while (p != NULL) {
            if (p->adjvex == v_location) {
                return false;
            }
            p = p->nextarc;
        }
    }
    p = (ArcNode*)malloc(sizeof(ArcNode));
    p->adjvex = w_location;
    p->info = info;
    p->nextarc = G->vertex[v_location].firstarc;
    G->vertex[v_location].firstarc = p;
    if (G->type == UDG) {
        p = (ArcNode*)malloc(sizeof(ArcNode));
        p->adjvex = v_location;
        p->info = info;
        p->nextarc = G->vertex[w_location].firstarc;
        G->vertex[w_location].firstarc = p;
    }
    G->arcnum++;
    return true;
}
/*
图 G 中边 (v,w) 或 <v,w> 的权重
成功返回权重, 失败返回 -1
*/
int GetArcInfo_List(ListGraph *G, VertexType v, VertexType w) {
    if (G->arcnum == 0) {
        return -1;
    }
    int v_location = LocateVertex_List(G, v);
    int w_location = LocateVertex_List(G, w);
    if (v_location == -1 || w_location == -1) {
        return -1;
    }
    else if (v_location == w_location) {
        return 0;
    }
    ArcNode *p;
    p = G->vertex[v_location].firstarc;
    while (p != NULL) {
        if (p->adjvex == w_location) {
            return p->info;
        }
        p = p->nextarc;
    }
    return -1;
}
/*
删除图 G 中的结点 v
成功返回 true, 失败返回 false
*/
bool DeleteVertex_List(ListGraph *G, VertexType v) {
    int v_location = LocateVertex_List(G, v);
    if (v_location == -1) {
        return false;
    }
    //将vertex数组中的结点重新排列
    //将vertex数组中所有的结点连接v的边删去

    ArcNode *p, *pre;
    p = G->vertex[v_location].firstarc;
    while (p != NULL) {
        pre = p;
        p = p->nextarc;
        free(pre);
        G->arcnum--;
    }
    for (int i = 0; i < G->vexnum; i++) {
        pre = NULL;
        if (i != v_location) {  //跳过vertex[v_location]
            p = G->vertex[i].firstarc;
            while (p != NULL) {
                if (p->adjvex == v_location) {
                    if (pre == NULL) {
                        G->vertex[i].firstarc = p->nextarc;
                        free(p);
                        p = G->vertex[i].firstarc;
                    }
                    else {
                        pre->nextarc = p->nextarc;
                        free(p);
                        p = pre->nextarc;
                    }
                    G->arcnum--;
                    continue;
                }
                else if (p->adjvex > v_location) {
                    p->adjvex--;
                }
                pre = p;
                p = p->nextarc;
            }
        }
        if (i > v_location) {
            G->vertex[i-1] = G->vertex[i];
        }
    }
    G->vexnum--;
    return true;
}
/*
删除图 G 中的边 (v,w) 或 <v,w>
成功返回删除边的权重(大于等于零), 失败返回 -1
*/
int DeleteArc_List(ListGraph *G, VertexType v, VertexType w) {
    if (G->arcnum == 0) {
        return -1;
    }
    int v_location = LocateVertex_List(G, v);
    int w_location = LocateVertex_List(G, w);
    if (v_location == -1 || w_location == -1) {
        return -1;
    }
    int info;
    ArcNode *p, *pre;
    pre = NULL;
    p = G->vertex[v_location].firstarc;
    while (p != NULL) {
        if (p->adjvex == w_location) {
            if (pre == NULL) {
                G->vertex[v_location].firstarc = p->nextarc;
            }
            else {
                pre->nextarc = p->nextarc;
            }
            info = p->info;
            free(p);
            G->arcnum--;
            break;
        }
        pre = p;
        p = p->nextarc;
        if (p == NULL) {
            return -1;
        }
    }
    if (G->type == UDG) {
        pre = NULL;
        p = G->vertex[w_location].firstarc;
        while (p != NULL) {
            if (p->adjvex == v_location) {
                if (pre == NULL) {
                    G->vertex[w_location].firstarc = p->nextarc;
                }
                else {
                    pre->nextarc = p->nextarc;
                }
                free(p);
                break;
            }
            pre = p;
            p = p->nextarc;
            if (p == NULL) {
                return -1;
            }
        }
    }
    return info;
}
/*
销毁图 G
成功返回 true, 失败返回 false
*/
bool DestoryGraph_List(ListGraph *G) {
    ArcNode *p, *pre;
    for (int i = 0; i < G->vexnum; i++) {
        p = G->vertex[i].firstarc;
        while (p != NULL) {
            pre = p;
            p = p->nextarc;
            free(pre);
        }
        G->vertex[i].firstarc = NULL;
        G->vertex[i].data = 0;
    }
    G->arcnum = 0;
    G->vexnum = 0;
}
//深度优先遍历辅助函数
static void DFSTraverse_List(ListGraph *G, int i, void(visit)(VNode *s), int visited[]) {
    visit(&(G->vertex[i]));
    visited[i] = 1;
    ArcNode *p = G->vertex[i].firstarc;
    while (p != NULL) {
        if (!visited[p->adjvex]) {
            DFSTraverse_List(G, p->adjvex, visit, visited);
        }
        p = p->nextarc;
    }
}
/*
深度优先遍历图 G 结点
返回无向图 G 的连通分支数, 有向图返回 -1
*/
int DepthFirstSearchGraph_List(ListGraph *G, void(visit)(VNode *s)) {
    if (G->type == DG) {
        return -1;
    }
    int count = 0;
    int visited[G->vexnum];
    for (int i = 0; i < G->vexnum; visited[i++] = 0);
    for (int i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            DFSTraverse_List(G, i, visit, visited);
            count++;
        }
    }
    return count;
}
//广度优先遍历辅助函数
static void BFSTraverse_List(ListGraph *G, int i, void(visit)(VNode *s), int visited[]) {
    int rear = 0, front = 0;
    int queue[G->vexnum];
    for (int j = 0; j < G->vexnum; queue[j++] = 0);
    visit(&(G->vertex[i]));
    visited[i] = 1;
    queue[front++] = i; front%=G->vexnum;
    ArcNode *p = NULL;
    while (rear != front) {
        p = G->vertex[queue[rear++]].firstarc; rear%=G->vexnum;
        while (p != NULL) {
            if (!visited[p->adjvex]) {
                visit(&(G->vertex[p->adjvex]));
                visited[p->adjvex] = 1;
                queue[front++] = p->adjvex; front%=G->vexnum;
            }
            p = p->nextarc;
        }
    }
}
/*
广度优先遍历图 G 结点
返回无向图 G 的连通分支数, 有向图返回 -1
*/
int BreadthFirstSearchGraph_List(ListGraph *G, void(visit)(VNode *s)) {
    if (G->type == DG) {
        return -1;
    }
    int visited[G->vexnum];
    int count = 0;
    for (int i = 0; i < G->vexnum; visited[i++] = 0);
    for (int i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            BFSTraverse_List(G, i, visit, visited);
            count++;
        }
    }
    return count;
}
/*
用Prim算法从点 root 开始生成无向图 G 的最小生成树
edge[] 是长度不低于 G->vexnum-1 的数组, 用于保存生成树中边结点在 G->vertex 数组中的下标
若在生成最小生成树时发现图 G 不连通, 则返回 false, 成功返回 true
*/
bool MiniSpanTree_List_Prim(ListGraph *G, VertexType root, Edge edge[]) {
    if (G->type == DG) {
        return false;
    }
    int u = LocateVertex_List(G, root), v = u;
    if (u == -1) {
        return false;
    }
    //集合U是已在最小生成树中的结点集合
    struct {
        int u;//集合U结点中距离该结点最近的一个
        int distance;
    } distanceToU[G->vexnum];
    //将distanceToU数组初始化
    for (int i = 0; i < G->vexnum; i++) {
        distanceToU[i].u = u;
        distanceToU[i].distance = MAX_ARC_INFO+1;
    }
    ArcNode *p = G->vertex[u].firstarc;
    while (p != NULL) {
        distanceToU[p->adjvex].distance = p->info;
        p = p->nextarc;
    }
    distanceToU[u].distance = MIN_ARC_INFO-1;
    //将剩下的结点一一添入到集合U中
    for (int i = 0; i < G->vexnum-1; i++) {
        //找到距离点集U距离最近的点v
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->vexnum; j++) {
            if (distanceToU[j].distance > MIN_ARC_INFO-1 &&
                distanceToU[j].distance < min) {
                min = distanceToU[j].distance;
                v = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {
            return false;
        }
        //将边 (u,v) 保存到最小生成树中
        u = distanceToU[v].u;
        edge[i].start = u;
        edge[i].end = v;
        edge[i].startVertex = G->vertex[u].data;
        edge[i].endVertex = G->vertex[v].data;
        edge[i].info = distanceToU[v].distance;
        //将结点v加入到集合U中
        distanceToU[v].distance = MIN_ARC_INFO-1;
        //更新其他点到点集U的距离
        //由于其他点到点集U-{v}中任一点的距离已不会改变,所以只需考察该点到点v的距离
        p = G->vertex[v].firstarc;
        while (p != NULL) {
            if (p->info < distanceToU[p->adjvex].distance) {
                distanceToU[p->adjvex].u = v;
                distanceToU[p->adjvex].distance = p->info;
            }
            p = p->nextarc;
        }
    }
    return true;
}
/*
用Kruskal算法生成无向图 G 的最小生成树
edge[] 是长度不低于 G->vexnum-1 的数组, 用于保存生成树中边结点在 G->vertex 数组中的下标
若在生成最小生成树时发现图 G 不连通, 则返回 false, 成功返回 true
*/
bool MiniSpanTree_List_Kruskal(ListGraph *G, Edge edge[]) {
    if (G->type == DG) {
        return false;
    }
    Edge allEdge[G->arcnum];
    ArcNode *p;
    int k = 0, u, v;
    for (int i = 0; i < G->vexnum; i++) {
        p = G->vertex[i].firstarc;
        while (p != NULL) {
            if (i < p->adjvex) {
                allEdge[k].start = i;
                allEdge[k].end = p->adjvex;
                allEdge[k].info = p->info;
                k++;
            }
            p = p->nextarc;
        }
    }
    int nodes[G->vexnum];//记录该点所处的环境(相连的最小结点)
    for (int i = 0; i < G->vexnum; nodes[i] = i, i++);
    for (int i = 0; i < G->vexnum-1; i++) {
        //找到权值最小且不与已选中边形成环的边
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->arcnum; j++) {
            if (allEdge[j].info > MIN_ARC_INFO-1 &&
                allEdge[j].info < min &&
                //选择不与已选入最小生成树的边形成回路的边
                nodes[allEdge[j].start] != nodes[allEdge[j].end]) {
                min = allEdge[j].info;
                k = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {
            return false;
        }
        //保存该边
        u = allEdge[k].start;
        v = allEdge[k].end;
        edge[i].start = u;
        edge[i].end = v;
        edge[i].startVertex = G->vertex[u].data;
        edge[i].endVertex = G->vertex[v].data;
        edge[i].info = min;
        //在nodes数组中记录在最小生成树中与点v相连的最小点
        for (int i = v; i < G->vexnum; i++) {
            if (nodes[i] == v) {
                nodes[i] = u;
            }
        }
        allEdge[k].info = MIN_ARC_INFO-1;
    }
    return true;
}
//打印图 G
void PrintGraph_List(ListGraph *G) {
    printf("#Graph_List\n");
    ArcNode *p;
    for (int i = 0; i < G->vexnum; i++) {
        printf("%5c", G->vertex[i].data);
        p = G->vertex[i].firstarc;
        while (p != NULL) {
            printf("  -(%d)-> %c", p->info, G->vertex[p->adjvex]);
            p = p->nextarc;
        }
        printf("\n");
    }
}
/*
寻找图 G 关键路径
cEdge是长度不少于 G->arcnum 的 Edge 型数组
成功返回关键路径的长度, 失败返回 -1
*/
int CriticalPathInGraph_List(ListGraph *G, Edge cEdge[]) {
    if (G->type == UDG) {
        return -1;
    }
    int n = 0, u, v;
    //计算各结点入度
    int indegree[G->vexnum];
    for (int i = 0; i < G->vexnum; indegree[i++] = 0);
    ArcNode *p;
    for (int i = 0; i < G->vexnum; i++) {
        p = G->vertex[i].firstarc;
        if (p == NULL) n++;//出度为零的结点个数
        while (p != NULL) {
            indegree[p->adjvex]++;
            p = p->nextarc;
        }
    }
    if (n != 1) {   //出度为零的结点应该只有一个
        return -1;
    }
    int earliestTime[G->vexnum];
    int latestTime[G->vexnum];
    int inverseTopoSeq[G->vexnum];
    int queue[G->vexnum], rear = 0, front = 0;
    for (n = 0; n < G->vexnum; n++) {
        if (indegree[n] == 0) {
            queue[front++] = n; front%=G->vexnum;
        }
        //最早发生时间初始化
        earliestTime[n] = 0;
    }
    if (front != 1) {    //入度为零的结点应该只有一个
        return -1;
    }
    //计算最早发生时间
    n = G->vexnum-1;
    while (rear != front) {
        u = queue[rear++]; rear%=G->vexnum;
        inverseTopoSeq[n--] = u;
        p = G->vertex[u].firstarc;
        while (p != NULL) {
            v = p->adjvex;
            indegree[v]--;
            if (indegree[v] == 0) {
                queue[front++] = v; front%=G->vexnum;
            }
            if (earliestTime[v] < earliestTime[u] + p->info) {
                earliestTime[v] = earliestTime[u] + p->info;
            }
            p = p->nextarc;
        }
    }
    if (n != -1) {  //防止 AOE 图中有环的结构
        return -1;
    }
    //最晚发生时间初始化
    for (n = 0; n < G->vexnum; n++) {
        latestTime[n] = earliestTime[G->vexnum-1];
    }
    //计算最晚发生时间
    for (n = 0; n < G->vexnum; n++) {
        u = inverseTopoSeq[n];
        p = G->vertex[u].firstarc;
        while (p != NULL) {
            v = p->adjvex;
            if (latestTime[u] > latestTime[v] - p->info) {
                latestTime[u] = latestTime[v] - p->info;
            }
            p = p->nextarc;
        }
    }
    //标记关键路径(两顶点 u, v 都在关键路径上也不能说明边 (u,v) 在关键路径上)
    //依次处理每个结点的出边
    for (u = 0, n = 0; u < G->vexnum; u++) {
        if (earliestTime[u] == latestTime[u]) {
            p = G->vertex[u].firstarc;
            while (p != NULL) {
                v = p->adjvex;
                if (earliestTime[v] == latestTime[v] &&
                    earliestTime[v] - earliestTime[u] == p->info) {
                    cEdge[n].start = u;
                    cEdge[n].end = v;
                    cEdge[n].startVertex = G->vertex[u].data;
                    cEdge[n].endVertex = G->vertex[v].data;
                    cEdge[n].info = p->info;
                    n++;
                }
                p = p->nextarc;
            }
        }
    }
    return n;
}
/*
用 Dijkstra 算法计算图 G 中某结点到其他所有点的最短路径长度
distance 为长度不小于 G->vexnum 的数组
成功返回 true, 失败返回 false
*/
bool ShortestPath_List_Dijkstra(ListGraph *G, VertexType u_node, int distance[]) {
    int u = LocateVertex_List(G, u_node), v;
    if (u == -1) {
        return false;
    }
    for (int i = 0; i < G->vexnum; i++) {
        distance[i] = MAX_ARC_INFO+1;
    }
    distance[u] = 0;
    ArcNode *p = G->vertex[u].firstarc;
    while (p != NULL) {
        distance[p->adjvex] = p->info;
        p = p->nextarc;
    }
    //已求出最短路径长度的结点归为集合U
    int nodesInU[G->vexnum];
    for (int i = 0; i < G->vexnum; nodesInU[i++] = 0);
    nodesInU[u] = 1;
    for (int i = 0; i < G->vexnum-1; i++) {
        //每次循环找到distanceToU[].distance中的最小值
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->vexnum; j++) {
            if (distance[j] < min && nodesInU[j] == 0) {
                min = distance[j];
                v = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {    //图不连通
            return true;
        }
        //distance[v]是最小的
        nodesInU[v] = 1;
        //更新distance数组
        p = G->vertex[v].firstarc;
        while (p != NULL) {
            if (distance[p->adjvex] > distance[v] + p->info) {
                distance[p->adjvex] = distance[v] + p->info;
            }
            p = p->nextarc;
        }
    }
    return true;
}
/*
用 Floyd 算法求图 G 中任二结点的最短路径及距离
distance 和 path 是大小为 G->vexnum * G->vexnum 的二维数组
distance[i][j] 为点 i 到点 j 的最短距离
path[i][j] 为 i, j 之间最短路径上, i 的后继点
*/
void ShortestPath_List_Floyd(ListGraph *G, int distance[][G->vexnum], int path[][G->vexnum]) {
    for (int i = 0; i < G->vexnum; i++) {
        for (int j = 0; j < G->vexnum; j++) {
            distance[i][j] = MAX_ARC_INFO+1;
            path[i][j] = -1;
        }
        ArcNode *p = G->vertex[i].firstarc;
        while (p != NULL) {
            distance[i][p->adjvex] = p->info;
            p = p->nextarc;
        }
        distance[i][i] = 0;
    }
    for (int k = 0; k < G->vexnum; k++) {
        for (int i = 0; i < G->vexnum; i++) {
            for (int j = 0; j < G->vexnum; j++) {
                if (distance[i][j] > distance[i][k] + distance[k][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                    path[i][j] = k;
                }
            }
        }
    }
}
//函数 PrintPathAndDistance_List_pro 的辅助函数
static void PrintPath_List(ListGraph *G, int path[][G->vexnum], int i, int j) {
    if (path[i][j] == -1) {
        return ;
    }
    PrintPath_List(G, path, i, path[i][j]);
    printf(" %c ->", G->vertex[path[i][j]].data);
    PrintPath_List(G, path, path[i][j], j);
}
//显示结点间最短路径与距离
void PrintPathAndDistance_List_pro(ListGraph *G, int distance[][G->vexnum], int path[][G->vexnum]) {
    for (int i = 0; i < G->vexnum; i++) {
        for (int j = 0; j < G->vexnum; j++) {
            if (distance[i][j] == MAX_ARC_INFO+1 || i == j) {
                if (i != j) {
                    printf(" %c 到 %c 没有路径\n", G->vertex[i], G->vertex[j]);
                }
            }
            else {
                printf(" %c 到 %c 距离为 %d, ", G->vertex[i].data, G->vertex[j].data, distance[i][j]);
                printf("%c ->", G->vertex[i].data);
                PrintPath_List(G, path, i, j);
                printf(" %c\n", G->vertex[j].data);
            }
        }
    }
}





//返回顶点 v 在vertex数组中的下标，如果v不存在，返回-1
int LocateVertex_Matrix(MatrixGraph *G, VertexType v) {
    for (int i = 0; i < G->vexnum; i++) {
        if (G->vertex[i] == v) {
            return i;
        }
    }
    return -1;
}
//初始化图 G, 并设置图的类型 (DG, UDG)
void InitGraph_Matrix(MatrixGraph *G, GraphType e) {
    G->arcnum = 0;
    G->vexnum = 0;
    G->type = e;
}
/*
向图 G 中添加结点 v
成功返回 true, 失败返回 false
*/
bool InsertVertex_Matrix(MatrixGraph *G, VertexType v) {
    int i;
    if (G->vexnum == MAX_VERTEX_NUM) {
        return false;
    }
    int v_location = LocateVertex_Matrix(G, v);
    if (v_location != -1) {
        return false;
    }
    G->vertex[G->vexnum] = v;
    for (i = 0; i <= G->vexnum; i++) {
        G->arcs[G->vexnum][i] = MAX_ARC_INFO+1;
    }
    for (i = 0; i < G->vexnum; i++) {
        G->arcs[i][G->vexnum] = MAX_ARC_INFO+1;
    }
    G->arcs[G->vexnum][G->vexnum] = 0;
    G->vexnum++;
    return true;
}
/*
向图 G 中添加边 (v,w) 或 <v,w>, 并赋以权重(大于等于零)
成功返回 true, 失败返回 false
*/
bool InsertArc_Matrix(MatrixGraph *G, VertexType v, VertexType w, int info) {
    if (G->arcnum == MAX_VERTEX_NUM * MAX_VERTEX_NUM) {
        return false;
    }
    int v_location = LocateVertex_Matrix(G, v);
    int w_location = LocateVertex_Matrix(G, w);
    if (v_location == -1 || w_location == -1) {
        return false;
    }
    else if (v_location == w_location) {
        return false;
    }
    if (G->arcs[v_location][w_location] != MAX_ARC_INFO+1) {
        return false;
    }
    if (G->type == UDG && G->arcs[w_location][v_location] != MAX_ARC_INFO+1) {
        return false;
    }
    G->arcs[v_location][w_location] = info;
    if (G->type == UDG) {
        G->arcs[w_location][v_location] = info;
    }
    G->arcnum++;
    return true;
}
/*
图 G 中边 (v,w) 或 <v,w> 的权重
成功返回权重, 失败返回 -1
*/
int GetArcInfo_Matrix(MatrixGraph *G, VertexType v, VertexType w) {
    if (G->arcnum == 0) {
        return -1;
    }
    int v_location = LocateVertex_Matrix(G, v);
    int w_location = LocateVertex_Matrix(G, w);
    if (v_location == -1 || w_location == -1) {
        return -1;
    }
    return G->arcs[v_location][w_location];
}
/*
删除图 G 中的结点 v
成功返回 true, 失败返回 false
*/
bool DeleteVertex_Matrix(MatrixGraph *G, VertexType v) {
    int v_location = LocateVertex_Matrix(G, v);
    if (v_location == -1) {
        return false;
    }
    int count = 0;
    for (int i = 0; i < G->vexnum; i++) {
        if (G->arcs[v_location][i] <= MAX_ARC_INFO && v_location != i) {
            count++;
        }
        if (G->arcs[i][v_location] <= MAX_ARC_INFO && v_location != i) {
            count++;
        }
    }
    if (G->type == UDG) count/=2;
    for (int i = v_location+1; i < G->vexnum; i++) {
        G->vertex[i-1] = G->vertex[i];
        for (int j = 0; j < G->vexnum; j++) {
            G->arcs[i-1][j] = G->arcs[i][j];
            G->arcs[j][i-1] = G->arcs[i][j];
        }
        G->arcs[i-1][i-1] = 0;
    }
    G->vexnum--;
    G->arcnum -= count;
    return true;
}
/*
删除图 G 中的边 (v,w) 或 <v,w>
成功返回删除边的权重(大于等于零), 失败返回 -1
*/
int DeleteArc_Matrix(MatrixGraph *G, VertexType v, VertexType w) {
    int v_location = LocateVertex_Matrix(G, v);
    int w_location = LocateVertex_Matrix(G, w);
    if (v_location == -1 || w_location == -1 || v_location == w_location) {
        return -1;
    }
    if (G->arcs[v_location][w_location] == MAX_ARC_INFO+1) {
        return -1;
    }
    if (G->type == UDG && G->arcs[w_location][v_location] == MAX_ARC_INFO+1) {
        return -1;
    }
    int info = G->arcs[v_location][w_location];
    G->arcs[v_location][w_location] = MAX_ARC_INFO+1;
    if (G->type == UDG) {
        G->arcs[w_location][v_location] = MAX_ARC_INFO+1;
    }
    G->arcnum--;
    return info;
}
//打印图 G
void PrintGraph_Matrix(MatrixGraph *G) {
    printf("#Graph_Matrix\n");
    if (G == NULL) {
        return;
    }
    printf(" |");
    for (int i = 0; i < G->vexnum; i++) {
        printf("%5c", G->vertex[i]);
    }
    printf("\n");
    for (int i = 0; i < G->vexnum; i++) {
        printf("------");
    }
    printf("\n");
    for (int i = 0; i < G->vexnum; i++) {
        printf("%2c|", G->vertex[i]);
        for (int j = 0; j < G->vexnum; j++) {
            if (G->arcs[i][j]<=MAX_ARC_INFO)
                printf("%5d",G->arcs[i][j]);
            else 
                printf("    *");
        }
        printf("\n");
    }
}
//结点 index 的第一个邻接结点, 返回结点下标, 失败返回 -1
static int FirstAdjacencyVertex_Matrix(MatrixGraph *G, int index) {
    for (int i = 0; i < G->vexnum; i++) {
        if (G->arcs[index][i] <= MAX_ARC_INFO && index != i) {
            return i;
        }
    }
    return -1;
}
//结点 index 的在结点 k 后的邻接结点, 返回结点下标, 失败返回 -1
static int NextAdjacencyVertex_Matrix(MatrixGraph *G, int index, int k) {
    for (int i = k+1; i < G->vexnum; i++) {
        if (G->arcs[index][i] <= MAX_ARC_INFO && index != i) {
            return i;
        }
    }
    return -1;
}
//深度优先遍历辅助函数
static void DFSTraverse_Matrix(MatrixGraph *G, int i, void(visit)(VertexType *s), int visited[]) {
    visit(&(G->vertex[i]));
    visited[i] = 1;
    int k = FirstAdjacencyVertex_Matrix(G, i);
    while (k != -1) {
        if (!visited[k]) {
            DFSTraverse_Matrix(G, k, visit, visited);
        }
        k = NextAdjacencyVertex_Matrix(G, i, k);
    }
}
/*
深度优先遍历图 G 结点
返回无向图 G 的连通分支数, 有向图返回 -1
*/
int DepthFirstSearchMatrixGraph(MatrixGraph *G, void(visit)(VertexType *s)) {
    printf("DFS:  ");
    if (G->type == DG) {
        return -1;
    }
    int count = 0;
    int visited[G->vexnum];
    for (int i = 0; i < G->vexnum; visited[i++] = 0);
    for (int i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            DFSTraverse_Matrix(G, i, visit, visited);
            count++;
        }
    }
    printf("\n");
    return count;
}
//广度优先遍历辅助函数
static void BFSTraverse_Matrix(MatrixGraph *G, int i, void(visit)(VertexType *s), int visited[]) {
    int rear = 0, front = 0;
    int queue[G->vexnum];
    for (int i = 0; i < G->vexnum; queue[i++] = 0);
    visit(&(G->vertex[i]));
    visited[i] = 1;
    queue[front++] = i; front%=G->vexnum;
    int k, n;
    while (rear != front) {
        n = queue[rear++]; rear%=G->vexnum;
        k = FirstAdjacencyVertex_Matrix(G, n);
        while (k != -1) {
            if (!visited[k]) {
                visit(&(G->vertex[k]));
                visited[k] = 1;
                queue[front++] = k; front%=G->vexnum;
            }
        k = NextAdjacencyVertex_Matrix(G, n, k);
        }
    }
}
/*
广度优先遍历图 G 结点
返回无向图 G 的连通分支数, 有向图返回 -1
*/
int BreadthFirstSearchGraph_Matrix(MatrixGraph *G, void(visit)(VertexType *s)) {
    printf("BFS:  ");
    if (G->type == DG) {
        return -1;
    }
    int visited[G->vexnum];
    for (int i = 0; i < G->vexnum; visited[i++] = 0);
    int count = 0;
    for (int i = 0; i < G->vexnum; i++) {
        if (!visited[i]) {
            BFSTraverse_Matrix(G, i, visit, visited);
            count++;
        }
    }
    printf("\n");
    return count;
}
/*
用Prim算法从点 root 开始生成无向图 G 的最小生成树
edge[] 是长度不低于 G->vexnum-1 的数组, 用于保存生成树中边结点在 G->vertex 数组中的下标
若在生成最小生成树时发现图 G 不连通, 则返回 false, 成功返回 true
*/
bool MiniSpanTree_Matrix_Prim(MatrixGraph *G, VertexType root, Edge edge[]) {
    if (G->type == DG) {
        return false;
    }
    int u = LocateVertex_Matrix(G, root), v;
    if (u == -1) {
        return false;
    }
    struct {
        int u;
        int distance;
    } distanceToU[G->vexnum];
    for (int i = 0; i < G->vexnum; i++) {
        distanceToU[i].u = u;
        distanceToU[i].distance = MAX_ARC_INFO+1;
    }
    v = FirstAdjacencyVertex_Matrix(G, u);
    while (v != -1) {
        distanceToU[v].distance = G->arcs[u][v];
        v = NextAdjacencyVertex_Matrix(G, u, v);
    }
    distanceToU[u].distance = MIN_ARC_INFO-1;
    for (int i = 0; i < G->vexnum-1; i++) {
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->vexnum; j++) {
            if (distanceToU[j].distance > MIN_ARC_INFO-1 &&
                distanceToU[j].distance < min) {
                min = distanceToU[j].distance;
                v = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {
            return false;
        }
        u = distanceToU[v].u;
        edge[i].start = u;
        edge[i].startVertex = G->vertex[u];
        edge[i].end = v;
        edge[i].endVertex = G->vertex[v];
        edge[i].info = distanceToU[v].distance;
        //更新其他点到点集U的距离
        distanceToU[v].distance = MIN_ARC_INFO-1;
        u = v;
        v = FirstAdjacencyVertex_Matrix(G, u);
        while (v != -1) {
            if (G->arcs[u][v] < distanceToU[v].distance) {
                distanceToU[v].distance = G->arcs[u][v];
                distanceToU[v].u = u;
            }
            v = NextAdjacencyVertex_Matrix(G, u, v);
        }
    }
    return true;
}
/*
用Kruskal算法生成无向图 G 的最小生成树
edge[] 是长度不低于 G->vexnum-1 的数组, 用于保存生成树中边结点在 G->vertex 数组中的下标
若在生成最小生成树时发现图 G 不连通, 则返回 false, 成功返回 true
*/
bool MiniSpanTree_Matrix_Kruskal(MatrixGraph *G, Edge edge[]) {
    if (G->type == DG) {
        return false;
    }
    Edge allEdge[G->arcnum];
    int k = 0, u, v;
    for (int i = 0; i < G->vexnum; i++) {
        v = FirstAdjacencyVertex_Matrix(G, i);
        while (v != -1) {
            if (i < v) {
                allEdge[k].start = i;
                allEdge[k].end = v;
                allEdge[k].info = G->arcs[i][v];
                k++;
            }
            v = NextAdjacencyVertex_Matrix(G, i, v);
        }
    }
    int nodes[G->vexnum];//判断该点是否已经在最小生成树中
    for (int i = 0; i < G->vexnum; nodes[i] = i, i++);
    for (int i = 0; i < G->vexnum-1; i++) {
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->arcnum; j++) {
            if (allEdge[j].info > MIN_ARC_INFO-1 &&
                allEdge[j].info < min &&
                //下一行的条件意为选择不与已选入最小生成树的边形成回路的边//错误
                nodes[allEdge[j].start] != nodes[allEdge[j].end]) {
                min = allEdge[j].info;
                k = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {
            return false;
        }
        u = allEdge[k].start;
        v = allEdge[k].end;
        edge[i].start = u;
        edge[i].end = v;
        edge[i].startVertex = G->vertex[u];
        edge[i].endVertex = G->vertex[v];
        edge[i].info = min;
        for (int i = v; i < G->vexnum; i++) {
            if (nodes[i] == v) {
                nodes[i] = u;
            }
        }
        allEdge[k].info = MIN_ARC_INFO-1;
    }
    return true;
}
/*
寻找图 G 关键路径
cEdge是长度不少于 G->arcnum 的 Edge 型数组
成功返回关键路径的长度, 失败返回 -1
*/
int CriticalPathInGraph_Matrix(MatrixGraph *G, Edge cEdge[]) {
    if (G->type == UDG) {
        return -1;
    }
    int n = 0, u, v;
    int indegree[G->vexnum];
    for (int i = 0; i < G->vexnum; indegree[i++] = 0);
    for (int i = 0; i < G->vexnum; i++) {
        v = FirstAdjacencyVertex_Matrix(G, i);
        if (v == -1) n++;//出度为零的结点个数
        while (v != -1) {
            indegree[v]++;
            v = NextAdjacencyVertex_Matrix(G, i, v);
        }
    }
    if (n != 1) {
        return -1;
    }
    int earliestTime[G->vexnum];
    int latestTime[G->vexnum];
    int inverseTopoSeq[G->vexnum];
    int queue[G->vexnum], rear = 0, front = 0;
    for (n = 0; n < G->vexnum; n++) {
        if (indegree[n] == 0) {
            queue[front++] = n; front%=G->vexnum;
        }
        //最早发生时间初始化
        earliestTime[n] = 0;
    }
    if (front != 1) {    //入度为零的结点应该只有一个
        return -1;
    }
    n = G->vexnum-1;
    while (rear != front) {
        u = queue[rear++]; rear%=G->vexnum;
        inverseTopoSeq[n--] = u;
        v = FirstAdjacencyVertex_Matrix(G, u);
        while (v != -1) {
            indegree[v]--;
            if (indegree[v] == 0) {
                queue[front++] = v; front%=G->vexnum;
            }
            if (earliestTime[v] < earliestTime[u] + G->arcs[u][v]) {
                earliestTime[v] = earliestTime[u] + G->arcs[u][v];
            }
            v = NextAdjacencyVertex_Matrix(G, u ,v);
        }
    }
    if (n != -1) {
        return -1;
    }
    for (n = 0; n < G->vexnum; n++) {
        latestTime[n] = earliestTime[G->vexnum-1];
    }
    for (n = 0; n < G->vexnum; n++) {
        u = inverseTopoSeq[n];
        v = FirstAdjacencyVertex_Matrix(G, u);
        while (v != -1) {
            if (latestTime[u] > latestTime[v] - G->arcs[u][v]) {
                latestTime[u] = latestTime[v] - G->arcs[u][v];
            }
            v  = NextAdjacencyVertex_Matrix(G, u, v);
        }
    }
    for (u = 0, n = 0; u < G->vexnum; u++) {
        if (earliestTime[u] == latestTime[u]) {
            v = FirstAdjacencyVertex_Matrix(G, u);
            while (v != -1) {
                if (earliestTime[v] == latestTime[v] &&
                    earliestTime[v] - earliestTime[u] == G->arcs[u][v]) {
                    cEdge[n].start = u;
                    cEdge[n].end = v;
                    cEdge[n].startVertex = G->vertex[u];
                    cEdge[n].endVertex = G->vertex[v];
                    cEdge[n].info = G->arcs[u][v];
                    n++;
                }
                v = NextAdjacencyVertex_Matrix(G, u, v);
            }
        }
    }
    return n;
}
/*
用 Dijkstra 算法计算图 G 中某结点到其他所有点的最短路径长度
distance 为长度不小于 G->vexnum 的数组
成功返回 true, 失败返回 false
*/
bool ShortestPath_Matrix_Dijkstra(MatrixGraph *G, VertexType u_node, int distance[]) {
    int u = LocateVertex_Matrix(G, u_node), v;
    if (u == -1) {
        return false;
    }
    for (int i = 0; i < G->vexnum; i++) {
        distance[i] = MAX_ARC_INFO+1;
    }
    distance[u] = 0;
    v = FirstAdjacencyVertex_Matrix(G, u);
    while (v != -1) {
        distance[v] = G->arcs[u][v];
        v = NextAdjacencyVertex_Matrix(G, u, v);
    }
    int nodesInU[G->vexnum];
    for (int i = 0; i < G->vexnum; nodesInU[i++] = 0);
    nodesInU[u] = 1;
    for (int i = 0; i < G->vexnum-1; i++) {
        int min = MAX_ARC_INFO+1;
        for (int j = 0; j < G->vexnum; j++) {
            if (distance[j] < min && nodesInU[j] == 0) {
                min = distance[j];
                v = j;
            }
        }
        if (min == MAX_ARC_INFO+1) {
            return true;
        }
        nodesInU[v] = 1;
        u = v;
        v = FirstAdjacencyVertex_Matrix(G, u);
        while (v != -1) {
            if (distance[v] > distance[u] + G->arcs[u][v]) {
                distance[v] = distance[u] + G->arcs[u][v];
            }
            v = NextAdjacencyVertex_Matrix(G, u, v);
        }
    }
    return true;
}
/*
用 Floyd 算法求图 G 中任二结点的最短路径及距离
distance 和 path 是大小为 G->vexnum * G->vexnum 的二维数组
distance[i][j] 为点 i 到点 j 的最短距离
path[i][j] 为 i, j 之间最短路径上, i 的后继点
*/
void ShortestPath_Matrix_Floyd(MatrixGraph *G, int distance[][G->vexnum], int path[][G->vexnum]) {
    for (int i = 0; i < G->vexnum; i++) {
        for (int j = 0; j < G->vexnum; j++) {
            distance[i][j] = G->arcs[i][j];
            path[i][j] = -1;
        }
    }
    for (int k = 0; k < G->vexnum; k++) {//结点i与结点j之间的最短路径经过结点k
        for (int i = 0; i < G->vexnum; i++) {
            for (int j = 0; j < G->vexnum; j++) {
                if (distance[i][j] > distance[i][k] + distance[k][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                    path[i][j] = k;
                }
            }
        }
    }
}
//函数 PrintPathAndDistance_Matrix_pro 的辅助函数
static void PrintPath_Matrix(MatrixGraph *G, int path[][G->vexnum], int i, int j) {
    if (path[i][j] == -1) {
        return ;
    }
    PrintPath_Matrix(G, path, i, path[i][j]);
    printf(" %c ->", G->vertex[path[i][j]]);
    PrintPath_Matrix(G, path, path[i][j], j);
}
//显示结点间最短路径与距离
void PrintPathAndDistance_Matrix_pro(MatrixGraph *G, int distance[][G->vexnum], int path[][G->vexnum]) {
    for (int i = 0; i < G->vexnum; i++) {
        for (int j = 0; j < G->vexnum; j++) {
            if (distance[i][j] == MAX_ARC_INFO+1 || i == j) {
                if (i != j) {
                    printf(" %c 到 %c 没有路径\n", G->vertex[i], G->vertex[j]);
                }
            }
            else {
                printf(" %c 到 %c 距离为 %d, ", G->vertex[i], G->vertex[j], distance[i][j]);
                printf("%c ->", G->vertex[i]);
                PrintPath_Matrix(G, path, i, j);
                printf(" %c\n", G->vertex[j]);
            }
        }
    }
}


void visit(VertexType *s) {
    printf("   %c", *s);
}

int main(void) {
    MatrixGraph g;
    MatrixGraph *G = &g;
    InitGraph_Matrix(&g, DG);
    for (int i = 0; i < MAX_VERTEX_NUM; i++) {
        InsertVertex_Matrix(&g, 'A'+i);
    }
    InsertArc_Matrix(&g, 'E', 'D', 12);
    InsertArc_Matrix(&g, 'E', 'C', 17);
    //InsertArc_Matrix(&g, 'D', 'C', 21);
    //DeleteArc_Matrix(G, 'D', 'C');
    InsertArc_Matrix(&g, 'D', 'B', 14);
    InsertArc_Matrix(G, 'C', 'B', 7);
    InsertArc_Matrix(G, 'B', 'A', 16);
    InsertArc_Matrix(G, 'A', 'E', 20);
    PrintGraph_Matrix(&g);
    printf("%d\n", G->arcnum);
    int n = MAX_VERTEX_NUM;
    /*
    DepthFirstSearchGraph_Matrix(G, visit);
    BreadthFirstSearchGraph_Matrix(G, visit);
    
    
    Edge edge[n];
    MiniSpanTree_Matrix_Prim(G, 'D', edge);
    for (int i = 0; i < n; i++) {
        printf("  %c(%d)%c", edge[i].startVertex, edge[i].info, edge[i].endVertex);
    }
    printf("\n");
    MiniSpanTree_Matrix_Kruskal(G, edge);
    for (int i = 0; i < n; i++) {
        printf("  %c(%d)%c", edge[i].startVertex, edge[i].info, edge[i].endVertex);
    }
    printf("\n");
*/
    int distance[G->vexnum];
    ShortestPath_Matrix_Dijkstra(G, 'C', distance);
    for (int i = 0; i < G->vexnum; i++) {
        printf("  %c(", G->vertex[i]);
        printf(distance[i]==MAX_ARC_INFO+1?"*)":"%d)", distance[i]);
    }
    printf("\n");

    int dis[n][n];
    int path[n][n];
    ShortestPath_Matrix_Floyd(G, dis, path);
    printf("Matrix Graph Shortest Distance:\n  |");
    for (int i = 0; i < G->vexnum; i++) {
        printf("%5c", G->vertex[i]);
    }
    printf("\n");
    for (int i = 0; i < G->vexnum; i++) {
        printf("------");
    }
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%2c|", G->vertex[i]);
        for (int j = 0; j < n; j++) {
            printf(dis[i][j]==MAX_ARC_INFO+1?"    *":"%5d",dis[i][j]);
        }
        printf("\n");
    }
    PrintPathAndDistance_Matrix_pro(G, dis, path);
    /*
    Edge cEdge[G->arcnum];
    int n = CriticalPathInGraph_Matrix(G, cEdge);
    for (int i = 0; i < n; i++) {
        printf("  %d(%d)%d", cEdge[i].startVertex, cEdge[i].info, cEdge[i].endVertex);
    }
    printf("\n");*/



    ListGraph gg;
    ListGraph *GG = &gg;
    InitGraph_List(&gg, DG);
    for (int i = 0; i < MAX_VERTEX_NUM; i++) {
        InsertVertex_List(&gg, 'A'+i);
    }
    InsertArc_List(&gg, 'E', 'D', 12);
    InsertArc_List(&gg, 'E', 'C', 17);
    //InsertArc_List(&gg, 'D', 'C', 21);
    //DeleteArc_List(GG, 'D', 'C');
    InsertArc_List(&gg, 'D', 'B', 14);
    InsertArc_List(GG, 'C', 'B', 7);
    InsertArc_List(GG, 'B', 'A', 16);
    InsertArc_List(GG, 'A', 'E', 20);
    PrintGraph_List(&gg);
    printf("%d\n", GG->arcnum);
    /*
    DepthFirstSearchGraph_List(GG, visit);
    BreadthFirstSearchGraph_List(GG, visit);
    
    
    Edge edge[n];
    MiniSpanTree_List_Prim(GG, 'D', edge);
    for (int i = 0; i < n; i++) {
        printf("  %c(%d)%c", edge[i].startVertex, edge[i].info, edge[i].endVertex);
    }
    printf("\n");
    MiniSpanTree_List_Kruskal(GG, edge);
    for (int i = 0; i < n; i++) {
        printf("  %c(%d)%c", edge[i].startVertex, edge[i].info, edge[i].endVertex);
    }
    printf("\n");
*/
    int distanceg[GG->vexnum];
    ShortestPath_List_Dijkstra(GG, 'C', distanceg);
    for (int i = 0; i < GG->vexnum; i++) {
        printf("  %c(", GG->vertex[i]);
        printf(distanceg[i]==MAX_ARC_INFO+1?"*)":"%d)", distanceg[i]);
    }
    printf("\n");

    
    ShortestPath_List_Floyd(GG, dis, path);
    printf("List Graph Shortest Distance:\n  |");
    for (int i = 0; i < GG->vexnum; i++) {
        printf("%5c", GG->vertex[i]);
    }
    printf("\n");
    for (int i = 0; i < GG->vexnum; i++) {
        printf("------");
    }
    printf("\n");
    for (int i = 0; i < n; i++) {
        printf("%2c|", GG->vertex[i]);
        for (int j = 0; j < n; j++) {
            printf(dis[i][j]==MAX_ARC_INFO+1?"    *":"%5d",dis[i][j]);
        }
        printf("\n");
    }
    PrintPathAndDistance_List_pro(GG, dis, path);    
    return 0;
}

