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

#include <smile/eval/compiler.h>
#include <smile/eval/compiler_internal.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/smiletypes/smilepair.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/smiletypes/text/smilesymbol.h>
#include <smile/parsing/parsemessage.h>
#include <smile/parsing/internal/parsedecl.h>
#include <smile/parsing/internal/parsescope.h>

void Compiler_EmitPop1(Compiler compiler)
{
	Byte lastOpcode, newOpcode;
	ByteCodeSegment segment = compiler->currentFunction->byteCodeSegment;
	Int offset;

	if (segment->numByteCodes <= 0) {
		EMIT0(Op_Pop1, -1);
		return;
	}

	lastOpcode = RECENT_BYTECODE(-1).opcode;
	
	switch (lastOpcode) {
		case Op_Ld8: case Op_Ld16: case Op_Ld32: case Op_Ld64: case Op_Ld128:
		case Op_LdR16: case Op_LdR32: case Op_LdR64: case Op_LdR128:
		case Op_LdF16: case Op_LdF32: case Op_LdF64: case Op_LdF128:
		case Op_LdBool:
		case Op_LdNull:
		case Op_LdStr:
		case Op_LdObj:
		case Op_LdSym:
		case Op_LdClos:
		case Op_Dup: case Op_Dup1: case Op_Dup2:
		case Op_LdX:
		case Op_LdArg:
		case Op_LdArg0: case Op_LdArg1: case Op_LdArg2: case Op_LdArg3:
		case Op_LdArg4: case Op_LdArg5: case Op_LdArg6: case Op_LdArg7:
		case Op_LdLoc:
		case Op_LdLoc0: case Op_LdLoc1: case Op_LdLoc2: case Op_LdLoc3:
		case Op_LdLoc4: case Op_LdLoc5: case Op_LdLoc6: case Op_LdLoc7:
			// Delete last instruction, since it's a simple load and wasn't actually needed.
			segment->numByteCodes--;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_LdProp:
			// Delete last instruction, and pop the object before it.
			segment->numByteCodes--;
			ApplyStackDelta(compiler->currentFunction, -1);
			Compiler_EmitPop1(compiler);
			return;
		
		case Op_LdMember:
			// Delete last instruction, and pop the two objects before it.
			segment->numByteCodes--;
			ApplyStackDelta(compiler->currentFunction, -1);
			Compiler_EmitPop1(compiler);
			Compiler_EmitPop1(compiler);
			return;
		
		case Op_Pop1:
			// Upgrade this to a Pop2.
			RECENT_BYTECODE(-1).opcode = Op_Pop2;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_Pop2:
			// Upgrade this to a Pop 3.
			RECENT_BYTECODE(-1).opcode = Op_Pop;
			RECENT_BYTECODE(-1).u.index = 3;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_Pop:
			// Upgrade this to a Pop N+1.
			RECENT_BYTECODE(-1).u.index++;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_Rep1:
			// Upgrade this to a Pop2.
			RECENT_BYTECODE(-1).opcode = Op_Pop2;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_Rep2:
			// Upgrade this to a Pop 3.
			RECENT_BYTECODE(-1).opcode = Op_Pop;
			RECENT_BYTECODE(-1).u.index = 3;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;

		case Op_Rep:
			// Upgrade this to a Pop N+1.
			RECENT_BYTECODE(-1).opcode = Op_Pop;
			RECENT_BYTECODE(-1).u.index++;
			ApplyStackDelta(compiler->currentFunction, -1);
			return;
		
		case Op_StX:	newOpcode = Op_StpX;	break;
		case Op_StArg:	newOpcode = Op_StpArg;	break;
		case Op_StArg0:	newOpcode = Op_StpArg0;	break;
		case Op_StArg1:	newOpcode = Op_StpArg1;	break;
		case Op_StArg2:	newOpcode = Op_StpArg2;	break;
		case Op_StArg3:	newOpcode = Op_StpArg3;	break;
		case Op_StArg4:	newOpcode = Op_StpArg4;	break;
		case Op_StArg5:	newOpcode = Op_StpArg5;	break;
		case Op_StArg6:	newOpcode = Op_StpArg6;	break;
		case Op_StArg7:	newOpcode = Op_StpArg7;	break;
		case Op_StLoc:	newOpcode = Op_StpLoc;	break;
		case Op_StLoc0:	newOpcode = Op_StpLoc0;	break;
		case Op_StLoc1:	newOpcode = Op_StpLoc1;	break;
		case Op_StLoc2:	newOpcode = Op_StpLoc2;	break;
		case Op_StLoc3:	newOpcode = Op_StpLoc3;	break;
		case Op_StLoc4:	newOpcode = Op_StpLoc4;	break;
		case Op_StLoc5:	newOpcode = Op_StpLoc5;	break;
		case Op_StLoc6:	newOpcode = Op_StpLoc6;	break;
		case Op_StLoc7:	newOpcode = Op_StpLoc7;	break;
		case Op_StProp:	newOpcode = Op_StpProp;	break;
		case Op_StMember:	newOpcode = Op_StpMember;	break;
		default:	newOpcode = lastOpcode;	break;
	}

	if (newOpcode != lastOpcode) {
		// Rewrite the most recent store as a store-and-pop.
		RECENT_BYTECODE(-1).opcode = newOpcode;
		ApplyStackDelta(compiler->currentFunction, -1);
		return;
	}
	else {
		// The last opcode wasn't a store, so just pop whatever it was.
		EMIT0(Op_Pop1, -1);
	}
}

