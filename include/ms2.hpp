//
// ms2.hpp
// ionFinder
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

#ifndef ms2_hpp
#define ms2_hpp

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <cassert>
// #include <list>
#include <vector>
#include <string>
#include <map>

#include <utils.hpp>
#include <bufferFile.hpp>

namespace ms2 {
	
	int const MD_NUM = 4;
	size_t const SCAN_INDEX_NOT_FOUND = std::string::npos;

	class Scan;
	class Ms2File;
    class PrecursorScan;
    template <typename MZ_T, typename INTENSITY_T> class Ion;

    template <typename MZ_T, typename INTENSITY_T>
    class Ion {
    protected:
        MZ_T _mz;
        INTENSITY_T _intensity;
    public:
        Ion(){_mz = 0; _intensity = 0;}
        Ion(MZ_T mz, INTENSITY_T intensity){_mz = mz; _intensity = intensity;}

        Ion(const Ion<MZ_T, INTENSITY_T>& rhs) {
            _mz = rhs._mz;
            _intensity = rhs._intensity;
        }
        Ion<MZ_T, INTENSITY_T>& operator = (const Ion<MZ_T, INTENSITY_T>& rhs) {
            _mz = rhs._mz;
            _intensity = rhs._intensity;
            return *this;
        }

        void setIntensity(INTENSITY_T intensity) {
            _intensity = intensity;
        }
        void setMZ(MZ_T mz) {
            _mz = mz;
        }

        MZ_T getMZ() const {
            return _mz;
        }
        INTENSITY_T getIntensity() const {
            return _intensity;
        }
    };

    class PrecursorScan: protected Ion<std::string, double>{
    private:
        std::string _scan;
        double _rt;
        std::string _file;
        int _charge;

    public:
        PrecursorScan(): Ion("", 0){
            _scan = "";
            _rt = 0;
            _file = "";
            _charge = 0;
        }
        PrecursorScan(const PrecursorScan& rhs): Ion(rhs){
            _scan = rhs._scan;
            _rt = rhs._rt;
            _file = rhs._file;
            _charge = rhs._charge;
        }

        PrecursorScan& operator = (const PrecursorScan& rhs){
            Ion::operator=(rhs);
            _scan = rhs._scan;
            _rt = rhs._rt;
            _file = rhs._file;
            _charge = rhs._charge;
            return *this;
        }

        //modifiers
        void setScan(const std::string &scan){
            _scan = scan;
        }
        void setRT(double rt){
            _rt = rt;
        }
        void setFile(const std::string &file){
            _file = file;
        }
        void setCharge(int charge){
            _charge = charge;
        }
        void clear();

        //properties
        const std::string &getScan() const{
            return _scan;
        }
        double getRT() const{
            return _rt;
        }
        const std::string &getFile() const{
            return _file;
        }
        int getCharge() const{
            return _charge;
        }
    };

    class Scan {

	private:
	    std::string _parentFileBase;
	    size_t _scanNum;
	    PrecursorScan precursorScan;
	public:

	    void clear();

	    PrecursorScan& getPrecursor();
        const PrecursorScan& getPrecursor() const;

    };
	
	class Ms2File : public utils::BufferFile{
	private:
		typedef std::pair<size_t, size_t> IntPair;
		typedef std::vector<IntPair> OffsetIndexType;
		
		//!Stores pairs of offset values for scans
		OffsetIndexType _offsetIndex;
		//!Maps scan numbers to indecies in _offsetIndex
		std::map<size_t, size_t> _scanMap;
		//!Actual number of scans read from file
		size_t _scanCount{};
		
		//metadata
		//!base file name without extension
		std::string _parentMs2FileBase;
		size_t firstScan, lastScan;
		std::string dataType;
		std::string scanType;
		
		bool getMetaData();
		void calcParentMs2(std::string path){
            _parentMs2FileBase = utils::baseName(utils::removeExtension(path));
		}
		
		void copyMetadata(const Ms2File& rhs);
		void initMetadata();
		void _buildIndex();
		size_t _getScanIndex(size_t) const;
		
	public:

		Ms2File(std::string fname = "") : BufferFile(fname){
			initMetadata();
		}
		~Ms2File(){}
		
		//!copy constructor
		Ms2File(const Ms2File& rhs) : BufferFile(rhs){
			copyMetadata(rhs);
		}
		//!copy assignment
		Ms2File& operator = (Ms2File rhs)
		{
			BufferFile::operator=(rhs);
			copyMetadata(rhs);
			return *this;
		}
		
		//modifiers
		bool read(std::string);
		bool read();
		
		//properties
		bool getScan(std::string, Scan&) const;
		bool getScan(size_t, Scan&) const;
	};
		
}//end of namespace

#endif /* ms2_hpp */
