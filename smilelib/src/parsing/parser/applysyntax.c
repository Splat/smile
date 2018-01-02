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

#include <smile/types.h>
#include <smile/smiletypes/smileobject.h>
#include <smile/smiletypes/smilepair.h>
#include <smile/smiletypes/smilelist.h>
#include <smile/string.h>
#include <smile/stringbuilder.h>
#include <smile/parsing/parser.h>
#include <smile/parsing/internal/parserinternal.h>
#include <smile/parsing/internal/parsescope.h>
#include <smile/parsing/internal/parsesyntax.h>

//-------------------------------------------------------------------------------------------------
// Applying custom syntax rules to the incoming token stream.

/// <summary>
/// Look up the current syntax class object that contains the tree of syntax rules rooted under
/// the given class symbol.
/// </summary>
/// <param name="parser">The parser that describes the current parsing state.</param>
/// <param name="syntaxClassSymbol">The nonterminal symbol of the class to locate.</param>
/// <returns>The class, if a matching class is defined, or NULL if no such class exists in the
/// current scope.</returns>
/// <remarks>
/// This is highly-optimized for the nine special syntax classes; for those, execution time is
/// guaranteed to be O(1) (with a small constant factor, and no searching).  For all other syntax
/// classes, this code will still work, but is only amortized O(1), and may involve searching.
/// </remarks>
static ParserSyntaxClass GetSyntaxClass(Parser parser, Symbol syntaxClassSymbol)
{
	ParserSyntaxTable syntaxTable = parser->currentScope->syntaxTable;
	ParserSyntaxClass syntaxClass;

	switch (syntaxClassSymbol) {
	
		case SMILE_SPECIAL_SYMBOL_STMT:
			return syntaxTable->stmtClass;
		case SMILE_SPECIAL_SYMBOL_EXPR:
			return syntaxTable->exprClass;
		case SMILE_SPECIAL_SYMBOL_CMPEXPR:
			return syntaxTable->cmpExprClass;
		case SMILE_SPECIAL_SYMBOL_ADDEXPR:
			return syntaxTable->addExprClass;
		case SMILE_SPECIAL_SYMBOL_MULEXPR:
			return syntaxTable->mulExprClass;
		case SMILE_SPECIAL_SYMBOL_BINARYEXPR:
			return syntaxTable->binaryExprClass;
		case SMILE_SPECIAL_SYMBOL_PREFIXEXPR:
			return syntaxTable->prefixExprClass;
		case SMILE_SPECIAL_SYMBOL_POSTFIXEXPR:
			return syntaxTable->postfixExprClass;
		case SMILE_SPECIAL_SYMBOL_TERM:
			return syntaxTable->termClass;
		case SMILE_SPECIAL_SYMBOL_NAME:
			return NULL;

		default:
			if (!Int32Dict_TryGetValue(syntaxTable->syntaxClasses, syntaxClassSymbol, (void **)&syntaxClass))
				return NULL;
			return syntaxClass;
	}
}

static ParserSyntaxNode Parser_MatchCustomTerminal(Token token, Int32Dict nextDict)
{
	ParserSyntaxNode nextNode;
	Symbol tokenSymbol;

	// Can't match a next state if there is no next state.
	if (nextDict == NULL) return NULL;

	// Match the next token in the input to the names in the nextDict.
	switch (token->kind) {

	case TOKEN_LEFTBRACE:
		tokenSymbol = Smile_KnownSymbols.left_brace;
		goto haveSymbolToken;
	case TOKEN_RIGHTBRACE:
		tokenSymbol = Smile_KnownSymbols.right_brace;
		goto haveSymbolToken;
	case TOKEN_LEFTPARENTHESIS:
		tokenSymbol = Smile_KnownSymbols.left_parenthesis;
		goto haveSymbolToken;
	case TOKEN_RIGHTPARENTHESIS:
		tokenSymbol = Smile_KnownSymbols.right_parenthesis;
		goto haveSymbolToken;
	case TOKEN_COMMA:
		tokenSymbol = Smile_KnownSymbols.comma;
		goto haveSymbolToken;
	case TOKEN_SEMICOLON:
		tokenSymbol = Smile_KnownSymbols.semicolon;
		goto haveSymbolToken;
	case TOKEN_COLON:
		tokenSymbol = Smile_KnownSymbols.colon;
		goto haveSymbolToken;

	case TOKEN_UNKNOWNALPHANAME:
	case TOKEN_ALPHANAME:
	case TOKEN_UNKNOWNPUNCTNAME:
	case TOKEN_PUNCTNAME:
		tokenSymbol = SymbolTable_GetSymbol(Smile_SymbolTable, token->text);

	haveSymbolToken:
		// We have a symbol of some kind from the input stream.  So check the current
		// tree node (dictionary) to see if this symbol appears as a terminal in it.
		return Int32Dict_TryGetValue(nextDict, tokenSymbol, (void **)&nextNode) ? nextNode : NULL;

	default:
		// Not a symbol, so not matchable.
		return NULL;
	}
}

