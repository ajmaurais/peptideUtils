//
// mzMLFile.cpp
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

#include <msInterface/mzMLFile.hpp>

using namespace utils;

std::string msInterface::MzMLFile::_parseScan(const std::string& _idLine) const {
    std::string newID = "";
    size_t scanStart = _idLine.find("scan");
    std::string scanStr = trim(_idLine.substr(scanStart + 4));
    if(scanStart == std::string::npos || scanStr.empty() || scanStr[0] != '=')
        throw InvalidXmlFile("Invalid spectrum ID: " + scanStr);
    for(size_t j = 1; j < scanStr.size(); j++){
        if(isspace(scanStr[j]))
            break;
        newID += scanStr[j];
    }
    return newID;
}

void msInterface::MzMLFile::_buildIndex()
{
    //Check the file type
    if(fileType != FileType::MZML)
        throw FileIOError("Incorrect file type for file: " + _fname);

    // Get indices of beginning and end of each scan
    std::vector<size_t> beginScans;
    std::vector<size_t> endScans;
    std::vector<size_t> beginRuns;
    beginScans.clear();
    endScans.clear();
    beginRuns.clear();
    getIdxOfSubstr(_buffer, "<spectrum ", beginScans);
    getIdxOfSubstr(_buffer, "</spectrum>", endScans);
    getIdxOfSubstr(_buffer, "<run", beginRuns);
    if(beginRuns.size() > 1)
        throw FileIOError("\n\tMore than 1 sample run found in:\n\t" +
                          _fname + "\n\tOnly a single run per file is supported.");

    // validate spectrum indices
    if(beginScans.size() > endScans.size())
       throw InvalidXmlFile("Unbounded <spectrum> in file: " + _fname);
    size_t len = beginScans.size();

    _scanCount = 0;
    std::string newID;
    std::string idLine;
    for(size_t i = 0; i < len; i++)
    {
        // Get the attributes in the <scan> node
        char* c = _buffer + beginScans[i];
        char* num = strstr(c, "id=\"");
        char* endNode = strchr(c, '>');
        if(num >= endNode)
            throw InvalidXmlFile("Not able to find required attribute \'num\' in <spectrum>");

        // Find the "num" attribute value
        idLine = "";
        for(char* it = num + 4; it < endNode; it++) {
            if(*it == '\"')
                break;
           idLine += *it;
        }
        newID = _parseScan(idLine);
        try {
            _scanMap[std::stoi(newID)] = _scanCount;
        } catch(std::invalid_argument& e){
            throw InvalidXmlFile("Invalid spectrum ID: " + newID);
        }
        _offsetIndex.push_back(IntPair(beginScans[i], endScans[i]));
        _scanCount ++;
    }
    firstScan = _scanMap.begin()->first;
    lastScan = _scanMap.rbegin()->first;
    assert(firstScan <= lastScan);
}

/**
 \brief Get parsed msInterface::Spectrum from mzML file.

 \param queryScan scan number to search for
 \param scan empty msInterface::Spectrum to load scan into
 \return false if \p queryScan not found, true if successful
 */
