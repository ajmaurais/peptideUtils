//
//  fastaFile.hpp
//  utils
// -----------------------------------------------------------------------------
// Copyright 2018 Aaron maurais
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

namespace fastaFile {
	class FastaFile;
	
	const std::string PROT_SEQ_NOT_FOUND = "PROT_SEQ_NOT_FOUND";
	const std::string PEP_SEQ_NOT_FOUND = "PEP_SEQ_NOT_FOUND";
	
	class FastaFile : public base::BufferFile{
	private:
		typedef std::pair<size_t, size_t> IntPair;
		typedef std::map<std::string, IntPair> IndexMapType;
		
		//!All peptide sequences which were already found are stored internally
		std::map<std::string, std::string> _foundSequences;
		//!Stores begining and ending offset indecies of eacn protein ID
		IndexMapType _idIndex;
		
		void _buildIndex();
		
	public:
		FastaFile(std::string fname = "") : BufferFile(fname) {}
		FastaFile(const FastaFile& rhs) : BufferFile(rhs) {}
		~FastaFile(){}
		
		//modifers
		FastaFile& operator = (FastaFile rhs){
			BufferFile::operator=(rhs);
			_idIndex = rhs._idIndex;
			_foundSequences = rhs._foundSequences;
			return *this;
		}
		bool read();
		bool read(std::string);
		
		//properties
		std::string getSequence(std::string proteinID, bool verbose = false);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq, int modLoc);
		std::string getModifiedResidue(std::string proteinID, std::string peptideSeq,
									   int modLoc, bool verbose, bool& found);
		int getMoodifiedResidueNumber(std::string peptideSeq, int modLoc) const;
	};
}

#endif /* fastaFile_hpp */