/// <summary>
/// Pair up the given values with the given array of keys, generating a lookup dictionary
/// mapping the keys to values.  The keys must be unique.
/// </summary>
/// <param name="keys">The array of key names, in order.</param>
/// <param name="numKeys">The number of key names in the array.</param>
/// <param name="values">A list of the values to associate with each key, in order.</param>
/// <returns>A dictionary mapping the keys to their values.</returns>
static Int32Dict Parser_MapKeysToValues(Symbol *keys, Int numKeys, SmileList values)
{
	Int32Dict result = Int32Dict_CreateWithSize(64);

	for (; numKeys-- && SMILE_KIND(values) == SMILE_KIND_LIST; values = LIST_REST(values)) {
	
		Int32Dict_Add(result, *keys++, values->a);
	}

	return result;
}

static SmileObject Parser_RecursivelyClone(SmileObject expr)
{
	switch (expr->kind & (SMILE_KIND_MASK | SMILE_FLAG_WITHSOURCE)) {
		case SMILE_KIND_LIST:
		{
			SmileList oldList = (SmileList)expr;
			SmileList newList = SmileList_Cons(
				Parser_RecursivelyClone(oldList->a),
				Parser_RecursivelyClone(oldList->d)
			);
			return (SmileObject)newList;
		}

		case SMILE_KIND_LIST | SMILE_FLAG_WITHSOURCE:
		{
			struct SmileListWithSourceInt *oldList = (struct SmileListWithSourceInt *)expr;
			SmileList newList = SmileList_ConsWithSource(
				Parser_RecursivelyClone(oldList->a),
				Parser_RecursivelyClone(oldList->d),
				oldList->position
			);
			return (SmileObject)newList;
		}

		case SMILE_KIND_PAIR:
		{
			SmilePair oldPair = (SmilePair)expr;
			SmilePair newPair = SmilePair_Create(
				Parser_RecursivelyClone(oldPair->left),
				Parser_RecursivelyClone(oldPair->right)
			);
			return (SmileObject)newPair;
		}

		case SMILE_KIND_PAIR | SMILE_FLAG_WITHSOURCE:
		{
			struct SmilePairWithSourceInt *oldPair = (struct SmilePairWithSourceInt *)expr;
			SmilePair newPair = SmilePair_CreateWithSource(
				Parser_RecursivelyClone(oldPair->left),
				Parser_RecursivelyClone(oldPair->right),
				oldPair->position
			);
			return (SmileObject)newPair;
		}

		default:
			return expr;
	}
}

static SmileObject Parser_RecursivelyApplyTemplate(Parser parser, SmileObject expr, Int32Dict replacements, LexerPosition lexerPosition);

