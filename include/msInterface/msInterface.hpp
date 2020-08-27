//
// msFileBase.hpp
// utils
// -----------------------------------------------------------------------------
// Copyright 2020 Aaron Maurais
// -----------------------------------------------------------------------------
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

#ifndef msFileBase_hpp
#define msFileBase_hpp

#include <map>
#include <vector>

#include <bufferFile.hpp>
#include <msInterface/msScan.hpp>

namespace utils {

    class MsInterface;

    size_t const SCAN_INDEX_NOT_FOUND = std::string::npos;

    class MsInterface: public utils::BufferFile{
    public:
        enum class FileType{MS2, MZXML, MZML, UNKNOWN};

    protected:
        typedef std::pair<size_t, size_t> IntPair;
        typedef std::vector<IntPair> OffsetIndexType;

        //!Parent file type
        FileType fileType;

        //!Stores pairs of offset values for scans
        OffsetIndexType _offsetIndex;
        //!Maps scan numbers to indecies in _offsetIndex
        std::map<size_t, size_t> _scanMap;
        //!Actual number of scans read from file
        size_t _scanCount;

        //metadata
        // Maybe later this will expand. For now this is all I need.
        std::string _parentFileBase;
        size_t firstScan, lastScan;

        virtual void _buildIndex() = 0;
        void copyMetadata(const MsInterface& rhs);
        void initMetadata();
        size_t _getScanIndex(size_t) const;

    public:
        //! Default constructor
        explicit MsInterface(std::string fname = "");
        //!copy constructor
        MsInterface(const MsInterface& rhs);
        //!copy assignment
        MsInterface& operator = (const MsInterface& rhs);
        ~MsInterface(){}

        void calcParentFileBase(std::string path);

        virtual bool read(std::string) override;
        virtual bool read();
        virtual bool getScan(size_t, utils::Scan&) const = 0;
        bool getScan(std::string, utils::Scan&) const;

        //metadata getters
        size_t getScanCount() const{
            return _scanCount;
        }
        size_t getLastScan() const {
            return lastScan;
        }
        size_t getFirstScan() const {
            return firstScan;
        }
        std::string getParentFileBase() const{
            return _parentFileBase;
        }
        size_t nextScan(size_t i) const;

        static FileType getFileType(std::string fname);
    };
}

#endif