Int Compiler_SetSourceLocation(Compiler compiler, LexerPosition lexerPosition)
{
	Int oldSourceLocation = compiler->currentFunction->currentSourceLocation;

	// Update the source-location tracking to include a new lexer position.
	CompiledSourceLocation sourceLocation = &compiler->compiledTables->sourcelocations[oldSourceLocation];
	compiler->currentFunction->currentSourceLocation =
		Compiler_AddNewSourceLocation(compiler, lexerPosition->filename, lexerPosition->line, lexerPosition->column, sourceLocation->assignedName);

	return oldSourceLocation;
}

Int Compiler_SetAssignedSymbol(Compiler compiler, Symbol symbol)
{
	Int oldSourceLocation = compiler->currentFunction->currentSourceLocation;

	// Update the source-location tracking to indicate that whatever expression we're building,
	// it's being assigned to something with this name.
	CompiledSourceLocation sourceLocation = &compiler->compiledTables->sourcelocations[oldSourceLocation];
	compiler->currentFunction->currentSourceLocation =
		Compiler_AddNewSourceLocation(compiler, sourceLocation->filename, sourceLocation->line, sourceLocation->column, symbol);

	return oldSourceLocation;
}

/// <summary>
/// Create a new CompiledTables object.
/// </summary>
/// <returns>A new CompiledTables object, with room for strings, objects, and functions.</returns>
CompiledTables CompiledTables_Create(void)
{
	CompiledTables compiledTables;

	compiledTables = GC_MALLOC_STRUCT(struct CompiledTablesStruct);
	if (compiledTables == NULL)
		Smile_Abort_OutOfMemory();

	compiledTables->globalFunctionInfo = NULL;
	compiledTables->globalClosureInfo = NULL;

	compiledTables->strings = GC_MALLOC_STRUCT_ARRAY(String, 16);
	if (compiledTables->strings == NULL)
		Smile_Abort_OutOfMemory();
	compiledTables->numStrings = 0;
	compiledTables->maxStrings = 16;

	compiledTables->stringLookup = StringIntDict_Create();

	compiledTables->userFunctions = GC_MALLOC_STRUCT_ARRAY(UserFunctionInfo, 16);
	if (compiledTables->userFunctions == NULL)
		Smile_Abort_OutOfMemory();
	compiledTables->numUserFunctions = 0;
	compiledTables->maxUserFunctions = 16;

	compiledTables->sourcelocations = GC_MALLOC_STRUCT_ARRAY(struct CompiledSourceLocationStruct, 256);
	if (compiledTables->sourcelocations == NULL)
		Smile_Abort_OutOfMemory();
	compiledTables->numSourceLocations = 1;
	compiledTables->maxSourceLocations = 256;

	// Preallocate the zeroth entry so we can use '0' to mean 'none'.
	compiledTables->sourcelocations[0].filename = NULL;
	compiledTables->sourcelocations[0].line = 0;
	compiledTables->sourcelocations[0].column = 0;
	compiledTables->sourcelocations[0].assignedName = 0;

	return compiledTables;
}

