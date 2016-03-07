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

#include <smile/parsing/parser.h>

/// <summary>
/// When the parser generates errors/warnings/information, this is called to record that
/// information in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="message">The message to record.</param>
SMILE_API_FUNC void Parser_AddMessage(Parser parser, ParseMessage message)
{
	LIST_APPEND(parser->firstMessage, parser->lastMessage, message);
}

/// <summary>
/// When the parser generates supplementary information, this is called to record that
/// information in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the info was generated.</param>
/// <param name="message">A String_Format()-style format string that contains the message
/// of the info, and is followed by any optional format arguments.</param>
void Parser_AddInfo(Parser parser, LexerPosition position, const char *message, ...)
{
	va_list v;
	va_start(v, message);
	Parser_AddInfov(parser, position, message, v);
	va_end(v);
}

/// <summary>
/// When the parser generates supplementary information, this is called to record that
/// information in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the info was generated.</param>
/// <param name="message">A String_Formatv()-style format string that contains the message
/// of the info.</param>
/// <param name="v">Any optional format arguments for the format string.</param>
void Parser_AddInfov(Parser parser, LexerPosition position, const char *message, va_list v)
{
	String string = String_FormatV(message, v);
	ParseMessage parseMessage = ParseMessage_Create(PARSEMESSAGE_INFO, position, string);
	LIST_APPEND(parser->firstMessage, parser->lastMessage, parseMessage);
}

/// <summary>
/// When the parser encounters a info, this is called to record that info's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the info was found.</param>
/// <param name="message">A String_Format()-style format string that contains the message
/// of the info, and is followed by any optional format arguments.</param>
void Parser_AddWarning(Parser parser, LexerPosition position, const char *message, ...)
{
	va_list v;
	va_start(v, message);
	Parser_AddWarningv(parser, position, message, v);
	va_end(v);
}

/// <summary>
/// When the parser encounters a warning, this is called to record that warning's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the warning was found.</param>
/// <param name="message">A String_Formatv()-style format string that contains the message
/// of the warning.</param>
/// <param name="v">Any optional format arguments for the format string.</param>
void Parser_AddWarningv(Parser parser, LexerPosition position, const char *message, va_list v)
{
	String string = String_FormatV(message, v);
	ParseMessage parseMessage = ParseMessage_Create(PARSEMESSAGE_WARNING, position, string);
	LIST_APPEND(parser->firstMessage, parser->lastMessage, parseMessage);
}

/// <summary>
/// When the parser encounters an error, this is called to record that error's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the error was found.</param>
/// <param name="message">A String_Format()-style format string that contains the message
/// of the error, and is followed by any optional format arguments.</param>
void Parser_AddError(Parser parser, LexerPosition position, const char *message, ...)
{
	va_list v;
	va_start(v, message);
	Parser_AddErrorv(parser, position, message, v);
	va_end(v);
}

/// <summary>
/// When the parser encounters an error, this is called to record that error's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the error was found.</param>
/// <param name="message">A String_Formatv()-style format string that contains the message
/// of the error.</param>
/// <param name="v">Any optional format arguments for the format string.</param>
void Parser_AddErrorv(Parser parser, LexerPosition position, const char *message, va_list v)
{
	String string = String_FormatV(message, v);
	ParseMessage parseMessage = ParseMessage_Create(PARSEMESSAGE_ERROR, position, string);
	LIST_APPEND(parser->firstMessage, parser->lastMessage, parseMessage);
}

/// <summary>
/// When the parser encounters a fatal error, this is called to record that fatal error's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the fatal error was encountered.</param>
/// <param name="message">A String_Format()-style format string that contains the message
/// of the fatal error, and is followed by any optional format arguments.</param>
void Parser_AddFatalError(Parser parser, LexerPosition position, const char *message, ...)
{
	va_list v;
	va_start(v, message);
	Parser_AddFatalErrorv(parser, position, message, v);
	va_end(v);
}

/// <summary>
/// When the parser encounters a fatal error, this is called to record that fatal error's detail
/// in the parser's message collection.
/// </summary>
/// <param name="parser">The parser instance.</param>
/// <param name="position">The position in the input stream where the fatal error was encountered.</param>
/// <param name="message">A String_Formatv()-style format string that contains the message
/// of the fatal error.</param>
/// <param name="v">Any optional format arguments for the format string.</param>
void Parser_AddFatalErrorv(Parser parser, LexerPosition position, const char *message, va_list v)
{
	String string = String_FormatV(message, v);
	ParseMessage parseMessage = ParseMessage_Create(PARSEMESSAGE_FATAL, position, string);
	LIST_APPEND(parser->firstMessage, parser->lastMessage, parseMessage);
}
