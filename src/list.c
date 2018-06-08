#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "../include/list.h"
#include "../include/iterator.h"


struct __List{
    void ** items;
    int capasity;
    int length;
};


List * List_new(void){
    List * self = malloc(sizeof(List));
    if(self == NULL){
        fprintf(stderr, "can not allocate memory\n");        
        abort();
    }
    self->length = 0;
    self->capasity = 16;
    self->items = malloc(sizeof(void *) * self->capasity);
    if(self->items == NULL){
        fprintf(stderr, "can not allocate memory\n");
        abort();
    }
    return self;
}

void List_free(List * self){
    if(self)free(self->items);
    free(self);
}


void List_add(List * self, void * value){
    if(self->length >= self->capasity){
        self->items = realloc(self->items, (self->capasity * 2) * sizeof(void *));
        if(self->items == NULL){
            fprintf(stderr, "can not allocate memory\n");
            abort();
        }
        self->capasity *= 2;
    }
    self->items[self->length] = value;
    self->length++;
}

void List_insert(List * self, size_t index, void * value){
    assert(index <= self->length);
    if(index == self->length){
        List_add(self, value);
        return;
    }
    if(index < self->length){
        if(self->length >= self->capasity){
            self->items = realloc(self->items, (self->capasity * 2) * sizeof(void *));
            if(self->items == NULL){
                fprintf(stderr, "can not allocate memory\n");
                abort();
            }
            self->capasity *= 2;
        }
        for(int i = self->length; i > index; i--){
            self->items[i] = self->items[i - 1];
        }
        self->items[index] = value;
        self->length++;
    }
    else{
        fprintf(stderr, "incorrect index\n");
    }

}

void * List_at(List * self, size_t index){
    assert(index < self->length);
    if(index < self->length){
        return self->items[index];
    }
    fprintf(stderr, "incorrect index\n");
    return 0;
}

void List_set(List * self, size_t index, void * value){
    assert(index < self->length);    
    if(index < self->length){
        self->items[index] = value;
    }
    else{
        fprintf(stderr, "incorrect index\n");
    }
}

bool List_removeAt(List * self, size_t index){
    assert(index < self->length);    
    if(index < self->length){
        for(int i = index; i < self->length - 1; i++){
            self->items[i] = self->items[i + 1];
        }
        self->length--;
        if(self->length < self->capasity / 3 && self->length >= 32){
            self->items = realloc(self->items, (self->capasity / 2) * sizeof(void *));
            if(self->items == NULL){
                fprintf(stderr, "can not allocate memory\n");
                abort();
            }
            self->capasity /= 2;
        }
        return true;
    }
    fprintf(stderr, "incorrect index\n");
    return false;
}

size_t List_count(List * self){
    return self->length;
}

void List_clear(List * self){
    self->length = 0;
    self->capasity = 16;
    self->items = realloc(self->items, sizeof(void *) * self->capasity);
    if(self->items == NULL){
        fprintf(stderr, "can not allocate memory\n");
        abort();
    }
}

//Iterator

struct __Iterator {
    List * list;
    IteratorDistance index;
};

static Iterator * Iterator_New(List * list, IteratorDistance index){
    Iterator * self = malloc(sizeof(Iterator));
    self->list = list;
    self->index = index;
    return self;
}

Iterator * List_getNewBeginIterator (List * self){
    return Iterator_New(self, 0);
}

Iterator * List_getNewEndIterator   (List * self){
    return Iterator_New(self, List_count(self));
}