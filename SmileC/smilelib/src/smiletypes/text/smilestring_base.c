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

#include <smile/string.h>
#include <smile/stringbuilder.h>
#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smileuserobject.h>
#include <smile/smiletypes/numeric/smilebyte.h>
#include <smile/smiletypes/numeric/smileinteger16.h>
#include <smile/smiletypes/numeric/smileinteger32.h>
#include <smile/smiletypes/numeric/smileinteger64.h>
#include <smile/smiletypes/text/smilestring.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/smiletypes/base.h>

static Byte _stringChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
};

static Byte _stringComparisonChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	0, 0,
};

static Byte _stringNumberChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER64,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER64,
};

static Byte _indexOfChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_STRING,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER64,
};

STATIC_STRING(_invalidTypeError, "All arguments to 'String.%s' must be of type 'String'");

//-------------------------------------------------------------------------------------------------
// Generic type conversion

static SmileObject ToBool(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[0]) == SMILE_KIND_STRING)
		return ((SmileString)argv[0])->string.length ? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;

	return (SmileObject)Smile_KnownObjects.TrueObj;
}

static SmileObject ToInt(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[0]) == SMILE_KIND_STRING)
		return (SmileObject)SmileInteger64_Create(((SmileString)argv[0])->string.length);

	return (SmileObject)Smile_KnownObjects.ZeroInt64;
}

STATIC_STRING(_String, "String");

static SmileObject ToString(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[0]) == SMILE_KIND_STRING)
		return argv[0];

	return (SmileObject)SmileString_Create(_String);
}

static SmileObject Hash(Int argc, SmileObject *argv, void *param)
{
	Int64 hash;

	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[0]) == SMILE_KIND_STRING) {
		hash = String_Hash64(SmileString_GetString((SmileString)argv[0]));
		return (SmileObject)SmileInteger64_Create(hash);
	}

	return (SmileObject)SmileInteger64_Create(((PtrInt)argv[0]) ^ Smile_HashOracle);
}

//-------------------------------------------------------------------------------------------------
// Arithmetic operators

STATIC_STRING(_plusSuccessiveTypeError, "'+' requires its successive arguments to be Strings or Integers.");
STATIC_STRING(_plusIllegalUnicodeChar, "'+' cannot append an illegal Unicode character (>= 0x110000).");

static SmileObject Plus(Int argc, SmileObject *argv, void *param)
{
	String x;
	Int i;
	Int64 value;
	DECLARE_INLINE_STRINGBUILDER(stringBuilder, 256);

	UNUSED(param);

	if (SMILE_KIND(argv[0]) != SMILE_KIND_STRING)
		goto concat_many;

	if (argc == 1)
		return argv[0];

	if (argc == 2) {
		// Concatenating exactly two things: a string + (a string | a char | a Unicode char).
		x = SmileString_GetString((SmileString)argv[0]);
	
		switch (SMILE_KIND(argv[1])) {
			case SMILE_KIND_STRING:
				x = String_Concat(x, SmileString_GetString((SmileString)argv[1]));
				return (SmileObject)SmileString_Create(x);
			case SMILE_KIND_BYTE:
				x = String_ConcatByte(x, ((SmileByte)argv[1])->value);
				return (SmileObject)SmileString_Create(x);
			case SMILE_KIND_INTEGER16:
				value = ((SmileInteger16)argv[1])->value;
				break;
			case SMILE_KIND_INTEGER32:
				value = ((SmileInteger32)argv[1])->value;
				break;
			case SMILE_KIND_INTEGER64:
				value = ((SmileInteger64)argv[1])->value;
				break;
			default:
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _plusSuccessiveTypeError);
				value = 0;
				break;
		}
	
		if (value < 256 && value >= 0) {
			x = String_ConcatByte(x, (Byte)value);
			return (SmileObject)SmileString_Create(x);
		}
		if (value < 0 || value >= 0x110000) {
			Smile_ThrowException(Smile_KnownSymbols.native_method_error, _plusIllegalUnicodeChar);
		}
	
		INIT_INLINE_STRINGBUILDER(stringBuilder);
		StringBuilder_AppendString(stringBuilder, x);
		StringBuilder_AppendUnicode(stringBuilder, (UInt32)value);
		return (SmileObject)SmileString_Create(StringBuilder_ToString(stringBuilder));
	}

