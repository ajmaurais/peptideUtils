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

#include <msInterface/msInterface.hpp>

using namespace utils;

//! Copy constructor
msInterface::MsInterface::MsInterface(const msInterface::MsInterface &rhs) : BufferFile(rhs){
    copyMetadata(rhs);
    _offsetIndex = rhs._offsetIndex;
    _scanMap = rhs._scanMap;
    _scanCount = rhs._scanCount;
    fileType = rhs.fileType;
}

//! Default constructor
msInterface::MsInterface::MsInterface(std::string fname) : BufferFile(fname) {
    initMetadata();
    _offsetIndex = OffsetIndexType();
    _scanMap = std::map<size_t, size_t>();
    fileType = FileType::UNKNOWN;
}

//! Copy assignment
msInterface::MsInterface &msInterface::MsInterface::operator=(const msInterface::MsInterface& rhs) {
    BufferFile::operator=(rhs);
    copyMetadata(rhs);
    return *this;
}

void msInterface::MsInterface::clear(){
    _offsetIndex.clear();
    _scanMap.clear();
    initMetadata();
}

bool msInterface::MsInterface::read(std::string fname) {
    _fname = fname;
    return MsInterface::read();
}

bool msInterface::MsInterface::read()
{
    clear(); // clear all data

    // Determine file type
    FileType temp = getFileType(_fname);
    if(fileType != FileType::UNKNOWN && temp != fileType) // Do this check in case of reuse of a MsInterface instance
        throw std::runtime_error("Attempting to read a different file type!");
    fileType = temp;

    calcParentFileBase(_fname);
    if(!BufferFile::read(_fname)) return false;
    _buildIndex();
    if(_scanCount == 0)
        std::cerr << "WARN: no scans found in " << _fname << NEW_LINE;
    return true;
}

void msInterface::MsInterface::copyMetadata(const msInterface::MsInterface &rhs) {
    _parentFileBase = rhs._parentFileBase;
    firstScan = rhs.firstScan;
    lastScan = rhs.lastScan;
    _scanCount = rhs._scanCount;
}

void msInterface::MsInterface::initMetadata() {
    _parentFileBase = "";
    firstScan = 0;
    lastScan = 0;
    _scanCount = 0;
}

void msInterface::MsInterface::calcParentFileBase(std::string path) {
    _parentFileBase = utils::baseName(utils::removeExtension(path));
}

msInterface::MsInterface::FileType msInterface::MsInterface::getFileType(std::string fname)
{
    std::string ext = utils::toLower(utils::getExtension(fname));
    if(ext == "ms2")
        return FileType::MS2;
    else if(ext == "mzxml")
        return FileType::MZXML;
    else if(ext == "mzml")
        return FileType::MZML;
    else return FileType::UNKNOWN;
}

/**
 \brief Get index for scan in Ms2File::_offsetIndex. <br>

 If \p scan is not found, msInterface::SCAN_INDEX_NOT_FOUND is returned.

 \param scan Scan number to search for.
 \return Index for \p scan.
 */
size_t msInterface::MsInterface::_getScanIndex(size_t scan) const{
    auto it = _scanMap.find(scan);
    if(it == _scanMap.end())
        return SCAN_INDEX_NOT_FOUND;
    return it->second;
}

/**
 \brief Overloaded function with \p queryScan as string
 */
bool msInterface::MsInterface::getScan(std::string queryScan, Scan& scan) const{
    return getScan(std::stoi(queryScan), scan);
}

/**
 * Get the scan number of the next scan.
 * @param i Current scan.
 * @return Scan number of the next scan.
 * @throws std::out_of_range if scan \p i does not exist or if a scan after i does not exist.
 */
size_t msInterface::MsInterface::nextScan(size_t i) const {
    auto curScan = _scanMap.find(i);
    if(curScan == _scanMap.end() || (++curScan) == _scanMap.end())
        throw std::out_of_range("Scan " + std::to_string(i) + " out of range.");
    return curScan->first;
}

/**
 * Get the scan number of the previous scan.
 * @param i Current scan.
 * @return Scan number of the previous scan.
 * @throws std::out_of_range if scan \p i does not exist or if a scan after i does not exist.
 */
size_t msInterface::MsInterface::prevScan(size_t i) const {
    auto curScan = _scanMap.find(i);
    if(curScan == _scanMap.end() || curScan == _scanMap.begin())
        throw std::out_of_range("Scan " + std::to_string(i) + " out of range.");
    return (++curScan)->first;
}