Int CompilerFunction_AddLocal(CompilerFunction compilerFunction, Symbol local)
{
	Int localIndex, newMax;
	Symbol *newLocals;

	// If we're out of space, grow the array.
	if (compilerFunction->localSize >= compilerFunction->localMax) {
		newMax = compilerFunction->localMax * 2;
		newLocals = (Symbol *)GC_MALLOC_ATOMIC(sizeof(Symbol) * newMax);
		MemCpy(newLocals, compilerFunction->localNames, sizeof(Symbol) * compilerFunction->localSize);
		compilerFunction->localMax = newMax;
		compilerFunction->localNames = newLocals;
	}

	localIndex = compilerFunction->localSize++;

	compilerFunction->localNames[localIndex] = local;

	return localIndex;
}

/// <summary>
/// Create a new Compiler object.
/// </summary>
/// <returns>A new Compiler object, ready to begin compiling to a new set of CompiledTables.</returns>
Compiler Compiler_Create(void)
{
	Compiler compiler;

	compiler = GC_MALLOC_STRUCT(struct CompilerStruct);
	if (compiler == NULL)
		Smile_Abort_OutOfMemory();

	compiler->compiledTables = CompiledTables_Create();
	compiler->currentFunction = NULL;

	compiler->firstMessage = NULL;
	compiler->lastMessage = NULL;

	return compiler;
}

/// <summary>
/// Add a UserFunctionInfo object to the compiler's collection, and return its index.
/// </summary>
Int Compiler_AddUserFunctionInfo(Compiler compiler, UserFunctionInfo userFunctionInfo)
{
	CompiledTables compiledTables = compiler->compiledTables;
	Int index;

	// Do we have enough space to add it?  If not, reallocate.
	if (compiledTables->numUserFunctions >= compiledTables->maxUserFunctions) {
		UserFunctionInfo *newUserFunctions;
		Int newMax;
	
		newMax = compiledTables->maxUserFunctions * 2;
		newUserFunctions = GC_MALLOC_STRUCT_ARRAY(UserFunctionInfo, newMax);
		if (newUserFunctions == NULL)
			Smile_Abort_OutOfMemory();
		MemCpy(newUserFunctions, compiledTables->userFunctions, compiledTables->numUserFunctions);
		compiledTables->userFunctions = newUserFunctions;
		compiledTables->maxUserFunctions = newMax;
	}

	// Okay, we have enough space, so add it.
	index = compiledTables->numUserFunctions++;
	compiledTables->userFunctions[index] = userFunctionInfo;

	return index;
}

/// <summary>
/// Add a new SourceLocation object to the compiler's collection, and return its index.
/// </summary>
Int Compiler_AddNewSourceLocation(Compiler compiler, String filename, Int line, Int column, Symbol assignedName)
{
	CompiledSourceLocation sourceLocation;
	CompiledTables compiledTables = compiler->compiledTables;
	Int index;

	// Simple and dumb optimization:  See if we have one of these already as the last one added.
	index = compiledTables->numSourceLocations - 1;
	sourceLocation = &compiledTables->sourcelocations[index];
	if (filename == sourceLocation->filename    // Note that this is a pointer test, not a deep test.
		&& line == sourceLocation->line && column == sourceLocation->column
		&& assignedName == sourceLocation->assignedName)
		return index;

	// Do we have enough space to add it?  If not, reallocate.
	if (compiledTables->numSourceLocations >= compiledTables->maxSourceLocations) {
		struct CompiledSourceLocationStruct *newSourceLocations;
		Int newMax;

		newMax = compiledTables->maxSourceLocations * 2;
		newSourceLocations = GC_MALLOC_STRUCT_ARRAY(struct CompiledSourceLocationStruct, newMax);
		if (newSourceLocations == NULL)
			Smile_Abort_OutOfMemory();
		MemCpy(newSourceLocations, compiledTables->sourcelocations, compiledTables->numSourceLocations);
		compiledTables->sourcelocations = newSourceLocations;
		compiledTables->maxSourceLocations = newMax;
	}

	// Okay, we have enough space, and it's not there yet, so add it.
	index = compiledTables->numSourceLocations++;
	sourceLocation = &compiledTables->sourcelocations[index];
	sourceLocation->assignedName = assignedName;
	sourceLocation->filename = filename;
	sourceLocation->line = line;
	sourceLocation->column = column;

	return index;
}

