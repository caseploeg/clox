#include <stdlib.h>

#include "memory.h"
#include "vm.h"
#include "compiler.h"

#ifdef DEBUG_LOG_GC
#include <stdio.h>
#include "debug.h"
#endif


static void freeObject(Obj* object) {
#ifdef DEBUG_LOG_GC
    printf("%p free type %d\n", (void*)object, object->type);
#endif
    switch (object->type) {
        case OBJ_STRING: {
            ObjString* string = (ObjString*)object;
            FREE_ARRAY(char, string->chars, string->length + 1);
            FREE(ObjString, object);
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            freeChunk(&function->chunk);
            FREE(ObjFunction, object);
            break;
        }
        case OBJ_NATIVE: {
            FREE(ObjNative, object);
            break;
        }
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            FREE(ObjClosure, object);
            FREE_ARRAY(ObjUpValue*, closure->upvalues, closure->upvalueCount);
            break;
        }
        case OBJ_UPVALUE: {
            FREE(ObjUpValue, object);
            break;
        }
    }
}


void markObject(Obj* object) {
    if (object == NULL) return;
    if (object->isMarked) return;

    object->isMarked = true;

    if (vm.grayCapacity < vm.grayCount + 1) {
        vm.grayCapacity = GROW_CAPACITY(vm.grayCapacity);
        vm.grayStack = (Obj**)realloc(vm.grayStack, sizeof(Obj*) * vm.grayCapacity);

        if (vm.grayStack == NULL) exit(1);
    }
    vm.grayStack[vm.grayCount++] = object;
}
static void markArray(ValueArray* array) {
    for (int i = 0; i < array->count; i++) {
        markValue(array->values[i]);
    }
}

static void blackenObject(Obj* object) {
    #ifdef DEBUG_LOG_GC
    printf("%p blacken", (void*)object);
    printValue(OBJ_VAL(object));
    printf("\n");
    #endif 
    switch (object->type) {
        case OBJ_CLOSURE: {
            ObjClosure* closure = (ObjClosure*)object;
            markObject((Obj*)closure->function);
            for (int i = 0; i < closure->upvalueCount; i++) {
                markObject((Obj*)closure->upvalues[i]);
            }
            break;
        }
        case OBJ_FUNCTION: {
            ObjFunction* function = (ObjFunction*)object;
            markObject((Obj*)function->name);
            markArray(&function->chunk.constants);
            break;
        }
        case OBJ_UPVALUE:
            markValue(((ObjUpValue*)object)->closed); 
            break;
        case OBJ_NATIVE:
        case OBJ_STRING:
            break;
    }
}

static void traceReferences() {
    while (vm.grayCount > 0) {
        Obj* object = vm.grayStack[--vm.grayCount];
        blackenObject(object);
    }
}

void markValue(Value value) {
    // some Lox values are stored directly inline in the Value and
    // don't require heap allocation. The GC can ignore those, so we
    // only do real work for Objects
    if (IS_OBJ(value)) markObject(AS_OBJ(value));
}

static void markRoots() {
    // most roots are local variables or temporaries on the stack,
    // so start by walking the stack
    for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
        markValue(*slot);
    }

    // also keep track of closures
    for (int i = 0; i < vm.frameCount; i++) {
        markObject((Obj*)vm.frames[i].closure);
    }

    // also keep track of UpValues
    for (ObjUpValue* upvalue = vm.openUpvalues; upvalue != NULL; upvalue = upvalue->next) {
        markObject((Obj*)upvalue);
    }

    markTable(&vm.globals);
    markCompilerRoots();
}

// single-pass sweep through the object "linked-list",
// free all unmarked objects,
// unmark the marked objects for next time 
static void sweep() {
    Obj* previous = NULL;
    Obj* object = vm.objects;
    while (object != NULL) {
        if (object->isMarked) {
            object->isMarked = false;
            previous = object;
            object = object->next;
        } else {
            Obj* unreached = object;
            object = object->next;
            if (previous != NULL) {
                previous->next = object;
            } else {
                vm.objects = object;
            }

            freeObject(unreached);
        }
    }
}

#define GC_HEAP_GROW_FACTOR 2

void collectGarbage() {
#ifdef DEBUG_LOG_GC
    printf("-- gc begin\n");
    size_t before = vm.bytesAllocated;
#endif

    markRoots();
    traceReferences();
    tableRemoveWhite(&vm.strings);
    sweep();

    vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
    printf("-- gc end\n");
    printf(" collected %zu bytes (from %zu to %zu) next at %zu\n",
            before - vm.bytesAllocated, before, vm.bytesAllocated, vm.nextGC);
#endif
}

void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
    vm.bytesAllocated += newSize - oldSize;
    if (newSize > oldSize) {
        // very bad for performance, but will test
        // GC collector works during any possible
        // reallocation
        #ifdef DEBUG_STRESS_GC
            collectGarbage();
        #endif

        if (vm.bytesAllocated > vm.nextGC) {
            collectGarbage();
        }
    }
    if (newSize == 0) {
        free(pointer);
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    // when realloc fails we exit early
    if (result == NULL) exit(1);
    return result;
}


void freeObjects() {
    Obj* object = vm.objects;
    while (object != NULL) {
        Obj* next = object->next;
        freeObject(object);
        object = next;
    }
    free(vm.grayStack);
}
