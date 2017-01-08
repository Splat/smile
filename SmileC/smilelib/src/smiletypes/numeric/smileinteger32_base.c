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

#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smileuserobject.h>
#include <smile/smiletypes/numeric/smileinteger32.h>
#include <smile/smiletypes/smilefunction.h>

#define Setup(__name__, __value__) \
	(SmileUserObject_QuickSet(base, (__name__), (__value__)))

#define SetupFunction(__name__, __function__, __param__, __argNames__, __argCheckFlags__, __minArgs__, __maxArgs__, __numArgsToTypeCheck__, __argTypeChecks__) \
	(Setup((__name__), (SmileObject)SmileFunction_CreateExternalFunction((__function__), (__param__), \
		(__name__), (__argNames__), (__argCheckFlags__), (__minArgs__), (__maxArgs__), (__numArgsToTypeCheck__), (__argTypeChecks__))))

#define SetupSimpleFunction(__name__, __function__, __argNames__, __numArgs__) \
	(Setup((__name__), (SmileObject)SmileFunction_CreateExternalFunction((__function__), (__param__), \
		(__name__), (__argNames__), ARG_CHECK_EXACT, (__numArgs__), (__numArgs__), NULL)))

static Byte _integer32Checks[] = {
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
};

static Byte _integer32ComparisonChecks[] = {
	SMILE_KIND_MASK, SMILE_KIND_INTEGER32,
	0, 0,
};

STATIC_STRING(_divideByZero, "Divide by zero error");
STATIC_STRING(_negativeLog, "Logarithm of negative or zero value");
STATIC_STRING(_negativeSqrt, "Square root of negative number");

STATIC_STRING(_invalidTypeError, "All arguments to 'Integer32.%s' must be of type 'Integer32'.");

//-------------------------------------------------------------------------------------------------
// Type conversion

//-------------------------------------------------------------------------------------------------
// Arithmetic operators

