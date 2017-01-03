//---------------------------------------------------------------------------------------
//  Smile Programming Language Interpreter
//  Copyright 2004-2016 Sean Werkema
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

#include <smile/types.h>
#include <smile/string.h>
#include <smile/stringbuilder.h>
#include <smile/smiletypes/kind.h>
#include <smile/smiletypes/smilebool.h>
#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/smiletypes/smilepair.h>
#include <smile/smiletypes/smilesyntax.h>
#include <smile/smiletypes/numeric/smilebyte.h>
#include <smile/smiletypes/numeric/smileinteger16.h>
#include <smile/smiletypes/numeric/smileinteger32.h>
#include <smile/smiletypes/numeric/smileinteger64.h>
#include <smile/smiletypes/numeric/smileinteger128.h>
#include <smile/smiletypes/text/smilestring.h>
#include <smile/smiletypes/text/smilesymbol.h>

static void StringifyRecursive(SmileObject obj, StringBuilder stringBuilder, Int indent);

Bool SmileObject_ContainsNestedList(SmileObject obj)
{
	SmileObject item;
	SmileList list;
	SmilePair pair;

	switch (SMILE_KIND(obj)) {

	case SMILE_KIND_PAIR:
		pair = (SmilePair)obj;
		if (SmileObject_ContainsNestedList(pair->left)) return True;
		if (SmileObject_ContainsNestedList(pair->right)) return True;
		break;

	case SMILE_KIND_LIST:
		for (list = (SmileList)obj; SMILE_KIND(list) == SMILE_KIND_LIST; list = LIST_REST(list)) {
			item = list->a;
			if (SMILE_KIND(item) == SMILE_KIND_LIST) return True;
		}
		break;
	}

	return False;
}

Bool SmileObject_IsRegularList(SmileObject list)
{
	while (SMILE_KIND(list) == SMILE_KIND_LIST) {
		list = (SmileObject)LIST_REST((SmileList)list);
	}
	return SMILE_KIND(list) == SMILE_KIND_NULL;
}

String SmileObject_Stringify(SmileObject obj)
{
	DECLARE_INLINE_STRINGBUILDER(stringBuilder, 1024);
	INIT_INLINE_STRINGBUILDER(stringBuilder);
	StringifyRecursive(obj, stringBuilder, 0);
	return StringBuilder_ToString(stringBuilder);
}

const char *SmileObject_StringifyToC(SmileObject obj)
{
	return String_ToC(SmileObject_Stringify(obj));
}

STATIC_STRING(TrueString, "true");
STATIC_STRING(FalseString, "false");

