#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"


typedef enum {
    OP_RETURN,
    OP_PRINT,
    OP_JUMP_IF_FALSE,
    OP_JUMP,
    OP_LOOP,
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_NOT,
    OP_TRUE, 
    OP_FALSE, // 10
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_NIL,
    OP_POP,
    OP_DEFINE_GLOBAL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_GET_LOCAL, 
    OP_SET_LOCAL, // 20
    OP_CALL,
    OP_CLOSURE,
    OP_SET_UPVALUE,
    OP_GET_UPVALUE,
    OP_CLOSE_UPVALUE,
} OpCode;

// OP_CLOSURE is unique in that it has a variably sized encoding


typedef struct {
    // uint8_t is 1 byte, (8 bit unsigned int)
    int count;
    int capacity;
    uint8_t* code; // dynamic array of bytes (uint8_t is the byte type) 
    int* lines;
    ValueArray constants;

} Chunk;

void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);

#endif