/// <summary>
/// Begin compiling a function.
/// </summary>
/// <param name="compiler">The compiler that will be compiling this function.</param>
/// <param name="args">The function argument list.</param>
/// <param name="body">The function body.</param>
/// <returns>A new CompilerFunction object, which has its args/body assigned, but which is not yet
/// populated with any instructions.</returns>
CompilerFunction Compiler_BeginFunction(Compiler compiler, SmileList args, SmileObject body)
{
	CompilerFunction newFunction;

	// Create the new function.
	newFunction = GC_MALLOC_STRUCT(struct CompilerFunctionStruct);
	if (newFunction == NULL)
		Smile_Abort_OutOfMemory();
	newFunction->args = args;
	newFunction->body = body;
	newFunction->byteCodeSegment = ByteCodeSegment_Create();
	newFunction->numArgs = 0;
	newFunction->isCompiled = False;
	newFunction->parent = compiler->currentFunction;
	newFunction->localNames = GC_MALLOC_ATOMIC(sizeof(Symbol) * 16);
	newFunction->localSize = 0;
	newFunction->localMax = 16;
	newFunction->stackSize = 0;
	newFunction->functionDepth = compiler->currentFunction != NULL ? compiler->currentFunction->functionDepth + 1 : 0;
	newFunction->currentSourceLocation = 0;
	newFunction->userFunctionInfo = NULL;

	// Finally, make the new function the current function.
	compiler->currentFunction = newFunction;

	return newFunction;
}

/// <summary>
/// Finish compiling the current function, and return the compiler to working on
/// its previous (outer) function.
/// </summary>
/// <param name="compiler">The compiler that has compiled a function.</param>
void Compiler_EndFunction(Compiler compiler)
{
	CompilerFunction compilerFunction = compiler->currentFunction;

	compilerFunction->closureInfo = Compiler_MakeClosureInfoForCompilerFunction(compiler, compilerFunction);

	compiler->currentFunction = compilerFunction->parent;
}

CompileScope Compiler_BeginScope(Compiler compiler, Int kind)
{
	CompileScope newScope;

	// Create the new scope.
	newScope = GC_MALLOC_STRUCT(struct CompileScopeStruct);
	if (newScope == NULL)
		Smile_Abort_OutOfMemory();

	newScope->kind = kind;
	newScope->symbolDict = Int32Dict_Create();
	newScope->parent = compiler->currentScope;
	newScope->function = compiler->currentFunction;
	compiler->currentScope = newScope;

	return newScope;
}

CompiledLocalSymbol CompileScope_DefineSymbol(CompileScope scope, Symbol symbol, Int kind, Int index)
{
	Int size;
	CompiledLocalSymbol localSymbol;

	size = sizeof(struct CompiledLocalSymbolStruct);
	if (kind == PARSEDECL_TILL)
		size = sizeof(struct CompiledTillSymbolStruct);
	
	localSymbol = (CompiledLocalSymbol)GC_MALLOC(size);
	if (localSymbol == NULL)
		Smile_Abort_OutOfMemory();

	localSymbol->kind = kind;
	localSymbol->index = index;
	localSymbol->symbol = symbol;
	localSymbol->scope = scope;
	localSymbol->wasRead = False;
	localSymbol->wasReadDeep = False;
	localSymbol->wasWritten = False;
	localSymbol->wasWrittenDeep = False;

	Int32Dict_SetValue(scope->symbolDict, symbol, localSymbol);

	return localSymbol;
}

CompiledLocalSymbol CompileScope_FindSymbol(CompileScope compileScope, Symbol symbol)
{
	CompiledLocalSymbol localSymbol;

	for (; compileScope != NULL; compileScope = compileScope->parent)
	{
		if (Int32Dict_TryGetValue(compileScope->symbolDict, symbol, &localSymbol))
			return localSymbol;
	}

	return NULL;
}

CompiledLocalSymbol CompileScope_FindSymbolHere(CompileScope compileScope, Symbol symbol)
{
	CompiledLocalSymbol localSymbol;
	return Int32Dict_TryGetValue(compileScope->symbolDict, symbol, &localSymbol) ? localSymbol : NULL;
}

