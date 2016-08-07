
#ifndef __SMILE_SMILETYPES_OBJECT_H__
#define __SMILE_SMILETYPES_OBJECT_H__

#ifndef __SMILE_TYPES_H__
#include <smile/types.h>
#endif

#ifndef __SMILE_STRING_H__
#include <smile/string.h>
#endif

#ifndef __SMILE_ENV_ENV_H__
#include <smile/env/env.h>
#endif

#ifndef __SMILE_ENV_SYMBOLTABLE_H__
#include <smile/env/symboltable.h>
#endif

#ifndef __SMILE_SMILETYPES_KIND_H__
#include <smile/smiletypes/kind.h>
#endif

#ifndef __SMILE_SMILETYPES_PREDECL_H__
#include <smile/smiletypes/predecl.h>
#endif

//-------------------------------------------------------------------------------------------------
//  The core Object declaration itself

/// <summary>
/// These four fields are shared by all Smile objects, and must appear at the start of each
/// object's data in memory.
/// </summary>
#define DECLARE_BASE_OBJECT_PROPERTIES \
	UInt32 kind;	/* What kind of native object this is, from the SMILE_KIND enumeration */ \
	Symbol assignedSymbol;	/* The symbol assigned to this object (for debugging) */ \
	SmileVTable vtable;	/* A pointer to this object's virtual table, which must match SMILE_VTABLE_TYPE. */ \
	SmileObject base	/* A pointer to the "base" object for this object, i.e., the object from which this object inherits any properties. */

/// <summary>
/// This is the core Smile "Object" object itself.  Only one instance of this will ever exist.
/// </summary>
struct SmileObjectInt {
	DECLARE_BASE_OBJECT_PROPERTIES;
};

//-------------------------------------------------------------------------------------------------
//  The virtual table common to all objects.

