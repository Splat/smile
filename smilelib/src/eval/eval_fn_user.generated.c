// ===================================================
//   WARNING: THIS IS A GENERATED FILE. DO NOT EDIT!
// ===================================================

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

#include <smile/eval/eval.h>
#include <smile/smiletypes/smilepair.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/smiletypes/smilebool.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/smiletypes/smileuserobject.h>
#include <smile/smiletypes/text/smilesymbol.h>

extern Closure _closure;
extern CompiledTables _compiledTables;
extern ByteCodeSegment _segment;
extern ByteCode _byteCode;

//-------------------------------------------------------------------------------------------------
// User functions, with a fixed small count of arguments.

static const char *_argCountErrorMessage = "'%S' requires %d arguments, but was called with %d.";
static const char *_minArgCountErrorMessage = "'%S' requires at least %d arguments, but was called with %d.";
static const char *_maxArgCountErrorMessage = "'%S' allows at most %d arguments, but was called with %d.";

void SmileUserFunction_NoArgs_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 0) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 0, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */

	Closure_PopCount(_closure, extra + 0);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast1_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 1) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 1, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];

	Closure_PopCount(_closure, extra + 1);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast2_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 2) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 2, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];

	Closure_PopCount(_closure, extra + 2);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast3_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 3) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 3, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];

	Closure_PopCount(_closure, extra + 3);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast4_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 4) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 4, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];
	childClosure->variables[3] = _closure->stackTop[-4];

	Closure_PopCount(_closure, extra + 4);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast5_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 5) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 5, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];
	childClosure->variables[3] = _closure->stackTop[-4];
	childClosure->variables[4] = _closure->stackTop[-5];

	Closure_PopCount(_closure, extra + 5);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast6_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 6) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 6, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];
	childClosure->variables[3] = _closure->stackTop[-4];
	childClosure->variables[4] = _closure->stackTop[-5];
	childClosure->variables[5] = _closure->stackTop[-6];

	Closure_PopCount(_closure, extra + 6);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast7_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 7) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 7, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];
	childClosure->variables[3] = _closure->stackTop[-4];
	childClosure->variables[4] = _closure->stackTop[-5];
	childClosure->variables[5] = _closure->stackTop[-6];
	childClosure->variables[6] = _closure->stackTop[-7];

	Closure_PopCount(_closure, extra + 7);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Fast8_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != 8) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), 8, argc));
	}

	/* Create a new child closure for this function. */
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	/* Copy the arguments. */
	childClosure->variables[0] = _closure->stackTop[-1];
	childClosure->variables[1] = _closure->stackTop[-2];
	childClosure->variables[2] = _closure->stackTop[-3];
	childClosure->variables[3] = _closure->stackTop[-4];
	childClosure->variables[4] = _closure->stackTop[-5];
	childClosure->variables[5] = _closure->stackTop[-6];
	childClosure->variables[6] = _closure->stackTop[-7];
	childClosure->variables[7] = _closure->stackTop[-8];

	Closure_PopCount(_closure, extra + 8);

	/* We're now in the child, so set up the globals for running inside it. */
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}



//-------------------------------------------------------------------------------------------------
// User functions, with optional arguments, rest arguments, or type-checked arguments.

// This handles the case where the number of arguments is fixed, but larger than 8.
void SmileUserFunction_Slow_Call(SmileFunction self, Int argc, Int extra)
{
	Int numArgs = self->u.u.userFunctionInfo->numArgs;
	Int i;
	UserFunctionInfo userFunctionInfo;
	Closure childClosure;

	if (argc != numArgs) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_argCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), numArgs, argc));
	}

	// Create a new child closure for this function.
	userFunctionInfo = self->u.u.userFunctionInfo;
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	// Copy the arguments.
	for (i = 0; i < numArgs; i++) {
		childClosure->variables[i] = _closure->stackTop[-argc + i];
	}
	Closure_PopCount(_closure, extra + argc);

	// We're now in the child, so set up the globals for running inside it.
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

