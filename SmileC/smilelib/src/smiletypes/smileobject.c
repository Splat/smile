//---------------------------------------------------------------------------------------
//  Smile Programming Language Interpreter
//  Copyright 2004-2015 Sean Werkema
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
#include <smile/smiletypes/text/smilestring.h>

SmileObject SmileObject_Create(void)
{
	SmileObject obj = GC_MALLOC_STRUCT(struct SmileObjectInt);
	obj->kind = SMILE_KIND_OBJECT;
	obj->base = NULL;
	obj->vtable = SmileObject_VTable;
	return obj;
}

Bool SmileObject_CompareEqual(SmileObject self, SmileObject other)
{
	return self == other;
}

UInt32 SmileObject_Hash(SmileObject self)
{
	UNUSED(self);
	return 0;
}

void SmileObject_SetSecurity(SmileObject self, Int security)
{
	UNUSED(self);
	UNUSED(security);
	Smile_ThrowException(Smile_KnownSymbols.object_security_error,
		String_Format("Cannot alter security on base Object, which is read-only."));
}

Int SmileObject_GetSecurity(SmileObject self)
{
	UNUSED(self);
	return SMILE_SECURITY_READONLY;
}

SmileObject SmileObject_GetProperty(SmileObject self, Symbol propertyName)
{
	UNUSED(self);
	UNUSED(propertyName);
	return (SmileObject)Smile_KnownObjects.Null;
}

void SmileObject_SetProperty(SmileObject self, Symbol propertyName, SmileObject value)
{
	UNUSED(self);
	UNUSED(value);
	Smile_ThrowException(Smile_KnownSymbols.object_security_error,
		String_Format("Cannot set property \"%S\" on base Object, which is read-only.",
			SymbolTable_GetName(Smile_SymbolTable, propertyName)));
}

Bool SmileObject_HasProperty(SmileObject self, Symbol propertyName)
{
	UNUSED(self);
	UNUSED(propertyName);
	return False;
}

SmileList SmileObject_GetPropertyNames(SmileObject self)
{
	UNUSED(self);
	return Smile_KnownObjects.Null;
}

Bool SmileObject_ToBool(SmileObject self)
{
	UNUSED(self);
	return False;
}

Int32 SmileObject_ToInteger32(SmileObject self)
{
	UNUSED(self);
	return 0;
}

Real64 SmileObject_ToReal64(SmileObject self)
{
	UNUSED(self);
	return 0.0;
}

String SmileObject_ToString(SmileObject self)
{
	UNUSED(self);
	return KNOWN_STRING(Object);
}

SMILE_VTABLE(SmileObject_VTable, SmileObject)
{
	SmileObject_CompareEqual,
	SmileObject_Hash,
	SmileObject_SetSecurity,
	SmileObject_GetSecurity,

	SmileObject_GetProperty,
	SmileObject_SetProperty,
	SmileObject_HasProperty,
	SmileObject_GetPropertyNames,

	SmileObject_ToBool,
	SmileObject_ToInteger32,
	SmileObject_ToReal64,
	SmileObject_ToString,
};
