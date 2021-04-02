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
    namespace msInterface {

        typedef double ScanIntensity;
        typedef double ScanMZ;

        //! Represents ion activation methods
        enum class ActivationMethod {
            CID, /**< collision-induced dissociation */
            MPD, /**< photodissociation */
            ECD, /**< electron capture dissociation */
            ETD, /**< electron transfer dissociation */
            PQD, /**< pulsed q dissociation */
            HCD, /**< higher-energy collisional dissociation */
            UNKNOWN
        };
        ActivationMethod strToActivation(const std::string&);
        std::string activationToOBO(msInterface::ActivationMethod activation);
        std::string activationToString(msInterface::ActivationMethod am);
        ActivationMethod oboToActivation(std::string obo);
        //! Represents MS scan modes
        enum class Polarity {POSITIVE, NEGATIVE, UNKNOWN};

        class Scan;

        class PrecursorScan;

        template<typename MZ_T, typename INTENSITY_T>
        class Ion;

        typedef Ion<ScanMZ, ScanIntensity> ScanIon;

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
            bool operator==(const Ion& rhs) const{
                return _mz == rhs._mz && _intensity == rhs._intensity;
            }
            bool operator!=(const Ion& rhs) const{
                return _mz != rhs._mz && _intensity != rhs._intensity;
            }
            bool almostEqual(const Ion& rhs, double epsilon = DBL_EPSILON) const{
                return utils::almostEqual(_mz, rhs._mz, epsilon) &&
                       utils::almostEqual(_intensity, rhs._intensity, epsilon);
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

            //for utils::insertSorted()
            inline bool insertCompare(const Ion &comp) const {
                return _intensity > comp._intensity;
            }

            struct MZComparison {
                bool operator()(const Ion &lhs, const Ion &rhs) const {
                    return lhs.getMZ() < rhs.getMZ();
                }

                bool operator()(const Ion &lhs, double rhs) const {
                    return lhs.getMZ() < rhs;
                }
            };

            struct IntComparison {
                bool operator()(Ion *lhs, Ion *rhs) const {
                    return lhs->insertCompare(*rhs);
                }
            };
        };

        class PrecursorScan : public Ion<std::string, ScanIntensity> {
        private:
            std::string _scan;
            double _rt;
            //! parent file path
            std::string _file;
            //! sample name (usually this should just be the file basename with no extension)
            std::string _sample;
            ActivationMethod _activationMethod;
            int _charge;

        public:
            PrecursorScan() : Ion("", 0) {
                _scan = "";
                _rt = 0;
                _file = "";
                _sample = "";
                _charge = 0;
                _activationMethod = ActivationMethod::UNKNOWN;
            }

            PrecursorScan(const PrecursorScan &rhs) : Ion(rhs) {
                _scan = rhs._scan;
                _rt = rhs._rt;
                _file = rhs._file;
                _sample = rhs._sample;
                _charge = rhs._charge;
                _activationMethod = rhs._activationMethod;
            }

            PrecursorScan &operator=(const PrecursorScan &rhs) {
                Ion::operator=(rhs);
                _scan = rhs._scan;
                _rt = rhs._rt;
                _file = rhs._file;
                _sample = rhs._sample;
                _charge = rhs._charge;
                _activationMethod = rhs._activationMethod;
                return *this;
            }
            bool operator==(const PrecursorScan& rhs) const;

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
            void setSample(const std::string &sample) {
                _sample = sample;
            }
            void setActivationMethod(ActivationMethod am) {
                _activationMethod = am;
            }
            void setCharge(int charge) {
                _charge = charge;
            }
            void clear();

            //properties
            bool almostEqual(const PrecursorScan& rhs,
                             double epsilon = DBL_EPSILON) const;
            const std::string &getScan() const {
                return _scan;
            }
            std::string getSample() const {
                return _sample;
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
            ActivationMethod getActivationMethod() const {
                return _activationMethod;
            }
        };

        class Scan {
        private:
            ScanMZ _minMZ;
            ScanMZ _maxMZ;
        protected:
            typedef std::vector<ScanIon> IonsType;
            size_t _scanNum;
            PrecursorScan precursorScan;

            //dynamic metadata
            ScanIntensity _maxInt;
            ScanIntensity _minInt;
            ScanMZ _mzRange;
            int _level;
            Polarity _polarity;
            //! ion injection time in millisecond
            double _ionInjectionTime;

            //! vector of Ion(s)
            IonsType _ions;

        public:

            Scan() {
                _maxInt = 0;
                _minInt = 0;
                _minMZ = 0;
                _maxMZ = 0;
                _mzRange = 0;
                _level = 0;
                _polarity = Polarity::UNKNOWN;
                _ionInjectionTime = 0;
                precursorScan = PrecursorScan();
                _ions = IonsType();
                _scanNum = std::string::npos;
            }

            Scan(const Scan &);
            Scan &operator=(const Scan &);
            bool operator==(const Scan& rhs) const;

            void clear();
            void add(const ScanIon &);
            void add(ScanMZ, ScanIntensity);
            void setMinMZ(ScanMZ);
            void setMaxMZ(ScanMZ);
            void updateRanges();

            bool almostEqual(const Scan& rhs, double epsilon = DBL_EPSILON) const;
            PrecursorScan &getPrecursor() {
                return precursorScan;
            }
            const PrecursorScan &getPrecursor() const {
                return precursorScan;
            }
            IonsType &getIons() {
                return _ions;
            }
            ScanIon &operator[](size_t i) {
                return _ions[i];
            }
            const ScanIon &at(size_t i) const {
                return _ions.at(i);
            }
            ScanIon &at(size_t i) {
                return _ions.at(i);
            }
            const IonsType &getIons() const {
                return _ions;
            }
            IonsType::iterator begin() {
                return _ions.begin();
            }
            IonsType::iterator end() {
                return _ions.end();
            }
            IonsType::const_iterator begin() const {
                return _ions.begin();
            }
            IonsType::const_iterator end() const {
                return _ions.end();
            }
            size_t size() {
                return _ions.size();
            }
            ScanIntensity getMaxInt() const {
                return _maxInt;
            }
            ScanIntensity getMinInt() const {
                return _minInt;
            }
            ScanMZ getMinMZ() const {
                return _minMZ;
            }
            ScanMZ getMaxMZ() const {
                return _maxMZ;
            }
            ScanMZ getMzRange() const {
                return _mzRange;
            }
            size_t getScanNum() const {
                return _scanNum;
            }
            int getLevel() const {
                return _level;
            }
            Polarity getPolarity() const {
                return _polarity;
            }
            double getIonInjectionTime() const{
                return _ionInjectionTime;
            }
            void setIonInjectionTime(double t){
                _ionInjectionTime = t;
            }
            void setScanNum(size_t scanNum) {
                _scanNum = scanNum;
            }
            void setMaxInt(ScanIntensity maxInt) {
                _maxInt = maxInt;
            }
            void setMinInt(ScanIntensity minInt) {
                _minInt = minInt;
            }
            void setLevel(int l) {
                _level = l;
            }
            void setPolarity(Polarity p) {
                _polarity = p;
            }
        };
    }
}

#endif
