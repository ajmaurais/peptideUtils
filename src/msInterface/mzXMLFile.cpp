//
// mzXMLFile.cpp
// utils
// -----------------------------------------------------------------------------
// MIT License
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

#include <msInterface/mzXMLFile.hpp>

using namespace utils;

void msInterface::MzXMLFile::_buildIndex()
{
    //Check the file type
    if(fileType != FileType::MZXML)
        throw utils::FileIOError("Incorrect file type for file: " + _fname);

    // Get indices of beginning and end of each scan
    std::vector<size_t> beginScans, endScans;
    utils::getIdxOfSubstr(_buffer, "<scan", beginScans);
    utils::getIdxOfSubstr(_buffer, "</scan>", endScans);

    //validate scan indices
    if(beginScans.size() != endScans.size())
        throw utils::InvalidXmlFile("Unbounded <scan> in file: " + _fname);
    size_t len = beginScans.size();
    for(size_t i = 0; i < len; i++)
        if(beginScans[i] >= endScans[i])
            throw utils::InvalidXmlFile("Unbounded <scan> in file: " + _fname);

    _scanCount = 0;
    for(size_t i = 0; i < len; i++)
    {
        // Get the attributes in the <scan> node
        char* c = _buffer + beginScans[i];
        char* num = strstr(c, "num");
        char* endNode = strchr(c, '>');
        if(num >= endNode)
            throw utils::InvalidXmlFile("Not able to find required attribute \'num\' in <scan>");

        // Find the "num" attribute value
        // Yes I know that it would be much easier to do this with a regex.
        // I am choosing not to use a regex because this way is much faster.
        char* beginNum = nullptr;
        char* endNum = nullptr;
        for(char* it = num + 3; it < endNode; ++it) {
            if(isspace(*it) || *it == '=') continue;
            if(*it == '\"'){ //We found the beginning of the number
                ++it;
                beginNum = it;
                endNum = it;
                for(; it < endNode; ++it){
                    if(*it == '\"') break;
                    if(isdigit(*it)) ++endNum;
                    else throw utils::InvalidXmlFile("Invalid scan header: " + std::string(c, endNode));
                }
            } //end if
            break;
        } //end for it

        if(beginNum == endNum)
            throw utils::InvalidXmlFile("Not able to find required attribute \'num\' in <scan>");
        try {
            _scanMap[std::stoi(std::string(beginNum, endNum))] = _scanCount;
        } catch(std::invalid_argument& e){
            throw utils::InvalidXmlFile("Invalid spectrum ID: " + std::string(beginNum, endNum));
        }
        _offsetIndex.push_back(IntPair(beginScans[i], endScans[i]));
        _scanCount ++;
    }//end for i
    firstScan = _scanMap.begin()->first;
    lastScan = _scanMap.rbegin()->first;
    assert(firstScan <= lastScan);
}

/**
 \brief Get parsed utils::msInterface::Spectrum from mzXML file.

 \param queryScan scan number to search for
 \param scan empty utils::msInterface::Spectrum to load scan into
 \return false if \p queryScan not found, true if successful
 */
