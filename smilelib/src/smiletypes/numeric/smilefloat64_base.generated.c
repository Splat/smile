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

#include <smile/numeric/float64.h>
#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smileuserobject.h>
#include <smile/smiletypes/smilebool.h>
#include <smile/smiletypes/numeric/smileinteger32.h>
#include <smile/smiletypes/numeric/smileinteger64.h>
#include <smile/smiletypes/numeric/smilefloat64.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/smiletypes/base.h>
#include <smile/internal/staticstring.h>

#include <math.h>

SMILE_IGNORE_UNUSED_VARIABLES

static Byte _float64Checks[] = {
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
};

static Byte _float64ComparisonChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_UNBOXED_FLOAT64,
	0, 0,
};

typedef struct MathInfoStruct {
	Bool isLoud;
} *MathInfo;

static struct MathInfoStruct _loudMath[] = { True };
static struct MathInfoStruct _quietMath[] = { False };

STATIC_STRING(_divideByZero, "Divide by zero error");
STATIC_STRING(_negativeSqrt, "Square root of negative number");

//-------------------------------------------------------------------------------------------------
// Generic type conversion

SMILE_EXTERNAL_FUNCTION(ToBool)
{
	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_UNBOXED_FLOAT64)
		return SmileUnboxedBool_From(argv[0].unboxed.f64 != 0.0);

	return SmileUnboxedBool_From(True);
}

SMILE_EXTERNAL_FUNCTION(ToInt)
{
	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_UNBOXED_FLOAT64)
		return SmileUnboxedInteger64_From((Int64)argv[0].unboxed.f64);

	return SmileUnboxedFloat64_From(0.0);
}

SMILE_EXTERNAL_FUNCTION(ToString)
{
	STATIC_STRING(float64, "Float64");

	if (SMILE_KIND(argv[0].obj) == SMILE_KIND_UNBOXED_FLOAT64) {
		return SmileArg_From((SmileObject)(Float64_ToStringEx(argv[0].unboxed.f64, 0, 0, False)));
	}

	return SmileArg_From((SmileObject)float64);
}

SMILE_EXTERNAL_FUNCTION(Hash)
{
	SmileFloat64 obj = (SmileFloat64)argv[0].obj;

	if (SMILE_KIND(obj) == SMILE_KIND_UNBOXED_FLOAT64)
		return SmileUnboxedInteger64_From((UInt32)(*(UInt64 *)&obj->value ^ (*(UInt64 *)&obj->value >> 32)));

	return SmileUnboxedInteger64_From((UInt32)((PtrInt)obj ^ Smile_HashOracle));
}

//-------------------------------------------------------------------------------------------------
// Basic arithmetic