static SmileObject Plus(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];
		
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x += ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x += ((SmileInteger32)argv[1])->value;
			x += ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x += ((SmileInteger32)argv[1])->value;
			x += ((SmileInteger32)argv[2])->value;
			x += ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x += ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject Minus(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 1:
			x = ((SmileInteger32)argv[0])->value;
			return (SmileObject)SmileInteger32_Create(-x);

		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x -= ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x -= ((SmileInteger32)argv[1])->value;
			x -= ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x -= ((SmileInteger32)argv[1])->value;
			x -= ((SmileInteger32)argv[2])->value;
			x -= ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);

		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x -= ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject Star(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x *= ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x *= ((SmileInteger32)argv[1])->value;
			x *= ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x *= ((SmileInteger32)argv[1])->value;
			x *= ((SmileInteger32)argv[2])->value;
			x *= ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);

		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x *= ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject UStar(Int argc, SmileObject *argv, void *param)
{
	UInt32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			x *= (UInt32)((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		case 3:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			x *= (UInt32)((SmileInteger32)argv[1])->value;
			x *= (UInt32)((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		case 4:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			x *= (UInt32)((SmileInteger32)argv[1])->value;
			x *= (UInt32)((SmileInteger32)argv[2])->value;
			x *= (UInt32)((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		default:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x *= (UInt32)((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create((Int32)x);
	}
}

/// <summary>
/// Perform division classic-C-style, which rounds towards zero no matter what the sign is.
/// </summary>
Inline Int32 CDiv(Int32 dividend, Int32 divisor)
{
	if (dividend < 0) {
		if (divisor < 0)
			return (Int32)((UInt32)-dividend / (UInt32)-divisor);
		else
			return -(Int32)((UInt32)-dividend / (UInt32)divisor);
	}
	else if (divisor < 0)
		return -(Int32)((UInt32)dividend / (UInt32)-divisor);
	else
		return (Int32)((UInt32)dividend / (UInt32)divisor);
}

/// <summary>
/// Perform division like mathematicians expect, which rounds toward negative infinity (always).
/// </summary>
Inline Int32 MathematiciansDiv(Int32 dividend, Int32 divisor)
{
	if (dividend < 0) {
		if (divisor < 0) {
			return (Int32)((UInt32)-dividend / (UInt32)-divisor);
		}
		else {
			UInt32 positiveQuotient = (UInt32)-dividend / (UInt32)divisor;
			UInt32 positiveRemainder = (UInt32)-dividend % (UInt32)divisor;
			return positiveRemainder == 0 ? -(Int32)positiveQuotient : -(Int32)positiveQuotient - 1;
		}
	}
	else if (divisor < 0) {
		UInt32 positiveQuotient = (UInt32)dividend / (UInt32)-divisor;
		UInt32 positiveRemainder = (UInt32)dividend % (UInt32)-divisor;
		return positiveRemainder == 0 ? -(Int32)positiveQuotient : -(Int32)positiveQuotient - 1;
	}
	else {
		return dividend / divisor;
	}
}

static SmileObject Slash(Int argc, SmileObject *argv, void *param)
{
	Int32 x, y;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			if ((y = ((SmileInteger32)argv[3])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = MathematiciansDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				if ((y = ((SmileInteger32)argv[i])->value) == 0)
					Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
				x = MathematiciansDiv(x, y);
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject USlash(Int argc, SmileObject *argv, void *param)
{
	UInt32 x, y;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			if ((y = (UInt32)((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		case 3:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			if ((y = (UInt32)((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			if ((y = (UInt32)((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		case 4:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			if ((y = (UInt32)((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			if ((y = (UInt32)((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			if ((y = (UInt32)((SmileInteger32)argv[3])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x /= y;
			return (SmileObject)SmileInteger32_Create((Int32)x);

		default:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				if ((y = (UInt32)((SmileInteger32)argv[i])->value) == 0)
					Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
				x /= y;
			}
			return (SmileObject)SmileInteger32_Create((Int32)x);
	}
}

static SmileObject Div(Int argc, SmileObject *argv, void *param)
{
	Int32 x, y;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			if ((y = ((SmileInteger32)argv[3])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = CDiv(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				if ((y = ((SmileInteger32)argv[i])->value) == 0)
					Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
				x = CDiv(x, y);
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

/// <summary>
/// Perform modulus like mathematicians expect, in which the modulus has the same sign as the divisor (y).
/// </summary>
Inline Int32 MathematiciansModulus(Int32 x, Int32 y)
{
	Int32 rem;

	if (x < 0) {
		if (y < 0)
			return -(-x % -y);
		else {
			rem = -x % y;
			return rem != 0 ? y - rem : 0;
		}
	}
	else if (y < 0) {
		rem = x % -y;
		return rem != 0 ? y + rem : 0;
	}
	else
		return x % y;
}

/// <summary>
/// Perform remainder, in which the result has the same sign as the dividend (x).
/// </summary>
Inline Int32 MathematiciansRemainder(Int32 x, Int32 y)
{
	Int32 rem;

	if (x < 0) {
		if (y < 0) {
			rem = -x % -y;
			return rem != 0 ? rem + y : 0;
		}
		else
			return -(-x % y);
	}
	else if (y < 0)
		return x % -y;
	else {
		rem = x % y;
		return rem != 0 ? rem - y : 0;
	}
}

static SmileObject Mod(Int argc, SmileObject *argv, void *param)
{
	Int32 x = ((SmileInteger32)argv[0])->value;
	Int32 y = ((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	if (y == 0)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);

	return (SmileObject)SmileInteger32_Create(MathematiciansModulus(x, y));
}

static SmileObject UMod(Int argc, SmileObject *argv, void *param)
{
	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	if (y == 0)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);

	return (SmileObject)SmileInteger32_Create((Int32)(x % y));
}

static SmileObject Rem(Int argc, SmileObject *argv, void *param)
{
	Int32 x = ((SmileInteger32)argv[0])->value;
	Int32 y = ((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	if (y == 0)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);

	return (SmileObject)SmileInteger32_Create(MathematiciansRemainder(x, y));
}

//-------------------------------------------------------------------------------------------------
// Arithmetic extensions

static SmileObject Sign(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return value == 0 ? (SmileObject)Smile_KnownObjects.ZeroInt32
		: value > 0 ? (SmileObject)Smile_KnownObjects.OneInt32
		: (SmileObject)Smile_KnownObjects.NegOneInt32;
}

static SmileObject Abs(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return value < 0 ? (SmileObject)SmileInteger32_Create(-value) : argv[0];
}

static SmileObject Clip(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;
	Int32 min = ((SmileInteger32)argv[1])->value;
	Int32 max = ((SmileInteger32)argv[2])->value;

	UNUSED(argc);
	UNUSED(param);

	if (value > max) {
		value = max;
		return value < min ? argv[1] : argv[2];
	}
	else if (value < min) {
		return argv[1];
	}
	else return argv[0];
}

static SmileObject UClip(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 min = (UInt32)((SmileInteger32)argv[1])->value;
	UInt32 max = (UInt32)((SmileInteger32)argv[2])->value;

	UNUSED(argc);
	UNUSED(param);

	if (value > max) {
		value = max;
		return value < min ? argv[1] : argv[2];
	}
	else if (value < min) {
		return argv[1];
	}
	else return argv[0];
}

static SmileObject Min(Int argc, SmileObject *argv, void *param)
{
	Int32 x, y;
	Int i, j;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y < x) i = 1;
			return argv[i];

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y < x) i = 1, x = y;
			y = ((SmileInteger32)argv[2])->value;
			if (y < x) i = 2, x = y;
			return argv[i];

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y < x) i = 1, x = y;
			y = ((SmileInteger32)argv[2])->value;
			if (y < x) i = 2, x = y;
			y = ((SmileInteger32)argv[3])->value;
			if (y < x) i = 3, x = y;
			return argv[i];

		default:
			x = ((SmileInteger32)argv[0])->value;
			j = 0;
			for (i = 1; i < argc; i++) {
				y = ((SmileInteger32)argv[i])->value;
				if (y < x) j = i, x = y;
			}
			return argv[j];
	}
}

static SmileObject UMin(Int argc, SmileObject *argv, void *param)
{
	UInt32 x, y;
	Int i, j;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y < x) i = 1;
			return argv[i];

		case 3:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y < x) i = 1, x = y;
			y = (UInt32)((SmileInteger32)argv[2])->value;
			if (y < x) i = 2, x = y;
			return argv[i];

		case 4:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y < x) i = 1, x = y;
			y = (UInt32)((SmileInteger32)argv[2])->value;
			if (y < x) i = 2, x = y;
			y = (UInt32)((SmileInteger32)argv[3])->value;
			if (y < x) i = 3, x = y;
			return argv[i];

		default:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			j = 0;
			for (i = 1; i < argc; i++) {
				y = (UInt32)((SmileInteger32)argv[i])->value;
				if (y < x) j = i, x = y;
			}
			return argv[j];
	}
}

static SmileObject Max(Int argc, SmileObject *argv, void *param)
{
	Int32 x, y;
	Int i, j;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y > x) i = 1;
			return argv[i];

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y > x) i = 1, x = y;
			y = ((SmileInteger32)argv[2])->value;
			if (y > x) i = 2, x = y;
			return argv[i];

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			i = 0;
			y = ((SmileInteger32)argv[1])->value;
			if (y > x) i = 1, x = y;
			y = ((SmileInteger32)argv[2])->value;
			if (y > x) i = 2, x = y;
			y = ((SmileInteger32)argv[3])->value;
			if (y > x) i = 3, x = y;
			return argv[i];

		default:
			x = ((SmileInteger32)argv[0])->value;
			j = 0;
			for (i = 1; i < argc; i++) {
				y = ((SmileInteger32)argv[i])->value;
				if (y > x) j = i, x = y;
			}
			return argv[j];
	}
}

static SmileObject UMax(Int argc, SmileObject *argv, void *param)
{
	UInt32 x, y;
	Int i, j;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];

		case 2:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y > x) i = 1;
			return argv[i];

		case 3:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y > x) i = 1, x = y;
			y = (UInt32)((SmileInteger32)argv[2])->value;
			if (y > x) i = 2, x = y;
			return argv[i];

		case 4:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			i = 0;
			y = (UInt32)((SmileInteger32)argv[1])->value;
			if (y > x) i = 1, x = y;
			y = (UInt32)((SmileInteger32)argv[2])->value;
			if (y > x) i = 2, x = y;
			y = (UInt32)((SmileInteger32)argv[3])->value;
			if (y > x) i = 3, x = y;
			return argv[i];

		default:
			x = (UInt32)((SmileInteger32)argv[0])->value;
			j = 0;
			for (i = 1; i < argc; i++) {
				y = (UInt32)((SmileInteger32)argv[i])->value;
				if (y > x) j = i, x = y;
			}
			return argv[j];
	}
}

Inline Int32 IntPower(Int32 value, Int32 exponent)
{
	if (exponent < 0) return 0;

	Int32 result = 1;

	while (exponent > 0) {
		if ((exponent & 1) != 0) {
			result *= value;
		}
		exponent >>= 1;
		value *= value;
	}

	return result;
}

static SmileObject Power(Int argc, SmileObject *argv, void *param)
{
	Int32 x, y;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 3:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			if ((y = ((SmileInteger32)argv[1])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			if ((y = ((SmileInteger32)argv[2])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			if ((y = ((SmileInteger32)argv[3])->value) == 0)
				Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
			x = IntPower(x, y);
			return (SmileObject)SmileInteger32_Create(x);

		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				if ((y = ((SmileInteger32)argv[i])->value) == 0)
					Smile_ThrowException(Smile_KnownSymbols.native_method_error, _divideByZero);
				x = IntPower(x, y);
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

Inline UInt32 IntSqrt(UInt32 value)
{
	UInt32 root, bit, trial;

	root = 0;
	bit = (value >= 0x10000U) ? (1U << 30) : (1U << 14);

	do {
		trial = root + bit;
		if (value >= trial) {
			value -= trial;
			root = trial + bit;
		}
		root >>= 1;
		bit >>= 2;
	} while (bit != 0);

	return root;
}

static SmileObject Sqrt(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	if (value < 0)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _negativeSqrt);

	return (SmileObject)SmileInteger32_Create((Int32)IntSqrt((UInt32)value));
}

static SmileObject Pow2Q(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return value > 0 && (value & (value - 1)) == 0 ? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject NextPow2(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;
	UInt32 uvalue = (UInt32)value;

	UNUSED(argc);
	UNUSED(param);

	if (value <= 0) return (SmileObject)Smile_KnownObjects.OneInt32;

	uvalue--;
	uvalue |= uvalue >> 1;
	uvalue |= uvalue >> 2;
	uvalue |= uvalue >> 4;
	uvalue |= uvalue >> 8;
	uvalue |= uvalue >> 16;
	uvalue++;

	return (SmileObject)SmileInteger32_Create((Int32)uvalue);
}

static SmileObject IntLg(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;
	UInt32 uvalue = (UInt32)value;
	UInt32 log;

	UNUSED(argc);
	UNUSED(param);

	if (value <= 0)
		Smile_ThrowException(Smile_KnownSymbols.native_method_error, _negativeLog);

	log = 0;
	if ((uvalue & 0xFFFF0000) != 0) uvalue >>= 16, log += 16;
	if ((uvalue & 0x0000FF00) != 0) uvalue >>= 8, log += 8;
	if ((uvalue & 0x000000F0) != 0) uvalue >>= 4, log += 4;
	if ((uvalue & 0x0000000C) != 0) uvalue >>= 2, log += 2;
	if ((uvalue & 0x00000002) != 0) uvalue >>= 1, log += 1;

	return (SmileObject)SmileInteger32_Create((Int32)log);
}

//-------------------------------------------------------------------------------------------------
// Bitwise operators

static SmileObject BitAnd(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];
		
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x &= ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x &= ((SmileInteger32)argv[1])->value;
			x &= ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x &= ((SmileInteger32)argv[1])->value;
			x &= ((SmileInteger32)argv[2])->value;
			x &= ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x &= ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject BitOr(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];
		
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x |= ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x |= ((SmileInteger32)argv[1])->value;
			x |= ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x |= ((SmileInteger32)argv[1])->value;
			x |= ((SmileInteger32)argv[2])->value;
			x |= ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x |= ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject BitXor(Int argc, SmileObject *argv, void *param)
{
	Int32 x;
	Int i;

	UNUSED(param);

	switch (argc) {
		case 1:
			return argv[0];
		
		case 2:
			x = ((SmileInteger32)argv[0])->value;
			x ^= ((SmileInteger32)argv[1])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		case 3:
			x = ((SmileInteger32)argv[0])->value;
			x ^= ((SmileInteger32)argv[1])->value;
			x ^= ((SmileInteger32)argv[2])->value;
			return (SmileObject)SmileInteger32_Create(x);

		case 4:
			x = ((SmileInteger32)argv[0])->value;
			x ^= ((SmileInteger32)argv[1])->value;
			x ^= ((SmileInteger32)argv[2])->value;
			x ^= ((SmileInteger32)argv[3])->value;
			return (SmileObject)SmileInteger32_Create(x);
		
		default:
			x = ((SmileInteger32)argv[0])->value;
			for (i = 1; i < argc; i++) {
				x ^= ((SmileInteger32)argv[i])->value;
			}
			return (SmileObject)SmileInteger32_Create(x);
	}
}

static SmileObject BitNot(Int argc, SmileObject *argv, void *param)
{
	Int32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return value < 0 ? (SmileObject)SmileInteger32_Create(~value) : argv[0];
}

//-------------------------------------------------------------------------------------------------
// Shift/rotate operators

static SmileObject LogicalShiftLeft(Int argc, SmileObject *argv, void *param)
{
	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)(x << y));
}

static SmileObject LogicalShiftRight(Int argc, SmileObject *argv, void *param)
{
	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)(x >> y));
}

static SmileObject ArithmeticShiftLeft(Int argc, SmileObject *argv, void *param)
{
	Int32 x = ((SmileInteger32)argv[0])->value;
	Int32 y = ((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create(x << y);
}

static SmileObject ArithmeticShiftRight(Int argc, SmileObject *argv, void *param)
{
	Int32 x = ((SmileInteger32)argv[0])->value;
	Int32 y = ((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create(x >> y);
}

static SmileObject RotateLeft(Int argc, SmileObject *argv, void *param)
{
	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)Smile_RotateLeft32(x, y));
}

static SmileObject RotateRight(Int argc, SmileObject *argv, void *param)
{
	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)Smile_RotateRight32(x, y));
}

//-------------------------------------------------------------------------------------------------
// Bit twiddling

Inline UInt32 CountBitsSet(UInt32 value)
{
	value = value - ((value >> 1) & 0x55555555);
	value = (value & 0x33333333) + ((value >> 2) & 0x33333333);
	return ((value + (value >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
}

Inline UInt32 ComputeReverseBits(UInt32 value)
{
	value = ((value >> 1) & 0x55555555) | ((value & 0x55555555) << 1);
	value = ((value >> 2) & 0x33333333) | ((value & 0x33333333) << 2);
	value = ((value >> 4) & 0x0F0F0F0F) | ((value & 0x0F0F0F0F) << 4);
	value = ((value >> 8) & 0x00FF00FF) | ((value & 0x00FF00FF) << 8);
	value = (value >> 16) | (value << 16);
	return value;
}

Inline UInt32 ComputeCountOfRightZeros(UInt32 value)
{
	UInt32 c = 32;
	value &= ~value + 1;
	if (value != 0) c--;
	if ((value & 0x0000FFFF) != 0) c -= 16;
	if ((value & 0x00FF00FF) != 0) c -= 8;
	if ((value & 0x0F0F0F0F) != 0) c -= 4;
	if ((value & 0x33333333) != 0) c -= 2;
	if ((value & 0x55555555) != 0) c -= 1;
	return c;
}

static SmileObject CountOnes(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)CountBitsSet((UInt32)value));
}

static SmileObject CountZeros(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)CountBitsSet(~(UInt32)value));
}

static SmileObject Parity(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = ((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	value ^= value >> 16;
	value ^= value >> 8;
	value ^= value >> 4;
	value &= 0xF;
	value = (0x6996 >> value) & 1;

	return (SmileObject)SmileInteger32_Create((Int32)value);
}

static SmileObject ReverseBits(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)ComputeReverseBits(value));
}

static SmileObject ReverseBytes(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	value = (UInt32)( ((value >> 24) & 0x000000FFU)
						| ((value >>  8) & 0x0000FF00U)
						| ((value <<  8) & 0x00FF0000U)
						| ((value << 24) & 0xFF000000U) );

	return (SmileObject)SmileInteger32_Create((Int32)value);
}

static SmileObject CountRightZeros(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)ComputeCountOfRightZeros(value));
}

static SmileObject CountRightOnes(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)ComputeCountOfRightZeros(~value));
}

static SmileObject CountLeftZeros(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)ComputeCountOfRightZeros(ComputeReverseBits(value)));
}

static SmileObject CountLeftOnes(Int argc, SmileObject *argv, void *param)
{
	UInt32 value = (UInt32)((SmileInteger32)argv[0])->value;

	UNUSED(argc);
	UNUSED(param);

	return (SmileObject)SmileInteger32_Create((Int32)ComputeCountOfRightZeros(~ComputeReverseBits(value)));
}

//-------------------------------------------------------------------------------------------------
// Comparisons

static SmileObject Eq(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_INTEGER32
		|| ((SmileInteger32)argv[0])->value != ((SmileInteger32)argv[1])->value)
		return (SmileObject)Smile_KnownObjects.FalseObj;

	return (SmileObject)Smile_KnownObjects.TrueObj;
}

static SmileObject Ne(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	if (SMILE_KIND(argv[1]) != SMILE_KIND_INTEGER32
		|| ((SmileInteger32)argv[0])->value != ((SmileInteger32)argv[1])->value)
		return (SmileObject)Smile_KnownObjects.TrueObj;

	return (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject Lt(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return ((SmileInteger32)argv[0])->value < ((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject Gt(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return ((SmileInteger32)argv[0])->value > ((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject Le(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return ((SmileInteger32)argv[0])->value <= ((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject Ge(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return ((SmileInteger32)argv[0])->value >= ((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject ULt(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return (UInt32)((SmileInteger32)argv[0])->value < (UInt32)((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject UGt(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return (UInt32)((SmileInteger32)argv[0])->value > (UInt32)((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject ULe(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return (UInt32)((SmileInteger32)argv[0])->value <= (UInt32)((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject UGe(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	return (UInt32)((SmileInteger32)argv[0])->value >= (UInt32)((SmileInteger32)argv[1])->value
		? (SmileObject)Smile_KnownObjects.TrueObj : (SmileObject)Smile_KnownObjects.FalseObj;
}

static SmileObject Compare(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	Int32 x = ((SmileInteger32)argv[0])->value;
	Int32 y = ((SmileInteger32)argv[1])->value;

	if (x == y)
		return (SmileObject)Smile_KnownObjects.ZeroInt32;
	else if (x < y)
		return (SmileObject)Smile_KnownObjects.NegOneInt32;
	else
		return (SmileObject)Smile_KnownObjects.OneInt32;
}

static SmileObject UCompare(Int argc, SmileObject *argv, void *param)
{
	UNUSED(argc);
	UNUSED(param);

	UInt32 x = (UInt32)((SmileInteger32)argv[0])->value;
	UInt32 y = (UInt32)((SmileInteger32)argv[1])->value;

	if (x == y)
		return (SmileObject)Smile_KnownObjects.ZeroInt32;
	else if (x < y)
		return (SmileObject)Smile_KnownObjects.NegOneInt32;
	else
		return (SmileObject)Smile_KnownObjects.OneInt32;
}

//-------------------------------------------------------------------------------------------------

void SmileInteger32_Setup(SmileUserObject base)
{
	SetupFunction("+", Plus, NULL, "augend addend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("~+", Plus, NULL, "augend addend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("-", Minus, NULL, "minuend subtrahend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("~-", Minus, NULL, "minuend subtrahend", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("*", Star, NULL, "multiplier multiplicand", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("~*", UStar, NULL, "multiplier multiplicand", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("/", Slash, NULL, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("~/", USlash, NULL, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("div", Div, NULL, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("udiv", USlash, NULL, "dividend divisor", ARG_CHECK_MIN | ARG_CHECK_TYPES, 2, 0, 8, _integer32Checks);
	SetupFunction("mod", Mod, NULL, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("umod", UMod, NULL, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("rem", Rem, NULL, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("urem", UMod, NULL, "dividend divisor", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);

	SetupFunction("sign", Sign, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("abs", Abs, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("clip", Clip, NULL, "value min max", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 3, 3, 3, _integer32Checks);
	SetupFunction("uclip", Clip, NULL, "value min max", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 3, 3, 3, _integer32Checks);
	SetupFunction("min", Min, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("umin", Min, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("max", Max, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("umax", Max, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("^", Power, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("sqrt", Sqrt, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("pow2?", Pow2Q, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("next-pow2", NextPow2, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("int-lg", IntLg, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);

	SetupFunction("band", BitAnd, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("bor", BitOr, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("bxor", BitXor, NULL, "x y", ARG_CHECK_MIN | ARG_CHECK_TYPES, 1, 0, 8, _integer32Checks);
	SetupFunction("~", BitNot, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);

	SetupFunction("<<<", LogicalShiftLeft, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction(">>>", LogicalShiftRight, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("<<", ArithmeticShiftLeft, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction(">>", ArithmeticShiftRight, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("<<+", RotateLeft, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("+>>", RotateRight, NULL, "value count", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);

	SetupFunction("count-ones", CountOnes, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("count-zeros", CountZeros, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("parity", Parity, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("reverse-bits", ReverseBits, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("reverse-bytes", ReverseBytes, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("count-right-zeros", CountRightZeros, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("count-right-ones", CountRightOnes, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("count-left-zeros", CountLeftZeros , NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);
	SetupFunction("count-left-ones", CountLeftOnes, NULL, "value", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 1, 1, 1, _integer32Checks);

	SetupFunction("==", Eq, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32ComparisonChecks);
	SetupFunction("!=", Ne, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32ComparisonChecks);
	SetupFunction("<", Lt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("~<", ULt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction(">", Gt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("~>", UGt, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("<=", Le, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("~<=", ULe, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction(">=", Ge, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("~>=", UGe, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);

	SetupFunction("compare", Compare, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
	SetupFunction("ucompare", UCompare, NULL, "x y", ARG_CHECK_EXACT | ARG_CHECK_TYPES, 2, 2, 2, _integer32Checks);
}
