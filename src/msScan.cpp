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

#include <msScan.hpp>

utils::Scan& utils::Scan::operator=(const utils::Scan& rhs)\
{
    _maxInt = rhs._maxInt;
    _minInt = rhs._minInt;
    _minMZ = rhs._minMZ;
    _maxMZ = rhs._maxMZ;
    _mzRange = rhs._mzRange;
    precursorScan = rhs.precursorScan;
    _ions = rhs._ions;
    _scanNum = rhs._scanNum;
    return *this;
}

utils::Scan::Scan(const utils::Scan& rhs)
{
    _maxInt = rhs._maxInt;
    _minInt = rhs._minInt;
    _minMZ = rhs._minMZ;
    _maxMZ = rhs._maxMZ;
    _mzRange = rhs._mzRange;
    precursorScan = rhs.precursorScan;
    _ions = rhs._ions;
    _scanNum = rhs._scanNum;
}

void utils::PrecursorScan::clear()
{
    _mz.clear();
    _scan.clear();
    _rt = 0;
    _file.clear();
    _charge = 0;
    _intensity = 0;
}

void utils::Scan::clear()
{
    _maxInt = 0;
    _minInt = 0;
    _minMZ = 0;
    _maxMZ = 0;
    _mzRange = 0;
    _ions.clear();
}

void utils::Scan::setMinMZ(ScanMZ minMZ) {
    _minMZ = minMZ;
    _mzRange = (_maxMZ - _minMZ);
}
void utils::Scan::setMaxMZ(ScanMZ maxMZ) {
    _maxMZ = maxMZ;
    _mzRange = (_maxMZ - _minMZ);
}

//! Updates minimum and maximum mz and intensity.
void utils::Scan::updateRanges()
{
    if(!_ions.empty()) {
        _minMZ = _ions.begin()->getMZ();
        _minInt = _ions.begin()->getIntensity();

        for (auto it = _ions.begin(); it != _ions.end(); ++it) {
            if (it->getMZ() < _minMZ)
                _minMZ = it->getMZ();
            if (it->getMZ() > _maxMZ)
                _maxMZ = it->getMZ();
            if (it->getIntensity() < _minInt)
                _minInt = it->getIntensity();
            if (it->getIntensity() > _maxInt)
                _maxInt = it->getIntensity();
        }
        _mzRange = (_maxMZ - _minMZ);
    }
}

void utils::Scan::add(const ScanIon& ion) {
    _ions.push_back(ion);
}

void utils::Scan::add(ScanMZ mz, ScanIntensity intensity) {
    _ions.emplace_back(mz, intensity);
}
