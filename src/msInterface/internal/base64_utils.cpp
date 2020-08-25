
#include <msInterface/internal/base64_utils.hpp>

/**
 * Convert binary to 32 bit integer.
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param l Binary data.
 * @param bigEndian Byte order.
 * @return Decoded integer.
 */
unsigned long utils::_dtohl(uint32_t l, bool bigEndian)
{
    if (!bigEndian) {
        l = (l << 24) | ((l << 8) & 0xFF0000) |
            (l >> 24) | ((l >> 8) & 0x00FF00);
    }
    if (bigEndian) {
        l = (l << 24) | ((l << 8) & 0xFF0000) |
            (l >> 24) | ((l >> 8) & 0x00FF00);
    }
    return l;
}

/**
 * Convert binary to 64 bit integer.
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param l Binary data.
 * @param bigEndian Byte order.
 * @return Decoded integer.
 */
uint64_t utils::_dtohl(uint64_t l, bool bigEndian)
{
    if (!bigEndian) {
        l = (l << 56) | ((l << 40) & 0xFF000000000000LL) | ((l << 24) & 0x0000FF0000000000LL) |
            ((l << 8) & 0x000000FF00000000LL) |
            (l >> 56) | ((l >> 40) & 0x0000000000FF00LL) | ((l >> 24) & 0x0000000000FF0000LL) |
            ((l >> 8) & 0x00000000FF000000LL);
    }
    if (bigEndian) {
        l = (l << 56) | ((l << 40) & 0x00FF000000000000LL) | ((l << 24) & 0x0000FF0000000000LL) |
            ((l << 8) & 0x000000FF00000000LL) |
            (l >> 56) | ((l >> 40) & 0x000000000000FF00LL) | ((l >> 24) & 0x0000000000FF0000LL) |
            ((l >> 8) & 0x00000000FF000000LL);
    }
    return l;
}


/**
 * Decode base 64 encoded data. <br><br>
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param dest Pointer to decoded data.
 * @param src Pointer to encoded data.
 * @param size Length of data in \p src.
 * @return The total number of bytes decoded
 */
int utils::_b64_decode_mio( char *dest,  char *src, size_t size )
{
    char *temp = dest;
    char *end = dest + size;

    for(;;)
    {
        int a;
        int b;
        int t1,t2,t3,t4;

        if (!(t1 = *src++) || !(t2 = *src++) || !(t3 = *src++) || !(t4 = *src++))
            return (int)(temp-dest);

        if (t1 == 61 || temp >= end)		// if == '='
            return(int)(temp-dest);

        if( t1 > 96 )		// [a-z]
            a = (t1 - 71);
        else if( t1 > 64 )		// [A-Z]
            a = (t1 - 65);
        else if( t1 > 47 )		// [0-9]
            a = (t1 + 4);
        else if( t1 == 43 )
            a = 62;
        else				// src[0] == '/'
            a = 63;


        if( t2 > 96 )		// [a-z]
            b = (t2 - 71);
        else if( t2 > 64 )		// [A-Z]
            b = (t2 - 65);
        else if( t2 > 47 )		// [0-9]
            b = (t2 + 4);
        else if( t2 == 43 )
            b = 62;
        else				// src[0] == '/'
            b = 63;

        *temp++ = ( a << 2) | ( b >> 4);

        if (t3 == 61 || temp >= end)
            return (int)(temp-dest);;

        if( t3 > 96 )		// [a-z]
            a = (t3 - 71);
        else if( t3 > 64 )		// [A-Z]
            a = (t3 - 65);
        else if( t3 > 47 )		// [0-9]
            a = (t3 + 4);
        else if( t3 == 43 )
            a = 62;
        else				// src[0] == '/'
            a = 63;

        *temp++ = ( b << 4) | ( a >> 2);

        if (t4 == 61 || temp >= end)
            return (int)(temp-dest);;

        if( t4 > 96 )		// [a-z]
            b = (t4 - 71);
        else if( t4 > 64 )		// [A-Z]
            b = (t4 - 65);
        else if( t4 > 47 )		// [0-9]
            b = (t4 + 4);
        else if( t4 == 43 )
            b = 62;
        else				// src[0] == '/'
            b = 63;

        *temp++ = ( a << 6) | ( b );
    }
}

/**
 * Decode 32 bit base 64 binary m/z-int array.
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param scan utils::Scan to populate with m/z, int values.
 * @param pData Base 64 encoded data.
 * @param dataSize Length of \p pData.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param bigEndian Is the byte order big endian?
 */
void utils::_decode32(utils::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian)
{
    size_t size = peaksCount * 2 * sizeof(uint32_t);
    char* pDecoded = (char *) new char[size];
    memset(pDecoded, 0, size);

    if(peaksCount > 0) {
        // Base64 decoding
        // By comparing the size of the unpacked data and the expected size
        // an additional check of the data file integrity can be performed
        int length = utils::_b64_decode_mio( (char*) pDecoded , (char*) pData, dataSize);
        if(length != size) {
            std::cerr << " decoded size " << length << " and required size " << (unsigned long)size << " dont match:\n";
            std::cerr << " Cause: possible corrupted file.\n";
            exit(EXIT_FAILURE);
        }
    }

    // And byte order correction
    union udata {
        float fData;
        uint32_t iData;
    } uData{};

    int n = 0;
    auto* pDecodedInts = (uint32_t*)pDecoded; // cast to uint_32 for reading int sized chunks
    double _mz, _int;
    for(int i = 0; i < peaksCount; i++) {
        uData.iData = utils::_dtohl(pDecodedInts[n++], bigEndian);
        _mz = (double)uData.fData;
        uData.iData = utils::_dtohl(pDecodedInts[n++], bigEndian);
        _int = (double)uData.fData;
        scan.add(_mz, _int);
    }

    // Free allocated memory
    delete[] pDecoded;
}

/**
 * @brief Decode 64 bit base 64 binary m/z-int array. <br><br>
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param scan utils::Scan to populate with m/z, int values.
 * @param pData Base 64 encoded data.
 * @param dataSize Length of \p pData.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param bigEndian Is the byte order big endian?
 */
void utils::_decode64(utils::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian) {
    size_t size = peaksCount * 2 * sizeof(uint32_t);
    char *pDecoded = (char *) new char[size];
    memset(pDecoded, 0, size);

    if (peaksCount > 0) {
        // Base64 decoding
        // By comparing the size of the unpacked data and the expected size
        // an additional check of the data file integrity can be performed
        int length = utils::_b64_decode_mio((char *) pDecoded, (char *) pData, dataSize);
        if (length != size) {
            std::cerr << " decoded size " << length << " and required size " << (unsigned long) size
                      << " dont match:\n";
            std::cerr << " Cause: possible corrupted file.\n";
            exit(EXIT_FAILURE);
        }
    }

    // And byte order correction
    union udata {
        double fData;
        uint64_t iData;
    } uData{};

    int n = 0;
    auto *pDecodedInts = (uint64_t *) pDecoded; // cast to uint_64 for reading int sized chunks
    double _mz, _int;
    for (int i = 0; i < peaksCount; i++) {
        uData.iData = utils::_dtohl(pDecodedInts[n++], bigEndian);
        _mz = uData.fData;
        uData.iData = utils::_dtohl(pDecodedInts[n++], bigEndian);
        _int = uData.fData;
        scan.add(_mz, _int);
    }

    // Free allocated memory
    delete[] pDecoded;
}
