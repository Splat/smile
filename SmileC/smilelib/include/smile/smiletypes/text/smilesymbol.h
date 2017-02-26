
#ifndef __SMILE_SMILETYPES_TEXT_SMILESYMBOL_H__
#define __SMILE_SMILETYPES_TEXT_SMILESYMBOL_H__

#ifndef __SMILE_STRING_H__
#include <smile/string.h>
#endif

#ifndef __SMILE_SMILETYPES_PREDECL_H__
#include <smile/smiletypes/predecl.h>
#endif

#ifndef __SMILE_SMILETYPES_SMILEOBJECT_H__
#include <smile/smiletypes/smileobject.h>
#endif

//-------------------------------------------------------------------------------------------------
//  Type declarations

struct SmileSymbolInt {
	DECLARE_BASE_OBJECT_PROPERTIES;
	Symbol symbol;
};

//-------------------------------------------------------------------------------------------------
//  Public interface

SMILE_API_DATA SmileVTable SmileSymbol_VTable;

SMILE_API_FUNC SmileSymbol SmileSymbol_Create(Symbol symbol);

#endif
