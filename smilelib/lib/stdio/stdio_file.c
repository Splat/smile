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

#include <stdio.h>
#include <smile/smiletypes/easyobject.h>
#include <smile/smiletypes/smilefunction.h>
#include <smile/smiletypes/base.h>
#include <smile/smiletypes/smilehandle.h>
#include <smile/internal/staticstring.h>

#include "stdio_internal.h"

STATIC_STRING(_stdinName, "<stdin>");
STATIC_STRING(_stdoutName, "<stdout>");
STATIC_STRING(_stderrName, "<stderr>");

#if ((SMILE_OS & SMILE_OS_FAMILY) == SMILE_OS_WINDOWS_FAMILY)

	static const Int FileCostEstimate = 0x1000;

	static Bool Stdio_File_Win32End(SmileHandle handle, Bool userInvoked)
	{
		UNUSED(userInvoked);

		HANDLE win32Handle = ((Stdio_File)handle->ptr)->handle;

		if (win32Handle == NULL) return True;

		HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		HANDLE stderrHandle = GetStdHandle(STD_ERROR_HANDLE);

		if (win32Handle == stdinHandle || win32Handle == stdoutHandle || win32Handle == stderrHandle)
			return False;

		CloseHandle(win32Handle);
		((Stdio_File)handle->ptr)->handle = NULL;

		return True;
	}

	SMILE_INTERNAL_FUNC SmileHandle Stdio_File_CreateFromWin32Handle(SmileObject base, String name, HANDLE win32Handle, UInt32 mode)
	{
		Stdio_File file = GC_MALLOC_STRUCT(struct Stdio_FileStruct);
		if (file == NULL)
			Smile_Abort_OutOfMemory();

		SmileHandle handle = SmileHandle_Create(base, Stdio_File_Win32End, SymbolTable_GetSymbolC(Smile_SymbolTable, "File"), FileCostEstimate, file);

		file->path = name;
		file->mode = mode;
		file->lastErrorCode = 0;
		file->lastErrorMessage = String_Empty;
		file->isOpen = True;

		file->handle = win32Handle;

		return handle;
	}

	void Stdio_File_DeclareStdInOutErr(Closure globalClosure, SmileObject fileBase)
	{
		SmileHandle stdinHandle = Stdio_File_CreateFromWin32Handle((SmileObject)fileBase, _stdinName, GetStdHandle(STD_INPUT_HANDLE), FILE_MODE_READ);
		SmileHandle stdoutHandle = Stdio_File_CreateFromWin32Handle((SmileObject)fileBase, _stdoutName, GetStdHandle(STD_OUTPUT_HANDLE), FILE_MODE_WRITE | FILE_MODE_APPEND);
		SmileHandle stderrHandle = Stdio_File_CreateFromWin32Handle((SmileObject)fileBase, _stderrName, GetStdHandle(STD_OUTPUT_HANDLE), FILE_MODE_WRITE | FILE_MODE_APPEND);

		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stdin"), (SmileObject)stdinHandle);
		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stdout"), (SmileObject)stdoutHandle);
		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stderr"), (SmileObject)stderrHandle);
	}

	SmileHandle Stdio_File_CreateFromPath(SmileObject base, String path, UInt32 mode)
	{
		HANDLE win32Handle;
		wchar_t *path16;
		UInt32 desiredAccess, shareMode, creationDisposition, flagsAndAttributes;
		Stdio_File file;
		SmileHandle handle;

		// Windows wants backslashes in the path, not forward slashes.
		if (String_IndexOfChar(path, '/', 0) >= 0) {
			path = String_ReplaceChar(path, '/', '\\');
		}

		// Convert the path to UTF-16, because that's what the Windows APIs need.
		path16 = (wchar_t *)String_ToUtf16(path, NULL);

		// Compute out the various mode and attribute bits Windows needs from our mode mask.
		desiredAccess = 0;
		shareMode = 0;
		creationDisposition = 0;
		flagsAndAttributes = 0;

		if (mode & FILE_MODE_READ) desiredAccess |= GENERIC_READ;
		if (mode & (FILE_MODE_WRITE | FILE_MODE_APPEND)) desiredAccess |= GENERIC_WRITE;

		switch (mode & (FILE_MODE_OPEN_MASK | FILE_MODE_TRUNCATE)) {
			case FILE_MODE_OPEN_ONLY:
				creationDisposition = OPEN_EXISTING;
				break;
			case FILE_MODE_CREATE_ONLY:
			case FILE_MODE_CREATE_ONLY | FILE_MODE_TRUNCATE:
				creationDisposition = CREATE_NEW;
				break;
			case FILE_MODE_CREATE_OR_OPEN:
				creationDisposition = OPEN_ALWAYS;
				break;
			case FILE_MODE_OPEN_ONLY | FILE_MODE_TRUNCATE:
				creationDisposition = TRUNCATE_EXISTING;
				break;
			case FILE_MODE_CREATE_OR_OPEN | FILE_MODE_TRUNCATE:
				creationDisposition = CREATE_ALWAYS;
				break;
			default:
				handle = Stdio_File_CreateFromWin32Handle(base, path, NULL, mode);
				file = (Stdio_File)handle->ptr;
				file->isOpen = False;
				file->lastErrorCode = (UInt32)~0;
				file->lastErrorMessage = String_FromC("Invalid mode flags.");
				return handle;
		}

		flagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

		// Open the file for real.
		win32Handle = CreateFileW((LPCWSTR)path16, desiredAccess, shareMode, NULL, creationDisposition, flagsAndAttributes, NULL);

		// Create a wrapper object around it, even if it's not open.
		handle = Stdio_File_CreateFromWin32Handle(base, path, win32Handle, mode);
		file = (Stdio_File)handle->ptr;

		// Record any errors.
		if (win32Handle == NULL) {
			UInt32 lastError = GetLastError();
			file->isOpen = False;
			file->lastErrorCode = lastError;
			file->lastErrorMessage = Smile_Win32_GetErrorString(lastError);
		}

		return handle;
	}

