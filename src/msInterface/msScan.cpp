//
// msScan.cpp
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

#include <msInterface/msScan.hpp>

using namespace utils;

msInterface::Scan& msInterface::Scan::operator=(const msInterface::Scan& rhs) {
    _maxInt = rhs._maxInt;
    _minInt = rhs._minInt;
    _minMZ = rhs._minMZ;
    _maxMZ = rhs._maxMZ;
    _mzRange = rhs._mzRange;
    precursorScan = rhs.precursorScan;
    _ions = rhs._ions;
    _scanNum = rhs._scanNum;
    _level = rhs._level;
    _polarity = rhs._polarity;
    return *this;
}

msInterface::Scan::Scan(const msInterface::Scan& rhs) {
    _maxInt = rhs._maxInt;
    _minInt = rhs._minInt;
    _minMZ = rhs._minMZ;
    _maxMZ = rhs._maxMZ;
    _mzRange = rhs._mzRange;
    precursorScan = rhs.precursorScan;
    _ions = rhs._ions;
    _scanNum = rhs._scanNum;
    _level = rhs._level;
    _polarity = rhs._polarity;
}

void msInterface::PrecursorScan::clear() {
    _mz.clear();
    _scan.clear();
    _rt = 0;
    _file.clear();
    _charge = 0;
    _intensity = 0;
}

bool msInterface::PrecursorScan::operator==(const msInterface::PrecursorScan &rhs) const {
    if(!(_scan == rhs._scan &&
         _rt == rhs._rt &&
         _file == rhs._file &&
         _sample == rhs._sample &&
         _charge == rhs._charge &&
         _activationMethod == rhs._activationMethod))
        return false;
    return true;
}

bool msInterface::PrecursorScan::almostEqual(const msInterface::PrecursorScan &rhs, double epsilon) const {
    if(!(_scan == rhs._scan &&
         _file == rhs._file &&
         _sample == rhs._sample &&
         _charge == rhs._charge &&
         _activationMethod == rhs._activationMethod))
        return false;

    if(!(utils::almostEqual(_rt, rhs._rt, epsilon)))
        return false;

    return true;
}

void msInterface::Scan::clear() {
    _maxInt = 0;
    _minInt = 0;
    _minMZ = 0;
    _maxMZ = 0;
    _mzRange = 0;
    _level = 0;
    _polarity = Polarity::UNKNOWN;
    _scanNum = std::string::npos;
    precursorScan.clear();
    _ions.clear();
}

void msInterface::Scan::setMinMZ(ScanMZ minMZ) {
    _minMZ = minMZ;
    _mzRange = (_maxMZ - _minMZ);
}
void msInterface::Scan::setMaxMZ(ScanMZ maxMZ) {
    _maxMZ = maxMZ;
    _mzRange = (_maxMZ - _minMZ);
}

//! Updates minimum and maximum mz and intensity.
void msInterface::Scan::updateRanges(){
    if(!_ions.empty()) {
        _minMZ = _ions.begin()->getMZ();
        _minInt = _ions.begin()->getIntensity();
        _maxInt = 0;
        _maxMZ = 0;

        for(auto & _ion : _ions) {
            if (_ion.getMZ() < _minMZ)
                _minMZ = _ion.getMZ();
            if (_ion.getMZ() > _maxMZ)
                _maxMZ = _ion.getMZ();
            if (_ion.getIntensity() < _minInt)
                _minInt = _ion.getIntensity();
            if (_ion.getIntensity() > _maxInt)
                _maxInt = _ion.getIntensity();
        }
        _mzRange = (_maxMZ - _minMZ);
    }
}

void msInterface::Scan::add(const ScanIon& ion) {
    _ions.push_back(ion);
}

void msInterface::Scan::add(ScanMZ mz, ScanIntensity intensity) {
    _ions.emplace_back(mz, intensity);
}

