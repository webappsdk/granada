#include "granada/http/oauth2.h"

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