bool msInterface::MzXMLFile::getScan(size_t queryScan, msInterface::Scan& scan) const
{
    scan.clear();
    scan.getPrecursor().setSample(_parentFileBase);
    scan.getPrecursor().setFile(_fname);
    if(!((queryScan >= firstScan) && (queryScan <= lastScan))){
        std::cerr << "queryScan: " << queryScan << " not in file scan range!" << NEW_LINE;
        return false;
    }

    size_t scanOffset, endOfScan;
    size_t scanIndex = _getScanIndex(queryScan);
    if(scanIndex == SCAN_INDEX_NOT_FOUND){
        std::cerr << "queryScan: " << queryScan << ", could not be found in: " << _fname << NEW_LINE;
        return false;
    }
    scanOffset = _offsetIndex[scanIndex].first;
    endOfScan = _offsetIndex[scanIndex].second;

    // Copy <scan> ... </scan> and initialize xml parser
    size_t copyLen = (endOfScan + 7) - scanOffset;
    std::string scan_s = std::string(_buffer + scanOffset, copyLen);
    char* c = strcpy(new char[scan_s.length() + 1], scan_s.c_str());
    rapidxml::xml_document <> doc;
    doc.parse<0>(c);

    //parser flags
    std::string precision = "";
    std::string byteOrder = "";
    std::string compressionType = "none";
    unsigned long compressedLen = 0;
    bool compressedLenSet = false;
    size_t peaksCount = 0;

    // parse scan attributes
    rapidxml::xml_node<> *node = doc.first_node();
    for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()){
        if(utils::internal::_isAttr("retentionTime", attr->name()))
            scan.getPrecursor().setRT(utils::internal::_xs_duration_to_seconds(attr->value(), attr->value_size()));
        else if(utils::internal::_isAttr("num", attr->name()))
            scan.setScanNum(std::stol(attr->value()));
        else if(utils::internal::_isAttr("peaksCount", attr->name()))
            peaksCount = std::stol(attr->value());
        else if(utils::internal::_isAttr("msLevel", attr->name()))
            scan.setLevel(std::stoi(attr->value()));
        else if(utils::internal::_isAttr("polarity", attr->name())) {
            char polarity = *attr->value();
            Polarity setPolarity = Polarity::UNKNOWN;
            if(polarity == '+')
                setPolarity = Polarity::POSITIVE;
            else if(polarity == '-')
                setPolarity = Polarity::NEGATIVE;
            scan.setPolarity(setPolarity);
        }
    }
    //iterate through scan child nodes
    for(node = node->first_node(); node; node = node->next_sibling()) {
        if(utils::internal::_isVal(node->name(), "precursorMz")){
            for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
                if(utils::internal::_isAttr("precursorIntensity", attr->name()))
                    scan.getPrecursor().setIntensity(std::stod(attr->value()));
                else if(utils::internal::_isAttr("precursorCharge", attr->name()))
                    scan.getPrecursor().setCharge(std::stoi(attr->value()));
                else if(utils::internal::_isAttr("activationMethod", attr->name()))
                    scan.getPrecursor().setActivationMethod(msInterface::strToActivation(std::string(attr->value())));
            }
            scan.getPrecursor().setMZ(std::string(node->value()));
        }
        else if(utils::internal::_isVal(node->name(), "peaks"))
        {
            // peak attributes
            for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute())
            {
                if(utils::internal::_isAttr("precision", attr->name()))
                    precision = std::string(attr->value(), attr->value_size());
                else if(utils::internal::_isAttr("byteOrder", attr->name()))
                    byteOrder = std::string(attr->value(), attr->value_size());
                else if(utils::internal::_isAttr("compressionType", attr->name()))
                    compressionType = std::string(attr->value(), attr->value_size());
                else if(utils::internal::_isAttr("compressedLen", attr->name())) {
                    compressedLen = std::stoul(attr->value());
                    compressedLenSet = true;
                }
                else if(!utils::internal::_checkAttrVal("contentType", "m/z-int", attr, queryScan))
                    return false;
            }

            //retrieve peak list
            if(precision == "32" && compressionType == "none")
                utils::internal::_decode32(scan,
                                 node->value(),
                                 node->value_size(),
                                 peaksCount,
                                 byteOrder == "network");
            else if(precision == "64" && compressionType == "none")
                utils::internal::_decode64(scan,
                                 node->value(),
                                 node->value_size(),
                                 peaksCount,
                                 byteOrder == "network");
            else if(precision == "32" && compressionType == "zlib" && compressedLen != 0)
                utils::internal::_decompress32(scan,
                                     std::string(node->value(), node->value_size()),
                                     peaksCount,
                                     compressedLen,
                                     byteOrder == "network");
            else if(precision == "64" && compressionType == "zlib" && compressedLen != 0)
                utils::internal::_decompress64(scan,
                                     std::string(node->value(), node->value_size()),
                                     peaksCount,
                                     compressedLen,
                                     byteOrder == "network");
            else {
                std::cerr << "ERROR: In scan: " << queryScan << NEW_LINE;
                if(precision.empty())
                    std::cerr << "\tMissing value for precision";
                else if(precision != "32" && precision != "64")
                    std::cerr << "\tInvalid value for precision: " << precision << NEW_LINE;
                if(compressionType != "none" && compressionType != "zlib")
                    std::cerr << "\tUnsupported compression type: " << compressionType << NEW_LINE;
                if(compressionType != "none" && !compressedLenSet)
                    std::cerr << "\tRequired value: \'compressedLen\' not found!" << NEW_LINE;
                return false;
            }
        }
    }
    delete [] c;
    scan.updateRanges();
    return true;
}
