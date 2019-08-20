//
//  fastaFile.hpp
//  utils
// -----------------------------------------------------------------------------
// Copyright 2018 Aaron Maurais
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

#ifndef fastaFile_hpp
#define fastaFile_hpp

#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <sstream>
#include <string>

#include <utils.hpp>
#include <bufferFile.hpp>
#include <peptideUtils.hpp>

namespace utils {
	class FastaFile;
	
	const size_t PROT_ID_NOT_FOUND = std::string::npos;
	
	class FastaFile : public utils::BufferFile{
	private:
		typedef std::pair<size_t, size_t> IntPair;
		typedef std::map<std::string, size_t> IdMapType;
		typedef std::vector<IntPair> IndexMapType;
		
		//!All peptide sequences which were already found are stored internally
		std::map<std::string, std::string> _foundSequences;
		//!Stores beginning and ending offset indices of each protein index
		IndexMapType _indexOffsets;
		//!Stores index values for each protein ID
		IdMapType _idIndex;
		//!Total number of entries in fasta file
		size_t _sequenceCount;
		
		//! Should previously read sequences be stored internally?
		bool _storeFound;

		void _buildIndex();
		void _copyValues(const FastaFile&);

	public:
		/**
		\brief Default constructor.

		\param storeFound Should previously read sequences be stored internally?
		This value should be set to false if object is used in multi-threaded environments.
		\param fname Path to fasta file.
		*/
		FastaFile(bool storeFound = true, std::string fname = "") : BufferFile(fname) {
			_sequenceCount = 0;
			_storeFound = storeFound;

		}
		//!Copy constructor
		FastaFile(const FastaFile& rhs) : BufferFile(rhs){
			_copyValues(rhs);
		}
		~FastaFile(){}
		
		//modifiers
		FastaFile& operator = (FastaFile rhs){
			BufferFile::operator=(rhs);
			_copyValues(rhs);
			return *this;
		}
		bool read();
		bool read(std::string);
		
		//properties
		std::string getSequence(std::string proteinID, bool verbose = false);
		std::string getSequence(std::string proteinID, bool verbose = false) const;
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc) const;
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq,
									   int modLoc, bool verbose, bool& found);
		int getMoodifiedResidueNumber(std::string peptideSeq, int modLoc) const;
		bool empty() const;
		std::string nBefore(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false);
		std::string nAfter(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false);
		std::string nBefore(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false) const;
		std::string nAfter(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false) const;
	};
}

#endif /* fastaFile_hpp */
