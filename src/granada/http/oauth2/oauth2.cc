#include "granada/http/oauth2/oauth2.h"

#define _OAUTH2_ERRORS
#define DAT(a_, b_) const oauth2_error oauth2_errors::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS
#undef DAT

#define _OAUTH2_ERRORS_DESCRIPTION
#define DAT(a_, b_) const oauth2_error_description oauth2_errors_description::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_ERRORS_DESCRIPTION
#undef DAT

#define _OAUTH2_CLIENT_TYPES
#define DAT(a_, b_) const oauth2_client_type oauth2_client_types::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_CLIENT_TYPES
#undef DAT

#define _OAUTH2_STRINGS_2
#define DAT(a_, b_) const oauth2_string_2 oauth2_strings_2::a_(_XPLATSTR(b_));
#include "granada/http/http_constants.dat"
#undef _OAUTH2_STRINGS_2
#undef DAT