bool msInterface::Scan::almostEqual(const msInterface::Scan &rhs, double epsilon) const {

    //first to equality comparisons
    if(!(_scanNum == rhs._scanNum &&
         _level == rhs._level &&
         _polarity == rhs._polarity))
        return false;

    //float comparisons
    if(!(precursorScan.almostEqual(rhs.precursorScan, epsilon) &&
         utils::almostEqual(_maxInt, rhs._maxInt, epsilon) &&
         utils::almostEqual(_minInt, rhs._minInt, epsilon) &&
         utils::almostEqual(_minMZ, rhs._minMZ, epsilon) &&
         utils::almostEqual(_maxMZ, rhs._maxMZ, epsilon) &&
         utils::almostEqual(_mzRange, rhs._mzRange, epsilon)))
        return false;

    size_t len = _ions.size();
    if(len != rhs._ions.size()) return false;
    for(size_t i = 0; i < len; i++) {
        if (!(_ions[i].almostEqual(rhs._ions[i], epsilon)))
            return false;
    }

    return true;
}

bool msInterface::Scan::operator==(const msInterface::Scan &rhs) const {
    if(!(_maxInt == rhs._maxInt &&
         _minInt == rhs._minInt &&
         _minMZ == rhs._minMZ &&
         _maxMZ == rhs._maxMZ &&
         _mzRange == rhs._mzRange &&
         precursorScan == rhs.precursorScan &&
         _scanNum == rhs._scanNum &&
         _level == rhs._level &&
         _polarity == rhs._polarity))
        return false;

    size_t len = _ions.size();
    if(len != rhs._ions.size()) return false;
    for(size_t i = 0; i < len; i++)
        if(_ions[i] != rhs._ions[i])
            return false;

    return true;
}

msInterface::ActivationMethod msInterface::oboToActivation(std::string obo) {
    if(obo == "MS:1000133") return ActivationMethod::CID;
    else if(obo == "MS:1000435") return ActivationMethod::MPD;
    else if(obo == "MS:1000250") return ActivationMethod::ECD;
    else if(obo == "MS:1000599") return ActivationMethod::PQD;
    else if(obo == "MS:1000598") return ActivationMethod::ETD;
    else if(obo == "MS:1000422") return ActivationMethod::HCD;
    else return ActivationMethod::UNKNOWN;
}

std::string msInterface::activationToOBO(msInterface::ActivationMethod activation) {
    std::string obo;
    switch (activation) {
        case ActivationMethod::CID:
            obo = "MS:1000133";
            break;
        case ActivationMethod::MPD:
            obo = "MS:1000435";
            break;
        case ActivationMethod::ECD:
            obo = "MS:1000250";
            break;
        case ActivationMethod::PQD:
            obo = "MS:1000599";
            break;
        case ActivationMethod::ETD:
            obo = "MS:1000598";
            break;
        case ActivationMethod::HCD:
            obo = "MS:1000422";
            break;
        case ActivationMethod::UNKNOWN:
        default:
            obo = "MS:9999999";
            break;
    }
    return obo;
}

//!Convert msInterface::ActivationMethod to 3 letter acronym as std::string.
std::string msInterface::activationToString(msInterface::ActivationMethod am){
    std::string ret;
    switch (am) {
        case ActivationMethod::CID:
            ret = "CID";
            break;
        case ActivationMethod::MPD:
            ret = "MPD";
            break;
        case ActivationMethod::ECD:
            ret = "ECD";
            break;
        case ActivationMethod::PQD:
            ret = "PQD";
            break;
        case ActivationMethod::ETD:
            ret = "ETD";
            break;
        case ActivationMethod::HCD:
            ret = "HCD";
            break;
        case ActivationMethod::UNKNOWN:
        default:
            ret = "UNKNOWN";
            break;
    }
    return ret;
}

//!Convert \p s to msInterface::ActivationMethod. \p s should be 3 letter acronym for method.
msInterface::ActivationMethod msInterface::strToActivation(const std::string& s){
    if(s == "CID") return ActivationMethod::CID;
    else if(s == "MPD") return ActivationMethod::MPD;
    else if(s == "ECD") return ActivationMethod::ECD;
    else if(s == "PQD") return ActivationMethod::PQD;
    else if(s == "ETD") return ActivationMethod::ETD;
    else if(s == "HCD") return ActivationMethod::HCD;
    else return ActivationMethod::UNKNOWN;
}

