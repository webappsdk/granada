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

#pragma once

#include "cpprest/http_msg.h"

/**
 * Default namespaces in the storage system
 * Example:
 * 		key of value : session:data:DaptTt8CfPn7fsWW5Qx2WOJTLa6sX4BoeUmufl8HcDLUwNphEqQaaMIznk1QuBZV
 * 		=> namespace is: session:data:
 */
typedef std::string cache_namespace;
class cache_namespaces
{
public:
#define _CACHE_NAMESPACES
#define GRANADA_DEFAULT(a_, b_) static const cache_namespace a_;
#include "granada/defaults.dat"
#undef _CACHE_NAMESPACES
#undef GRANADA_DEFAULT
};

/**
 * Default entity keys of values in the storage system
 * and default keys of values in the configuration files.
 */
typedef std::string entity_key;
class entity_keys
{
public:
#define _ENTITY_KEYS
#define GRANADA_DEFAULT(a_, b_) static const entity_key a_;
#include "granada/defaults.dat"
#undef _ENTITY_KEYS
#undef GRANADA_DEFAULT
};

/**
 * Length of the random alphanumeric "ids"
 */
typedef int nonce_length;
class nonce_lengths
{
public:
#define _NONCE_LENGTHS
#define GRANADA_DEFAULT(a_, b_) static const nonce_length a_;
#include "granada/defaults.dat"
#undef _NONCE_LENGTHS
#undef GRANADA_DEFAULT
};

/**
 * Default string values of the entities.
 */
typedef std::string default_string;
class default_strings
{
public:
#define _GRANADA_DEFAULT_STRINGS
#define GRANADA_DEFAULT(a_, b_) static const default_string a_;
#include "granada/defaults.dat"
#undef _GRANADA_DEFAULT_STRINGS
#undef GRANADA_DEFAULT
};

/**
 * Default numberic values of the entities.
 */
typedef int default_number;
class default_numbers
{
public:
#define _GRANADA_DEFAULT_NUMBERS
#define GRANADA_DEFAULT(a_, b_) static const default_number a_;
#include "granada/defaults.dat"
#undef _GRANADA_DEFAULT_NUMBERS
#undef GRANADA_DEFAULT
};

/**
 * Default error codes of the entities.
 */
typedef std::string default_error;
class default_errors
{
public:
#define _GRANADA_DEFAULT_ERRORS
#define GRANADA_DEFAULT(a_, b_) static const default_error a_;
#include "granada/defaults.dat"
#undef _GRANADA_DEFAULT_ERRORS
#undef GRANADA_DEFAULT
};

/**
 * Default error codes descriptions of the entities.
 */
typedef std::string default_error_description;
class default_error_descriptions
{
public:
#define _GRANADA_DEFAULT_ERROR_DESCRIPTIONS
#define GRANADA_DEFAULT(a_, b_) static const default_error_description a_;
#include "granada/defaults.dat"
#undef _GRANADA_DEFAULT_ERROR_DESCRIPTIONS
#undef GRANADA_DEFAULT
};