/// <summary>
/// Declare the type of a Smile object virtual table.
/// </summary>
/// <param name="__name__">The name of the virtual table type, like "struct FooObjectVTable".
/// (This is not a string, but the actual identifier to use in code.)</param>
/// <param name="__type__">The type of object this virtual table's methods manipulate, like "FooObject".
/// (Again, this is not a string, but the actual identifier to use in code.)</param>
/// <returns>A struct declaration that matches the given virtual table.</returns>
/// <remarks>
/// <h3>Documentation for the standard fourteen Smile object methods</h3>
///
/// <hr />
/// <h4>Base operations:</h4>
///
/// <code>compareEqual</code>:	<p>Compare this object against another object, which could be an object of any type.
///	Return true if they are logically equal, false if they are not.  This must follow the
///	rules listed below:</p>
///	<ul>
///	<li>Objects may only be equal if they are of the same SMILE_KIND;</li>
///	<li>Equality is commutative; that is, "a == b" has the same meaning as "b == a".</li>
///	<li>The equality function must be defined consistent with the hash() function below.</li>
///	<li>The only object that may equal the Object instance is the Object instance.</li>
///	<li>The only object that may equal the Null instance is the Null instance.</li>
///	</ul>
///	
/// <code>hash</code>:	<p>Generate a 32-bit hash code for this object that would allow it to be suitable for
///	use as a dictionary key.  The following rules for hash codes must be observed:</p>
///	<ul>
///	<li>Hashing must be consistent; that is, hash(x) must equal hash(x) no matter how
///			many times it is invoked nor where or when it is invoked for the same x.</li>
///	<li>Hashing must be consistent with object equality; that is, if a ==b, then
///			hash(a) == hash(b).</li>
///	<li>Hash codes should be as close to uniformly-randomly-distributed as possible.</li>
///	</ul>
///
/// <hr />
/// <h4>Security operations:</h4>
///
/// <code>setSecurityKey</code>:	<p>Change the security key of this object to be the provided object instance.  Security
///	keys allow an object to be locked down to prevent alteration by unauthorized parties.
///	Any object may be used as a security key.  By default, all objects start with Null as
///	their security key.  This requires you to pass the object's current security key in order
///	to be authorized to set a new one.  Not all object types participate in security; but
///	notably, UserObject and Fa�ade do, and they alone are sufficient to be able to apply
///	security to all other objects.</p>
///	
/// <code>setSecurity</code>:	<p>Change the current security flags applied to this object, from the SMILE_SECURITY_*
///	enum.  This replaces the current security flags, in full.  You may only alter this object's
///	security if you provide the correct security key for it.  An object may choose to ignore
///	or alter requests to change its security, according to its own internal rules (for example,
///	an always-immutable object type, like SmileString, may always prohibit attempts to
///	force it to allow WRITE access to its data).</p>
///	
/// <code>getSecurity</code>:	<p>Get the current security flags applied to this object, from the SMILE_SECURITY_*
///	enum.  All objects may be queried as to their security, and must respond according to
///	what they permit.</p>
///	
/// <hr />	
/// <h4>Property operations:</h4>	
///	
/// <code>getProperty</code>:	<p>Retrieve a property of this object, by its symbol.  If no such property exists, this
///	must return the Null object.  (Note that this necessarily implies that there is no difference
///	as far as getProperty() is concerned between a nonexistent property and a property
///	containing Null.).  If this object prohibits READ security access, this should return
///	the Null object.</p>
///	
/// <code>setProperty</code>:	<p>Add or update a property of this object, by its symbol. If no such property exists,
///	this <em>may</em> create the property. Some objects may prohibit property additions
///	and modifications; some may only allow specific values for some properties; and some
///	may observe the security rules when adding or updating properties.  No guarantees are
///	placed on the behavior of this method.</p>
///	
/// <code>hasProperty</code>:	<p>Determine whether this object has the named property.  Should return true if
///	the object has this property, or false if it does not.  This method usually (but not always!)
///	matches whether getProperty() for this object would return a non-Null value.  If this
///	object prohibits READ access, thish should usually return false for all symbols.</p>
///	
/// <code>getPropertyNames</code>:	<p>Retrieve a list of symbols that identify all of the properties of this object.  This
///	data is informative, not normative.  In general, it <em>should</em> return a list
///	that contains all the symbols for which hasProperty() would return true.  If this object
///	prohibits READ access, this should usually return Null (the empty list).</p>
///		
/// <hr />	
/// <h4>Conversion operations:</h4>	
///	
/// <code>toBool</code>:	<p>Construct a Boolean representation of this object.  This conversion will be used
///	in many places, including if-statements, and should be based on the content of the
///	object:  For example, empty strings convert to false, but nonempty strings convert to
///	true.  Numeric values convert to false if they are zero, and true otherwise.  Most objects
///	other than numbers and strings should return true, but there can be legitimate reasons
///	for a nontrivial object to be false (a closed network stream, for example).</p>
///	
/// <code>toInteger32</code>:	<p>Construct an Integer32 representation of this object.  This conversion is used in
///	several places, and should be based on the content of the object:  Strings and arrays, for
///	example, return their length.  This should generally be nonzero for most objects, but
///	there can be legitimate reasons for a nontrivial object to be zero (an empty queue, for
///	example).</p>
///	
/// <code>toFloat64</code>:	<p>Construct a Float64 representation of this object.  This conversion is used in only
///	a few specific places.  It should be based on the content of the object:  Strings and arrays, for
///	example, return their length.  This should generally be nonzero for most objects, but
///	there can be legitimate reasons for a nontrivial object to be zero (the cartesian length of a
///	3D vector, for example).</p>
///	
/// <code>toReal64</code>:	<p>Construct a Real64 representation of this object.  This conversion is used in only
///	a few specific places.  It should be based on the content of the object:  Strings and arrays, for
///	example, return their length.  This should generally be nonzero for most objects, but
///	there can be legitimate reasons for a nontrivial object to be zero (the cartesian length of a
///	3D vector, for example).</p>
///	
/// <code>toString</code>:	<p>Construct a string representation of this object.  This conversion is used in many
///	places, and is intended to be <em>human-readable</em>, not necessarily a formal
///	serialization of the object.  It is used during debugging to observe the state of objects, and
///	is used by all of the <code>print</code> methods for outputting objects to streams.
///	Despite the fact that is it not intended to be a serialization method, many of the standard
///	built-in objects produce strings that are equivalent to a serialized form.</p>
/// </remarks>
#define SMILE_VTABLE_TYPE(__name__, __type__) \
	__name__ { \
		Bool (*compareEqual)(__type__ self, SmileObject other); \
		UInt32 (*hash)(__type__ self); \
		\
		void (*setSecurityKey)(__type__ self, SmileObject newSecurityKey, SmileObject oldSecurityKey); \
		void (*setSecurity)(__type__ self, Int security, SmileObject securityKey); \
		Int (*getSecurity)(__type__ self); \
		\
		SmileObject (*getProperty)(__type__ self, Symbol propertyName); \
		void (*setProperty)(__type__ self, Symbol propertyName, SmileObject value); \
		Bool (*hasProperty)(__type__ self, Symbol propertyName); \
		SmileList (*getPropertyNames)(__type__ self); \
		\
		Bool (*toBool)(__type__ self); \
		Int32 (*toInteger32)(__type__ self); \
		Float64 (*toFloat64)(__type__ self); \
		Real64 (*toReal64)(__type__ self); \
		String (*toString)(__type__ self); \
	}