SMILE_EXTERNAL_FUNCTION(Plus)
{
	Float64 x;
	Int i;

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = argv[0].unboxed.f64;
			x += argv[1].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			x += argv[1].unboxed.f64;
			x += argv[2].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			x += argv[1].unboxed.f64;
			x += argv[2].unboxed.f64;
			x += argv[3].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				x += argv[i].unboxed.f64;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

SMILE_EXTERNAL_FUNCTION(Minus)
{
	Float64 x;
	Int i;

	switch (argc) {
		case 1:
			x = argv[0].unboxed.f64;
			x = -x;
			return SmileUnboxedFloat64_From(x);

		case 2:
			x = argv[0].unboxed.f64;
			x -= argv[1].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			x -= argv[1].unboxed.f64;
			x -= argv[2].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			x -= argv[1].unboxed.f64;
			x -= argv[2].unboxed.f64;
			x -= argv[3].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				x -= argv[i].unboxed.f64;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

SMILE_EXTERNAL_FUNCTION(Star)
{
	Float64 x;
	Int i;

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = argv[0].unboxed.f64;
			x *= argv[1].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			x *= argv[1].unboxed.f64;
			x *= argv[2].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			x *= argv[1].unboxed.f64;
			x *= argv[2].unboxed.f64;
			x *= argv[3].unboxed.f64;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				x *= argv[i].unboxed.f64;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

/// <summary>
/// Deal with division-by-zero.
/// </summary>
/// <param name="param">A pointer to a MathInfo struct that describes how to handle divide-by-zero.</param>
/// <returns>0 if this is a quiet divide-by-zero, or a thrown exception if this is supposed to be an error.</returns>
Inline SmileArg DivideByZero(void *param)
{
	MathInfo mathInfo = (MathInfo)param;

	if (mathInfo->isLoud)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);

	return SmileUnboxedFloat64_From(0.0);
}

SMILE_EXTERNAL_FUNCTION(Slash)
{
	Float64 x, y;
	Int i;

	switch (argc) {
		case 2:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			if ((y = argv[2].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			if ((y = argv[2].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			if ((y = argv[3].unboxed.f64) == 0.0)
				return DivideByZero(param);
			x /= y;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				if ((y = argv[i].unboxed.f64) == 0.0)
					return DivideByZero(param);
				x /= y;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

/// <summary>
/// Perform modulus like mathematicians expect, in which the modulus has the same sign as the divisor (y).
/// </summary>
Inline Float64 MathematiciansModulus(Float64 x, Float64 y)
{
	Float64 rem;

	if (x < 0.0) {
		if (y < 0.0)
			return (Float64)-fmod(-x, -y);
		else {
			rem = (Float64)fmod(-x, y);
			return rem != 0.0 ? y - rem : 0.0;
		}
	}
	else if (y < 0.0) {
		rem = (Float64)fmod(x, -y);
		return rem != 0.0 ? y + rem : 0.0;
	}
	else
		return (Float64)fmod(x, y);
}

/// <summary>
/// Perform remainder, in which the result has the same sign as the dividend (x).
/// </summary>
Inline Float64 MathematiciansRemainder(Float64 x, Float64 y)
{
	Float64 rem;

	if (x < 0.0) {
		if (y < 0.0) {
			rem = (Float64)fmod(-x, -y);
			return rem != 0 ? rem + y : 0.0;
		}
		else
			return -((Float64)fmod(-x, y));
	}
	else if (y < 0.0)
		return (Float64)fmod(x, -y);
	else {
		rem = (Float64)fmod(x, y);
		return rem != 0.0 ? rem - y : 0.0;
	}
}

SMILE_EXTERNAL_FUNCTION(Mod)
{
	Float64 x = argv[0].unboxed.f64;
	Float64 y = argv[1].unboxed.f64;

	if (y == 0.0)
		return DivideByZero(param);

	return SmileUnboxedFloat64_From(MathematiciansModulus(x, y));
}

SMILE_EXTERNAL_FUNCTION(Rem)
{
	Float64 x = argv[0].unboxed.f64;
	Float64 y = argv[1].unboxed.f64;

	if (y == 0.0)
		return DivideByZero(param);

	return SmileUnboxedFloat64_From(MathematiciansRemainder(x, y));
}

//-------------------------------------------------------------------------------------------------
// Arithmetic extensions

SMILE_EXTERNAL_FUNCTION(Sign)
{
	Float64 value;

	value = argv[0].unboxed.f64;

	if (value < 0.0)
		return SmileUnboxedFloat64_From((Float64)-1.0);
	else if (value > 0.0)
		return SmileUnboxedFloat64_From((Float64)+1.0);
	else if (value == 0.0)
		return SmileUnboxedFloat64_From((Float64)+0.0);
	else // NaN
		return SmileUnboxedFloat64_From(value);
}

SMILE_EXTERNAL_FUNCTION(Abs)
{
	return SmileUnboxedFloat64_From((Float64)fabs(argv[0].unboxed.f64));
}

SMILE_EXTERNAL_FUNCTION(Clip)
{
	Float64 value = argv[0].unboxed.f64;
	Float64 min = argv[1].unboxed.f64;
	Float64 max = argv[2].unboxed.f64;

	if (value > max) {
		value = max;
		return value < min ? argv[1] : argv[2];
	}
	else if (value < min) {
		return argv[1];
	}
	else return argv[0];
}

SMILE_EXTERNAL_FUNCTION(Min)
{
	Float64 x, y;
	Int i;

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) < x) x = y;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) < x) x = y;
			if ((y = argv[2].unboxed.f64) < x) x = y;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) < x) x = y;
			if ((y = argv[2].unboxed.f64) < x) x = y;
			if ((y = argv[3].unboxed.f64) < x) x = y;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				if ((y = argv[i].unboxed.f64) < x) x = y;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

SMILE_EXTERNAL_FUNCTION(Max)
{
	Float64 x, y;
	Int i;

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) > x) x = y;
			return SmileUnboxedFloat64_From(x);

		case 3:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) > x) x = y;
			if ((y = argv[2].unboxed.f64) > x) x = y;
			return SmileUnboxedFloat64_From(x);

		case 4:
			x = argv[0].unboxed.f64;
			if ((y = argv[1].unboxed.f64) > x) x = y;
			if ((y = argv[2].unboxed.f64) > x) x = y;
			if ((y = argv[3].unboxed.f64) > x) x = y;
			return SmileUnboxedFloat64_From(x);

		default:
			x = argv[0].unboxed.f64;
			for (i = 1; i < argc; i++) {
				if ((y = argv[i].unboxed.f64) > x) x = y;
			}
			return SmileUnboxedFloat64_From(x);
	}
}