bool msInterface::MzMLFile::getScan(size_t queryScan, msInterface::Scan& scan) const
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
    size_t copyLen = (endOfScan + 11) - scanOffset;
    std::string scan_s = std::string(_buffer + scanOffset, copyLen);
    char* c = strcpy(new char[scan_s.length() + 1], scan_s.c_str());
    rapidxml::xml_document <> doc;
    doc.parse<0>(c);
    rapidxml::xml_node<> *root = doc.first_node();

    //get scan number and make sure it matches queryScan
    std::string idLine = _parseScan(internal::_getAttrValStr("id", root));
    size_t scanNum = std::stoul(idLine);
    if(scanNum != queryScan) throw InvalidXmlFile("queryScan number and actual scan number do not match!");
    scan.setScanNum(scanNum);

    //First parse the cvParams at the top of the scan
    for(auto *child = internal::_getFirstChildNode("cvParam", root); child; child = child->next_sibling("cvParam")) {
        std::string accession = internal::_getAttrValStr("accession", child);
        if(accession == "MS:1000511" ) //ms level
            scan.setLevel(internal::_getAttrValInt("value", child));
        else if(accession == "MS:1000130") //positive mode scan
            scan.setPolarity(Polarity::POSITIVE);
        else if(accession == "MS:1000129") //negative mode scan
            scan.setPolarity(Polarity::NEGATIVE);
        else if(accession == "MS:1001581") // Ion mobility CV
        {
            scan.setIMCV(internal::_getAttrValdouble("value", child));
            scan.setIsIonMobilityScan(true);
        }
        else if(accession == "MS:1000128") //profile scan
            throw InvalidXmlFile("Profile spectra are not supported!");
    }

    //get RT
    auto* scanList = root->first_node("scanList");
    if(scanList){
        for(auto* cvParam = internal::_getFirstChildNode("scan", scanList)->first_node("cvParam");
            cvParam; cvParam = cvParam->next_sibling("cvParam")){
                std::string accession = internal::_getAttrValStr("accession", cvParam);
                if(accession == "MS:1000016") // Retention time
                    scan.getPrecursor().setRT(internal::_obo_to_seconds(internal::_getAttrValdouble("value", cvParam),
                                                                        internal::_getAttrValStr("unitAccession", cvParam)));
                else if(accession == "MS:1000927") // Ion injection time
                    scan.setIonInjectionTime(internal::_obo_to_seconds(internal::_getAttrValdouble("value", cvParam),
                                                                       internal::_getAttrValStr("unitAccession", cvParam)) * 1e3);
                else if(accession == "MS:1001581") // Ion mobility CV
                {
                    scan.setIMCV(internal::_getAttrValdouble("value", cvParam));
                    scan.setIsIonMobilityScan(true);
                }
        }
    }

    //Find the precursorList node
    auto* precursorNode = root->first_node("precursorList");
    if(precursorNode) {
        // precursor scan
        precursorNode = internal::_getFirstChildNode("precursor", precursorNode);
        std::string precursorScanName;
        try {
            precursorScanName = internal::_getAttrValStr("spectrumRef", precursorNode);
        } catch (utils::InvalidXmlFile) {
            precursorScanName = "";
        }
        // auto* spectruRef = precursorNode->first_attribute("spectrumRef");
        scan.getPrecursor().setScan(_parseScan(precursorScanName));

        //selectedIon
        for(auto* iter = internal::_getFirstChildNode("cvParam",
                                                      internal::_getFirstChildNode("selectedIon",
                                                      internal::_getFirstChildNode("selectedIonList", precursorNode)));
            iter; iter = iter->next_sibling("cvParam")) {
            std::string accession = internal::_getAttrValStr("accession", iter);
            if(accession == "MS:1000744") //precursor m/z
                scan.getPrecursor().setMZ(internal::_getAttrValStr("value", iter));
            else if(accession == "MS:1000041") //precursor charge
                scan.getPrecursor().setCharge(internal::_getAttrValInt("value", iter));
            else if(accession == "MS:1000042") //precursor intensity
                scan.getPrecursor().setIntensity(internal::_getAttrValdouble("value", iter));
        }

        //activation method
        msInterface::ActivationMethod am = ActivationMethod::UNKNOWN;
        for(auto* iter = internal::_getFirstChildNode("cvParam", internal::_getFirstChildNode("activation", precursorNode));
            iter; iter = iter->next_sibling("cvParam")){
                am = msInterface::oboToActivation(internal::_getAttrValStr("accession", iter));
                if(am != ActivationMethod::UNKNOWN) break;
        }
        scan.getPrecursor().setActivationMethod(am);
    }

    //decode scan ions
    auto* binaryDataArrayNode = internal::_getFirstChildNode("binaryDataArrayList", root);
    std::vector<double> mzArray, intensityArray;
    bool parsed_mz = false, parsed_intensity = false;
    internal::BinaryData binaryParser;
    size_t defaultArrayLength = internal::_getAttrValInt("defaultArrayLength", root);
    binaryParser.setPeaksCount(defaultArrayLength);
    for(auto* node = binaryDataArrayNode->first_node("binaryDataArray");
        node; node = node->next_sibling("binaryDataArray")) {
        for(auto* cvParam = node->first_node("cvParam"); cvParam; cvParam = cvParam->next_sibling("cvParam")){
            std::string accession = internal::_getAttrValStr("accession", cvParam);
            if(accession == "MS:1000514") { // m/z array
                binaryParser.processBinaryArray(mzArray, node);
                parsed_mz = true;
                break;
            }
            else if(accession == "MS:1000515") { // intensity array
                binaryParser.processBinaryArray(intensityArray, node);
                parsed_intensity = true;
                break;
            }
        }
    }
    size_t len = mzArray.size();
    if(len > 0) {
        if(!(parsed_mz && parsed_intensity))
            throw InvalidXmlFile("ERROR In scan: " + idLine +"\n\tNever found array(s) for: " +
                                 (parsed_mz ? "" : "m/z") + (!parsed_intensity && !parsed_mz ? " or " : "") +
                                 (parsed_intensity ? "" : "intensity"));
        if(len != intensityArray.size())
            throw InvalidXmlFile("ERROR In scan: " + idLine +"\n\tMZ and intensity lengths do not match! m/z: " +
                                  std::to_string(len) + ", int: " + std::to_string(intensityArray.size()));
        for(size_t i = 0; i < len; i++)
            scan.add(mzArray[i], intensityArray[i]);
    }

    scan.updateRanges();
    delete [] c;
    return true;
}