Inline SmileObject Parser_ApplyListOf(Parser parser, SmileList list, Int32Dict replacements)
{
	SmileList head = NullList, tail = NullList;

	for (; SMILE_KIND(list) != SMILE_KIND_NULL; list = LIST_REST(list)) {
		LexerPosition lexerPosition = (list->kind & SMILE_FLAG_WITHSOURCE) ? ((struct SmileListWithSourceInt *)list)->position : NULL;
		SmileObject oldValue = list->a;
		SmileObject newValue = Parser_RecursivelyApplyTemplate(parser, oldValue, replacements, lexerPosition);

		if (lexerPosition != NULL) {
			LIST_APPEND_WITH_SOURCE(head, tail, newValue, lexerPosition);
		}
		else {
			LIST_APPEND(head, tail, newValue);
		}
	}

	return (SmileObject)head;
}

Inline SmileObject Parser_ApplyListCombine(Parser parser, SmileList list, Int32Dict replacements)
{
	SmileList head = NullList, tail = NullList;

	for (; SMILE_KIND(list) != SMILE_KIND_NULL; list = LIST_REST(list)) {
		LexerPosition lexerPosition = (list->kind & SMILE_FLAG_WITHSOURCE) ? ((struct SmileListWithSourceInt *)list)->position : NULL;
		SmileObject oldValue = list->a;
		SmileObject newValue = Parser_RecursivelyApplyTemplate(parser, oldValue, replacements, lexerPosition);

		if (!SmileList_IsWellFormed(newValue)) {
			Parser_AddError(parser, lexerPosition,
				"Cannot splice into a templated list an object that is not itself a well-formed list.");
		}

		// Iterate tail to its end so that we can splice the next list onto it.
		if (SMILE_KIND(tail) == SMILE_KIND_LIST) {
			for (; SMILE_KIND(tail->d) == SMILE_KIND_LIST; tail = (SmileList)tail->d) ;
		}

		// Splice this onto the end of the current tail.
		if (SMILE_KIND(head) == SMILE_KIND_NULL)
			tail = head = (SmileList)newValue;
		else tail = (SmileList)(tail->d = newValue);
	}

	return (SmileObject)head;
}

Inline SmileObject Parser_ApplyListCons(Parser parser, SmileList list, Int32Dict replacements)
{
	LexerPosition lexerPosition = (list->kind & SMILE_FLAG_WITHSOURCE) ? ((struct SmileListWithSourceInt *)list)->position : NULL;

	SmileObject oldA = list->a;
	SmileObject newA = Parser_RecursivelyApplyTemplate(parser, oldA, replacements, lexerPosition);

	SmileObject oldD = ((SmileList)list->d)->a;
	SmileObject newD = Parser_RecursivelyApplyTemplate(parser, oldD, replacements, lexerPosition);

	return (lexerPosition != NULL)
		? (SmileObject)SmileList_ConsWithSource(newA, newD, lexerPosition)
		: (SmileObject)SmileList_Cons(newA, newD);
}

Inline SmileObject Parser_ApplyPairOf(Parser parser, SmileList list, Int32Dict replacements)
{
	LexerPosition lexerPosition = (list->kind & SMILE_FLAG_WITHSOURCE) ? ((struct SmileListWithSourceInt *)list)->position : NULL;

	SmileObject oldLeft = list->a;
	SmileObject newLeft = Parser_RecursivelyApplyTemplate(parser, oldLeft, replacements, lexerPosition);

	SmileObject oldRight = ((SmileList)list->d)->a;
	SmileObject newRight = Parser_RecursivelyApplyTemplate(parser, oldRight, replacements, lexerPosition);

	return (lexerPosition != NULL)
		? (SmileObject)SmilePair_CreateWithSource(newLeft, newRight, lexerPosition)
		: (SmileObject)SmilePair_Create(newLeft, newRight);
}