/// <summary>
/// Add the given string to the compiler's string table (or find a preexisting string in the
/// string table that matches).
/// </summary>
/// <param name="compiler">The compiler that has the string table that this string will be added to.</param>
/// <param name="string">The string to add.</param>
/// <returns>The index of that string in the string table.</returns>
Int Compiler_AddString(Compiler compiler, String string)
{
	CompiledTables compiledTables = compiler->compiledTables;
	Int index;

	// See if we have this string already.
	if (StringIntDict_TryGetValue(compiledTables->stringLookup, string, &index))
		return index;

	// Do we have enough space to add it?  If not, reallocate.
	if (compiledTables->numStrings >= compiledTables->maxStrings) {
		String *newStrings;
		Int newMax;
	
		newMax = compiledTables->maxStrings * 2;
		newStrings = GC_MALLOC_STRUCT_ARRAY(String, newMax);
		if (newStrings == NULL)
			Smile_Abort_OutOfMemory();
		MemCpy(newStrings, compiledTables->strings, compiledTables->numStrings);
		compiledTables->strings = newStrings;
		compiledTables->maxStrings = newMax;
	}

	// Okay, we have enough space, and it's not there yet, so add it.
	index = compiledTables->numStrings++;
	compiledTables->strings[index] = string;
	StringIntDict_Add(compiledTables->stringLookup, string, index);

	return index;
}

/// <summary>
/// Add a new TillContinuationInfo object to the compiler's till-object table.
/// </summary>
/// <param name="compiler">The compiler that has the till-object table that the new till-object will be added to.</param>
/// <param name="obj">The till-object to add.</param>
/// <returns>A pointer to the new till-object.</returns>
TillContinuationInfo Compiler_AddTillContinuationInfo(Compiler compiler, UserFunctionInfo userFunctionInfo, Int numOffsets)
{
	CompiledTables compiledTables = compiler->compiledTables;
	Int index;
	TillContinuationInfo tillInfo;

	// Do we have enough space to add a new one?  If not, reallocate.
	if (compiledTables->numTillInfos >= compiledTables->maxTillInfos) {
		TillContinuationInfo *newTillInfos;
		Int newMax;

		newMax = compiledTables->maxTillInfos * 2;
		newTillInfos = GC_MALLOC_STRUCT_ARRAY(TillContinuationInfo, newMax);
		if (newTillInfos == NULL)
			Smile_Abort_OutOfMemory();
		MemCpy(newTillInfos, compiledTables->tillInfos, compiledTables->numTillInfos);
		compiledTables->tillInfos = newTillInfos;
		compiledTables->maxTillInfos = newMax;
	}

	// Okay, we have enough space, and it's not there yet, so add it.
	index = compiledTables->numTillInfos++;
	tillInfo = (TillContinuationInfo)GC_MALLOC(sizeof(struct TillContinuationInfoStruct) + (sizeof(Int) * (numOffsets - 1)));
	if (tillInfo == NULL)
		Smile_Abort_OutOfMemory();
	tillInfo->tillIndex = index;
	tillInfo->userFunctionInfo = userFunctionInfo;
	tillInfo->numOffsets = numOffsets;
	compiledTables->tillInfos[index] = tillInfo;

	return tillInfo;
}

/// <summary>
/// Add the given object to the compiler's static-object table.
/// </summary>
/// <param name="compiler">The compiler that has the object table that this object will be added to.</param>
/// <param name="obj">The object to add.</param>
/// <returns>The index of that object in the object table.</returns>
Int Compiler_AddObject(Compiler compiler, SmileObject obj)
{
	CompiledTables compiledTables = compiler->compiledTables;
	Int index;

	// Do we have enough space to add it?  If not, reallocate.
	if (compiledTables->numObjects >= compiledTables->maxObjects) {
		SmileObject *newObjects;
		Int newMax;

		newMax = compiledTables->maxObjects * 2;
		newObjects = GC_MALLOC_STRUCT_ARRAY(SmileObject, newMax);
		if (newObjects == NULL)
			Smile_Abort_OutOfMemory();
		MemCpy(newObjects, compiledTables->objects, compiledTables->numObjects);
		compiledTables->objects = newObjects;
		compiledTables->maxObjects = newMax;
	}

	// Okay, we have enough space, and it's not there yet, so add it.
	index = compiledTables->numObjects++;
	compiledTables->objects[index] = obj;

	return index;
}

