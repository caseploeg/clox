#ifndef clox_object_h
#define clox_object_h

#include "common.h"
#include "value.h"
#include "chunk.h"

#define OBJ_TYPE(value)     (AS_OBJ(value)->type)

#define IS_FUNCTION(value)  isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    isObjType(value, OBJ_NATIVE)
#define IS_STRING(value)    isObjType(value, OBJ_STRING)
#define IS_CLOSURE(value)   isObjType(value, OBJ_CLOSURE)

#define AS_STRING(value)    ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value)   (((ObjString*)AS_OBJ(value))->chars)
#define AS_FUNCTION(value)  ((ObjFunction*)AS_OBJ(value))
#define AS_NATIVE(value)    (((ObjNative*)AS_OBJ(value))->function)
#define AS_CLOSURE(value)   ((ObjClosure*)AS_OBJ(value))


typedef enum {
    OBJ_STRING,
    OBJ_FUNCTION,
    OBJ_NATIVE,
    OBJ_CLOSURE,     
    OBJ_UPVALUE,
} ObjType;

struct Obj {
    ObjType type;
    bool isMarked;
    struct Obj* next;
};

struct ObjString {
    Obj obj;
    int length;
    char* chars;
    uint32_t hash;
};

typedef struct {
    Obj obj;
    int arity;
    int upvalueCount;
    Chunk chunk;
    ObjString* name;
} ObjFunction;


typedef struct ObjUpValue ObjUpValue; // forward declared

struct ObjUpValue {
    Obj obj;
    Value* location;
    Value closed;
    ObjUpValue* next;
};



typedef struct {
    Obj obj;
    ObjFunction* function;
    ObjUpValue** upvalues;
    int upvalueCount;
} ObjClosure;

typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

ObjClosure* newClosure(ObjFunction* function);
ObjFunction* newFunction();
ObjNative* newNative(NativeFn function);
ObjUpValue* newUpValue(Value* slot);
ObjString* takeString(char* chars, int length);
ObjString* copyString(const char* chars, int length);
void printObject(Value value);

static inline bool isObjType(Value value, ObjType type) {
    return IS_OBJ(value) && AS_OBJ(value)->type == type;
}


#endif