#pragma once

#define CS_HLWA_R                     int

#define CS_HLWA_R_SUCCESS             0x01
#define CS_HLWA_R_NOTADMIN            0x02
#define CS_HLWA_R_INVALIDARGS         0x04
#define CS_HLWA_R_INVALIDTYPE         0x08 // Not in macro right now; possible use elsewhere.
#define CS_HLWA_R_INVALID             0x10
#define CS_HLWA_R_NOTFOUND            0x20
#define CS_HLWA_R_ALREADYEXISTS       0x80 // This often gets paired with CS_HLWA_R_SUCCESS.

#if defined(CS_HLWA_USEUTF8STRINGS)
#define CS_HLWA_DEFAULTENTRY          ""
#define CS_HLWA_STRING                std::string
#else
#define CS_HLWA_DEFAULTENTRY          L""        // Default entry in a key uses no string as the name.
#define CS_HLWA_STRING                std::wstring
#endif