static void StringifyRecursive(SmileObject obj, StringBuilder stringBuilder, Int indent)
{
	SmileList list;
	SmilePair pair;
	Bool isFirst;

	if (obj == NULL) {
		StringBuilder_AppendC(stringBuilder, "<NULL>", 0, 6);
		return;
	}

	switch (SMILE_KIND(obj)) {

	case SMILE_KIND_LIST:
		list = (SmileList)obj;
		if (!SmileObject_IsRegularList(obj)) {
			StringBuilder_AppendByte(stringBuilder, '(');
			while (SMILE_KIND(list) == SMILE_KIND_LIST) {
				StringifyRecursive((SmileObject)list->a, stringBuilder, indent + 1);
				StringBuilder_AppendC(stringBuilder, " ## ", 0, 4);
				list = LIST_REST(list);
			}
			StringifyRecursive((SmileObject)list, stringBuilder, indent + 1);
			StringBuilder_AppendByte(stringBuilder, ')');
		}
		else if (SmileObject_ContainsNestedList(obj)) {
			Bool isFirst = True;
			StringBuilder_AppendByte(stringBuilder, '[');
			while (SMILE_KIND(list) == SMILE_KIND_LIST && SMILE_KIND(list->a) == SMILE_KIND_SYMBOL) {
				if (!isFirst) {
					StringBuilder_AppendByte(stringBuilder, ' ');
				}
				StringifyRecursive((SmileObject)list->a, stringBuilder, indent + 1);
				list = LIST_REST(list);
				isFirst = False;
			}
			StringBuilder_AppendByte(stringBuilder, '\n');
			while (SMILE_KIND(list) == SMILE_KIND_LIST) {
				StringBuilder_AppendRepeat(stringBuilder, ' ', (indent + 1) * 4);
				StringifyRecursive((SmileObject)list->a, stringBuilder, indent + 1);
				StringBuilder_AppendByte(stringBuilder, '\n');
				list = LIST_REST(list);
			}
			StringBuilder_AppendRepeat(stringBuilder, ' ', indent * 4);
			StringBuilder_AppendByte(stringBuilder, ']');
		}
		else {
			isFirst = True;
			StringBuilder_AppendByte(stringBuilder, '[');
			while (SMILE_KIND(list) == SMILE_KIND_LIST) {
				if (!isFirst) {
					StringBuilder_AppendByte(stringBuilder, ' ');
				}
				StringifyRecursive((SmileObject)list->a, stringBuilder, indent + 1);
				list = LIST_REST(list);
				isFirst = False;
			}
			StringBuilder_AppendByte(stringBuilder, ']');
		}
		return;

	case SMILE_KIND_PRIMITIVE:
		StringBuilder_AppendC(stringBuilder, "Primitive", 0, 9);
		return;

	case SMILE_KIND_NULL:
		StringBuilder_AppendC(stringBuilder, "null", 0, 4);
		return;

	case SMILE_KIND_BOOL:
		StringBuilder_AppendString(stringBuilder, ((SmileBool)obj)->value ? TrueString : FalseString);
		return;
	
	case SMILE_KIND_PAIR:
		pair = (SmilePair)obj;
		if (SMILE_KIND(pair->left) == SMILE_KIND_PAIR || ((SMILE_KIND(pair->left) & 0xF0) == 0x10)) {  // Pairs and numbers
			StringBuilder_AppendByte(stringBuilder, '(');
			StringifyRecursive(pair->left, stringBuilder, indent + 1);
			StringBuilder_AppendByte(stringBuilder, ')');
		}
		else {
			StringifyRecursive(pair->left, stringBuilder, indent);
		}
		StringBuilder_AppendByte(stringBuilder, '.');
		if (SMILE_KIND(pair->right) == SMILE_KIND_PAIR || ((SMILE_KIND(pair->right) & 0xF0) == 0x10)) {  // Pairs and numbers
			StringBuilder_AppendByte(stringBuilder, '(');
			StringifyRecursive(pair->right, stringBuilder, indent + 1);
			StringBuilder_AppendByte(stringBuilder, ')');
		}
		else {
			StringifyRecursive(pair->right, stringBuilder, indent);
		}
		return;

	case SMILE_KIND_SYMBOL:
		StringBuilder_AppendString(stringBuilder, SymbolTable_GetName(Smile_SymbolTable, ((SmileSymbol)obj)->symbol));
		return;

	case SMILE_KIND_BYTE:
		StringBuilder_AppendFormat(stringBuilder, "%u", (Int32)((SmileByte)obj)->value);
		return;

	case SMILE_KIND_INTEGER16:
		StringBuilder_AppendFormat(stringBuilder, "%d", (Int32)((SmileInteger16)obj)->value);
		return;

	case SMILE_KIND_INTEGER32:
		StringBuilder_AppendFormat(stringBuilder, "%d", ((SmileInteger32)obj)->value);
		return;

	case SMILE_KIND_INTEGER64:
		StringBuilder_AppendFormat(stringBuilder, "%ld", ((SmileInteger64)obj)->value);
		return;

	case SMILE_KIND_STRING:
		StringBuilder_AppendFormat(stringBuilder, "\"%S\"", String_AddCSlashes((String)&((SmileString)obj)->string));
		return;

	case SMILE_KIND_NONTERMINAL:
		StringBuilder_AppendFormat(stringBuilder, "[%S %S %S %S]",
			((SmileNonterminal)obj)->nonterminal != 0 ? SymbolTable_GetName(Smile_SymbolTable, ((SmileNonterminal)obj)->nonterminal) : String_Empty,
			((SmileNonterminal)obj)->name != 0 ? SymbolTable_GetName(Smile_SymbolTable, ((SmileNonterminal)obj)->name) : String_Empty,
			((SmileNonterminal)obj)->repeat != 0 ? SymbolTable_GetName(Smile_SymbolTable, ((SmileNonterminal)obj)->repeat) : String_Empty,
			((SmileNonterminal)obj)->separator != 0 ? SymbolTable_GetName(Smile_SymbolTable, ((SmileNonterminal)obj)->separator) : String_Empty
		);
		return;

	case SMILE_KIND_SYNTAX:
		StringBuilder_AppendFormat(stringBuilder, "#syntax %S ",
			((SmileSyntax)obj)->nonterminal != 0 ? SymbolTable_GetName(Smile_SymbolTable, ((SmileSyntax)obj)->nonterminal) : String_Empty);
		StringifyRecursive((SmileObject)((SmileSyntax)obj)->pattern, stringBuilder, indent + 1);
		StringBuilder_AppendC(stringBuilder, " => ", 0, 4);
		StringifyRecursive(((SmileSyntax)obj)->replacement, stringBuilder, indent + 1);
		return;

	default:
		StringBuilder_AppendFormat(stringBuilder, "<%d>", SMILE_KIND(obj));
		return;
	}
}
