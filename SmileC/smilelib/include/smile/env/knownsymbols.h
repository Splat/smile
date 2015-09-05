#ifndef __SMILE_ENV_KNOWNSYMBOLS_H__
#define __SMILE_ENV_KNOWNSYMBOLS_H__

#ifndef __SMILE_ENV_SYMBOLTABLE_H__
#include <smile/env/symboltable.h>
#endif

// Preregistered symbol IDs for the special forms.
#define SMILE_SPECIAL_FORM_EQUALS 1
#define SMILE_SPECIAL_FORM_OP_EQUALS 2
#define SMILE_SPECIAL_FORM_IF 3
#define SMILE_SPECIAL_FORM_WHILE 4
#define SMILE_SPECIAL_FORM_TILL 5
#define SMILE_SPECIAL_FORM_VAR 6
#define SMILE_SPECIAL_FORM_CATCH 7
#define SMILE_SPECIAL_FORM_FN 8
#define SMILE_SPECIAL_FORM_SCOPE 9
#define SMILE_SPECIAL_FORM_QUOTE 10
#define SMILE_SPECIAL_FORM_PROG1 11
#define SMILE_SPECIAL_FORM_PROGN 12
#define SMILE_SPECIAL_FORM_NEW 13
#define SMILE_SPECIAL_FORM_RETURN 14
#define SMILE_SPECIAL_FORM_NOT 15
#define SMILE_SPECIAL_FORM_OR 16
#define SMILE_SPECIAL_FORM_AND 17
#define SMILE_SPECIAL_FORM_IS 18
#define SMILE_SPECIAL_FORM_TYPEOF 19
#define SMILE_SPECIAL_FORM_SUPEREQ 20
#define SMILE_SPECIAL_FORM_SUPERNE 21
#define SMILE_SPECIAL_FORM_BRK 22

// The set of known symbols, preregistered at startup time to save on runtime-initialization costs.
typedef struct KnownSymbolsStruct {

	// Specials.
	Symbol equals_, op_equals_;
	Symbol if_, while_, till_, var_, catch_, fn_, scope_, quote_, prog1_, progn_, new_, return_;
	Symbol not_, or_, and_;
	Symbol is_, typeof_, supereq_, superne_;
	Symbol brk_;

	// Operator symbols.
	Symbol plus, minus, star, slash, caret;
	Symbol shift_left, shift_right, arithmetic_shift_left, arithmetic_shift_right, rotate_left, rotate_right;
	Symbol eq, ne, lt, gt, le, ge;

	// Typename symbols.
	Symbol Actor_, Array_, ArrayBase_, Bool_, Byte_, ByteRange_, ByteArray_, Char_, Closure, Enumerable_, Exception_, Facade_, FacadeProper_, Fn_, Handle_;
	Symbol IntegerArrayBase_, Integer_, Integer16_, Integer32_, Integer32Array_, Integer32Map_, Integer32Range_, Integer64_, Integer64Array_, Integer64Map_, Integer64Range_, IntegerBase_, IntegerRange_, IntegerRangeBase_;
	Symbol List_, Map_, MapBase_, MathException, Number_, NumericArrayBase_, Null_, Object_, Pair_, Program_, Random_, Range_;
	Symbol RealArrayBase_, Real_, Real32_, Real32Array_, Real32Range_, Real64_, Real64Array_, Real64Range_, RealBase_, RealRange_, RealRangeBase_;
	Symbol Regex_, String_, StringMap_, Symbol_, SymbolMap_, UChar_, UserObject_;

	// General symbols.
	Symbol a, abs, acos, add_c_slashes, alnum_q, alpha_q, apply, apply_method, arguments, asin, assertions, assigned_name, atan, atan2;
	Symbol base_, bit_and, bit_not, bit_or, bit_xor, body, bool_, byte_, byte_array;
	Symbol call, call_method, camelCase, CamelCase, case_fold, case_insensitive, case_sensitive, category, ceil, char_, chip, chop;
	Symbol cident_q, clip, clone, code_at, code_length, compare, compare_i, compose, composed_q, cons, contains, contains_i, control_q, context, cos, count, count64;
	Symbol count_left_ones, count_left_zeros, count_of, count_of_i, count_ones, count_right_ones, count_right_zeros, count_zeros, crc32, create, create_child_closure;
	Symbol d, decompose, diacritic_q, digit_q, div, divide_by_zero, does_not_understand;
	Symbol each, end, ends_with, ends_with_i, escape, eval, even_q, exit, exp, extend_object, extend_where_new;
	Symbol false_, filename_mode, first, floor, fn, fold, from_seed;
	Symbol get_member, get_object_security, get_property;
	Symbol handle_kind, has_property, hash, hex_string, hex_string_pretty, html_decode, html_encode, hyphenize;
	Symbol id, in_, include, index_of, index_of_i, int_, int16_, int32_, int64_, int_lg;
	Symbol join;
	Symbol keys, kind;
	Symbol last_index_of, last_index_of_i, latin1_to_utf8, left, length, letter, letter_q;
	Symbol letter_lowercase, letter_modifier, letter_other, letter_titlecase, letter_uppercase;
	Symbol lg, list, load, log, log_domain, lower, lowercase, lowercase_q, ln;
	Symbol map, mark, mark_enclosing, mark_non_spacing, mark_spacing_combining;
	Symbol match, matches, max, message, mid, min, mod;
	Symbol name, neg_q, newline_q, next_pow2, normalize_diacritics, nth, nth_cell, null_, number;
	Symbol number_decimal_digit, number_letter, number_other, numeric_q;
	Symbol octal_q, of, of_size, odd_q, one_q, options, other;
	Symbol other_control, other_format, other_not_assigned, other_private_use, other_surrogate;
	Symbol parity, parse, parse_and_eval, pos_q, post, pow2_q, pre, primary_category, printf, process_id, property_names, punct_q, punctuation;
	Symbol punctuation_close, punctuation_connector, punctuation_dash, punctuation_final_quote, punctuation_initial_quote, punctuation_open, punctuation_other;
	Symbol raw_reverse, read_append, read_only, read_write, read_write_append, real_, real32_, real64_, rem, replace, resize, rest, result;
	Symbol reverse, reverse_bits, reverse_bytes, right, rot_13;
	Symbol separator, separator_line, separator_paragraph, separator_space;
	Symbol set_member, set_object_security, set_property, sign, sin, space_q, splice, split, sprintf;
	Symbol sqrt, sqrt_domain, start, starts_with, starts_with_i, step, stepping;
	Symbol string_, strip_c_slashes, studied_, study, substr, substring, symbol;
	Symbol symbol_currency, symbol_math, symbol_modifier, symbol_other;
	Symbol tan, text, this_, this_closure, throw_, title, titlecase, titlecase_q, trim, trim_end, trim_start, true_, type;
	Symbol uchar, underscorize, unknown, upper, uppercase, uppercase_q, url_decode, url_encode, url_query_encode, utf8_to_latin1;
	Symbol values;
	Symbol where_, whitespace_q, wildcard_matches, without;
	Symbol xdigit_q, xor;
	Symbol zero_q;

	// Error and exception symbols.
	Symbol compile_error, configuration_error, eval_error, exec_error, json_error, lexer_error, load_error, native_method_error;
	Symbol post_condition_assertion, pre_condition_assertion, syntax_error, system_exception, type_assertion, user_exception;
} *KnownSymbols;

SMILE_API void KnownSymbols_PreloadSymbolTable(SymbolTable symbolTable, KnownSymbols knownSymbols);

#endif