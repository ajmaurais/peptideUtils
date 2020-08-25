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

utils::MsInterface::MsInterface(const utils::MsInterface &rhs) : BufferFile(rhs){
    copyMetadata(rhs);
    _offsetIndex = rhs._offsetIndex;
    _scanMap = rhs._scanMap;
    _scanCount = rhs._scanCount;
    fileType = rhs.fileType;
}

utils::MsInterface::MsInterface(std::string fname) : BufferFile(fname) {
    initMetadata();
    _offsetIndex = OffsetIndexType();
    _scanMap = std::map<size_t, size_t>();
    _scanCount = 0;
    fileType = FileType::UNKNOWN;
}

utils::MsInterface &utils::MsInterface::operator=(const utils::MsInterface& rhs) {
    BufferFile::operator=(rhs);
    copyMetadata(rhs);
    return *this;
}

bool utils::MsInterface::read(std::string fname) {
    _fname = fname;
    return read();
}

bool utils::MsInterface::read(){
    calcParentFileBase(_fname);
    if(!BufferFile::read(_fname)) return false;
    _buildIndex();
    return true;
}

void utils::MsInterface::copyMetadata(const utils::MsInterface &rhs) {
    _parentFileBase = rhs._parentFileBase;
    firstScan = rhs.firstScan;
    lastScan = rhs.lastScan;
}

void utils::MsInterface::initMetadata() {
    _parentFileBase = "";
    firstScan = 0;
    lastScan = 0;
}

void utils::MsInterface::calcParentFileBase(std::string path) {
    _parentFileBase = utils::baseName(utils::removeExtension(path));
}

utils::MsInterface::FileType utils::MsInterface::getFileType(std::string fname)
{
    std::string ext = utils::toLower(utils::getExtension(fname));
    if(ext == "ms2")
        return utils::MsInterface::FileType::MS2;
    else if(ext == "mzxml")
        return utils::MsInterface::FileType::MZXML;
    else if(ext == "mzml")
        return utils::MsInterface::FileType::MZML;
    else return utils::MsInterface::FileType::UNKNOWN;
}

/**
 \brief Get index for scan in Ms2File::_offsetIndex. <br>

 If \p scan is not found, utils::SCAN_INDEX_NOT_FOUND is returned.

 \param scan Scan number to search for.
 \return Index for \p scan.
 */
size_t utils::MsInterface::_getScanIndex(size_t scan) const{
    auto it = _scanMap.find(scan);
    if(it == _scanMap.end())
        return SCAN_INDEX_NOT_FOUND;
    return it->second;
}

/**
 \brief Overloaded function with \p queryScan as string
 */
bool utils::MsInterface::getScan(std::string queryScan, Scan& scan) const{
    return getScan(std::stoi(queryScan), scan);
}

