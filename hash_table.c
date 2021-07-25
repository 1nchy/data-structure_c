#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    HASH_OK,
    HASH_ERROR,
    HASH_ADDED,
    HASH_REPLACED_VALUE,
    HASH_ALREADY_ADDED,
    HASH_DELETED,
    HASH_FOUND,
    HASH_NOT_FOUND,
    HASH_END
} HASH_RESULT;

typedef struct __HashEntry HashEntry;
struct __HashEntry {
    union{
        char  *str_value;
        double dbl_value;
        int    int_value;
    } key;
    union{
        char  *str_value;
        double dbl_value;
        int    int_value;
        long   long_value;
        void  *ptr_value;
    } value;
    HashEntry *next;
};

typedef struct __HashTable HashTable;
struct __HashTable {
    HashEntry **bucket;        
    int size;
    HASH_RESULT last_error;
};
/*
 * HASH_RESULT 类型为相关函数的返回类型
 * HashEntry 为哈希表所保存元素（即键值对 <key, value> ）类型
 * HashTable 为哈希表，其中 bucket 指向大小为size的、元素类型为 HashEntry*的指针数组
*/

/*哈希函数*/
static long hash_string(const char *str) {
    long hash = 5381;
    int c;
    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    if(hash < 0)
        hash *= -1;
    return hash;
}
//创建大小为 size 的哈希表，创建成功后返回 HashTable 则返回 NULL
HashTable *HashCreate(int size) {
    HashTable* table = (HashTable*)malloc(sizeof(HashTable));
    if (table == NULL) {
        return NULL;
    }
    table->bucket = (HashEntry**)malloc(size * sizeof(HashEntry*));
    if (table->bucket == NULL) {
        free(table);
        return NULL;
    }
    memset(table->bucket, 0, size);
    table->size = size;
    table->last_error = HASH_OK;
    return table;
}
//向哈希表中添加元素,键类型为 char* ,元素类型为 int
HASH_RESULT HashInsert(HashTable *table, const char *key, int value) {
    long location = hash_string(key) % table->size;//关键字key所对应的bucket数组位置
    //查看关键字是否在哈希表中,若存在则直接修改
    HashEntry *p = table->bucket[location];
    while (p != NULL) {
        if (!strcmp(p->key.str_value, key)) {
            if (p->value.int_value == value) {
                table->last_error = HASH_ALREADY_ADDED;
                return table->last_error;
            }
            p->value.int_value = value;
            table->last_error = HASH_REPLACED_VALUE;
            return table->last_error;
        }
        p = p->next;
    }
    //若关键字不在哈希表中,则申请内存
    p = (HashEntry*)malloc(sizeof(HashEntry));
    if (p == NULL) {
        table->last_error = HASH_ERROR;
        return table->last_error;
    }
    //为key.str_value分配内存
    int key_length = 0;
    while (*(key+key_length) != 0) key_length++;
    p->key.str_value = (char*)malloc((key_length+1) * sizeof(char));//为空字符分配内存
    if (p->key.str_value == NULL) {
        free(p);
        table->last_error = HASH_ERROR;
        return table->last_error;
    }
    strncpy(p->key.str_value, key, key_length+1);
    //p->key.str_value = (char*)key;
    p->value.int_value = value;
    p->next = table->bucket[location];
    table->bucket[location] = p;
    table->last_error = HASH_ADDED;
    return table->last_error;
}
//删除哈希表中的元素,键类型为 char*
HASH_RESULT HashDelete(HashTable *table, const char *key) {
    long location = hash_string(key) % table->size;
    HashEntry *p = table->bucket[location];
    HashEntry *pre = NULL;
    while (p != NULL) {
        if (!strcmp(p->key.str_value, key)) {
            if (pre == NULL) {
                table->bucket[location] = p->next;
            }
            else {
                pre->next = p->next;
            }
            free(p->key.str_value);
            free(p);
            table->last_error = HASH_DELETED;
            return table->last_error;
        }
        pre = p;
        p = p->next;
    }
    table->last_error = HASH_NOT_FOUND;
    return table->last_error;
}
//查找哈希表中的元素,键类型为 char* ,将元素值保存到 value 中
HASH_RESULT HashSearch(HashTable *table, const char *key, int *value) {
    long location = hash_string(key) % table->size;
    HashEntry *p = table->bucket[location];
    while (p != NULL) {
        if (!strcmp(p->key.str_value, key)) {
            *value = p->value.int_value;
            table->last_error = HASH_FOUND;
            return table->last_error;
        }
        p = p->next;
    }
    table->last_error = HASH_NOT_FOUND;
    return table->last_error;
}
//销毁哈希表
HASH_RESULT HashDestory(HashTable **table) {
    HashEntry *p, *pre;
    for (int i = 0; i < (*table)->size; i++) {
        p = (*table)->bucket[i];
        while (p != NULL) {
            pre = p;
            p = p->next;
            free(pre->key.str_value);
            free(pre);
        }
    }
    free((*table)->bucket);
    free(*table);
    (*table) = NULL;
    return HASH_END;
}
//打印哈希表
void HashPrint(HashTable *table) {
    HashEntry *p = NULL;
    printf("#Hash Table\n");
    if (table == NULL || table->bucket == NULL) {
        return;
    }
    for (int i = 0; i < table->size; i++) {
        printf("%2d|", i);
        p = table->bucket[i];
        while (p != NULL) {
            printf(" -> %3d", p->value);
            p = p->next;
        }
        printf("\n");
    }
}

int main(void) {
    HashTable *table = HashCreate(5);
    HashInsert(table, "five", 5);
    HashInsert(table, "six", 6);
    HashInsert(table, "one", 1);
    HashInsert(table, "three", 3);
    HashInsert(table, "two", 2);
    HashInsert(table, "four", 4);
    HashInsert(table, "eight", 8);
    HashInsert(table, "seven", 7);
    HashInsert(table, "ten", 10);
    HashInsert(table, "nine", 9);
    HashPrint(table);
    HashDelete(table, "three");
    HashDelete(table, "ten");
    HashPrint(table);
    HashDestory(&table);
    HashPrint(table);
    return 0;
}