concat_many:
	// Concatenating many things, which should all be Strings or Integers (chars or Unicode chars).
	// We allow for the special case of the first thing not being a String, so that this can be
	// used in a "static" fashion.
	INIT_INLINE_STRINGBUILDER(stringBuilder);

	for (i = 0; i < argc; i++) {
	
		switch (SMILE_KIND(argv[i])) {
		
		case SMILE_KIND_STRING:
			StringBuilder_AppendString(stringBuilder, SmileString_GetString((SmileString)argv[i]));
			break;
		case SMILE_KIND_BYTE:
			StringBuilder_AppendByte(stringBuilder, ((SmileByte)argv[1])->value);
			break;
		case SMILE_KIND_INTEGER16:
			StringBuilder_AppendUnicode(stringBuilder, ((SmileInteger16)argv[1])->value);
			break;
		case SMILE_KIND_INTEGER32:
			value = ((SmileInteger32)argv[1])->value;
			goto append_unicode;
		case SMILE_KIND_INTEGER64:
			value = ((SmileInteger64)argv[1])->value;
		append_unicode:
			if (value < 0 || value >= 0x110000) {
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _plusIllegalUnicodeChar);
			}
			StringBuilder_AppendUnicode(stringBuilder, (UInt32)value);
			break;		
		default:
			if (i == 0) continue;
			Smile_ThrowException(Smile_KnownSymbols.native_method_error, _plusSuccessiveTypeError);
			break;
		}
	}

	return (SmileObject)SmileString_Create(StringBuilder_ToString(stringBuilder));
}

static SmileObject Remove(Int argc, SmileObject *argv, void *param)
{
	String x;

	UNUSED(param);

	if (argc == 2) {
		// Subtract:  Remove string y from string x.
		x = SmileString_GetString((SmileString)argv[0]);
		x = String_Replace(x, SmileString_GetString((SmileString)argv[1]), String_Empty);
		return (SmileObject)SmileString_Create(x);
	}
	else {
		// Negate:  Reverse string x.
		x = SmileString_GetString((SmileString)argv[0]);
		x = String_Reverse(x);
		return (SmileObject)SmileString_Create(x);
	}
}

static SmileObject Repeat(Int argc, SmileObject *argv, void *param)
{
	String x;

	UNUSED(argc);
	UNUSED(param);

	x = SmileString_GetString((SmileString)argv[0]);
	x = String_Repeat(x, (Int)((SmileInteger64)argv[1])->value);
	return (SmileObject)SmileString_Create(x);
}

static SmileObject SlashAppend(Int argc, SmileObject *argv, void *param)
{
	String x;
	Int i, j;
	String strs[16];

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];
		
		case 2:
			strs[0] = SmileString_GetString((SmileString)argv[0]);
			strs[1] = SmileString_GetString((SmileString)argv[1]);
			x = String_SlashAppend(strs, 2);
			return (SmileObject)SmileString_Create(x);
	
		case 3:
			strs[0] = SmileString_GetString((SmileString)argv[0]);
			strs[1] = SmileString_GetString((SmileString)argv[1]);
			strs[2] = SmileString_GetString((SmileString)argv[2]);
			x = String_SlashAppend(strs, 3);
			return (SmileObject)SmileString_Create(x);

		case 4:
			strs[0] = SmileString_GetString((SmileString)argv[0]);
			strs[1] = SmileString_GetString((SmileString)argv[1]);
			strs[2] = SmileString_GetString((SmileString)argv[2]);
			strs[3] = SmileString_GetString((SmileString)argv[3]);
			x = String_SlashAppend(strs, 4);
			return (SmileObject)SmileString_Create(x);

		default:
			// Smash together the strings in batches of up to 16 strings each.
			x = SmileString_GetString((SmileString)argv[0]);
			for (i = 1; i < argc; i++) {
				strs[0] = x;
				for (j = 1; j < 16 && i < argc; j++, i++) {
					strs[j] = SmileString_GetString((SmileString)argv[i]);
				}
				x = String_SlashAppend(strs, j);
			}
			return (SmileObject)SmileString_Create(x);
	}
}

