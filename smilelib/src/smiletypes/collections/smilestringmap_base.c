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

#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smileuserobject.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/smiletypes/smilebool.h>
#include <smile/smiletypes/numeric/smileinteger64.h>
#include <smile/smiletypes/collections/smilestringmap.h>
#include <smile/smiletypes/text/smilesymbol.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/eval/eval.h>
#include <smile/smiletypes/base.h>
#include <smile/internal/staticstring.h>

SMILE_IGNORE_UNUSED_VARIABLES

static Byte _stringMapChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
};

static Byte _stringMapMemberChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	0, 0,
};

static Byte _eachChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRINGMAP,
	SMILE_KIND_MASK, SMILE_KIND_FUNCTION,
};

//-------------------------------------------------------------------------------------------------
// Generic type conversion

SMILE_EXTERNAL_FUNCTION(ToBool)
{
	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_STRINGMAP)
		return SmileUnboxedBool_From(StringDict_Count(&((SmileStringMap)argv[0].obj)->dict) > 0);

	return SmileUnboxedBool_From(True);
}

SMILE_EXTERNAL_FUNCTION(ToInt)
{
	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_STRINGMAP)
		return SmileUnboxedInteger64_From(StringDict_Count(&((SmileStringMap)argv[0].obj)->dict));

	return SmileUnboxedInteger64_From(0);
}

SMILE_EXTERNAL_FUNCTION(ToString)
{
	STATIC_STRING(stringMap, "StringMap");

	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_STRINGMAP)
		return SmileArg_From((SmileObject)String_Format("StringMap of %d", StringDict_Count(&((SmileStringMap)argv[0].obj)->dict)));

	return SmileArg_From((SmileObject)stringMap);
}

SMILE_EXTERNAL_FUNCTION(Hash)
{
	return SmileUnboxedInteger64_From((PtrInt)argv[0].obj ^ Smile_HashOracle);
}

//-------------------------------------------------------------------------------------------------
// Instantiation

SMILE_EXTERNAL_FUNCTION(Create)
{
	SmileStringMap stringMap = SmileStringMap_Create();

	return SmileArg_From((SmileObject)stringMap);
}

//-------------------------------------------------------------------------------------------------
// Member-access

SMILE_EXTERNAL_FUNCTION(GetMember)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	String string = (String)argv[1].obj;

	void *value;
	if (!StringDict_TryGetValue(&stringMap->dict, string, &value))
		return SmileArg_From(NullObject);

	return SmileArg_From((SmileObject)value);
}

SMILE_EXTERNAL_FUNCTION(SetMember)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	String string = (String)argv[1].obj;
	SmileObject value = SmileArg_Box(argv[2]);

	StringDict_SetValue(&stringMap->dict, string, (void *)value);

	return argv[2];
}

//-------------------------------------------------------------------------------------------------
// Map-specific methods.

SMILE_EXTERNAL_FUNCTION(Add)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	String string = (String)argv[1].obj;
	SmileObject value = SmileArg_Box(argv[2]);

	return SmileUnboxedBool_From(StringDict_Add(&stringMap->dict, string, (void *)value));
}

SMILE_EXTERNAL_FUNCTION(Remove)
{
	SmileStringMap stringMap;
	String string;
	Int i;
	Bool result = True;

	if (SMILE_KIND(argv[0].obj) != SMILE_KIND_STRINGMAP)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format("Argument 1 to 'remove' should be a StringMap but is a %S.",
			SmileKind_GetName(SMILE_KIND(argv[0].obj))));
	stringMap = (SmileStringMap)argv[0].obj;

	// Remove all of the provided symbols, at once.
	for (i = 1; i < argc; i++) {
		if (SMILE_KIND(argv[i].obj) != SMILE_KIND_STRING) {
			Smile_ThrowException(Smile_KnownSymbols.native_method_error, String_Format("Argument %d to 'remove' should be a String but is a %S.",
				i + 1, SmileKind_GetName(SMILE_KIND(argv[i].obj))));
		}

		string = (String)argv[i].obj;
		result &= StringDict_Remove(&stringMap->dict, string);
	}

	// True if all of them were removed, false if any of them didn't exist.
	return SmileUnboxedBool_From(result);
}

SMILE_EXTERNAL_FUNCTION(ContainsKey)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	String string = (String)argv[1].obj;

	return SmileUnboxedBool_From(StringDict_ContainsKey(&stringMap->dict, string));
}

SMILE_EXTERNAL_FUNCTION(Keys)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	SmileList head = NullList, tail = NullList;

	STRINGDICT_WALK(&stringMap->dict, {
		LIST_APPEND(head, tail, node->key);
	})

	return SmileArg_From((SmileObject)head);
}

SMILE_EXTERNAL_FUNCTION(Values)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	SmileList head = NullList, tail = NullList;

	STRINGDICT_WALK(&stringMap->dict, {
		LIST_APPEND(head, tail, node->value);
	})

	return SmileArg_From((SmileObject)head);
}

SMILE_EXTERNAL_FUNCTION(Pairs)
{
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	SmileList head = NullList, tail = NullList;

	STRINGDICT_WALK(&stringMap->dict, {
		LIST_APPEND(head, tail, SmileList_CreateTwo(node->key, node->value));
	})

	return SmileArg_From((SmileObject)head);
}

//-------------------------------------------------------------------------------------------------

typedef struct EachInfoStruct {
	SmileStringMap stringMap;
	SmileFunction function;
	Int32 bucket, nodeIndex;
	Int32 index, numArgs;
} *EachInfo;

