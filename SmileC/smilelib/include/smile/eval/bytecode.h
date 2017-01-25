#ifndef __SMILE_EVAL_BYTECODE_H__
#define __SMILE_EVAL_BYTECODE_H__

#ifndef __SMILE_TYPES_H__
#include <smile/types.h>
#endif

#ifndef __SMILE_ENV_SYMBOLTABLE_H__
#include <smile/env/symboltable.h>
#endif

#ifndef __SMILE_GC_H__
#include <smile/gc.h>
#endif

#ifndef __SMILE_MEM_H__
#include <smile/mem.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Type declarations.

/// <summary>
/// This is the shape of a single byte-code instruction:  It has an opcode (see 'opcode.h'), which
/// is 32 bits; a source location, which is 32 bits; and one or two operands, taking at most 64 bits
/// for the combined operand.
///
/// Total size:  16 bytes.
/// </summary>
typedef struct ByteCodeStruct {
	Byte opcode;	// The opcode for this instruction.
	Byte reserved[3];	
	Int32 sourceLocation;	// The index of the source location that generated this (for debugging).

	union {
		Int64 int64;
		Int32 int32;
		Int16 int16;
		Byte byte;
		Real64 real64;
		Real32 real32;
		Float32 float32;
		Float64 float64;
		Symbol symbol;
		Int index;
		Int delta;
		UInt ch;
		UInt uch;
		Bool boolean;
		struct {
			Int32 a, b;
		} i2;
	} u;
} *ByteCode;

/// <summary>
/// A byte-code segment is nothing more than an easily-growable array of byte codes.
/// </summary>
typedef struct ByteCodeSegmentStruct {
	ByteCode byteCodes;
	Int32 numByteCodes;
	Int32 maxByteCodes;
} *ByteCodeSegment;

//-------------------------------------------------------------------------------------------------
//  External API.

struct CompiledTablesStruct;
struct CompilerFunctionStruct;

SMILE_API_FUNC void ByteCodeSegment_Grow(ByteCodeSegment segment, Int count);
SMILE_API_FUNC ByteCodeSegment ByteCodeSegment_CreateWithSize(Int size);
SMILE_API_FUNC String ByteCodeSegment_ToString(ByteCodeSegment segment, struct CompilerFunctionStruct *compilerFunction, struct CompiledTablesStruct *compiledTables);
SMILE_API_FUNC String ByteCode_ToString(ByteCode byteCode, Int address, struct CompilerFunctionStruct *compilerFunction, struct CompiledTablesStruct *compiledTables);

SMILE_API_DATA String *Opcode_Names;

//-------------------------------------------------------------------------------------------------
//  Inline implementation.

/// <summary>
/// Create a new byte-code segment, with room for 16 instructions initially.
/// </summary>
Inline ByteCodeSegment ByteCodeSegment_Create(void)
{
	return ByteCodeSegment_CreateWithSize(16);
}

/// <summary>
/// Ensure that the given byte-code segment has at least 'count' empty instruction slots
/// at the end, and if it doesn't, grow it until it does.
/// </summary>
/// <param name="segment">The segment to possibly grow.</param>
/// <param name="count">How much space is needed for the new instruction(s) (often 1).</param>
Inline void ByteCodeSegment_More(ByteCodeSegment segment, Int count)
{
	if (segment->numByteCodes + count >= segment->maxByteCodes)
		ByteCodeSegment_Grow(segment, count);
}

/// <summary>
/// Emit a byte-code instruction to the given segment's stream.
/// </summary>
/// <param name="segment">The segment to append an instruction to.</param>
/// <param name="opcode">The opcode of the instruction to append.</param>
/// <returns>A pointer to the instruction that was added.  The operator will be
/// set to the given opcode, and the operand(s) will be zeroed out.</returns>
Inline Int ByteCodeSegment_Emit(ByteCodeSegment segment, Int opcode, Int location)
{
	ByteCode byteCode;
	Int offset;

	ByteCodeSegment_More(segment, 1);
	byteCode = segment->byteCodes + (offset = segment->numByteCodes++);
	byteCode->opcode = (Byte)opcode;
	byteCode->sourceLocation = (Int32)location;
	byteCode->u.int64 = 0;

	return offset;
}

#endif