//-------------------------------------------------------------------------------------------------
// Comparisons

static SmileObject Eq(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_STRING
		|| !String_Equals(SmileString_GetString((SmileString)argv[0]), SmileString_GetString((SmileString)argv[1])))
		return (SmileObject)Smile_KnownObjects.FalseObj;

	return (SmileObject)Smile_KnownObjects.TrueObj;
}

static SmileObject Ne(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_STRING
		|| !String_Equals(SmileString_GetString((SmileString)argv[0]), SmileString_GetString((SmileString)argv[1])))
		return (SmileObject)Smile_KnownObjects.TrueObj;

	return (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject EqI(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_STRING
		|| String_CompareI(SmileString_GetString((SmileString)argv[0]), SmileString_GetString((SmileString)argv[1])) != 0)
		return (SmileObject)Smile_KnownObjects.FalseObj;

	return (SmileObject)Smile_KnownObjects.TrueObj;
}

static SmileObject NeI(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_STRING
		|| String_CompareI(SmileString_GetString((SmileString)argv[0]), SmileString_GetString((SmileString)argv[1])) != 0)
		return (SmileObject)Smile_KnownObjects.TrueObj;

	return (SmileObject)Smile_KnownObjects.FalseObj;
}

#define RELATIVE_COMPARE(__name__, __func__, __op__) \
	static SmileObject __name__(Int argc, SmileObject *argv, void *param) \
	{ \
		String x = SmileString_GetString((SmileString)argv[0]); \
		String y = SmileString_GetString((SmileString)argv[1]); \
		Int cmp = __func__(x, y); \
		\
		UNUSED(argc); \
		UNUSED(param); \
		\
		return cmp __op__ 0 ? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj; \
	}

RELATIVE_COMPARE(Lt, String_Compare, <)
RELATIVE_COMPARE(Gt, String_Compare, >)
RELATIVE_COMPARE(Le, String_Compare, <=)
RELATIVE_COMPARE(Ge, String_Compare, >=)
RELATIVE_COMPARE(LtI, String_CompareI, <)
RELATIVE_COMPARE(GtI, String_CompareI, >)
RELATIVE_COMPARE(LeI, String_CompareI, <=)
RELATIVE_COMPARE(GeI, String_CompareI, >=)

static SmileObject Compare(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Int cmp = String_Compare(x, y);

	UNUSED(argc);
	UNUSED(param);

	if (cmp == 0)
		return (SmileObject)Smile_KnownObjects.ZeroInt64;
	else if (cmp < 0)
		return (SmileObject)Smile_KnownObjects.NegOneInt64;
	else
		return (SmileObject)Smile_KnownObjects.OneInt64;
}

static SmileObject CompareI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Int cmp = String_CompareI(x, y);

	UNUSED(argc);
	UNUSED(param);

	if (cmp == 0)
		return (SmileObject)Smile_KnownObjects.ZeroInt64;
	else if (cmp < 0)
		return (SmileObject)Smile_KnownObjects.NegOneInt64;
	else
		return (SmileObject)Smile_KnownObjects.OneInt64;
}

//-------------------------------------------------------------------------------------------------

