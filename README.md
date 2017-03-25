# Smile Programming Language Interpreter

**Copyright &copy; 2004-2017 Sean Werkema**

-------------

**This is the C implementation of the Smile Programming Language.**

Smile is a functional, object-oriented, dynamically-typed programming
language with a flexible syntax.  It is heavily inspired by Lisp and
Smalltalk and JavaScript, and is as mutatable and extensible as a true
Lisp, but it reads more like Python or Ruby.

More information on Smile can be found at http://www.smile-lang.org.  An
informal development blog can also be found at http://www.werkema.com.

(_Note: This implementation is currently incomplete but growing,
and it will eventually become the official implementation.  It is **not**
usable in its current form (yet).  If you want the much-more-complete and
fairly-usable (but slow) C# implementation, you'll need to look elsewhere,
as it's not hosted here._)

-------------

## License

**Licensed under the Apache License, Version 2.0 (the "License")**;
you may not use this software except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

_(A copy of the Apache license is also included in the source repository,
in the file ''LICENSE''.)_

_(**Short short version:** It's free as in speech and free as in beer.
You can use it any way you want, but you can't claim you wrote it,
and since it's free, you can't complain if it doesn't work for you.)_

-------------

## Third-Party Software

Smile uses third-party software libraries for some functionality:

- The Boehm mostly-conservative garbage collector.  http://www.hboehm.info/gc/
- The Intel Decimal Floating-Point Math Library.  https://software.intel.com/en-us/articles/intel-decimal-floating-point-math-library

These software libraries are covered under their own licenses, but all use forms of
non-restrictive open-source licenses (in the cases above, the MIT and BSD licenses,
respectively).

Smile does not contain commercially-licensed third-party software.
Smile does not contain GPL- or LGPL-licensed third-party software.

