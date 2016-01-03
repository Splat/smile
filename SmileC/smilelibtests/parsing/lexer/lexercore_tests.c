//---------------------------------------------------------------------------------------
//  Smile Programming Language Interpreter (Unit Tests)
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

#include "../../stdafx.h"

#include <smile/parsing/lexer.h>

TEST_SUITE(_LexerCoreTests)

STATIC_STRING(TestFilename, "test.sm");

//-------------------------------------------------------------------------------------------------
//  Setup helper.

static Lexer Setup(const char *string)
{
	String source;
	Lexer lexer;

	Smile_ResetEnvironment();

	source = String_FromC(string);

	lexer = Lexer_Create(source, 0, String_Length(source), TestFilename, 1, 1);
	lexer->symbolTable = Smile_SymbolTable;

	return lexer;
}

//-------------------------------------------------------------------------------------------------
//  Simple whitespace tests.

START_TEST(EmptyInputResultsInEoi)
{
	Lexer lexer = Setup("");

	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(EoiShouldStayEoiOnceYouReachIt)
{
	Lexer lexer = Setup("");

	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(AllWhitespaceInputResultsInEoi)
{
	Lexer lexer = Setup("  \t  \r  \n  \a  \b  \v  \f  ");

	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(SingleLineCommentsShouldBeSkipped)
{
	Lexer lexer = Setup("// This is a comment.\r\n"
		"// This is a comment on the next line.\r\n"
		"\r\n"
		"// This is another comment.\r\n"
		"\r\n");

	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(SingleLineCommentsShouldBeSkipped2)
{
	Lexer lexer = Setup("// This is a comment.\r\n"
		"// This is a comment on the next line.\r\n"
		".\r\n"
		"// This is another comment.\r\n"
		"+\r\n");

	ASSERT(Lexer_Next(lexer) == TOKEN_DOT);
	ASSERT(Lexer_Next(lexer) == TOKEN_PLUS);
	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(MultiLineCommentsShouldBeSkipped)
{
	Lexer lexer = Setup("/* This is a comment.\r\n"
		"This is a comment on the next line.\r\n"
		"\r\n"
		"This is another comment. */\r\n"
		"\r\n");

	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

START_TEST(MultiLineCommentsShouldBeSkipped2)
{
	Lexer lexer = Setup("/* This is a comment.\r\n"
		"This is a comment on the next line.*/\r\n"
		".\r\n"
		"/* This is another comment. */\r\n"
		"+\r\n");

	ASSERT(Lexer_Next(lexer) == TOKEN_DOT);
	ASSERT(Lexer_Next(lexer) == TOKEN_PLUS);
	ASSERT(Lexer_Next(lexer) == TOKEN_EOI);
}
END_TEST

#include "lexercore_tests.generated.inc"
