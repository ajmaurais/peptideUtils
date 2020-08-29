//
// base64_utils.hpp
// utils
// -----------------------------------------------------------------------------
// Most of the functions in this file were taken from the mstoolkit library.
// (https://github.com/mhoopmann/mstoolkit).
// mstoolkit is distributed under an Apache License, Version 2.0.
// Copyright 2010 Mike Hoopmann, Institute for Systems Biology
// -----------------------------------------------------------------------------
//

#ifndef base64_utils_hpp
#define base64_utils_hpp

#include <iostream>

#include <thirdparty/rapidxml/rapidxml.hpp>
#include <zlib.h>
#include <thirdparty/msnumpress/MSNumpress.hpp>
#include <utils.hpp>
#include <msInterface/msScan.hpp>
#include <msInterface/internal/xml_utils.hpp>

namespace utils {
    namespace internal{

        uint64_t _dtohl(uint64_t l, bool bigEndian);
        unsigned long _dtohl(uint32_t l, bool bigEndian);
        int _b64_decode_mio( char *dest,  char *src, size_t size );
        void _decode32(msInterface::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian = true);
        void _decode64(msInterface::Scan& scan, const char* pData, size_t dataSize, size_t peaksCount, bool bigEndian = true);
        void _decompress32(msInterface::Scan& scan,
                           const std::string& data,
                           size_t peaksCount,
                           unsigned long compressedLen,
                           bool bigEndian = true);
        void _decompress64(msInterface::Scan& scan,
                           const std::string& data,
                           size_t peaksCount,
                           unsigned long compressedLen,
                           bool bigEndian = true);

        class BinaryData{
        public:
            enum class DataType{FLOAT_32, FLOAT_64};
        private:
            std::string data;
            size_t peaksCount;
            unsigned long compressedLen;
            bool bigEndian;
            DataType dataType;

            //compression types
            bool zlib;
            bool numpressLinear;
            bool numpressSlof;
            bool numpressPic;
        public:
            BinaryData(){
                zlib = false;
                numpressLinear = false;
                numpressSlof = false;
                numpressPic = false;
                data = "";
                peaksCount = 0;
                compressedLen = 0;
                bigEndian = true;
                dataType = DataType::FLOAT_32;
            }

            void processBinaryArray(std::vector<double>&, rapidxml::xml_node<>*);
            void decode(std::vector<double>&) const;
            void clear();

            // setters
            void setZlib(bool zlib);
            void setNumpressLinear(bool numpressLinear);
            void setNumpressSlof(bool numpressSlof);
            void setNumpressPic(bool numpressPic);
            void setData(const std::string &data);
            void setPeaksCount(size_t peaksCount);
            void setCompressedLen(unsigned long compressedLen);
            void setBigEndian(bool bigEndian);
            void setDataType(DataType dataType);

            // getters
            bool isZlib() const;
            bool isNumpressLinear() const;
            bool isNumpressSlof() const;
            bool isNumpressPic() const;
            const std::string &getData() const;
            size_t getPeaksCount() const;
            unsigned long getCompressedLen() const;
            bool isBigEndian() const;
            DataType getDataType() const;
        };
    }
}

#endif
