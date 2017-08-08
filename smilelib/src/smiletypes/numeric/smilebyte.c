//---------------------------------------------------------------------------------------
//  Smile Programming Language Interpreter
//  Copyright 2004-2017 Sean Werkema
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//---------------------------------------------------------------------------------------

#include <smile/numeric/real64.h>
#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/numeric/smilebyte.h>
#include <smile/smiletypes/easyobject.h>

SMILE_IGNORE_UNUSED_VARIABLES

SMILE_EASY_OBJECT_VTABLE(SmileByte);

SmileByte SmileByte_CreateInternal(Byte value)
{
	// We MALLOC_ATOMIC here because the base is a known pointer that will never be collected.
	SmileByte smileByte = (SmileByte)GC_MALLOC_ATOMIC(sizeof(struct SmileByteInt));
	if (smileByte == NULL) Smile_Abort_OutOfMemory();
	smileByte->base = (SmileObject)Smile_KnownBases.Byte;
	smileByte->kind = SMILE_KIND_BYTE;
	smileByte->vtable = SmileByte_VTable;
	smileByte->value = value;
	return smileByte;
}

SMILE_EASY_OBJECT_READONLY_SECURITY(SmileByte)
SMILE_EASY_OBJECT_NO_CALL(SmileByte, "A Byte object")
SMILE_EASY_OBJECT_NO_SOURCE(SmileByte)
SMILE_EASY_OBJECT_NO_PROPERTIES(SmileByte)

SMILE_EASY_OBJECT_HASH(SmileByte, obj->value)
SMILE_EASY_OBJECT_TOBOOL(SmileByte, obj->value != 0)
SMILE_EASY_OBJECT_TOINT(SmileByte, obj->value)
SMILE_EASY_OBJECT_TOREAL(SmileByte, Real64_FromInt32(obj->value))
SMILE_EASY_OBJECT_TOFLOAT(SmileByte, (Float64)obj->value)
SMILE_EASY_OBJECT_TOSTRING(SmileByte, String_Format("%ux", (UInt32)obj->value))

static Bool SmileByte_CompareEqual(SmileByte a, SmileUnboxedData aData, SmileObject b, SmileUnboxedData bData)
{
	if (SMILE_KIND(b) == SMILE_KIND_UNBOXED_BYTE) {
		return ((SmileByte)a)->value == bData.i8;
	}
	else if (SMILE_KIND(b) == SMILE_KIND_BYTE) {
		return ((SmileByte)a)->value == ((SmileByte)b)->value;
	}
	else return False;
}

static Bool SmileByte_DeepEqual(SmileByte a, SmileUnboxedData aData, SmileObject b, SmileUnboxedData bData, PointerSet visitedPointers)
{
	UNUSED(visitedPointers);

	if (SMILE_KIND(b) == SMILE_KIND_UNBOXED_BYTE) {
		return ((SmileByte)a)->value == bData.i8;
	}
	else if (SMILE_KIND(b) == SMILE_KIND_BYTE) {
		return ((SmileByte)a)->value == ((SmileByte)b)->value;
	}
	else return False;
}

SmileObject SmileByte_Box(SmileArg src)
{
	return src.obj;
}

SmileArg SmileByte_Unbox(SmileByte smileByte)
{
	return SmileUnboxedByte_From(smileByte->value);
}

//-------------------------------------------------------------------------------------------------

SMILE_EASY_OBJECT_VTABLE(SmileUnboxedByte);

SMILE_EASY_OBJECT_READONLY_SECURITY(SmileUnboxedByte)
SMILE_EASY_OBJECT_NO_CALL(SmileUnboxedByte, "A Byte")
SMILE_EASY_OBJECT_NO_SOURCE(SmileUnboxedByte)
SMILE_EASY_OBJECT_NO_PROPERTIES(SmileUnboxedByte)

SMILE_EASY_OBJECT_HASH(SmileUnboxedByte, 0)
SMILE_EASY_OBJECT_TOBOOL(SmileUnboxedByte, (Bool)!!unboxedData.i8)
SMILE_EASY_OBJECT_TOINT(SmileUnboxedByte, unboxedData.i8)
SMILE_EASY_OBJECT_TOREAL(SmileUnboxedByte, Real64_FromInt32(unboxedData.i8))
SMILE_EASY_OBJECT_TOFLOAT(SmileUnboxedByte, unboxedData.i8)
SMILE_EASY_OBJECT_TOSTRING(SmileUnboxedByte, String_Format("%u", (UInt)unboxedData.i8))

static Bool SmileUnboxedByte_CompareEqual(SmileUnboxedByte a, SmileUnboxedData aData, SmileObject b, SmileUnboxedData bData)
{
	if (SMILE_KIND(b) == SMILE_KIND_UNBOXED_BYTE) {
		return aData.i8 == bData.i8;
	}
	else if (SMILE_KIND(b) == SMILE_KIND_BYTE) {
		return aData.i8 == ((SmileByte)b)->value;
	}
	else return False;
}

static Bool SmileUnboxedByte_DeepEqual(SmileUnboxedByte a, SmileUnboxedData aData, SmileObject b, SmileUnboxedData bData, PointerSet visitedPointers)
{
	UNUSED(visitedPointers);

	if (SMILE_KIND(b) == SMILE_KIND_UNBOXED_BYTE) {
		return aData.i8 == bData.i8;
	}
	else if (SMILE_KIND(b) == SMILE_KIND_BYTE) {
		return aData.i8 == ((SmileByte)b)->value;
	}
	else return False;
}

static SmileObject SmileUnboxedByte_Box(SmileArg src)
{
	return (SmileObject)SmileByte_Create(src.unboxed.b);
}

static SmileArg SmileUnboxedByte_Unbox(SmileUnboxedByte smileUnboxedByte)
{
	Smile_Abort_FatalError("Cannot re-unbox a unboxed object.");
	return (SmileArg){ 0 };
}

static struct SmileUnboxedByteInt SmileUnboxedByte_Instance_Struct = {
	SMILE_KIND_UNBOXED_BYTE,
	(SmileVTable)&SmileUnboxedByte_VTableData,
};

extern SmileUnboxedByte SmileUnboxedByte_Instance;
SmileUnboxedByte SmileUnboxedByte_Instance = &SmileUnboxedByte_Instance_Struct;
