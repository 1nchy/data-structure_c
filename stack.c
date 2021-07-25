#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Node {
    void* pData;
    struct Node* pNextNode;
} Node;

typedef struct {
    int length;
    size_t dataSize;
    struct Node* pHeadNode;
} Stack;
/*
栈初始化
pStack 是指向待初始化的栈的指针, dataSize 为栈存储数据的大小
*/
void InitStack(Stack* pStack, size_t dataSize) {
    if (pStack == NULL || dataSize <= 0) {
        printf("***InitStack Error!***\n");
        return;
    }
    pStack->length = 0;
    pStack->dataSize = dataSize;
    pStack->pHeadNode = NULL;
}
/*
创建一个与栈 pStack 匹配的结点
*/
static struct Node* CreateNode(Stack* pStack, void* pData) {
    struct Node* p = (struct Node*)malloc(sizeof(struct Node));
    p->pData = (char*)malloc(sizeof(pData));
    if (p == NULL || p->pData == NULL) {
        printf("***Memory allocation error!***\n");
        return NULL;
    }
    char* pt = (char*)pData;
    memcpy(p->pData, pt, pStack->dataSize);
    return p;
}
//栈是否为空
bool IsStackEmpty(Stack* pStack) {
    if (pStack->pHeadNode == NULL || pStack->length == 0) {
        return true;
    }
    else {
        return false;
    }
}
//将元素压入栈顶
bool PushIntoStack(Stack* pStack, void* pData) {
    struct Node* p = CreateNode(pStack, pData);
    if (p == NULL) {
        return false;
    }
    p->pNextNode = pStack->pHeadNode;
    pStack->pHeadNode = p;

    pStack->length++;
    return true;
}
/*
将元素压到栈的某个位置
order 取值从 0 到栈的长度
*/
bool PushIntoMidst(Stack* pStack, void* pData, int order) {
    if (0 > order || order > pStack->length) {
        return false;
    }
    struct Node* p = CreateNode(pStack, pData);
    if (p == NULL) {
        return false;
    }
    struct Node* cur = pStack->pHeadNode;
    struct Node* pre = NULL;
    while (order--) {
        pre = cur;
        cur = cur->pNextNode;
    }
    if (pre ==NULL) {
        p->pNextNode = pStack->pHeadNode;
        pStack->pHeadNode = p;
    }
    else {
        p->pNextNode = pre->pNextNode;
        pre->pNextNode = p;
    }
    pStack->length++;
    return true;
}

bool GetFromStack(Stack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pHeadNode->pData;
    memcpy(p, tmp, pStack->dataSize);
    return true;
}

bool GetFromMidst(Stack* pStack, void* pReception, int order) {
    if (IsStackEmpty(pStack) == true || order <= 0 || order > pStack->length) {
        return false;
    }
    struct Node* cur = pStack->pHeadNode;
    while (--order) {
        cur = cur->pNextNode;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)cur->pData;
    memcpy(p, tmp, pStack->dataSize);
    return true;
}

bool PopFromStack(Stack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pHeadNode->pData;
    memcpy(p, tmp, pStack->dataSize);

    struct Node* tempNode = pStack->pHeadNode;
    pStack->pHeadNode = tempNode->pNextNode;
    free(tempNode->pData);
    free(tempNode);

    pStack->length--;
    return true;
}

bool PopFromMidst(Stack* pStack, void* pReception, int order) {
    if (IsStackEmpty(pStack) == true || order <= 0 || order > pStack->length) {
        return false;
    }
    struct Node* cur = pStack->pHeadNode;
    struct Node* pre = NULL;
    while (--order) {
        pre = cur;
        cur = cur->pNextNode;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)cur;
    memcpy(p, tmp, pStack->dataSize);
    if (pre == NULL) {
        pStack->pHeadNode = cur->pNextNode;
    }
    else {
        pre->pNextNode = cur->pNextNode;
    }
    free(cur->pData);
    free(cur);
    pStack->length--;
    return true;
}

bool ReverseStack(Stack* pStack) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    struct Node* cur = pStack->pHeadNode;
    struct Node* pre = NULL;
    struct Node* temp = cur->pNextNode;
    while (temp != NULL) {
        temp = cur->pNextNode;
        cur->pNextNode = pre;
        pre = cur;
        cur = temp;
    }
    pStack->pHeadNode = pre;
    return true;
}

void DestroyStack(Stack* pStack) {
    struct Node* p;
    while(pStack->pHeadNode != NULL) {
        p = pStack->pHeadNode;
        pStack->pHeadNode = p->pNextNode;
        free(p->pData);
        free(p);
    }
    pStack->length = 0;
}

int LengthOfStack(Stack* pStack) {
    return pStack->length;
}