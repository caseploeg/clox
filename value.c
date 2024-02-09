#include <stdio.h>
#include "memory.h"
#include "value.h"
#include "object.h"


void initValueArray(ValueArray* array) {
    array->values = NULL;
    array->count = 0;
    array->capacity = 0;
}

void writeValueArray(ValueArray* array, Value value) {
    // if this is true, the newly added value will put
    // the array past max capacity
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        // these macros are reusable across our different
        // array implementations
        array->capacity = GROW_CAPACITY(oldCapacity);
        // Value type passed as an argument
        array->values = GROW_ARRAY(Value, array->values,
                                    oldCapacity, array->capacity);
    }
    array->values[array->count] = value;
    array->count++;
}

void freeValueArray(ValueArray* array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    initValueArray(array);
}

void printValue(Value value) {
    switch(value.type) {
        case VAL_BOOL:
            printf(AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NUMBER:
            printf("%g", AS_NUMBER(value));
            break;
        case VAL_NIL:
            printf("nil");
            break;
        case VAL_OBJ:
            printObject(value);
            break;
        default:
            printf("unknown type, can't print value");
            break;
    }
}
