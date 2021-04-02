//
// base64_utils.cpp
// utils
// -----------------------------------------------------------------------------
// Most of the functions in this file were taken from the mstoolkit library.
// (https://github.com/mhoopmann/mstoolkit).
// mstoolkit is distributed under an Apache License, Version 2.0.
// Copyright 2010 Mike Hoopmann, Institute for Systems Biology
// -----------------------------------------------------------------------------
//

#include <msInterface/internal/base64_utils.hpp>

/**
 * Correct byte order
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param l Binary data.
 * @param bigEndian Byte order.
 * @return Decoded integer.
 */
unsigned long utils::internal::_dtohl(uint32_t l, bool bigEndian)
{
    // if (!bigEndian) {
    //     l = (l << 24) | ((l << 8) & 0xFF0000) |
    //         (l >> 24) | ((l >> 8) & 0x00FF00);
    // }
    if (bigEndian) {
        l = (l << 24) | ((l << 8) & 0xFF0000) |
            (l >> 24) | ((l >> 8) & 0x00FF00);
    }
    return l;
}

/**
 * Correct byte order
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param l Binary data.
 * @param bigEndian Byte order.
 * @return Decoded integer.
 */
uint64_t utils::internal::_dtohl(uint64_t l, bool bigEndian)
{
    // if (!bigEndian) {
    //     l = (l << 56) | ((l << 40) & 0xFF000000000000LL) | ((l << 24) & 0x0000FF0000000000LL) |
    //         ((l << 8) & 0x000000FF00000000LL) |
    //         (l >> 56) | ((l >> 40) & 0x0000000000FF00LL) | ((l >> 24) & 0x0000000000FF0000LL) |
    //         ((l >> 8) & 0x00000000FF000000LL);
    // }
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
int utils::internal::_b64_decode_mio( char *dest,  char *src, size_t size )
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
 * @param scan utils::msInferface::Scan to populate with m/z, int values.
 * @param pData Base 64 encoded data.
 * @param dataSize Length of \p pData.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param bigEndian Is the byte order big endian?
 */
void utils::internal::_decode32(msInterface::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian)
{
    size_t size = peaksCount * 2 * sizeof(uint32_t);
    char* pDecoded = (char *) new char[size];
    memset(pDecoded, 0, size);

    if(peaksCount > 0) {
        // Base64 decoding
        // By comparing the size of the unpacked data and the expected size
        // an additional check of the data file integrity can be performed
        int length = utils::internal::_b64_decode_mio( (char*) pDecoded , (char*) pData, dataSize);
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
        uData.iData = utils::internal::_dtohl(pDecodedInts[n++], bigEndian);
        _mz = (double)uData.fData;
        uData.iData = utils::internal::_dtohl(pDecodedInts[n++], bigEndian);
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
 * @param scan utils::msInterface::Scan to populate with m/z, int values.
 * @param data Base 64 encoded data.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param bigEndian Is the byte order big endian?
 */
void utils::internal::_decode64(msInterface::Scan& scan,
                      const char* data,
                      size_t dataSize,
                      size_t peaksCount,
                      bool bigEndian)
{
    size_t size = peaksCount * 2 * sizeof(uint32_t);
    char *pDecoded = (char *) new char[size];
    memset(pDecoded, 0, size);

    if (peaksCount > 0) {
        // Base64 decoding
        // By comparing the size of the unpacked data and the expected size
        // an additional check of the data file integrity can be performed
        int length = utils::internal::_b64_decode_mio((char *) pDecoded, (char *) data, dataSize);
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
        uData.iData = utils::internal::_dtohl(pDecodedInts[n++], bigEndian);
        _mz = uData.fData;
        uData.iData = utils::internal::_dtohl(pDecodedInts[n++], bigEndian);
        _int = uData.fData;
        scan.add(_mz, _int);
    }

    // Free allocated memory
    delete[] pDecoded;
}

/**
 * @brief Decode zlib compressed, 32 bit, base 64 binary m/z-int array. <br><br>
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param scan utils::msInterface::Scan to populate with m/z, int values.
 * @param compressedData Base 32 encoded data.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param compressedLen Length of compressed data.
 * @param bigEndian Is the byte order big endian?
 */
void utils::internal::_decompress32(msInterface::Scan& scan,
                                    const std::string& compressedData,
                                    size_t peaksCount,
                                    unsigned long compressedLen,
                                    bool bigEndian){
    if(peaksCount < 1) return;

    union udata {
        float f;
        uint32_t i;
    } uData{};

    uLong uncomprLen;
    int length;
    const char* pData = compressedData.data();
    size_t stringSize = compressedData.size();
    assert(!(stringSize > 1 && compressedLen == 0));

    //Decode base64
    char* pDecoded = (char*) new char[compressedLen];
    memset(pDecoded, 0, compressedLen);
    length = utils::internal::_b64_decode_mio( (char*) pDecoded , (char*) pData, stringSize);

    //zLib decompression
    auto* data = new uint32_t[peaksCount*2];
    uncomprLen = peaksCount * 2 * sizeof(uint32_t);
    uncompress((Bytef*)data, &uncomprLen, (const Bytef*)pDecoded, length);
    delete [] pDecoded;

    //write data to arrays
    int n = 0;
    double _mz, _int;
    for(int i=0;i<peaksCount;i++){
        uData.i = utils::internal::_dtohl(data[n++], bigEndian);
        _mz = (double)uData.f;
        uData.i = utils::internal::_dtohl(data[n++], bigEndian);
        _int = (double)uData.f;
        scan.add(_mz, _int);
    }
    delete [] data;
}

/**
 * @brief Decode zlib compressed, 64 bit, base 64 binary m/z-int array. <br><br>
 * The original version of this function was taken from mstoolkit
 * (https://github.com/mhoopmann/mstoolkit).
 * @param scan utils::msInterface::Scan to populate with m/z, int values.
 * @param compressedData Base 64 encoded data.
 * @param peaksCount peaksCount attribute from mzXML file.
 * @param compressedLen Length of compressed data.
 * @param bigEndian Is the byte order big endian?
 */
void utils::internal::_decompress64(msInterface::Scan& scan,
                                    const std::string& compressedData,
                                    size_t peaksCount,
                                    unsigned long compressedLen,
                                    bool bigEndian)
{
    if(peaksCount < 1) return;

    union udata {
        double d;
        uint64_t i;
    } uData{};

    uLong uncomprLen;
    int length;
    const char* pData = compressedData.data();
    size_t stringSize = compressedData.size();
    assert(!(stringSize > 1 && compressedLen == 0));

    //Decode base64
    char *pDecoded = (char *) new char[compressedLen];
    memset(pDecoded, 0, compressedLen);
    length = utils::internal::_b64_decode_mio( (char*) pDecoded , (char*) pData, stringSize);

    //zLib decompression
    auto* data = new uint64_t[peaksCount*2];
    uncomprLen = peaksCount * 2 * sizeof(uint64_t);
    uncompress((Bytef*)data, &uncomprLen, (const Bytef*)pDecoded, length);
    delete [] pDecoded;

    //write data to arrays
    int n = 0;
    double _mz, _int;
    for(int i=0;i<peaksCount;i++){
        uData.i = utils::internal::_dtohl(data[n++], bigEndian);
        _mz = uData.d;
        uData.i = utils::internal::_dtohl(data[n++], bigEndian);
        _int = uData.d;
        scan.add(_mz, _int);
    }
    delete [] data;
}

void utils::internal::BinaryData::decode(std::vector<double>& d) const
{
    //If there is no data, back out now
    d.clear();
    if(peaksCount < 1) return;

    //For byte order correction
    union udata32 {
        float d;
        uint32_t i;
    } uData32{};

    union udata64 {
        double d;
        uint64_t i;
    } uData64{};

    const char* pData = data.data();
    size_t stringSize = data.size();

    char* decoded = new char[compressedLen];  //array for decoded base64 string
    int decodeLen;
    Bytef* unzipped = NULL;
    uLong unzippedLen;

    int i;

    //Base64 decoding
    decodeLen = utils::internal::_b64_decode_mio(decoded,(char*)pData,stringSize);

    //zlib decompression
    if(zlib) {

        if(dataType == DataType::FLOAT_32) {
            unzippedLen = peaksCount*sizeof(uint32_t);
        } else if(dataType == DataType::FLOAT_64) {
            unzippedLen = peaksCount*sizeof(uint64_t);
        } else {
            if(!numpressLinear && !numpressSlof && !numpressPic){
                std::cerr << "Unknown data format to unzip. Stopping file read." << NEW_LINE;
                exit(EXIT_FAILURE);
            }
            //don't know the unzipped size of numpressed data, so assume it to be no larger than unpressed 64-bit data
            unzippedLen = peaksCount*sizeof(uint64_t);
        }

        unzipped = new Bytef[unzippedLen];
        uncompress((Bytef*)unzipped, &unzippedLen, (const Bytef*)decoded, (uLong)decodeLen);
        delete [] decoded;

    }

    //Numpress decompression
    if(numpressLinear || numpressSlof || numpressPic){
        auto* unpressed=new double[peaksCount];

        try{
            if(numpressLinear){
                if(zlib) ms::numpress::MSNumpress::decodeLinear((unsigned char*)unzipped,(const size_t)unzippedLen,unpressed);
                else ms::numpress::MSNumpress::decodeLinear((unsigned char*)decoded,decodeLen,unpressed);
            } else if(numpressSlof){
                if(zlib) ms::numpress::MSNumpress::decodeSlof((unsigned char*)unzipped,(const size_t)unzippedLen,unpressed);
                else ms::numpress::MSNumpress::decodeSlof((unsigned char*)decoded,decodeLen,unpressed);
            } else if(numpressPic){
                if(zlib) ms::numpress::MSNumpress::decodePic((unsigned char*)unzipped,(const size_t)unzippedLen,unpressed);
                else ms::numpress::MSNumpress::decodePic((unsigned char*)decoded,decodeLen,unpressed);
            }
        } catch (const char* ch){
            std::cout << "Exception: " << ch << NEW_LINE;
            exit(EXIT_FAILURE);
        }

        if(zlib) delete [] unzipped;
        else delete [] decoded;
        for(i=0;i<peaksCount;i++) d.push_back(unpressed[i]);
        delete [] unpressed;
        return;
    }

    //Byte order correction
    if(zlib){
        if(dataType == DataType::FLOAT_32){
            uint32_t* unzipped32 = (uint32_t*)unzipped;
            for(i=0;i<peaksCount;i++){
                uData32.i = utils::internal::_dtohl(unzipped32[i], bigEndian);
                d.push_back(uData32.d);
            }
        } else if(dataType == DataType::FLOAT_64) {
            uint64_t* unzipped64 = (uint64_t*)unzipped;
            for(i=0;i<peaksCount;i++){
                uData64.i = utils::internal::_dtohl(unzipped64[i], bigEndian);
                d.push_back(uData64.d);
            }
        }
        delete [] unzipped;
    } else {
        if(dataType == DataType::FLOAT_32){
            uint32_t* decoded32 = (uint32_t*)decoded;
            for(i=0;i<peaksCount;i++){
                uData32.i = utils::internal::_dtohl(decoded32[i], bigEndian);
                d.push_back(uData32.d);
            }
        } else if(dataType == DataType::FLOAT_64) {
            uint64_t* decoded64 = (uint64_t*)decoded;
            for(i=0;i<peaksCount;i++){
                uData64.i = utils::internal::_dtohl(decoded64[i], bigEndian);
                d.push_back(uData64.d);
            }
        }
        delete [] decoded;
    }
}

bool utils::internal::BinaryData::isZlib() const {
    return zlib;
}

void utils::internal::BinaryData::setZlib(bool zlib) {
    BinaryData::zlib = zlib;
}

bool utils::internal::BinaryData::isNumpressLinear() const {
    return numpressLinear;
}

void utils::internal::BinaryData::setNumpressLinear(bool numpressLinear) {
    BinaryData::numpressLinear = numpressLinear;
}

bool utils::internal::BinaryData::isNumpressSlof() const {
    return numpressSlof;
}

void utils::internal::BinaryData::setNumpressSlof(bool numpressSlof) {
    BinaryData::numpressSlof = numpressSlof;
}

bool utils::internal::BinaryData::isNumpressPic() const {
    return numpressPic;
}

void utils::internal::BinaryData::setNumpressPic(bool numpressPic) {
    BinaryData::numpressPic = numpressPic;
}

const std::string &utils::internal::BinaryData::getData() const {
    return data;
}

void utils::internal::BinaryData::setData(const std::string &data) {
    BinaryData::data = data;
}

size_t utils::internal::BinaryData::getPeaksCount() const {
    return peaksCount;
}

void utils::internal::BinaryData::setPeaksCount(size_t peaksCount) {
    BinaryData::peaksCount = peaksCount;
}

unsigned long utils::internal::BinaryData::getCompressedLen() const {
    return compressedLen;
}

void utils::internal::BinaryData::setCompressedLen(unsigned long compressedLen) {
    BinaryData::compressedLen = compressedLen;
}

bool utils::internal::BinaryData::isBigEndian() const {
    return bigEndian;
}

void utils::internal::BinaryData::setBigEndian(bool bigEndian) {
    BinaryData::bigEndian = bigEndian;
}

utils::internal::BinaryData::DataType utils::internal::BinaryData::getDataType() const {
    return dataType;
}

void utils::internal::BinaryData::setDataType(utils::internal::BinaryData::DataType dataType) {
    BinaryData::dataType = dataType;
}

void utils::internal::BinaryData::clear() {
    zlib = false;
    numpressLinear = false;
    numpressSlof = false;
    numpressPic = false;
    data.clear();
    peaksCount = 0;
    compressedLen = 0;
    bigEndian = true;
    dataType = DataType::FLOAT_32;
}

void utils::internal::BinaryData::processBinaryArray(std::vector<double>& arr, rapidxml::xml_node<>* node)
{
    compressedLen = utils::internal::_getAttrValUL("encodedLength", node);
    data = std::string(utils::internal::_getFirstChildNode("binary", node)->value());
    bigEndian = false;

    //iterate through cvParm(s)
    for (auto *cvParam = node->first_node("cvParam");
         cvParam; cvParam = cvParam->next_sibling("cvParam")) {
        if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1000521") //32-bit float
            dataType = DataType::FLOAT_32;
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1000523") //64-bit float
            dataType = DataType::FLOAT_64;
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1000574") //zlib compression
          zlib = true;
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1002312") //MS-Numpress linear prediction compression
          numpressLinear = true;
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1002313") //MS-Numpress positive integer compression
          numpressPic = true;
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1002314") //MS-Numpress short logged float compression
          numpressSlof = true;
        //MS-Numpress linear prediction compression followed by zlib compression
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1002746") {
            zlib = true;
            numpressLinear = true;
        }
         //MS-Numpress short logged float compression followed by zlib compression
        else if(utils::internal::_getAttrValStr("accession", cvParam) == "MS:1002748") {
            zlib = true;
            numpressSlof = true;
        }
    }
    decode(arr);
}