static SmileObject Parser_RecursivelyApplyTemplate(Parser parser, SmileObject expr, Int32Dict replacements, LexerPosition lexerPosition)
{
	SmileList list;

	switch (SMILE_KIND(expr)) {

		case SMILE_KIND_SYMBOL:
			{
				SmileSymbol symbol = (SmileSymbol)expr;
				SmileObject newExpr;
				if (!Int32Dict_TryGetValue(replacements, symbol->symbol, (void **)&newExpr)) {
					Parser_AddError(parser, lexerPosition, "No nonterminal named '%S' is defined in the syntax pattern.",
						SymbolTable_GetName(Smile_SymbolTable, symbol->symbol));
					return NullObject;
				}
				newExpr = Parser_RecursivelyClone(newExpr);
				return newExpr;
			}

		case SMILE_KIND_NULL:
			return expr;

		case SMILE_KIND_LIST:
			list = (SmileList)expr;

			if (SMILE_KIND(list->a) == SMILE_KIND_SYMBOL) {
				SmileSymbol symbol = (SmileSymbol)list->a;
				if (symbol->symbol == SMILE_SPECIAL_SYMBOL__QUOTE) {
					return ((SmileList)list->d)->a;
				}
				else {
					Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unsupported [%S] form. (parser bug?)",
						SymbolTable_GetName(Smile_SymbolTable, symbol->symbol));
					return NullObject;
				}
			}

			if (SMILE_KIND(list->a) == SMILE_KIND_PAIR) {
				SmilePair pair = (SmilePair)list->a;

				if (SMILE_KIND(pair->left) == SMILE_KIND_SYMBOL && SMILE_KIND(pair->right) == SMILE_KIND_SYMBOL) {

					SmileSymbol objSymbol = (SmileSymbol)pair->left;
					SmileSymbol methodSymbol = (SmileSymbol)pair->right;

					if (objSymbol->symbol == Smile_KnownSymbols.List_) {
						if (methodSymbol->symbol == Smile_KnownSymbols.of) {
							// Evaluate arguments and then make a list of them.
							return Parser_ApplyListOf(parser, (SmileList)list->d, replacements);
						}
						else if (methodSymbol->symbol == Smile_KnownSymbols.combine) {
							// Evaluate arguments and then combine them.  Error (normal error) if any aren't lists.
							return Parser_ApplyListCombine(parser, (SmileList)list->d, replacements);
						}
						else if (methodSymbol->symbol == Smile_KnownSymbols.cons) {
							if (SmileList_SafeLength((SmileList)list->d) != 2) {
								Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unsupported [List.cons] form. (parser bug?)");
								return NullObject;
							}
							// Evaluate arguments and then cons them.
							return Parser_ApplyListCons(parser, (SmileList)list->d, replacements);
						}
						else {
							Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unsupported [List.*] form. (parser bug?)");
							return NullObject;
						}
					}
					else if (objSymbol->symbol == Smile_KnownSymbols.Pair_) {
						if (methodSymbol->symbol == Smile_KnownSymbols.of) {
							if (SmileList_SafeLength((SmileList)list->d) != 2) {
								Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unsupported [Pair.of] form. (parser bug?)");
								return NullObject;
							}
							// Evaluate arguments and then construct a Pair.
							return Parser_ApplyPairOf(parser, (SmileList)list->d, replacements);
						}
						else {
							Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unsupported [Pair.*] form. (parser bug?)");
							return NullObject;
						}
					}
				}
			}

			// For all other forms, fall-through to default error message.

		default:
			Parser_AddFatalError(parser, lexerPosition, "RecursivelyApplyTemplate encountered unknown form. (parser bug?)");
			return NullObject;
	}
}

static SmileObject Parser_Accept(Parser parser, SmileObject replacement, Symbol *replacementVariables, Int numReplacementVariables, SmileList replacementExpressions, LexerPosition lexerPosition)
{
	Int32Dict replacementDict;
	SmileObject result;

	replacementDict = Parser_MapKeysToValues(replacementVariables, numReplacementVariables, replacementExpressions);

	result = Parser_RecursivelyApplyTemplate(parser, replacement, replacementDict, lexerPosition);

	return result;
}

