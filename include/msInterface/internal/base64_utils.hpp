//
// base64_utils.hpp
// utils
// -----------------------------------------------------------------------------
//

#ifndef base64_utils_hpp
#define base64_utils_hpp

#include <iostream>

#include <utils.hpp>
#include <msInterface/msScan.hpp>

namespace utils {

    uint64_t _dtohl(uint64_t l, bool bigEndian);
    unsigned long _dtohl(uint32_t l, bool bigEndian);
    int _b64_decode_mio( char *dest,  char *src, size_t size );
    void _decode32(utils::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian = true);
    void _decode64(utils::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian = true);
}

#endif