SMILE_EXTERNAL_FUNCTION(Sqrt)
{
	Float64 value = argv[0].unboxed.f64;

	if (value < 0.0) {
		MathInfo mathInfo = (MathInfo)param;
		if (mathInfo->isLoud)
			Smile_ThrowException(Smile_KnownSymbols.native_method_error, _negativeSqrt);
		return SmileUnboxedFloat64_From(0.0);
	}

	return SmileUnboxedFloat64_From((Float64)sqrt(value));
}

//-------------------------------------------------------------------------------------------------
// Rounding/truncation operations

SMILE_EXTERNAL_FUNCTION(Ceil)
{
	return SmileUnboxedFloat64_From((Float64)ceil(argv[0].unboxed.f64));
}

SMILE_EXTERNAL_FUNCTION(Floor)
{
	return SmileUnboxedFloat64_From((Float64)floor(argv[0].unboxed.f64));
}

SMILE_EXTERNAL_FUNCTION(Trunc)
{
	return SmileUnboxedFloat64_From((Float64)trunc(argv[0].unboxed.f64));
}

SMILE_EXTERNAL_FUNCTION(Round)
{
	return SmileUnboxedFloat64_From((Float64)floor(argv[0].unboxed.f64 + 0.5));
}

static Float64 BankRoundInternal(Float64 value)
{
	Float64 intPart, fracPart;
	Float64 intTail;

	// Negative numbers do the same algorithm upside-down.
	if (value < 0.0) return -BankRoundInternal(-value);

	// Split into integer and fractional parts.
	fracPart = modf(value, &intPart);

	// Easy rounding cases.
	if (fracPart < 0.5)
		return intPart;
	else if (fracPart > 0.5)
		return intPart + 1;

	// Exactly 0.5, so we need to use the integer part to decide which way to go.
	intTail = fmod(intPart, 2.0);
	if (intTail < 0.5 || intTail > 1.5) {
		// The integer part is even, so round down.
		return intPart;
	}
	else {
		// The integer part is odd, so round up.
		return intPart + 1;
	}	
}

SMILE_EXTERNAL_FUNCTION(BankRound)
{
	return SmileUnboxedFloat64_From((Float64)BankRoundInternal(argv[0].unboxed.f64));
}

SMILE_EXTERNAL_FUNCTION(Modf)
{
	Float64 intPart, fracPart;
	SmileList list;

	fracPart = modf((Float64)argv[0].unboxed.f64, &intPart);

	list = SmileList_Cons((SmileObject)SmileFloat64_Create((Float64)intPart),
		(SmileObject)SmileList_Cons((SmileObject)SmileFloat64_Create((Float64)fracPart),
		NullObject));

	return SmileArg_From((SmileObject)list);
}

//-------------------------------------------------------------------------------------------------

void SmileFloat64_Setup(SmileUserObject base)
{
	SmileUnboxedFloat64_Instance->base = (SmileObject)base;

	SetupFunction("bool", ToBool, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("int", ToInt, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("string", ToString, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);
	SetupFunction("hash", Hash, NULL, "value", ARG_CHECK_EXACT, 1, 1, 0, NULL);

	SetupFunction("+", Plus, NULL, "augend addend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _float64Checks);
	SetupFunction("-", Minus, NULL, "minuend subtrahend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _float64Checks);
	SetupFunction("*", Star, NULL, "multiplier multiplicand", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _float64Checks);
	SetupFunction("/", Slash, &_quietMath, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _float64Checks);
	SetupFunction("/!", Slash, &_loudMath, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _float64Checks);
	SetupSynonym("/", "div");
	SetupSynonym("/!", "div!");
	SetupFunction("mod", Mod, &_quietMath, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _float64Checks);
	SetupFunction("mod!", Mod, &_loudMath, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _float64Checks);
	SetupFunction("rem", Rem, &_quietMath, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _float64Checks);
	SetupFunction("rem!", Rem, &_loudMath, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _float64Checks);
	SetupFunction("sqrt", Sqrt, &_quietMath, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("sqrt!", Sqrt, &_loudMath, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);

	SetupFunction("sign", Sign, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("abs", Abs, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("clip", Clip, NULL, "value min max", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 3, 3, 3, _float64Checks);
	SetupFunction("min", Min, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _float64Checks);
	SetupFunction("max", Max, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _float64Checks);

	SetupFunction("ceil", Ceil, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("floor", Floor, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("trunc", Trunc, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("round", Round, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("bank-round", BankRound, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
	SetupFunction("modf", Modf, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _float64Checks);
}
