/**
  * Copyright (c) <2016> granada <afernandez@cookinapps.io>
  *
  * This source code is licensed under the MIT license.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  * Default values and entity keys.
  */
 
#include "granada/defaults.h"

#define _CACHE_NAMESPACES
#define DAT(a_, b_) const cache_namespace cache_namespaces::a_(_XPLATSTR(b_));
#include "granada/defaults.dat"
#undef _CACHE_NAMESPACES
#undef DAT

#define _ENTITY_KEYS
#define DAT(a_, b_) const entity_key entity_keys::a_(_XPLATSTR(b_));
#include "granada/defaults.dat"
#undef _ENTITY_KEYS
#undef DAT

#define _NONCE_LENGTHS
#define DAT(a_, b_) const nonce_length nonce_lenghts::a_(_XPLATSTR(b_));
#include "granada/defaults.dat"
#undef _NONCE_LENGTHS
#undef DAT

#define _DEFAULT_STRINGS
#define DAT(a_, b_) const default_string default_strings::a_(_XPLATSTR(b_));
#include "granada/defaults.dat"
#undef _DEFAULT_STRINGS
#undef DAT

#define _DEFAULT_NUMBERS
#define DAT(a_, b_) const default_number default_numbers::a_(_XPLATSTR(b_));
#include "granada/defaults.dat"
#undef _DEFAULT_NUMBERS
#undef DAT