static CustomSyntaxResult Parser_RecursivelyApplyCustomSyntax(Parser parser, SmileObject *expr, Int modeFlags, Symbol syntaxClassSymbol, ParseError *parseError)
{
	switch (syntaxClassSymbol) {

		case SMILE_SPECIAL_SYMBOL_STMT:
			*parseError = Parser_ParseStmt(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_EXPR:
			*parseError = Parser_ParseEquals(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_CMPEXPR:
			*parseError = Parser_ParseCmpExpr(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_ADDEXPR:
			*parseError = Parser_ParseAddExpr(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_MULEXPR:
			*parseError = Parser_ParseMulExpr(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_BINARYEXPR:
			*parseError = Parser_ParseBinaryExpr(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_PREFIXEXPR:
			*parseError = Parser_ParsePrefixExpr(parser, expr, modeFlags);
			break;

		case SMILE_SPECIAL_SYMBOL_TERM:
			*parseError = Parser_ParseTerm(parser, expr, modeFlags, NULL);
			break;

		case SMILE_SPECIAL_SYMBOL_NAME:
			*parseError = Parser_ParseAnyName(parser, expr);
			break;

		default:
			return Parser_ApplyCustomSyntax(parser, expr, modeFlags, syntaxClassSymbol, SYNTAXROOT_RECURSE, 0, parseError);
	}

	return *parseError != NULL ? CustomSyntaxResult_PartialApplicationWithError : CustomSyntaxResult_SuccessfullyParsed;
}

Inline Symbol GetSymbolForToken(Token token)
{
	Symbol tokenSymbol;

	switch (token->kind) {

		case TOKEN_ALPHANAME:
		case TOKEN_UNKNOWNALPHANAME:
		case TOKEN_PUNCTNAME:
		case TOKEN_UNKNOWNPUNCTNAME:
			// If this token had escapes in it, we treat it as an unknown generic name.  If it didn't,
			// then we look it up in the symbol table to try to match it as a keyword.
			tokenSymbol = token->hasEscapes ? -1 : SymbolTable_GetSymbolNoCreate(Smile_SymbolTable, token->text);
			break;
		
		case TOKEN_COMMA:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_COMMA;
			break;
		
		case TOKEN_SEMICOLON:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_SEMICOLON;
			break;
		
		case TOKEN_COLON:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_COLON;
			break;
		
		case TOKEN_LEFTPARENTHESIS:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_LEFTPARENTHESIS;
			break;
		
		case TOKEN_RIGHTPARENTHESIS:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_RIGHTPARENTHESIS;
			break;
		
		case TOKEN_LEFTBRACE:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_LEFTBRACE;
			break;
		
		case TOKEN_RIGHTBRACE:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_RIGHTBRACE;
			break;
		
		case TOKEN_LEFTBRACKET:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_LEFTBRACKET;
			break;
		
		case TOKEN_RIGHTBRACKET:
			tokenSymbol = SMILE_SPECIAL_SYMBOL_RIGHTBRACKET;
			break;
		
		default:
			tokenSymbol = 0;
			break;
	}

	return tokenSymbol;
}

/// <summary>
/// This is responsible for parsing a looping nonterminal, like this:  [NAME+ names , ]
/// It attempts to greedily consume as much of the input as possible, and returns
/// the resulting syntax chunk as a list.
/// </summary>
static CustomSyntaxResult Parser_ParseNonterminalList(Parser parser, ParserSyntaxNode node, SmileObject *result,
	Int modeFlags, LexerPosition position, Symbol syntaxClassSymbol, ParseError *parseError)
{
	Bool isFirstInSet = True;
	SmileList innerHead = NullList, innerTail = NullList;
	SmileObject innerExpr;
	CustomSyntaxResult nestedSyntaxResult;

	for (;;) {

		// Parse the next element of the set.
		nestedSyntaxResult = Parser_RecursivelyApplyCustomSyntax(parser, &innerExpr, modeFlags, node->name, parseError);

		// Deal with it if we didn't get a result.
		if (nestedSyntaxResult == CustomSyntaxResult_NotMatchedAndNoTokensConsumed) {
			if (isFirstInSet && node->repetitionKind == '*') {
				// OK; you can safely have zero of these.
			}
			else if (node->repetitionSep == 0) {
				// OK; it's acceptable for the last one to just be missing when there's no separator.
			}
			else {
				// Not OK: We were definitely expecting this nonterminal at this point, so churn out an error message.
				*parseError = ParseMessage_Create(PARSEMESSAGE_ERROR, position,
					String_Format("Syntax error: Missing a '%S' in '%S'",
						SymbolTable_GetName(Smile_SymbolTable, node->name),
						SymbolTable_GetName(Smile_SymbolTable, syntaxClassSymbol)));
				nestedSyntaxResult = CustomSyntaxResult_PartialApplicationWithError;
				innerHead = NullList;
			}
			break;
		}
		else if (nestedSyntaxResult == CustomSyntaxResult_PartialApplicationWithError) {
			// Inner parser errored, so we're done.
			innerHead = NullList;
			break;
		}

		// Add the result to the set.
		LIST_APPEND(innerHead, innerTail, innerExpr);

		// Decide if we need to continue, hopefully based on the next token.
		if (node->repetitionSep == ',') {
			// Check for a comma lookhead to decide whether to continue.
			if (Lexer_Next(parser->lexer) != TOKEN_COMMA) {
				Lexer_Unget(parser->lexer);
				break;
			}
		}
		else if (node->repetitionSep == ';') {
			// Check for a semicolon lookhead to decide whether to continue.
			if (Lexer_Next(parser->lexer) != TOKEN_SEMICOLON) {
				Lexer_Unget(parser->lexer);
				break;
			}
		}

		isFirstInSet = False;
	}

	*result = (SmileObject)innerHead;
	return nestedSyntaxResult;
}

CustomSyntaxResult Parser_ApplyCustomSyntax(Parser parser, SmileObject *expr, Int modeFlags, Symbol syntaxClassSymbol,
	Int syntaxRootMode, Symbol rootSkipSymbol, ParseError *parseError)
{
	ParserSyntaxClass syntaxClass;
	ParserSyntaxNode node, nextNode;
	LexerPosition position;
	SmileObject localExpr;
	SmileList localHead, localTail;
	Bool isFirst;
	Int32Dict transitionTable;
	Symbol tokenSymbol;
	Int32Int32Dict oldCustomFollowSet;
	CustomSyntaxResult nestedSyntaxResult;
	ParserSyntaxNode recentNodes[16];	// For error-reporting.
	Int recentNodeIndex;

	// Get the class that contains all the rules under the provided nonterminal symbol.
	syntaxClass = GetSyntaxClass(parser, syntaxClassSymbol);
	if (syntaxClass == NULL) {
		*parseError = NULL;
		return CustomSyntaxResult_NotMatchedAndNoTokensConsumed;
	}

	// Begin walking the tree nodes of the class, consuming input tokens where they match.
	node = (ParserSyntaxNode)syntaxClass;
	isFirst = True;

	// This is the list where we'll be collecting the nonterminal matches as we find them.
	localTail = localHead = NullList;

	// Start the list of recent nodes so that we know where we've been, for error-reporting purposes.
	recentNodeIndex = 0;

	// For the special syntax classes, we may need to apply special behaviors for the initial transition.
	if (syntaxRootMode == SYNTAXROOT_KEYWORD) {
		// We can only transition via an initial terminal; we don't need to construct or find a
		// transition table, since the nextTerminals set will be sufficient.
		Lexer_Next(parser->lexer);
		tokenSymbol = GetSymbolForToken(parser->lexer->token);
		if (node->nextTerminals == NULL || !Int32Dict_TryGetValue(node->nextTerminals, tokenSymbol, (void **)&nextNode)) {
			Lexer_Unget(parser->lexer);
			return CustomSyntaxResult_NotMatchedAndNoTokensConsumed;
		}
		node = nextNode;
		goto handleTransition;
	}
	else if (syntaxRootMode == SYNTAXROOT_NONTERMINAL) {
		// We need to skip over the given initial nonterminal (which should already have been
		// parsed and sitting in *expr), and then parse everything after it.
		if (node->nextNonterminals == NULL || !Int32Dict_TryGetValue(node->nextNonterminals, rootSkipSymbol, (void **)&nextNode))
			return CustomSyntaxResult_NotMatchedAndNoTokensConsumed;
		node = nextNode;
		recentNodes[recentNodeIndex++ & 15] = node;
	}

	for (;;) {
		// Try to match the next item (terminal or nonterminal) in the pattern.
	
		// Find or construct a table that describes possible transitions to subsequent states.
		transitionTable = ParserSyntaxTable_GetTransitionTable(parser, parser->currentScope->syntaxTable, node);
		if (transitionTable == NULL) {
			// Couldn't construct a transition table due to a grammar conflict.
			*parseError = ParseMessage_Create(PARSEMESSAGE_ERROR, Token_GetPosition(parser->lexer->token),
				String_Format("Grammar error: In rule '%S', the next state after '%S' is ambiguous",
					SymbolTable_GetName(Smile_SymbolTable, syntaxClassSymbol),
					SymbolTable_GetName(Smile_SymbolTable, node->name)));
			return isFirst ? CustomSyntaxResult_NotMatchedAndNoTokensConsumed : CustomSyntaxResult_PartialApplicationWithError;
		}

		// Try to actually transition to the next state based on the next token in the input.
		Lexer_Next(parser->lexer);
		tokenSymbol = GetSymbolForToken(parser->lexer->token);
		if (!Int32Dict_TryGetValue(transitionTable, tokenSymbol, (void **)&nextNode)) {
			// Didn't match it exactly.  See if this transition table has an "every symbol" catch-all rule.
			if (!Int32Dict_TryGetValue(transitionTable, -1, (void **)&nextNode)) {
				// Nothing in the transition table matches the next incoming token, so we're done with this rule.
				Lexer_Unget(parser->lexer);
				break;
			}
		}
		node = nextNode;
	
	handleTransition:
		// Record this node as having been visited.
		recentNodes[recentNodeIndex++ & 15] = node;

		// We have a next state for this token in nextNode, so transition into it.
		if (!node->variable) {

			// The next node is a simple terminal (effectively a keyword or piece of punctuation), so consume the
			// token, and transition directly into it.
		}
		else {
			// The next node is a nonterminal, so recursively invoke it.

			// Collect the current position, for error-reporting.
			position = Token_GetPosition(parser->lexer->token);
			Lexer_Unget(parser->lexer);

			// Recursively traverse the syntax tree.  We record the 'follow' set so that the
			// main parser knows what symbols it can and cannot safely consume.
			oldCustomFollowSet = parser->customFollowSet;
			parser->customFollowSet = ParserSyntaxTable_GetFollowSet(parser->currentScope->syntaxTable, node);
			if (node->repetitionKind == '*' || node->repetitionKind == '+') {
				// This is a list of things, and needs to be parsed in a loop (above).
				nestedSyntaxResult = Parser_ParseNonterminalList(parser, node, &localExpr,
					modeFlags, position, syntaxClassSymbol, parseError);
			}
			else {
				// We handle '?' by simply ignoring the issue if we get back NotMatched.
				nestedSyntaxResult = Parser_RecursivelyApplyCustomSyntax(parser, &localExpr, modeFlags, node->name, parseError);
			}
			parser->customFollowSet = oldCustomFollowSet;

			// Handle the result.
			switch (nestedSyntaxResult) {
				case CustomSyntaxResult_NotMatchedAndNoTokensConsumed:
					if (node->repetitionKind != '?' && node->repetitionKind != '*') {
						*parseError = ParseMessage_Create(PARSEMESSAGE_ERROR, position,
							String_Format("Syntax error: Missing a '%S' in '%S'",
								SymbolTable_GetName(Smile_SymbolTable, node->name),
								SymbolTable_GetName(Smile_SymbolTable, syntaxClassSymbol)));
						return CustomSyntaxResult_PartialApplicationWithError;
					}
					break;

				case CustomSyntaxResult_PartialApplicationWithError:
					return CustomSyntaxResult_PartialApplicationWithError;
			}

			// Append localExpr to the list of expressions generated by this syntax pattern.
			LIST_APPEND(localHead, localTail, localExpr);
		}

		isFirst = False;

		// Continue until we reach an accept node or bail due to a syntax error.
		syntaxRootMode = SYNTAXROOT_ASIS;
	}

	if (node->replacement != NullObject) {

		// We're done, and have valid expressions for each of the nonterminals.  We now need
		// to use the expressions and the replacement form and generate the parsed output.
		*expr = Parser_Accept(parser, node->replacement, node->replacementVariables, node->numReplacementVariables, localHead, node->position);
		*parseError = NULL;

		return CustomSyntaxResult_SuccessfullyParsed;
	}
	else if (isFirst) {
		// No match, but we haven't consumed anything, so maybe we don't need to match.
		*parseError = NULL;
		return CustomSyntaxResult_NotMatchedAndNoTokensConsumed;
	}
	else if (Int32Dict_Count(transitionTable) == 0) {
		// No match, and we traversed at least one node of the tree, but there's nowhere past here
		// to go, so we found a broken syntax rule.  First, collect the failed/unmatched nonterminals.
		Int32 *nextKeys = Int32Dict_GetKeys(node->nextNonterminals);
		Int32 numKeys = Int32Dict_Count(node->nextNonterminals);
		StringBuilder errorBuilder = StringBuilder_Create();
		Int32 i;
		for (i = 0; i < numKeys; i++) {
			if (i != 0) StringBuilder_Append(errorBuilder, (const Byte *)"\"/\"", 0, 3);
			StringBuilder_AppendString(errorBuilder, SymbolTable_GetName(Smile_SymbolTable, nextKeys[i]));
		}

		// Now generate a suitable error message with them.
		*parseError = ParseMessage_Create(PARSEMESSAGE_ERROR, node->position,
			String_Format("Incomplete syntax rule after \"%S\": subsequent %s \"%S\" %s.",
				SymbolTable_GetName(Smile_SymbolTable, node->name),
				numKeys == 1 ? "nonterminal" : "nonterminals",
				StringBuilder_ToString(errorBuilder),
				numKeys == 1 ? "does not match any known class" : "do not match any known classes"
			));
		return CustomSyntaxResult_PartialApplicationWithError;
	}
	else {
		// No match, and we've traversed at least one node of the tree.  So the
		// input is an error, and we begin error recovery.

		String recentNodesAsString;
		String nextTokenAsString;
		DECLARE_INLINE_STRINGBUILDER(stringBuilder, 256);
		Int nodeIndex;
		Bool isFirstPrintedNode;

		// First, turn the recent-node array into a list of printable tokens.
		INIT_INLINE_STRINGBUILDER(stringBuilder);
		nodeIndex = recentNodeIndex - 15;
		if (nodeIndex < 0)
			nodeIndex = 0;
		else if (nodeIndex > 0) {
			StringBuilder_Append(stringBuilder, (const Byte *)"...", 0, 3);
		}
		for (isFirstPrintedNode = True; nodeIndex < recentNodeIndex; nodeIndex++) {
			node = recentNodes[nodeIndex & 15];
			if (!isFirstPrintedNode)
				StringBuilder_AppendByte(stringBuilder, ' ');
			StringBuilder_AppendString(stringBuilder, SymbolTable_GetName(Smile_SymbolTable, node->name));
			isFirstPrintedNode = False;
		}
		recentNodesAsString = StringBuilder_ToString(stringBuilder);

		// Get the next token, as a string.
		nextTokenAsString = SymbolTable_GetName(Smile_SymbolTable, tokenSymbol);

		// Now make an error message that describes what went wrong.
		*parseError = ParseMessage_Create(PARSEMESSAGE_ERROR, Token_GetPosition(parser->lexer->token),
			String_Format("Syntax error: '%S' is not allowed in %S after '%S'",
				nextTokenAsString, SymbolTable_GetName(Smile_SymbolTable, syntaxClassSymbol), recentNodesAsString));

		return CustomSyntaxResult_PartialApplicationWithError;
	}
}