// This handles the case where the number of arguments is variable, because
// there are trailing optional arguments with default values assigned.
void SmileUserFunction_Optional_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo = self->u.u.userFunctionInfo;
	UserFunctionArg argInfo = userFunctionInfo->args;
	Int numArgs = userFunctionInfo->numArgs;
	Int i;
	Closure childClosure;

	if (argc < userFunctionInfo->minArgs) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_minArgCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), userFunctionInfo->minArgs, argc));
	}
	if (argc > userFunctionInfo->maxArgs) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_maxArgCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), userFunctionInfo->maxArgs, argc));
	}

	// Create a new child closure for this function.
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	// Copy the provided arguments.
	for (i = 0; i < argc; i++) {
		childClosure->variables[i] = _closure->stackTop[-argc + i];
	}
	Closure_PopCount(_closure, extra + argc);

	// Fill in any missing default values.
	for (; i < numArgs; i++) {
		childClosure->variables[i] = argInfo[i].defaultValue;
	}

	// We're now in the child, so set up the globals for running inside it.
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

// This handles the case where the number of arguments is variable, because there
// is a 'rest' list that will collect the leftover (and possibly optional arguments too).
void SmileUserFunction_Rest_Call(SmileFunction self, Int argc, Int extra)
{
	UserFunctionInfo userFunctionInfo = self->u.u.userFunctionInfo;
	UserFunctionArg argInfo = userFunctionInfo->args;
	Int numArgs = userFunctionInfo->numArgs;
	Int i;
	SmileList restHead, restTail;
	Closure childClosure;
	SmileArg arg;

	if (argc < userFunctionInfo->minArgs) {
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format(_minArgCountErrorMessage,
			SMILE_VCALL1(self, toString, (SmileUnboxedData){ 0 }), userFunctionInfo->minArgs, argc));
	}

	// Create a new child closure for this function.
	childClosure = Closure_CreateLocal(&userFunctionInfo->closureInfo, self->u.u.declaringClosure,
		_closure, _segment, _byteCode - _segment->byteCodes);

	// Copy the provided arguments.
	if (argc < numArgs) {

		// Fewer arguments given than we have slots, so copy what we were given.
		for (i = 0; i < argc; i++) {
			childClosure->variables[i] = _closure->stackTop[-argc + i];
		}

		// Fill in any missing default values in the remaining slots.
		for (; i < numArgs - 1; i++) {
			childClosure->variables[i] = argInfo[i].defaultValue;
		}

		// The 'rest' argument is an empty list.
		arg.obj = NullObject;
		childClosure->variables[i] = arg;
	}
	else {

		// We're going to have at least one 'rest' argument.  So copy everything
		// provided before it.
		for (i = 0; i < numArgs - 1; i++) {
			childClosure->variables[i] = _closure->stackTop[-argc + i];
		}

		// Now turn the rest of the provided arguments, however many there may be, into a List.
		restHead = restTail = NullList;
		for (; i < argc; i++) {
			LIST_APPEND(restHead, restTail, SmileArg_Box(_closure->stackTop[-argc + i]));
		}
		arg.obj = (SmileObject)restHead;
		childClosure->variables[numArgs - 1] = arg;
	}

	// Clean up the calling stack.
	Closure_PopCount(_closure, extra + argc);

	// We're now in the child, so set up the globals for running inside it.
	_closure = childClosure;
	_segment = userFunctionInfo->byteCodeSegment;
	_compiledTables = _segment->compiledTables;
	_byteCode = &_segment->byteCodes[0];
}

void SmileUserFunction_Checked_Call(SmileFunction self, Int argc, Int extra)
{
	// TODO: FIXME: There is no type-checking currently.
	SmileUserFunction_Slow_Call(self, argc, extra);
}

void SmileUserFunction_CheckedRest_Call(SmileFunction self, Int argc, Int extra)
{
	// TODO: FIXME: There is no type-checking currently.
	SmileUserFunction_Rest_Call(self, argc, extra);
}
