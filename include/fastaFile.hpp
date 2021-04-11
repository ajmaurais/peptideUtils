// 
// fastaFile.hpp
// utils
// -----------------------------------------------------------------------------
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

namespace utils {
	class FastaFile;
	class FastaEntry;

	const std::string PROT_SEQ_NOT_FOUND = "PROT_SEQ_NOT_FOUND";
	const std::string PEP_SEQ_NOT_FOUND = "PEP_SEQ_NOT_FOUND";

	const size_t PROT_ID_NOT_FOUND = std::string::npos;

	class FastaEntry{
	private:
		size_t _beg, _end;
		std::string _id;
	public:
		//!Default constructor
		FastaEntry() : _beg(0), _end(0), _id(""){}
		FastaEntry(std::string id, size_t beg, size_t end);
		FastaEntry(const FastaEntry&);
		~FastaEntry() {}

		FastaEntry& operator = (const FastaEntry& rhs);
		
		//properties		
		size_t getBeg() const{
			return _beg;
		}
		size_t getEnd() const{
			return _end;
		}
		std::string getID() const{
			return _id;
		}
	};
	
	class FastaFile : public utils::BufferFile{
	private:

		typedef std::map<std::string, size_t> IdMapType;
		typedef std::vector<FastaEntry> IndexMapType;
		
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
		size_t getIdIndex(std::string proteinID) const;
		std::string getIndexID(size_t) const;
		bool empty() const;
		size_t getSequenceCount() const;
		std::string operator[] (size_t) const;
		std::string at(size_t) const;

		std::string getSequence(std::string proteinID, bool verbose = false);
		std::string getSequence(std::string proteinID, bool verbose = false) const;
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc) const;
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq,
									   int modLoc, bool verbose, bool& found);
		int getMoodifiedResidueNumber(std::string peptideSeq, int modLoc) const;
		
		std::string nBefore(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false);
		std::string nAfter(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false);
		std::string nBefore(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false) const;
		std::string nAfter(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false) const;
		size_t indexN(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false);
		size_t indexN(const std::string& query, const std::string& ref_id,
			unsigned n, bool noExcept = false) const;
	};

	std::string getModifiedResidue(const std::string& seq, const std::string& peptideSeq, int modLoc);

	bool align(const std::string& query, const std::string& ref, size_t& beg, size_t& end);
	std::string nBefore(const std::string& query, const std::string& ref, unsigned n, bool noExcept = false);
	std::string nAfter(const std::string& query, const std::string& ref, unsigned n, bool noExcept = false);
	size_t indexN(const std::string& query, const std::string& ref, size_t n, bool noExcept = false);
}

#endif /* fastaFile_hpp */
