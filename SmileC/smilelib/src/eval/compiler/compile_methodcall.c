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

void Compiler_CompileMethodCall(Compiler compiler, SmilePair pair, SmileList args)
{
	Int length;
	SmileList temp;
	Int offset;
	Symbol symbol;
	ByteCodeSegment segment = compiler->currentFunction->byteCodeSegment;
	Int oldSourceLocation = compiler->currentFunction->currentSourceLocation;

	// First, make sure the args are well-formed, and count how many of them there are.
	length = SmileList_Length(args);

	// Make sure this is a valid method call form.
	if (length < 0 || SMILE_KIND(pair->right) != SMILE_KIND_SYMBOL) {
		Compiler_AddMessage(compiler, ParseMessage_Create(PARSEMESSAGE_ERROR, NULL,
			String_FromC("Cannot compile method call: Argument list is not well-formed.")));
		return;
	}

	// Evaluate the left side of the pair (the object to invoke).
	Compiler_SetSourceLocationFromPair(compiler, pair);
	Compiler_CompileExpr(compiler, pair->left);
	symbol = ((SmileSymbol)pair->right)->symbol;

	// Evaluate all of the arguments.
	for (temp = args; SMILE_KIND(temp) == SMILE_KIND_LIST; temp = (SmileList)temp->d) {
		Compiler_SetSourceLocationFromList(compiler, temp);
		Compiler_CompileExpr(compiler, temp->a);
	}

	Compiler_RevertSourceLocation(compiler, oldSourceLocation);

	// Invoke the method by symbol name.
	if (length <= 7) {
		// If this is the special get-member method, use the special fast instruction for that.
		if (length == 1 && symbol == Smile_KnownSymbols.get_member) {
			EMIT0(Op_LdMember, -2 + 1);
		}
		else {
			// Use a short form.
			EMIT1(Op_Met0 + length, -(length + 1) + 1, symbol = symbol);
		}
	}
	else {
		EMIT2(Op_Met, -(length + 1) + 1, i2.a = length, i2.b = (Int32)symbol);
	}
}
