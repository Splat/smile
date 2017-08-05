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

#include <smile/eval/closure.h>

/// <summary>
/// Create a new ClosureInfo struct, which contains metadata about a closure.
/// </summary>
/// <param name="parent">The parent closureInfo whose lexical namespace this will be
/// contained within.</param>
/// <param name="kind">What kind of closure this is (local or global).</param>
/// <returns>The newly-created ClosureInfo object, which will be set to writable,
/// growable, and with no variables yet.</returns>
ClosureInfo ClosureInfo_Create(ClosureInfo parent, Int kind)
{
	ClosureInfo closureInfo = GC_MALLOC_STRUCT(struct ClosureInfoStruct);
	if (closureInfo == NULL)
		Smile_Abort_OutOfMemory();

	closureInfo->parent = parent;
	closureInfo->global = (kind == CLOSURE_KIND_GLOBAL ? closureInfo
		: parent != NULL ? parent->global
		: NULL);
	closureInfo->variableDictionary = VarDict_Create();
	closureInfo->kind = (Int16)kind;
	closureInfo->numVariables = 0;
	closureInfo->numArgs = 0;
	closureInfo->tempSize = 0;
	closureInfo->variableNames = NULL;

	return closureInfo;
}

Closure Closure_CreateGlobal(ClosureInfo closureInfo, Closure parent)
{
	Closure closure = (Closure)GC_MALLOC(sizeof(struct ClosureStruct) - sizeof(SmileArg));
	if (closure == NULL)
		Smile_Abort_OutOfMemory();

	closure->closureInfo = closureInfo;
	closure->parent = parent;
	closure->global = closure;

	closure->returnClosure = NULL;
	closure->returnSegment = NULL;
	closure->returnPc = 0;

	closure->locals = NULL;
	closure->stackTop = NULL;

	return closure;
}

Closure Closure_CreateLocal(ClosureInfo closureInfo, Closure parent,
	Closure returnClosure, ByteCodeSegment returnSegment, Int returnPc)
{
	Closure closure = (Closure)GC_MALLOC(sizeof(struct ClosureStruct)
		+ sizeof(SmileArg) * (closureInfo->numVariables + closureInfo->tempSize - 1));
	if (closure == NULL)
		Smile_Abort_OutOfMemory();

	closure->closureInfo = closureInfo;
	closure->parent = parent;
	closure->global = parent->global;

	closure->returnClosure = returnClosure;
	closure->returnSegment = returnSegment;
	closure->returnPc = returnPc;

	closure->locals = closure->variables + closureInfo->numArgs;
	closure->stackTop = closure->variables + closureInfo->numVariables;

	return closure;
}

ClosureStateMachine Closure_CreateStateMachine(StateMachine stateMachineStart, StateMachine stateMachineBody,
	Closure returnClosure, ByteCodeSegment returnSegment, Int returnPc)
{
	ClosureStateMachine closure = GC_MALLOC_STRUCT(struct ClosureStateMachineStruct);
	if (closure == NULL)
		Smile_Abort_OutOfMemory();

	closure->closureInfo = NULL;
	closure->parent = NULL;
	closure->global = NULL;

	closure->returnClosure = returnClosure;
	closure->returnSegment = returnSegment;
	closure->returnPc = returnPc;

	closure->stackTop = closure->variables;

	closure->stateMachineStart = stateMachineStart;
	closure->stateMachineBody = stateMachineBody;

	return closure;
}

SmileObject Closure_GetGlobalVariable(Closure closure, Symbol name)
{
	VarInfo varInfo;

	for (closure = closure->global; ; closure = closure->parent->global) {
		if (VarDict_TryGetValue(closure->closureInfo->variableDictionary, name, &varInfo))
			return varInfo->value;

		if (closure->parent == NULL)
			return NullObject;
	}
}

Bool Closure_HasGlobalVariable(Closure closure, Symbol name)
{
	for (closure = closure->global; ; closure = closure->parent->global) {
		if (VarDict_ContainsKey(closure->closureInfo->variableDictionary, name))
			return True;
	
		if (closure->parent == NULL)
			return False;
	}
}

void Closure_SetGlobalVariable(Closure closure, Symbol name, SmileObject value)
{
	VarInfo varInfo;
	Closure nearestGlobal = closure->global;

	for (closure = nearestGlobal; ; closure = closure->parent->global) {
		if (VarDict_TryGetValue(closure->closureInfo->variableDictionary, name, &varInfo)) {
			varInfo->value = value;
			return;
		}

		if (closure->parent == NULL) {
			varInfo = GC_MALLOC_STRUCT(struct VarInfoStruct);
			if (varInfo == NULL)
				Smile_Abort_OutOfMemory();
			varInfo->symbol = name;
			varInfo->kind = VAR_KIND_GLOBAL;
			varInfo->offset = 0;
			varInfo->value = value;
			VarDict_Add(nearestGlobal->closureInfo->variableDictionary, name, varInfo);
			return;
		}
	}
}

SmileArg Closure_GetArgumentInScope(Closure closure, Int scope, Int index)
{
	switch (scope) {
	case 0: return Closure_GetArgument(closure, index);
	case 1: return Closure_GetArgument(closure->parent, index);
	case 2: return Closure_GetArgument(closure->parent->parent, index);
	case 3: return Closure_GetArgument(closure->parent->parent->parent, index);
	default:
		while (scope--) {
			closure = closure->parent;
		}
		return Closure_GetArgument(closure, index);
	}
}

void Closure_SetArgumentInScope(Closure closure, Int scope, Int index, SmileArg arg)
{
	switch (scope) {
	case 0:
		Closure_SetArgument(closure, index, arg);
		break;
	case 1:
		Closure_SetArgument(closure->parent, index, arg);
		break;
	case 2:
		Closure_SetArgument(closure->parent->parent, index, arg);
		break;
	case 3:
		Closure_SetArgument(closure->parent->parent->parent, index, arg);
		break;
	default:
		while (scope--) {
			closure = closure->parent;
		}
		Closure_SetArgument(closure, index, arg);
		break;
	}
}

SmileArg Closure_GetLocalVariableInScope(Closure closure, Int scope, Int index)
{
	switch (scope) {
	case 0: return Closure_GetLocalVariable(closure, index);
	case 1: return Closure_GetLocalVariable(closure->parent, index);
	case 2: return Closure_GetLocalVariable(closure->parent->parent, index);
	case 3: return Closure_GetLocalVariable(closure->parent->parent->parent, index);
	default:
		while (scope--) {
			closure = closure->parent;
		}
		return Closure_GetLocalVariable(closure, index);
	}
}

void Closure_SetLocalVariableInScope(Closure closure, Int scope, Int index, SmileArg arg)
{
	switch (scope) {
	case 0:
		Closure_SetLocalVariable(closure, index, arg);
		break;
	case 1:
		Closure_SetLocalVariable(closure->parent, index, arg);
		break;
	case 2:
		Closure_SetLocalVariable(closure->parent->parent, index, arg);
		break;
	case 3:
		Closure_SetLocalVariable(closure->parent->parent->parent, index, arg);
		break;
	default:
		while (scope--) {
			closure = closure->parent;
		}
		Closure_SetLocalVariable(closure, index, arg);
		break;
	}
}