static SmileObject StartsWith(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_StartsWith(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

static SmileObject StartsWithI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_StartsWithI(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

static SmileObject EndsWith(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_EndsWith(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

static SmileObject EndsWithI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_EndsWithI(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

static SmileObject Contains(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_Contains(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

static SmileObject ContainsI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);
	Bool result = String_ContainsI(x, y);
	UNUSED(argc);
	UNUSED(param);
	return (SmileObject)Smile_KnownObjects.BooleanObjs[result];
}

//-------------------------------------------------------------------------------------------------

static SmileObject IndexOf(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);

	Int64 startIndex = argc > 2 ? ((SmileInteger64)argv[2])->value : 0;
	Int stringLength = String_Length(x);
	Int result;

	UNUSED(param);

	result = startIndex < stringLength ? String_IndexOf(x, y, (Int)startIndex) : -1;

	return (SmileObject)SmileInteger64_Create(result);
}

static SmileObject IndexOfI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);

	Int64 startIndex = argc > 2 ? ((SmileInteger64)argv[2])->value : 0;
	Int stringLength = String_Length(x);
	Int result;

	UNUSED(param);

	result = startIndex < stringLength ? String_IndexOfI(x, y, (Int)startIndex) : -1;

	return (SmileObject)SmileInteger64_Create(result);
}

static SmileObject LastIndexOf(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);

	Int64 startIndex = argc > 2 ? ((SmileInteger64)argv[2])->value : 0;
	Int stringLength = String_Length(x);
	Int result;

	UNUSED(param);

	result = startIndex < stringLength ? String_LastIndexOf(x, y, (Int)startIndex) : -1;

	return (SmileObject)SmileInteger64_Create(result);
}

static SmileObject LastIndexOfI(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	String y = SmileString_GetString((SmileString)argv[1]);

	Int64 startIndex = argc > 2 ? ((SmileInteger64)argv[2])->value : 0;
	Int stringLength = String_Length(x);
	Int result;

	UNUSED(param);

	result = startIndex < stringLength ? String_LastIndexOfI(x, y, (Int)startIndex) : -1;

	return (SmileObject)SmileInteger64_Create(result);
}

//-------------------------------------------------------------------------------------------------

STATIC_STRING(_indexOutOfRangeError, "Index to 'get-member' is beyond the length of the string.");

static SmileObject GetMember(Int argc, SmileObject *argv, void *param)
{
	struct StringInt *x = (struct StringInt *)SmileString_GetString((SmileString)argv[0]);
	Int64 index = ((SmileInteger64)argv[1])->value;
	Byte ch;

	UNUSED(argc);
	UNUSED(param);

	if (index < 0 || index >= x->length)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _indexOutOfRangeError);

	ch = x->text[(Int)index];

	return (SmileObject)SmileByte_Create(ch);
}

static SmileObject Substr(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);

	UNUSED(param);

	if (argc == 2) {
		Int64 index = ((SmileInteger64)argv[1])->value;
		return (SmileObject)SmileString_Create(String_SubstringAt(x, (Int)index));
	}
	else {
		Int64 index = ((SmileInteger64)argv[1])->value;
		Int64 length = ((SmileInteger64)argv[2])->value;
		return (SmileObject)SmileString_Create(String_Substring(x, (Int)index, (Int)length));
	}
}

static SmileObject Left(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	Int64 length = (Int)((SmileInteger64)argv[1])->value;
	Int stringLength = String_Length(x);

	UNUSED(argc);
	UNUSED(param);

	return length >= stringLength ? argv[0]
		: (SmileObject)SmileString_Create(String_Substring(x, 0, (Int)length));
}

static SmileObject Right(Int argc, SmileObject *argv, void *param)
{
	String x = SmileString_GetString((SmileString)argv[0]);
	Int64 length = (Int)((SmileInteger64)argv[1])->value;
	Int stringLength = String_Length(x);

	UNUSED(argc);
	UNUSED(param);

	return length >= stringLength ? argv[0]
		: (SmileObject)SmileString_Create(String_SubstringAt(x, (Int)stringLength - (Int)length));
}

//-------------------------------------------------------------------------------------------------

#define UnaryProxyFunction(__functionName__, __stringName__) \
	static SmileObject __functionName__(Int argc, SmileObject *argv, void *param) \
	{ \
		String x = SmileString_GetString((SmileString)argv[0]); \
		UNUSED(argc); \
		UNUSED(param); \
		return (SmileObject)SmileString_Create(__stringName__(x)); \
	}

UnaryProxyFunction(CaseFold, String_CaseFold)
UnaryProxyFunction(Uppercase, String_ToUpper)
UnaryProxyFunction(Lowercase, String_ToLower)
UnaryProxyFunction(Titlecase, String_ToTitle)
UnaryProxyFunction(Decompose, String_Decompose)
UnaryProxyFunction(Compose, String_Compose)
UnaryProxyFunction(NormalizeDiacritics, String_Normalize)

UnaryProxyFunction(Trim, String_Trim)
UnaryProxyFunction(TrimStart, String_TrimStart)
UnaryProxyFunction(TrimEnd, String_TrimEnd)
UnaryProxyFunction(CompactWhitespace, String_CompactWhitespace)

