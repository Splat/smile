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
#include <smile/smiletypes/text/smilesymbol.h>
#include <smile/parsing/parsemessage.h>
#include <smile/parsing/internal/parsedecl.h>
#include <smile/parsing/internal/parsescope.h>

// Form: [$set lvalue rvalue]
void Compiler_CompileSetf(Compiler compiler, SmileList args)
{
	Int length;
	SmileObject dest, value, index;
	SmilePair pair;
	Symbol symbol;
	Int offset;
	ByteCodeSegment segment = compiler->currentFunction->byteCodeSegment;
	Int oldSourceLocation = compiler->currentFunction->currentSourceLocation;

	// There are three possible legal forms for the arguments:
	//
	//   [$set symbol value]
	//   [$set obj.property value]
	//   [$set [(obj.get-member) index] value]
	//
	// We have to determine which one of these we have, and compile an appropriate
	// assignment (or method invocation) accordingly.

	// Make sure this is a well-formed list of exactly two elements.
	length = SmileList_Length(args);
	if (length != 2) {
		Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, SMILE_VCALL(args, getSourceLocation),
			String_FromC("Cannot compile [$set]: Expression is not well-formed.")));
		return;
	}

	// Get the destination object, and the value to be assigned.
	dest = args->a;
	value = ((SmileList)args->d)->a;

	switch (SMILE_KIND(dest)) {

	case SMILE_KIND_SYMBOL:
		// This is of the form [$set symbol value].
		symbol = ((SmileSymbol)dest)->symbol;
		oldSourceLocation = Compiler_SetAssignedSymbol(compiler, symbol);
		Compiler_SetSourceLocationFromList(compiler, (SmileList)args->d);

		// Load the value to store.
		Compiler_CompileExpr(compiler, value);

		// Store it, leaving a duplicate on the stack.
		Compiler_SetSourceLocationFromList(compiler, args);
		Compiler_CompileVariable(compiler, symbol, True);
		Compiler_RevertSourceLocation(compiler, oldSourceLocation);
		break;

	case SMILE_KIND_PAIR:
		// This is probably of the form [$set obj.property value].  Make sure the right side
		// of the pair is a symbol.
		pair = (SmilePair)dest;
		if (SMILE_KIND(pair->right) != SMILE_KIND_SYMBOL) {
			Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, SMILE_VCALL(args, getSourceLocation),
				String_FromC("Cannot compile [$set]: Expression is not well-formed.")));
			return;
		}
		symbol = ((SmileSymbol)pair->right)->symbol;

		Compiler_SetSourceLocationFromList(compiler, (SmileList)args->d);

		// Evaluate the left side first.
		Compiler_CompileExpr(compiler, pair->left);

		Compiler_SetAssignedSymbol(compiler, symbol);
		Compiler_SetSourceLocationFromList(compiler, args);

		// Evaluate the value second.  (Doing this second ensures that everything always
		// evaluates left-to-right, the order in which it was written.)
		Compiler_CompileExpr(compiler, value);

		// Assign the property.
		EMIT1(Op_StProp, -1, symbol = symbol);	// Leaves the value on the stack.
		Compiler_RevertSourceLocation(compiler, oldSourceLocation);
		break;

	case SMILE_KIND_LIST:
		// This is probably of the form [$set [(obj.get-member) index] value].  Make sure that the
		// inner list is well-formed, has two elements, the first element is a pair, and the right
		// side of the first element is the special symbol "get-member".
		if (SmileList_Length((SmileList)dest) != 2 || SMILE_KIND(((SmileList)dest)->a) != SMILE_KIND_PAIR) {
			Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, SMILE_VCALL(args, getSourceLocation),
				String_FromC("Cannot compile [$set]: Expression is not well-formed.")));
			return;
		}
		pair = (SmilePair)(((SmileList)dest)->a);
		if (SMILE_KIND(pair->right) != SMILE_KIND_SYMBOL) {
			Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, SMILE_VCALL(args, getSourceLocation),
				String_FromC("Cannot compile [$set]: Expression is not well-formed.")));
			return;
		}
		index = LIST_SECOND((SmileList)dest);

		// Okay.  We now have pair->left, pair->right, index, and value.  Let's compile them.
		Compiler_SetSourceLocationFromPair(compiler, pair);
		Compiler_CompileExpr(compiler, pair->left);
		Compiler_SetSourceLocationFromList(compiler, (SmileList)((SmileList)dest)->d);
		Compiler_CompileExpr(compiler, index);
		Compiler_SetSourceLocationFromList(compiler, (SmileList)args->d);
		Compiler_CompileExpr(compiler, value);
		Compiler_SetSourceLocationFromList(compiler, args);
		EMIT0(Op_StMember, -3);	// Leaves the value on the stack.
		break;

	default:
		Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, SMILE_VCALL(args, getSourceLocation),
			String_FromC("Cannot compile [$set]: Expression is not well-formed.")));
		return;
	}
}
