#include <stdlib.h>
#include <stdio.h>

#include "../include/bstree.h"
#include "../include/bintree.h"

#include <assert.h>
#include <limits.h>


typedef struct{
    int key;
    char * keyName;
    void * value;
}KeyValuePair;

struct __BSTree{
    BinTree * root;
    size_t count;
};


static KeyValuePair * Kvp_new(int key, void * value, char * keyName){
    KeyValuePair * self = malloc(sizeof(KeyValuePair));
    self->key = key;
    self->value = value;
    self->keyName = keyName;
    return self;
}

static void Kvp_free(KeyValuePair * self){
    free(self);
}

static BinTree * newNode(int key, void * value, char * keyName){
    KeyValuePair * pair = Kvp_new(key, value, keyName);
    BinTree * node =  BinTree_new(pair);
    return node;
}

static void freeNode(BinTree * self){
    Kvp_free(self->value);
    BinTree_free(self);
}

BSTree * BSTree_new(void){
    BSTree * self = malloc(sizeof(BSTree));
    self->root = NULL;
    self->count = 0;
    return self;
}

void BSTree_free(BSTree * self){
    free(self);
}

static void insert(BinTree * node, BinTree * child){
    int nodeKey = ((KeyValuePair *)node->value)->key;
    int childKey = ((KeyValuePair *)child->value)->key;
    if(childKey < nodeKey){
        if(node->left){
            insert(node->left, child);
        }else{
            node->left = child;
        }
    }else if(childKey > nodeKey){
        if(node->right){
            insert(node->right, child);
        }else{
            node->right = child;
        }
    }else{
        assert(0 && "Duplicated");
    }
}

void   BSTree_insert (BSTree * self, int key, void * value, char * keyName){
    BinTree * newChild = newNode(key, value, keyName);
    if(self->root == NULL){
        self->root = newChild;
    }else{
        insert(self->root, newChild);
    }
    self->count++;
}

static bool lookup(BinTree * node, int key){
    if(node == NULL) return false;
    int nodeKey = ((KeyValuePair *)node->value)->key;
    if(key < nodeKey){
        return lookup(node->left, key);
    }else if(key > nodeKey){
        return lookup(node->right, key);
    }else{
        return true;
    }
}

bool   BSTree_lookup   (BSTree * self, int key){
    return lookup(self->root, key);
}

static void * search(BinTree * node, int key){
    if(node == NULL) {
        assert(0 && "There is no such value in dictionary");
        return NULL;
    }
    int nodeKey = ((KeyValuePair *)node->value)->key;
    if(key < nodeKey){
        return search(node->left, key);
    }else if(key > nodeKey){
        return search(node->right, key);
    }else{
        return node->value;
    }
}

void * BSTree_search   (BSTree * self, int key){
    KeyValuePair * val =  search(self->root, key);
    if(val == NULL)return NULL;
    return val->value;
}

static KeyValuePair * delete(BinTree * node, int key){
    if(node == NULL) {
        assert(0 && "There is no such value in dictionary");
        return NULL;
    }
    BinTree * toRemove = NULL;
    if(node->right != NULL && ((KeyValuePair *)node->right->value)->key == key){
        toRemove = node->right;
    }
    if(node->left != NULL && ((KeyValuePair *)node->left->value)->key == key){
        toRemove = node->left;
    }
    if(toRemove != NULL){
        BinTree * newChild = NULL;
        if(toRemove->right != NULL && toRemove->left != NULL){

            BinTree * lastLeftNode = toRemove->left;
            BinTree * lastLeftNodeFather = toRemove;
            
            while(lastLeftNode->left != NULL){
                lastLeftNodeFather = lastLeftNode;
                lastLeftNode = lastLeftNode->left;
            }
            
            newChild = lastLeftNode;
            lastLeftNodeFather->left = lastLeftNode->right;
            lastLeftNode->right = toRemove->right;
            lastLeftNode->left = toRemove->left;
        }
        else if(toRemove->right != NULL && toRemove->left == NULL){
            newChild = toRemove->right;
        }
        else if(toRemove->right == NULL && toRemove->left != NULL){
            newChild = toRemove->left;
        }
        else{
            newChild = NULL;
        }
        if(toRemove == node->right) node->right = newChild;
        if(toRemove == node->left) node->left = newChild;
        void * val = toRemove->value;
        BinTree_free(toRemove);
        return val;
    }
    int nodeKey = ((KeyValuePair *)node->value)->key;
    if(key < nodeKey){
        return delete(node->left, key);
    }else if (key > nodeKey){
        return delete(node->right, key);
    }else{
        assert(0);
        return NULL;
    }
}

static void * BSTree_deleteKVP(BSTree * self, int key){
    BinTree * fictRoot = BinTree_new(NULL);
    KeyValuePair * val = NULL;
    if(((KeyValuePair *)self->root->value)->key == key){
        fictRoot->right = self->root;
        val = delete(fictRoot, key);
        self->root = fictRoot->right;
        
    }else{
        val = delete(self->root, key);
    }
    BinTree_free(fictRoot);
    if(val) self->count--;
    return val;
}

void * BSTree_delete(BSTree * self, int key){
    KeyValuePair * kvp =  BSTree_deleteKVP(self, key);
    void * val = kvp->value;
    Kvp_free(kvp);
    return val;
}

static void * set(BinTree * node, int key, void * value){
    if(node == NULL) {
        assert(0 && "There is no such value in dictionary");
        return NULL;
    }
    int nodeKey = ((KeyValuePair *)node->value)->key;
    if(key < nodeKey){
        return set(node->left, key, value);
    }else if(key > nodeKey){
        return set(node->right, key, value);
    }else{
        void * oldValue = ((KeyValuePair *)node->value)->value;
        ((KeyValuePair *)node->value)->value = value;
        return oldValue;
    }
}

void * BSTree_set(BSTree * self, int key, void * value){
    return set(self->root, key, value);
}


static void clear(BinTree * node){
    if(node->left) clear(node->left);
    if(node->right) clear(node->right);
    node->right = NULL;
    node->left = NULL;
    if(node->value) Kvp_free(node->value);
    node->value = NULL;
    BinTree_free(node);
}

void BSTree_clear(BSTree * self){
    if(self->root){
        clear(self->root);
        self->root = NULL;
    }
    self->count = 0;
}

size_t BSTree_count    (BSTree * self){
    return self->count;
}

static void inOrderCopyKeys(BinTree * node, List * keys){
    if(node == NULL) return;
    if(node->left) inOrderCopyKeys(node->left, keys);
    List_add(keys, ((KeyValuePair *)node->value)->keyName);
    if(node->right) inOrderCopyKeys(node->right, keys);
}

void   BSTree_keys     (BSTree * self, List * keys){
    assert(keys != NULL);
    if(self->root){
        inOrderCopyKeys(self->root, keys);
    }
}

static void inOrderCopyValues(BinTree * node, List * values){
    if(node == NULL) return;
    if(node->left) inOrderCopyValues(node->left, values);
    List_add(values, ((KeyValuePair *)node->value)->value);
    if(node->right) inOrderCopyValues(node->right, values);
}

void   BSTree_values(BSTree * self, List * values){
    assert(values != NULL);
    if(self->root){
        inOrderCopyValues(self->root, values);
    }
}