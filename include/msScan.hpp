//
// msScan.hpp
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

#ifndef msScan_hpp
#define msScan_hpp

#include <vector>

#include <utils.hpp>
#include <exceptions.hpp>

namespace utils {

    typedef double ScanIntensity;
    typedef double ScanMZ;

    class Scan;
    class PrecursorScan;
    template<typename MZ_T, typename INTENSITY_T> class Ion;

    template<typename MZ_T, typename INTENSITY_T>
    class Ion {
    protected:
        MZ_T _mz;
        INTENSITY_T _intensity;
    public:
        Ion() {
            _mz = 0;
            _intensity = 0;
        }

        Ion(MZ_T mz, INTENSITY_T intensity) {
            _mz = mz;
            _intensity = intensity;
        }

        Ion(const Ion<MZ_T, INTENSITY_T> &rhs) {
            _mz = rhs._mz;
            _intensity = rhs._intensity;
        }

        Ion<MZ_T, INTENSITY_T> &operator=(const Ion<MZ_T, INTENSITY_T> &rhs) {
            _mz = rhs._mz;
            _intensity = rhs._intensity;
            return *this;
        }

        // setters
        void setIntensity(INTENSITY_T intensity) {
            _intensity = intensity;
        }
        void setMZ(MZ_T mz) {
            _mz = mz;
        }

        // getters
        MZ_T getMZ() const {
            return _mz;
        }
        INTENSITY_T getIntensity() const {
            return _intensity;
        }
    };

    class PrecursorScan : public Ion<std::string, ScanIntensity> {
    private:
        std::string _scan;
        double _rt;
        //! parent file path
        std::string _file;
        //! sample name (usually this should just be the file basename with no extension)
        std::string _sample;
        int _charge;

    public:
        PrecursorScan() : Ion("", 0) {
            _scan = "";
            _rt = 0;
            _file = "";
            _sample = "";
            _charge = 0;
        }

        PrecursorScan(const PrecursorScan &rhs) : Ion(rhs) {
            _scan = rhs._scan;
            _rt = rhs._rt;
            _file = rhs._file;
            _sample = rhs._sample;
            _charge = rhs._charge;
        }

        PrecursorScan &operator=(const PrecursorScan &rhs) {
            Ion::operator=(rhs);
            _scan = rhs._scan;
            _rt = rhs._rt;
            _file = rhs._file;
            _sample = rhs._sample;
            _charge = rhs._charge;
            return *this;
        }

        //modifiers
        void setScan(const std::string &scan) {
            _scan = scan;
        }
        void setRT(double rt) {
            _rt = rt;
        }
        void setFile(const std::string &file) {
            _file = file;
        }
        void setSample(const std::string& sample) {
            _sample = sample;
        }

        void setCharge(int charge) {
            _charge = charge;
        }

        void clear();

        //properties
        const std::string &getScan() const {
            return _scan;
        }
        double getRT() const {
            return _rt;
        }
        const std::string &getFile() const {
            return _file;
        }
        int getCharge() const {
            return _charge;
        }
    };

    class Scan {

    private:
        typedef std::vector<Ion<ScanMZ, ScanIntensity> > IonsType;
        size_t _scanNum;
        PrecursorScan precursorScan;

        //dynamic metadata
        ScanIntensity _maxInt;
        ScanIntensity _minInt;
        ScanMZ _minMZ;
        ScanMZ _maxMZ;
        ScanMZ _mzRange;

        //! vector of Ion(s)
        IonsType _ions;

    public:

        Scan() {
            _maxInt = 0;
            _minInt = 0;
            _minMZ = 0;
            _maxMZ = 0;
            _mzRange = 0;
            precursorScan = PrecursorScan();
            _ions = IonsType();
            _scanNum = std::string::npos;
        }

        void clear();

        PrecursorScan& getPrecursor() {
            return precursorScan;
        }
        const PrecursorScan& getPrecursor() const {
            return precursorScan;
        }
        IonsType& getIons() {
            return _ions;
        }
        const IonsType& getIons() const {
            return _ions;
        }
        ScanIntensity getMaxInt() const {
            return _maxInt;
        }
        ScanIntensity getMinInt() const {
            return _minInt;
        }
        ScanMZ getMinMZ() const{
            return _minMZ;
        }
        ScanMZ getMaxMZ() const{
            return _maxMZ;
        }
        ScanMZ getMzRange() const{
            return _mzRange;
        }

        void setScanNum(size_t scanNum){
            _scanNum = scanNum;
        }
        void setMaxInt(ScanIntensity maxInt){
            _maxInt = maxInt;
        }
        void setMinInt(ScanIntensity minInt){
            _minInt = minInt;
        }
        void setMinMZ(ScanMZ);
        void setMaxMZ(ScanMZ);
        void updateRanges();
    };
}

#endif
