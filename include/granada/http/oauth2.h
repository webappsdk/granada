#pragma once

#include "cpprest/http_msg.h"

// Constant strings for OAuth 2.0.
typedef utility::string_t oauth2_error;
class oauth2_errors
{
public:
#define _OAUTH2_ERRORS
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_error a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS
#undef DAT
};


typedef utility::string_t oauth2_error_description;
class oauth2_errors_description
{
public:
#define _OAUTH2_ERRORS_DESCRIPTION
#define DAT(a_, b_) _ASYNCRTIMP static const oauth2_error_description a_;
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS_DESCRIPTION
#undef DAT
};