static Int EachCommon(ClosureStateMachine closure)
{
	EachInfo eachInfo = (EachInfo)closure->state;
	struct StringDictInt *dict = &eachInfo->stringMap->dict._opaque;
	struct StringDictNode *node;
	Int32 index;

	// Get the current list node, and then quietly move the index to the next list node.
	node = &dict->heap[eachInfo->nodeIndex];
	eachInfo->nodeIndex = node->next;
	index = eachInfo->index++;

	// Body: Set up to call the user's function with the next map item (a constructed pair).
	Closure_PushBoxed(closure, eachInfo->function);

	// Push the arguments to the user function.
	switch (eachInfo->numArgs) {
		case 1:
			// One arg: Pass a list, shaped like |[key value]|.
			Closure_PushBoxed(closure, SmileList_CreateTwo(node->key, node->value));
			return 1;

		case 2:
			// Two args: Pass a list and index, shaped like |[key value] index|.
			Closure_PushBoxed(closure, SmileList_CreateTwo(node->key, node->value));
			Closure_PushUnboxedInt64(closure, index);
			return 2;

		default:
			// Three args: Pass all the pieces individually, shaped like |key value index|
			Closure_PushBoxed(closure, node->key);
			Closure_UnboxAndPush(closure, node->value);
			Closure_PushUnboxedInt64(closure, index);
			return 3;
	}
}

static Int EachStartBucket(ClosureStateMachine closure)
{
	EachInfo eachInfo = (EachInfo)closure->state;
	struct StringDictInt *dict = &eachInfo->stringMap->dict._opaque;

	//---------- begin outer for-loop iteration ----------

nextBucket:
	// Condition: If we've run out of buckets, we're done.
	if (eachInfo->bucket > dict->mask) {
		Closure_Pop(closure);								// Pop the previous return value
		Closure_PushBoxed(closure, eachInfo->stringMap);	// and push 'stringMap' as the new return value.
		return -1;
	}

	// Start the inner loop that walks the linked list of nodes for this bucket.
	eachInfo->nodeIndex = dict->buckets[eachInfo->bucket];

	// If the linked list is empty, move to the next bucket.
	if (eachInfo->nodeIndex < 0) {
		eachInfo->bucket++;
		goto nextBucket;
	}

	//---------- begin inner for-loop iteration ----------

	return EachCommon(closure);
}

static Int EachNext(ClosureStateMachine closure)
{
	EachInfo eachInfo = (EachInfo)closure->state;

	// If the current linked list is empty, restart at the next bucket.
	if (eachInfo->nodeIndex < 0) {
		eachInfo->bucket++;
		return EachStartBucket(closure);
	}

	//---------- end previous inner for-loop iteration ----------

	//---------- begin next inner for-loop iteration ----------

	return EachCommon(closure);
}

SMILE_EXTERNAL_FUNCTION(Each)
{
	// We use Eval's state-machine construct to avoid recursing deeper on the C stack.
	SmileStringMap stringMap = (SmileStringMap)argv[0].obj;
	SmileFunction function = (SmileFunction)argv[1].obj;
	Int minArgs, maxArgs;
	EachInfo eachInfo;
	ClosureStateMachine closure;

	SmileFunction_GetArgCounts(function, &minArgs, &maxArgs);

	closure = Eval_BeginStateMachine(EachStartBucket, EachNext);

	eachInfo = (EachInfo)closure->state;
	eachInfo->stringMap = stringMap;
	eachInfo->function = function;
	eachInfo->bucket = 0;
	eachInfo->index = 0;
	eachInfo->numArgs = maxArgs < 3 ? maxArgs : 3;

	Closure_PushBoxed(closure, NullObject);	// Initial "return" value from 'each'.

	return (SmileArg) { NULL };	// We have to return something, but this value will be ignored.
}

//-------------------------------------------------------------------------------------------------

void SmileStringMap_Setup(SmileUserObject base)
{
	SetupFunction("bool", ToBool, NULL, "map", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("int", ToInt, NULL, "map", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("string", ToString, NULL, "map", ARG_CHECK_MIN | ARG_CHECK_MAX, 1, 2, 0, NULL);
	SetupFunction("hash", Hash, NULL, "map", ARG_CHECK_EXACT, 1, 1, 0, NULL);

	SetupFunction("create", Create, NULL, "", ARG_CHECK_MIN | ARG_CHECK_MAX, 0, 1, 0, NULL);

	SetupFunction("get-member", GetMember, NULL, "map string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringMapMemberChecks);
	SetupFunction("set-member", SetMember, NULL, "map string value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 3, 3, 3, _stringMapMemberChecks);

	SetupFunction("add", Add, NULL, "map string value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 3, 3, 3, _stringMapMemberChecks);
	SetupFunction("remove", Remove, NULL, "map strings...", ARG_CHECK_MIN, 2, 0, 0, NULL);

	SetupFunction("contains-key?", ContainsKey, NULL, "map string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringMapMemberChecks);
	SetupSynonym("contains-key?", "contains?");

	SetupFunction("keys", Keys, NULL, "map", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringMapChecks);
	SetupSynonym("keys", "keys-of");
	SetupFunction("values", Values, NULL, "map", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringMapChecks);
	SetupSynonym("values", "values-of");
	SetupFunction("pairs", Pairs, NULL, "map", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringMapChecks);
	SetupSynonym("pairs", "pairs-of");
	SetupSynonym("pairs", "list");

	SetupFunction("each", Each, NULL, "map fn", ARG_CHECK_EXACT | ARG_CHECK_TYPES | ARG_STATE_MACHINE, 2, 2, 2, _eachChecks);
}
