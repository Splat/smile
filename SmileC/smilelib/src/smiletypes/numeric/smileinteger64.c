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
#include <smile/smiletypes/numeric/smileinteger64.h>

SmileInteger64 SmileInteger64_CreateInternal(SmileEnv env, Int64 value)
{
	SmileInteger64 smileInt = GC_MALLOC_STRUCT(struct SmileInteger64Int);
	if (smileInt == NULL) Smile_Abort_OutOfMemory();
	smileInt->base = env->knownObjects.Object;
	smileInt->env = env;
	smileInt->kind = SMILE_KIND_INTEGER64;
	smileInt->vtable = SmileInteger64_VTable;
	smileInt->value = value;
	return smileInt;
}

Bool SmileInteger64_CompareEqual(SmileInteger64 self, SmileObject other)
{
	SmileInteger64 otherInt;

	if (other->kind != SMILE_KIND_INTEGER64) return False;
	otherInt = (SmileInteger64)other;

	return self->value == otherInt->value;
}

UInt32 SmileInteger64_Hash(SmileInteger64 self)
{
	UInt64 value = (UInt64)self->value;
	return (UInt32)(value ^ (value >> 32));
}

void SmileInteger64_SetSecurity(SmileInteger64 self, Int security)
{
	UNUSED(self);
	UNUSED(security);
	SmileEnv_ThrowException(self->env, self->env->knownSymbols.object_security_error,
		String_Format("Cannot alter security on integers, which are read-only."));
}

Int SmileInteger64_GetSecurity(SmileInteger64 self)
{
	UNUSED(self);
	return SMILE_SECURITY_READONLY;
}

SmileObject SmileInteger64_GetProperty(SmileInteger64 self, Symbol propertyName)
{
	return self->base->vtable->getProperty((SmileObject)self, propertyName);
}

void SmileInteger64_SetProperty(SmileInteger64 self, Symbol propertyName, SmileObject value)
{
	UNUSED(value);
	SmileEnv_ThrowException(self->env, self->env->knownSymbols.object_security_error,
		String_Format("Cannot set property \"%S\" on an integer, which is read-only.",
		SymbolTable_GetName(self->env->symbolTable, propertyName)));
}

Bool SmileInteger64_HasProperty(SmileInteger64 self, Symbol propertyName)
{
	UNUSED(self);
	UNUSED(propertyName);
	return False;
}

SmileList SmileInteger64_GetPropertyNames(SmileInteger64 self)
{
	return self->env->knownObjects.Null;
}

Bool SmileInteger64_ToBool(SmileInteger64 self)
{
	return self->value != 0;
}

Int32 SmileInteger64_ToInteger32(SmileInteger64 self)
{
	return (Int32)self->value;
}

Real64 SmileInteger64_ToReal64(SmileInteger64 self)
{
	return (Real64)self->value;
}

String SmileInteger64_ToString(SmileInteger64 self)
{
	return String_Format("%ldL", self->value);
}

SMILE_VTABLE(SmileInteger64_VTable, SmileInteger64)
{
	SmileInteger64_CompareEqual,
	SmileInteger64_Hash,
	SmileInteger64_SetSecurity,
	SmileInteger64_GetSecurity,

	SmileInteger64_GetProperty,
	SmileInteger64_SetProperty,
	SmileInteger64_HasProperty,
	SmileInteger64_GetPropertyNames,

	SmileInteger64_ToBool,
	SmileInteger64_ToInteger32,
	SmileInteger64_ToReal64,
	SmileInteger64_ToString,
};