UnaryProxyFunction(Rot13, String_Rot13)
UnaryProxyFunction(AddCSlashes, String_AddCSlashes)
UnaryProxyFunction(StripCSlashes, String_StripCSlashes)
UnaryProxyFunction(HtmlEncode, String_HtmlEncode)
UnaryProxyFunction(HtmlDecode, String_HtmlDecode)
UnaryProxyFunction(UrlEncode, String_UrlEncode)
UnaryProxyFunction(UrlQueryEncode, String_UrlQueryEncode)
UnaryProxyFunction(UrlDecode, String_UrlDecode)
UnaryProxyFunction(RegexEscape, String_RegexEscape)

//-------------------------------------------------------------------------------------------------

void SmileString_Setup(SmileUserObject base)
{
	SetupFunction("bool", ToBool, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("int", ToInt, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("string", ToString, NULL, "value", ARG_CHECK_MIN | ARG_CHECK_MAX, 1, 2, 0, NULL);
	SetupFunction("hash", Hash, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);

	SetupFunction("+", Plus, NULL, "x y", ARG_CHECK_MIN, 1, 0, 0, NULL);
	SetupFunction("remove", Remove, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_MAX | ARG_CHECK_TYPES, 1, 2, 2, _stringChecks);
	SetupSynonym("remove", "-");
	SetupFunction("repeat", Repeat, NULL, "x count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringNumberChecks);
	SetupSynonym("repeat", "*");
	SetupFunction("/", SlashAppend, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _stringChecks);

	SetupFunction("substr", Substr, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_MAX | ARG_CHECK_TYPES, 2, 3, 3, _stringNumberChecks);
	SetupSynonym("mid", "substr");
	SetupSynonym("substring", "substr");
	SetupFunction("left", Left, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringNumberChecks);
	SetupFunction("right", Right, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringNumberChecks);

	SetupFunction("index-of", IndexOf, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 3, 3, _indexOfChecks);
	SetupFunction("index-of~", IndexOfI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 3, 3, _indexOfChecks);
	SetupFunction("last-index-of", LastIndexOf, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 3, 3, _indexOfChecks);
	SetupFunction("last-index-of~", LastIndexOfI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 3, 3, _indexOfChecks);

	SetupFunction("trim", Trim, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("trim-start", TrimStart, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("trim-end", TrimEnd, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("compact-whitespace", CompactWhitespace, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);

	SetupFunction("case-fold", CaseFold, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupSynonym("case-fold", "fold");
	SetupFunction("lowercase", Lowercase, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("uppercase", Uppercase, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("titlecase", Titlecase, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("decompose", Decompose, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("compose", Compose, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("normalize-diacritics", NormalizeDiacritics, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);

	SetupFunction("starts-with?", StartsWith, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("starts-with~?", StartsWithI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("ends-with?", EndsWith, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("ends-with~?", EndsWithI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("contains?", Contains, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("contains~?", ContainsI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);

	SetupFunction("rot13", Rot13, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("add-c-slashes", AddCSlashes, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("strip-c-slashes", StripCSlashes, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("html-encode", HtmlEncode, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("html-decode", HtmlDecode, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("url-encode", UrlEncode, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("url-query-encode", UrlQueryEncode, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("url-decode", UrlDecode, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);
	SetupFunction("regex-escape", RegexEscape, NULL, "string", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _stringChecks);

	SetupFunction("==", Eq, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringComparisonChecks);
	SetupFunction("==~", EqI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringComparisonChecks);
	SetupFunction("!=", Ne, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringComparisonChecks);
	SetupFunction("!=~", NeI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringComparisonChecks);
	SetupFunction("<", Lt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("<~", LtI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction(">", Gt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction(">~", GtI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("<=", Le, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction("<=~", LeI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction(">=", Ge, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupFunction(">=~", GeI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);

	SetupFunction("compare", Compare, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupSynonym("compare", "cmp");
	SetupFunction("compare~", CompareI, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringChecks);
	SetupSynonym("compare~", "cmp~");

	SetupFunction("get-member", GetMember, NULL, "str index", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _stringNumberChecks);
}
