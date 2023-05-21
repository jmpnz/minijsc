#include <stdio.h>
#include <stdlib.h>

// In minijsc (RIP) values are all heap allocated in a heap, one might
// use std::vector<std::shared_ptr<JSValue>> and call reset() on each
// object we want to deallocate, hopefully we want shoot our legs off
// while doing this.
// What's an object in use ?
// 1. Referenced in a scope by a variable
// 2. Referenced by another in-use object
// Let's start by declaring a few object types, ints and pairs of ints.
#define MaxStackSize 256
#define MaxObjBeforeGC 8

typedef enum { ObjInt, ObjPair } ObjectType;

typedef struct sObject {
    ObjectType type;
    unsigned char marked;

    // intrusive list to the next allocated object
    struct sObject* next;

    union {
        int value;

        struct {
            struct sObject* first;
            struct sObject* second;
        };
    };
} Object;

typedef struct {
    Object* stack[MaxStackSize];
    int stackSize;

    // first object allocated by the vm
    Object* firstObject;
    // total allocated objects
    int numObjects;
    // max objects before we cleanup
    int maxObjects;
} VM;

void assert(int condition, const char* message) {
    if (!condition) {
        printf("%s\n", message);
        exit(1);
    }
}

VM* newVM() {
    VM* vm          = malloc(sizeof(VM));
    vm->stackSize   = 0;
    vm->firstObject = NULL;
    vm->numObjects  = 0;
    vm->maxObjects  = MaxObjBeforeGC;
    return vm;
}

void push(VM* vm, Object* value) {
    assert(vm->stackSize < MaxStackSize, "Stack overflow!");
    vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
    assert(vm->stackSize > 0, "Stack underflow!");
    return vm->stack[--vm->stackSize];
}

void mark(Object* object) {
    // return early to avoid infinite recursive cycles
    if (object->marked)
        return;

    object->marked = 1;

    if (object->type == ObjPair) {
        mark(object->first);
        mark(object->second);
    }
}

void markAll(VM* vm) {
    for (int i = 0; i < vm->stackSize; i++) {
        mark(vm->stack[i]);
    }
}

void sweep(VM* vm) {
    Object** object = &vm->firstObject;
    while (*object) {
        if (!(*object)->marked) {
            // This object wasn't reached, so remove it from the list and free it.
            Object* unreached = *object;

            *object = unreached->next;
            free(unreached);

            vm->numObjects--;
        } else {
            // This object was reached, so unmark it and move on.
            (*object)->marked = 0;
            object            = &(*object)->next;
        }
    }
}

void gc(VM* vm) {
    int numObjects = vm->numObjects;

    markAll(vm);
    sweep(vm);

    vm->maxObjects = vm->numObjects == 0 ? MaxObjBeforeGC : vm->numObjects * 2;

    printf("Collected %d objects, %d remaining.\n", numObjects - vm->numObjects,
           vm->numObjects);
}

Object* newObject(VM* vm, ObjectType type) {
    if (vm->numObjects == vm->maxObjects)
        gc(vm);

    Object* object  = malloc(sizeof(Object));
    object->type    = type;
    object->next    = vm->firstObject;
    vm->firstObject = object;
    object->marked  = 0;

    vm->numObjects++;

    return object;
}

void pushInt(VM* vm, int intValue) {
    Object* object = newObject(vm, ObjInt);
    object->value  = intValue;

    push(vm, object);
}

Object* pushPair(VM* vm) {
    Object* object = newObject(vm, ObjPair);
    object->second = pop(vm);
    object->first  = pop(vm);

    push(vm, object);
    return object;
}

void objectPrint(Object* object) {
    switch (object->type) {
    case ObjInt:
        printf("%d", object->value);
        break;

    case ObjPair:
        printf("(");
        objectPrint(object->first);
        printf(", ");
        objectPrint(object->second);
        printf(")");
        break;
    }
}

void freeVM(VM* vm) {
    vm->stackSize = 0;
    gc(vm);
    free(vm);
}

void testNoCleanup() {
    printf("Test 1: Objects on stack are preserved.\n");
    VM* vm = newVM();
    pushInt(vm, 1);
    pushInt(vm, 2);

    gc(vm);
    assert(vm->numObjects == 2, "Should have preserved objects.");
    freeVM(vm);
}

void testCleanupSimple() {
    printf("Test 2: Unreached objects are collected.\n");
    VM* vm = newVM();
    pushInt(vm, 1);
    pushInt(vm, 2);
    pop(vm);
    pop(vm);

    gc(vm);
    assert(vm->numObjects == 0, "Should have collected objects.");
    freeVM(vm);
}

void testCleanupNested() {
    printf("Test 3: Reach nested objects.\n");
    VM* vm = newVM();
    pushInt(vm, 1);
    pushInt(vm, 2);
    pushPair(vm);
    pushInt(vm, 3);
    pushInt(vm, 4);
    pushPair(vm);
    pushPair(vm);

    gc(vm);
    assert(vm->numObjects == 7, "Should have reached objects.");
    freeVM(vm);
}

void testCleanupCycle() {
    printf("Test 4: Handle cycles.\n");
    VM* vm = newVM();
    pushInt(vm, 1);
    pushInt(vm, 2);
    Object* a = pushPair(vm);
    pushInt(vm, 3);
    pushInt(vm, 4);
    Object* b = pushPair(vm);

    /* Set up a cycle, and also make 2 and 4 unreachable and collectible. */
    a->second = b;
    b->second = a;

    gc(vm);
    assert(vm->numObjects == 4, "Should have collected objects.");
    freeVM(vm);
}

void stopTheWorld1000() {
    printf("Performance Test.\n");
    VM* vm = newVM();

    for (int i = 0; i < 100000; i++) {
        for (int j = 0; j < 200; j++) {
            pushInt(vm, i);
        }

        for (int k = 0; k < 200; k++) {
            pop(vm);
        }
    }
    freeVM(vm);
}

int main(int argc, const char* argv[]) {
    testNoCleanup();
    testCleanupSimple();
    testCleanupNested();
    testCleanupCycle();
    stopTheWorld1000();

    return 0;
}
