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
    ScanMZ minMZ = 0;
    ScanMZ maxMZ = 0;
    ScanIntensity minInt = 0;
    ScanIntensity maxInt = 0;

    for(auto it = _ions.begin(); it != _ions.end(); ++it)
    {
        if(it == _ions.begin()){
            minMZ = it->getMZ();
            minInt = it->getIntensity();
        }

        if(it->getMZ() < minMZ)
            minMZ = it->getMZ();
        if(it->getMZ() > maxMZ)
            maxMZ = it->getMZ();
        if(it->getIntensity() < minInt)
            minInt = it->getIntensity();
        if(it->getIntensity() > maxInt)
            maxInt = it->getIntensity();
    }

    _maxInt = maxInt;
    _minInt = minInt;
    _minMZ = minMZ;
    _maxMZ = maxMZ;
    _mzRange = (maxMZ - minMZ);
}