#elif ((SMILE_OS & SMILE_OS_FAMILY) == SMILE_OS_UNIX_FAMILY)

	static const Int FileCostEstimate = 0x1000;

	static void Stdio_File_UnixEnd(SmileHandle handle, Bool userInvoked)
	{
		UNUSED(userInvoked);

		if (handle->value >= 0) {
			Int32 fd = handle->value;

			if (fd != STDIN_FILENO && fd != STDOUT_FILENO && fd != STDERR_FILENO) {
				close(fd);
			}
		}
	}

	SMILE_INTERNAL_FUNC SmileObject Stdio_File_CreateFromUnixFD(SmileObject base, String name, Int32 fd, UInt32 mode)
	{
		Stdio_File file = GC_MALLOC_STRUCT(struct Stdio_FileStruct);

		SmileHandle handle = SmileHandle_Create(base, Stdio_File_UnixEnd, SymbolTable_GetSymbolC(Smile_SymbolTable, "File"), FileCostEstimate, file);

		file->path = name;
		file->mode = mode;
		file->lastErrorCode = 0;
		file->lastErrorMessage = String_Empty;
		file->isOpen = True;

		file->fd = fd;

		return (SmileObject)handle;
	}

	void Stdio_File_DeclareStdInOutErr(Closure globalClosure, SmileObject fileBase)
	{
		SmileHandle stdinHandle = Stdio_File_CreateFromFD((SmileObject)fileBase, _stdinName, STDIN_FILENO, FILE_MODE_READ);
		SmileHandle stdoutHandle = Stdio_File_CreateFromFD((SmileObject)fileBase, _stdoutName, STDOUT_FILENO, FILE_MODE_WRITE | FILE_MODE_APPEND);
		SmileHandle stderrHandle = Stdio_File_CreateFromFD((SmileObject)fileBase, _stderrName, STDERR_FILENO, FILE_MODE_WRITE | FILE_MODE_APPEND);

		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stdin"), (SmileObject)stdinHandle);
		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stdout"), (SmileObject)stdoutHandle);
		Closure_SetGlobalVariable(globalClosure, SymbolTable_GetSymbolC(Smile_SymbolTable, "Stderr"), (SmileObject)stderrHandle);
	}

#else
#	error Unsupported OS.
#endif

