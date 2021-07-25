#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node {
    void* pData;
    struct Node* pNextNode;
    struct Node* pFormerNode;
} Node;

typedef struct {
    int length;
    size_t dataSize;
    struct Node* pTopNode;
    struct Node* pBaseNode;
} Destack;

void InitStack(Destack* pStack, size_t dataSize) {
    if (pStack == NULL || dataSize <= 0) {
        printf("***InitStack Error!***\n");
        return;
    }
    pStack->length = 0;
    pStack->dataSize = dataSize;
    pStack->pTopNode = NULL;
    pStack->pBaseNode = NULL;
}

static struct Node* CreateNode(Destack* pStack, void* pData) {
    struct Node* p = (struct Node*)malloc(sizeof(struct Node));
    p->pData = (char*)malloc(sizeof(pData));
    if (p == NULL || p->pData == NULL) {
        printf("***Memory allocation error!***\n");
        return NULL;
    }
    char* pt = (char*)pData;
    memcpy(p->pData, pt, pStack->dataSize);
    p->pFormerNode = NULL;
    p->pNextNode = NULL;
    return p;
}

bool IsStackEmpty(Destack* pStack) {
    if (pStack->pTopNode == NULL && pStack->pBaseNode == NULL) {
        return true;
    }
    else {
        return false;
    }
}

bool PushIntoTop(Destack* pStack, void* pData) {
    struct Node* p = CreateNode(pStack, pData);
    if (p == NULL) {
        return false;
    }

    p->pNextNode = pStack->pTopNode;
    if (pStack->pTopNode != NULL) {
        pStack->pTopNode->pFormerNode = p;
    }
    pStack->pTopNode = p;

    if (pStack->pBaseNode == NULL) {
        pStack->pBaseNode = p;
    }

    pStack->length++;
    return true;
}

bool PushIntoMidst(Destack* pStack, void* pData, int order) {
    if (0 > order || order > pStack->length) {
        return false;
    }
    struct Node* p = CreateNode(pStack, pData);
    if (p == NULL) {
        return false;
    }
    struct Node* cur = pStack->pTopNode;
    struct Node* pre = NULL;
    while (order--) {
        pre = cur;
        cur = cur->pNextNode;
    }
    if (pre == NULL) {
        p->pNextNode = pStack->pTopNode;
        if (pStack->pTopNode != NULL) {
            pStack->pTopNode->pFormerNode = p;
        }
        pStack->pTopNode = p;
        if (pStack->pBaseNode == NULL) {
            pStack->pBaseNode = p;
        }
    }
    else if (cur == NULL) {
        p->pFormerNode = pStack->pBaseNode;
        if (pStack->pBaseNode != NULL) {
            pStack->pBaseNode->pNextNode = p;
        }
        pStack->pBaseNode = p;
        if (pStack->pTopNode == NULL) {
            pStack->pTopNode = p;
        }
    }
    else {
        p->pNextNode = cur;
        p->pFormerNode = pre;
        cur->pFormerNode = p;
        pre->pNextNode = p;
    }
    pStack->length++;
    return true;
}

bool PushIntoBase(Destack* pStack, void* pData) {
    struct Node* p = CreateNode(pStack, pData);
    if (p == NULL) {
        return false;
    }

    p->pFormerNode = pStack->pBaseNode;
    if (pStack->pBaseNode != NULL) {
        pStack->pBaseNode->pNextNode = p;
    }
    pStack->pBaseNode = p;

    if (pStack->pTopNode == NULL) {
        pStack->pTopNode = p;
    }

    pStack->length++;
    return true;
}

bool GetFromTop(Destack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pTopNode->pData;
    memcpy(p, tmp, pStack->dataSize);
    return true;
}

bool GetFromMidst(Destack* pStack, void* pReception, int order) {
    if (IsStackEmpty(pStack) == true || order <= 0 || order > pStack->length) {
        return false;
    }
    struct Node* cur = pStack->pTopNode;
    while (--order) {
        cur = cur->pNextNode;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)cur->pData;
    memcpy(p, tmp, pStack->dataSize);
    return true;
}

bool GetFromBase(Destack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pBaseNode->pData;
    memcpy(p, tmp, pStack->dataSize);
    return true;
}

bool PopFromTop(Destack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true){
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pTopNode->pData;
    memcpy(p, tmp, pStack->dataSize);

    struct Node* tempNode = pStack->pTopNode;
    pStack->pTopNode = tempNode->pNextNode;
    if (tempNode->pNextNode != NULL) {
        tempNode->pNextNode->pFormerNode = NULL;
    }
    free(tempNode->pData);
    free(tempNode);

    pStack->length--;
    return true;
}

bool PopFromMidst(Destack* pStack, void* pReception, int order) {
    if (IsStackEmpty(pStack) == true || order <= 0 || order > pStack->length) {
        return false;
    }
    struct Node* cur = pStack->pTopNode;
    struct Node* pre = NULL;
    while (--order) {
        pre = cur;
        cur = cur->pNextNode;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)cur;
    memcpy(p, tmp, pStack->dataSize);
    if (pre == NULL) {
        pStack->pTopNode = cur->pNextNode;
        if (cur->pNextNode != NULL) {
            cur->pNextNode->pFormerNode = NULL;
        }
    }
    else {
        pre->pNextNode = cur->pNextNode;
        if (cur->pNextNode == NULL) {
            pStack->pBaseNode = pre;
        }
        else {
            cur->pNextNode->pFormerNode = pre;
        }
    }
    free(cur->pData);
    free(cur);
    pStack->length--;
    return true;
}

bool PopFromBase(Destack* pStack, void* pReception) {
    if (IsStackEmpty(pStack) == true){
        return false;
    }
    char* p = (char*)pReception;
    char* tmp = (char*)pStack->pBaseNode->pData;
    memcpy(p, tmp, pStack->dataSize);

    struct Node* tempNode = pStack->pBaseNode;
    pStack->pBaseNode = tempNode->pFormerNode;
    if (tempNode->pFormerNode != NULL) {
        tempNode->pFormerNode->pNextNode = NULL;
    }
    free(tempNode->pData);
    free(tempNode);

    pStack->length--;
    return true;
}

bool ReverseStack(Destack* pStack) {
    if (IsStackEmpty(pStack) == true) {
        return false;
    }
    struct Node* temp = pStack->pBaseNode;
    pStack->pBaseNode = pStack->pTopNode;
    pStack->pTopNode = temp;
    return true;
}

void DestroyStack(Destack* pStack) {
    struct Node* p;
    while(pStack->pTopNode != NULL) {
        p = pStack->pTopNode;
        pStack->pTopNode = p->pNextNode;
        free(p->pData);
        free(p);
    }
    pStack->length = 0;
    pStack->pBaseNode = NULL;
}

int LengthOfStack(Destack* pStack) {
    return pStack->length;
}