/// <summary>
/// Compile expressions in the global scope, creating a new global function for them.
/// </summary>
/// <param name="compiler">The compiler that will be compiling these expressions.</param>
/// <param name="expr">The expression to compile.</param>
/// <returns>The resulting compiled function.</returns>
UserFunctionInfo Compiler_CompileGlobal(Compiler compiler, SmileObject expr)
{
	CompilerFunction compilerFunction;
	Int offset;
	ByteCodeSegment segment;
	UserFunctionInfo userFunctionInfo;
	ClosureInfo closureInfo;
	String errorMessage;

	userFunctionInfo = UserFunctionInfo_Create(NULL, NullList, expr, &errorMessage);
	compilerFunction = Compiler_BeginFunction(compiler, NullList, expr);
	compilerFunction->userFunctionInfo = userFunctionInfo;
	compiler->compiledTables->globalFunctionInfo = userFunctionInfo;

	segment = compiler->currentFunction->byteCodeSegment;

	Compiler_CompileExpr(compiler, expr);

	compiler->currentFunction->currentSourceLocation = 0;
	EMIT0(Op_Ret, -1);

	Compiler_EndFunction(compiler);

	closureInfo = Compiler_MakeClosureInfoForCompilerFunction(compiler, compilerFunction);
	MemCpy(&userFunctionInfo->closureInfo, closureInfo, sizeof(struct ClosureInfoStruct));
	userFunctionInfo->byteCodeSegment = compilerFunction->byteCodeSegment;

	return userFunctionInfo;
}

/// <summary>
/// Prepare a ClosureInfo object, which is the compact runtime equivalent of a CompilerFunction.
/// </summary>
/// <param name="compilerFunction">The compiled function to compact into a ClosureInfo object.</param>
/// <returns>The compiled function's data, as a ClosureInfo object.</returns>
ClosureInfo Compiler_MakeClosureInfoForCompilerFunction(Compiler compiler, CompilerFunction compilerFunction)
{
	ClosureInfo closureInfo;
	Int numVariables, src, dest;
	Symbol *variableNames;
	Symbol symbol;
	struct VarInfoStruct varInfo;
	
	closureInfo = ClosureInfo_Create(compilerFunction->parent != NULL ? compilerFunction->parent->closureInfo : compiler->compiledTables->globalClosureInfo,
		CLOSURE_KIND_LOCAL);

	closureInfo->global = closureInfo->parent != NULL ? closureInfo->parent->global : compiler->compiledTables->globalClosureInfo;
	
	numVariables = compilerFunction->numArgs + compilerFunction->localSize;
	closureInfo->numVariables = (Int16)numVariables;
	closureInfo->tempSize = compilerFunction->stackSize;

	variableNames = (Symbol *)GC_MALLOC_ATOMIC(sizeof(Symbol) * numVariables);
	if (variableNames == NULL)
		Smile_Abort_OutOfMemory();

	closureInfo->variableNames = variableNames;
	dest = 0;

	for (SmileList args = compilerFunction->args; SMILE_KIND(args) != SMILE_KIND_NULL; args = LIST_REST(args)) {
		symbol = ((SmileSymbol)args->a)->symbol;

		varInfo.kind = VAR_KIND_ARG;
		varInfo.offset = dest;
		varInfo.symbol = symbol;
		varInfo.value = NullObject;
		VarDict_SetValue(closureInfo->variableDictionary, symbol, &varInfo);
	
		variableNames[dest++] = symbol;
	}

	for (src = 0; src < compilerFunction->localSize; src++) {
		symbol = compilerFunction->localNames[src];
	
		varInfo.kind = VAR_KIND_VAR;
		varInfo.offset = dest;
		varInfo.symbol = symbol;
		varInfo.value = NullObject;
		VarDict_SetValue(closureInfo->variableDictionary, symbol, &varInfo);

		variableNames[dest++] = symbol;
	}

	return closureInfo;
}

Bool Compiler_StripNots(SmileObject *objPtr)
{
	Bool not = False;
	SmileList list = (SmileList)*objPtr;

	// If this is of the form [$not x]...
	while (SMILE_KIND(list) == SMILE_KIND_LIST && SMILE_KIND(list->a) == SMILE_KIND_SYMBOL
		&& ((SmileSymbol)list->a)->symbol == Smile_KnownSymbols.not_
		&& SMILE_KIND(list->d) == SMILE_KIND_LIST && SMILE_KIND(((SmileList)list->d)->d) == SMILE_KIND_NULL)
	{
		// ...substitute it with just x, and keep track of how many [$not]s we removed.
		list = (SmileList)((SmileList)list->d)->a;
		not = !not;
	}

	*objPtr = (SmileObject)list;

	return not;
}
