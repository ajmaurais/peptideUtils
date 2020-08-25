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

const std::string utils::MzXMLFile::_scanBegin = "<scan";
const std::string utils::MzXMLFile::_scanEnd = "</scan>";

void utils::MzXMLFile::_buildIndex()
{
    // Get indices of beginning and end of each scan
    std::vector<size_t> beginScans, endScans;
    utils::getIdxOfSubstr(_buffer, _scanBegin.c_str(), beginScans);
    utils::getIdxOfSubstr(_buffer, _scanEnd.c_str(), endScans);

    //validate scan indices
    if(beginScans.size() != endScans.size())
        throw utils::InvalidXmlFile();
        //throw utils::InvalidXmlFile("Unbounded <scan>");
    size_t len = beginScans.size();
    for(size_t i = 0; i < len; i++)
        if(beginScans[i] >= endScans[i])
            throw utils::InvalidXmlFile();

    _scanCount = 0;
    std::string newID;
    std::regex rgx("num\\s?=\\s?\"([\\w\\d.+-]+)\"");
    std::smatch match;
    for(size_t i = 0; i < len; i++)
    {
        // Get the attributes in the <scan> node
        char* c = _buffer + beginScans[i];
        char* num = strstr(c, "num=\"");
        char* endNode = strchr(c, '>');
        if(num >= endNode)
            throw utils::InvalidXmlFile();

        // Find the "num" attribute
        const std::string tmp(c, endNode);
        if(!std::regex_search(tmp.begin(), tmp.end(), match, rgx)){
            throw utils::InvalidXmlFile();
        }
        newID = match[1];

        _scanMap[std::stoi(newID)] = _scanCount;
        _offsetIndex.push_back(IntPair(beginScans[i], endScans[i]));
        _scanCount ++;
    }
    firstScan = _scanMap.begin()->first;
    lastScan = _scanMap.rbegin()->first;
    assert(firstScan <= lastScan);
}

bool utils::MzXMLFile::getScan(size_t queryScan, utils::Scan& scan) const
{
    scan.clear();
    scan.getPrecursor().setSample(_parentFileBase);
    scan.getPrecursor().setFile(_fname);
    if(!((queryScan >= firstScan) && (queryScan <= lastScan))){
        std::cerr << "queryScan not in file scan range!" << NEW_LINE;
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

    std::vector<std::string> elems;
    std::string line;
    size_t numIons = 0;

    // Copy <scan> ... </scan> and initialize xml parser
    size_t copyLen = (endOfScan + _scanEnd.length()) - scanOffset;
    char* c = strncpy(new char[copyLen], _buffer + scanOffset, copyLen);
    rapidxml::xml_document <> doc;
    doc.parse<0>(c);

    //parser flags
    std::string precision = "";
    std::string byteOrder = "";
    size_t peaksCount = 0;

    // parse scan attributes
    rapidxml::xml_node<> *node = doc.first_node();
    for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()){
        if(utils::_isAttr("retentionTime", attr->name()))
            scan.getPrecursor().setRT(utils::_xs_duration_to_seconds(attr->value(), attr->value_size()));
        else if(utils::_isAttr("num", attr->name()))
            scan.setScanNum(std::stol(attr->value()));
        else if(utils::_isAttr("peaksCount", attr->name()))
            peaksCount = std::stol(attr->value());
        else if(utils::_isAttr("msLevel", attr->name()))
            scan.setLevel(std::stoi(attr->value()));
    }
    //iterate through scan child nodes
    for(node = node->first_node(); node; node = node->next_sibling()) {
        if(utils::_isVal(node->name(), "precursorMz")){
            for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute()) {
                if(utils::_isAttr("precursorIntensity", attr->name()))
                    scan.getPrecursor().setIntensity(std::stod(attr->value()));
                else if(utils::_isAttr("precursorCharge", attr->name()))
                    scan.getPrecursor().setCharge(std::stoi(attr->value()));
                else if(utils::_isAttr("activationMethod", attr->name()))
                    scan.getPrecursor().setActivationMethod(std::string(attr->value()));
            }
            scan.getPrecursor().setMZ(std::string(node->value()));
        }
        else if(utils::_isVal(node->name(), "peaks"))
        {
            // peak attributes
            for(auto *attr = node->first_attribute(); attr; attr = attr->next_attribute())
            {
                if(utils::_isAttr("precision", attr->name()))
                    precision = std::string(attr->value(), attr->value_size());
                else if(utils::_isAttr("byteOrder", attr->name()))
                    byteOrder = std::string(attr->value(), attr->value_size());
                else if(!utils::_checkAttrVal("contentType", "m/z-int", attr, queryScan)) return false;
                else if(!utils::_checkAttrVal("compressionType", "none", attr, queryScan)) return false;
            }

            //retrieve peak list
            if(precision == "32")
                utils::_decode32(scan, node->value(), node->value_size(), peaksCount, byteOrder == "network");
            else if(precision == "64")
                utils::_decode64(scan, node->value(), node->value_size(), peaksCount, byteOrder == "network");
            else {
                if(precision.empty())
                    std::cerr << "Missing value for precision";
                else std::cerr << "Invalid value for precision: " << precision << ",";
                std::cerr << " in scan: " << queryScan << NEW_LINE;
                return false;
            }
        }
    }

    delete [] c;
    scan.updateRanges();

    return true;
}