/// <summary>
/// Declare an instance of a Smile object virtual table.
/// </summary>
/// <remarks>
/// <p>This should be followed by a curly-brace block that contains pointers to the methods of the
/// virtual table, like this:</p>
/// <pre>
///	SMILE_VTABLE(FooObjectVTable, FooObject) {
///		FooObject_CompareEqual,
///		FooObject_Hash,
///		...
///	}
/// </pre>
/// <p>This generates a static data struct named <em>FooObjectVTable</em> that is of a proper
/// VTable type and that is filled in with the methods named in the curly-brace block.</p>
/// </remarks>
/// <param name="__name__">The name of the virtual table declaration, like "FooObjectVTable".
/// (This is not a string, but the actual identifier to use in code.)</param>
/// <param name="__type__">The type of object this virtual table's methods manipulate, like "FooObject".
/// (Again, this is not a string, but the actual identifier to use in code.)</param>
/// <returns>Code that declares an instance of the virtual table as static data.</returns>
#define SMILE_VTABLE(__name__, __type__) \
	SMILE_VTABLE_TYPE(struct __name__##Int, __type__); \
	static struct __name__##Int __name__##Data; \
	\
	SmileVTable __name__ = (SmileVTable)&__name__##Data; \
	\
	static struct __name__##Int __name__##Data =

/// <summary>
/// Get whatever kind of native Smile object this object is.
/// </summary>
/// <param name="obj">The object to get the kind of.</param>
/// <returns>The kind of this object, from the SMILE_KIND_* enum.</returns>
#define SMILE_KIND(obj) ((obj)->kind & SMILE_KIND_MASK)

/// <summary>
/// Perform a virtual call to the given method on the object, passing no arguments.
/// </summary>
/// <param name="__obj__">The object whose method you would like to call.</param>
/// <param name="__method__">The name of the method to call, like "toString" (without quotes).</param>
/// <returns>The return value from the method.</returns>
#define SMILE_VCALL(__obj__, __method__) \
	((__obj__)->vtable->__method__((SmileObject)(__obj__)))

/// <summary>
/// Perform a virtual call to the given method on the object, passing one argument.
/// </summary>
/// <param name="__obj__">The object whose method you would like to call.</param>
/// <param name="__arg1__">The first argument to pass to the method.</param>
/// <param name="__method__">The name of the method to call, like "getProperty" (without quotes).</param>
/// <returns>The return value from the method.</returns>
#define SMILE_VCALL1(__obj__, __method__, __arg1__) \
	((__obj__)->vtable->__method__((SmileObject)(__obj__), __arg1__))

/// <summary>
/// Perform a virtual call to the given method on the object, passing two arguments.
/// </summary>
/// <param name="__obj__">The object whose method you would like to call.</param>
/// <param name="__arg1__">The first argument to pass to the method.</param>
/// <param name="__arg2__">The second argument to pass to the method.</param>
/// <param name="__method__">The name of the method to call, like "setProperty" (without quotes).</param>
/// <returns>The return value from the method.</returns>
#define SMILE_VCALL2(__obj__, __method__, __arg1__, __arg2__) \
	((__obj__)->vtable->__method__((SmileObject)(__obj__), __arg1__, __arg2__))

//-------------------------------------------------------------------------------------------------
//  Declare the core SmileObject itself, its virtual table, and common (external) operations
//  for working with it.

SMILE_VTABLE_TYPE(struct SmileVTableInt, SmileObject);

SMILE_API_DATA SmileVTable SmileObject_VTable;

SMILE_API_FUNC Bool SmileObject_CompareEqual(SmileObject self, SmileObject other);
SMILE_API_FUNC UInt32 SmileObject_Hash(SmileObject self);
SMILE_API_FUNC void SmileObject_SetSecurity(SmileObject self, Int security, SmileObject securityKey);
SMILE_API_FUNC Int SmileObject_GetSecurity(SmileObject self);
SMILE_API_FUNC SmileObject SmileObject_GetProperty(SmileObject self, Symbol propertyName);
SMILE_API_FUNC void SmileObject_SetProperty(SmileObject self, Symbol propertyName, SmileObject value);
SMILE_API_FUNC Bool SmileObject_HasProperty(SmileObject self, Symbol propertyName);
SMILE_API_FUNC SmileList SmileObject_GetPropertyNames(SmileObject self);
SMILE_API_FUNC Bool SmileObject_ToBool(SmileObject self);
SMILE_API_FUNC Int32 SmileObject_ToInteger32(SmileObject self);
SMILE_API_FUNC Float64 SmileObject_ToFloat64(SmileObject self);
SMILE_API_FUNC Real64 SmileObject_ToReal64(SmileObject self);
SMILE_API_FUNC String SmileObject_ToString(SmileObject self);

SMILE_API_FUNC SmileObject SmileObject_Create(void);

//-------------------------------------------------------------------------------------------------
// These aren't core operations, but they're commonly-needed.

SMILE_API_FUNC String SmileObject_Stringify(SmileObject obj);
SMILE_API_FUNC const char *SmileObject_StringifyToC(SmileObject obj);

SMILE_API_FUNC Bool SmileObject_IsRegularList(SmileObject list);
SMILE_API_FUNC Bool SmileObject_ContainsNestedList(SmileObject obj);

//-------------------------------------------------------------------------------------------------
//  Inline operations on SmileObject.

Inline Bool SmileObject_IsList(SmileObject self)
{
	register UInt32 kind = SMILE_KIND(self);
	return kind == SMILE_KIND_LIST || kind == SMILE_KIND_NULL;
}

Inline Bool SmileObject_IsListWithSource(SmileObject self)
{
	return SmileObject_IsList(self) && (self->kind & SMILE_FLAG_WITHSOURCE);
}

Inline Bool SmileObject_IsPair(SmileObject self)
{
	return SMILE_KIND(self) == SMILE_KIND_PAIR;
}

Inline Bool SmileObject_IsPairWithSource(SmileObject self)
{
	return SmileObject_IsPair(self) && (self->kind & SMILE_FLAG_WITHSOURCE);
}

Inline Bool SmileObject_IsSymbol(SmileObject self)
{
	return SMILE_KIND(self) == SMILE_KIND_SYMBOL;
}

Inline Bool SmileObject_IsNull(SmileObject self)
{
	return SMILE_KIND(self) == SMILE_KIND_NULL;
}

#endif
