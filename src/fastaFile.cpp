// 
// fastaFile.cpp
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

#include <fastaFile.hpp>

/**
\brief Constructor.

\param id Entry ID
\param beg Begin offset.
\param end End offset
*/
utils::FastaEntry::FastaEntry(std::string id, size_t beg, size_t end){
	_id = id; _beg = beg; _end = end;
}

//!Copy constructor
utils::FastaEntry::FastaEntry(const FastaEntry& rhs)
{
	_id = rhs._id;
	_beg = rhs._beg;
	_end = rhs._end;
}

//!Copy assignment
utils::FastaEntry& utils::FastaEntry::operator = (const FastaEntry& rhs)
{
	_id = rhs._id;
	_beg = rhs._beg;
	_end = rhs._end;
	return *this;
}

/**
\brief Return protein sequence at index \p i. <br>

If i > _indexOffsets.size(), an empty string is returned.

\return Protein sequence
*/
std::string utils::FastaFile::operator [](size_t i) const
{
	if(i >= _indexOffsets.size())
		return "";

	std::string temp(_buffer + _indexOffsets[i].getBeg(),
		_buffer + (_indexOffsets[i].getBeg() + (_indexOffsets[i].getEnd() - _indexOffsets[i].getBeg())));
	std::stringstream ss(temp);
	
	std::string line;
	std::string seq = "";
	int begin_count = 0;
	while(utils::safeGetline(ss, line))
	{
		if(line[0] == '>')
		{
			if(begin_count > 0)
				break;
			begin_count ++;
			continue;
		}
		seq += line;
	}
	return seq;
}

/**
\brief Return protein sequence at index \p i.

\throws std::out_of_range if \p i not in _indexOffsets.
\return Protein sequence
*/
std::string utils::FastaFile::at(size_t i) const
{
	std::string ret = (*this)[i];
	if(ret.empty())
		throw std::out_of_range("Protein index does not exist!");
	return ret;
}

//!Copy FastaFile members from \p rhs. Should not be called directly.
void utils::FastaFile::_copyValues(const FastaFile& rhs)
{
	_indexOffsets = rhs._indexOffsets;
	_idIndex = rhs._idIndex;
	_foundSequences = rhs._foundSequences;
	_sequenceCount = rhs._sequenceCount;
	_storeFound = rhs._storeFound;
}

/**
 \brief Get integer index of \p proteinID in IndexMapType <br>

 If \p proteinID is not found, utils::PROT_ID_NOT_FOUND is returned.
 \param proteinID Protein identifier in fasta file to lookup.
 \return Index of \p proteinID.
*/
size_t utils::FastaFile::getIdIndex(std::string proteinID) const
{
	auto it = _idIndex.find(proteinID);
	if(it == _idIndex.end())
		return PROT_ID_NOT_FOUND;
	return it->second;
}

std::string utils::FastaFile::getIndexID(size_t i) const
{
	if(i >= _indexOffsets.size())
		return utils::PROT_SEQ_NOT_FOUND;
	return _indexOffsets[i].getID();
}

/**
 \brief Search for \p proteinID in FastaFile::_buffer and extract protein sequence.
 \param proteinID uniprot ID of protein to search for.
 \param verbose Should detials of ids not found be printed to std::cerr?
 \return If found, parent protein sequence. If protein sequence is not found returns
 utils::PROT_SEQ_NOT_FOUND.
 */
std::string utils::FastaFile::getSequence(std::string proteinID, bool verbose) const
{
	//get offset of proteinID
	size_t proteinIndex_temp = getIdIndex(proteinID);
	if(proteinIndex_temp == utils::PROT_ID_NOT_FOUND){
		if(verbose){
			std::cerr << "Warning! ID: " << proteinID << " not found in fastaFile.\n";
		}
		return utils::PROT_SEQ_NOT_FOUND;
	}
	
	return (*this)[proteinIndex_temp];
}

/**
 \brief Search for \p proteinID in FastaFile::_buffer and extract protein sequence.
 \param proteinID uniprot ID of protein to search for.
 \param verbose Should detials of ids not found be printed to std::cerr?
 \return If found, parent protein sequence. If protein sequence is not found returns
 utils::PROT_SEQ_NOT_FOUND.
 */
std::string utils::FastaFile::getSequence(std::string proteinID, bool verbose)
{
	if(_storeFound){
		//if proteinID has already been parsed, return the seq and exit
		auto foundIt = _foundSequences.find(proteinID);
		if(foundIt != _foundSequences.end())
			return foundIt->second;
	}
	
	//get offset of proteinID
	size_t proteinIndex_temp = getIdIndex(proteinID);
	if(proteinIndex_temp == utils::PROT_ID_NOT_FOUND){
		if(verbose){
			std::cerr << "Warning! ID: " << proteinID << " not found in fastaFile.\n";
		}
		return utils::PROT_SEQ_NOT_FOUND;
	}
	
	std::string seq = (*this)[proteinIndex_temp];
	_foundSequences[proteinID] = seq;
	return seq;
}

//! const overloaded version of FastaFile::getModifiedResidue
std::string utils::FastaFile::getModifiedResidue(std::string proteinID,
												 std::string peptideSeq,
												 int modLoc) const
{
	std::string seq = getSequence(proteinID);
	return utils::getModifiedResidue(seq, peptideSeq, modLoc);	
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param proteinID parent protein uniprot ID of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 */
std::string utils::FastaFile::getModifiedResidue(std::string proteinID,
												 std::string peptideSeq,
												 int modLoc)
{
	std::string seq = getSequence(proteinID);
	return utils::getModifiedResidue(seq, peptideSeq, modLoc);	
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param proteinID parent protein uniprot ID of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 \param verbose Should detials of ids not found be printed to std::cerr?
 \param found set to false if first instance of searching for protein and it not being found
 \return string representation of modified residue.
 */
std::string utils::FastaFile::getModifiedResidue(std::string proteinID,
												 std::string peptideSeq,
												 int modLoc,
												 bool verbose,
												 bool& found)
{
	found = true;
	std::string seq = getSequence(proteinID, verbose);
	if(seq == utils::PROT_SEQ_NOT_FOUND)
		found = false;
	//else seq = _foundSequences[proteinID];
	if(seq == utils::PROT_SEQ_NOT_FOUND)
		return utils::PROT_SEQ_NOT_FOUND;
	
	return utils::getModifiedResidue(seq, peptideSeq, modLoc);
}

/**
 \brief iterate through _buffer to search for indices of beginning and end of all entries.
 */
void utils::FastaFile::_buildIndex()
{
	//iterate through _buffer to search for where entries begin
	std::vector<size_t> combined, trIdx;
	utils::getIdxOfSubstr(_buffer, ">sp", combined);
	utils::getIdxOfSubstr(_buffer, ">tr", trIdx);
	
	//combine and sort vectors
	combined.insert(combined.end(), trIdx.begin(), trIdx.end());
	combined.push_back(_size); //add index to end of buffer
	std::sort(combined.begin(), combined.end());
	
	//build _indexOffsets
	_sequenceCount = 0;
	int const scanLen = 20;
	std::string newID;
	size_t len = combined.size();
	for(size_t i = 0; i < len - 1; i++) //for all but last index in combined
	{
		char* c = &_buffer[combined[i] + 4];
		newID = "";
		for(int j = 0; j < scanLen; j++)
		{
			newID += *c;
			c += 1;
			if(*c == '|')
				break;
		}

		//add sequence offset to class members
		_idIndex[newID] = _sequenceCount;
		_indexOffsets.push_back(utils::FastaEntry(newID, combined[i], combined.at(i + 1)));
		_sequenceCount++;
	}
}


bool utils::FastaFile::read(){
	if(!BufferFile::read()) return false;
		_buildIndex();
	return true;
}

bool utils::FastaFile::read(std::string fname){
	_fname = fname;
	return FastaFile::read();
}

/**
\return true if FastaFile::_buffer is empty.
*/
bool utils::FastaFile::empty() const{
	return buffer_empty();
}

size_t utils::FastaFile::getSequenceCount() const{
	return _sequenceCount;
}

/**
\brief Get \p n residues after \p query in protein \p ref_id. <br>

If \p n overruns \p ref, the maximum number of characters will be returned.

\param query String to search for.
\param ref_id ID of protein in fasta file.
\param n Number of residues in output.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return \p n residues after \p query.
*/
std::string utils::FastaFile::nAfter(const std::string& query, const std::string& ref_id, unsigned n,
	bool noExcept){
	std::string ref = getSequence(ref_id);
	return utils::nAfter(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

/**
\brief Get \p n residues before \p query in protein \p ref_id. <br>

If \p n overruns \p ref, the maximum number of characters will be returned.

\param query String to search for.
\param ref_id ID of protein in fasta file.
\param n Number of residues in output.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return \p n residues before \p query.
*/
std::string utils::FastaFile::nBefore(const std::string& query, const std::string& ref_id,
	unsigned n, bool noExcept){
	std::string ref = getSequence(ref_id);
	return utils::nBefore(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

//!const overloaded version of FastaFile::nAfter
std::string utils::FastaFile::nAfter(const std::string& query, const std::string& ref_id, unsigned n,
	bool noExcept) const{
	std::string ref = getSequence(ref_id);
	return utils::nAfter(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

//!const overloaded version of FastaFile::nBefore
std::string utils::FastaFile::nBefore(const std::string& query, const std::string& ref_id,
	unsigned n, bool noExcept) const{
	std::string ref = getSequence(ref_id);
	return utils::nBefore(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

/**
\brief Get the index of the nth residue in \p query in protein \p ref_id. <br>

If \p query not found in \p ref_id, returns std::string::npos.

\param query String to search for.
\param ref_it Identifier of parent protein.
\param n nth residue. If n is std::string::npos, the index of the last residue in query is returned.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return Residue index of nth residue in \p ref.
*/
size_t utils::FastaFile::indexN(const std::string &query, const std::string &ref_id, unsigned int n, bool noExcept)
{
    std::string ref = getSequence(ref_id);
    return utils::indexN(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

//!const overloaded version of FastaFile::indexN
size_t utils::FastaFile::indexN(const std::string &query, const std::string &ref_id, unsigned int n, bool noExcept) const
{
    std::string ref = getSequence(ref_id);
    return utils::indexN(query, (ref == utils::PROT_SEQ_NOT_FOUND ? "" : ref), n, noExcept);
}

/**
 \brief Get position residue and position of \p modLoc in parent protein
 of \p peptideSeq.
 \param seq parent protein sequence of \p peptideSeq
 \param peptideSeq unmodified peptide sequence.
 \param modLoc location of modified residue in peptide
 (where 0 is the beginning of the peptide.)
 */
std::string utils::getModifiedResidue(const std::string& seq, const std::string& peptideSeq, int modLoc)
{
	if(seq == utils::PROT_SEQ_NOT_FOUND)
		return utils::PROT_SEQ_NOT_FOUND;
	
	size_t begin = seq.find(peptideSeq);
	if(begin == std::string::npos)
		return utils::PEP_SEQ_NOT_FOUND;
	size_t modNum = begin + modLoc;
	if(modLoc > peptideSeq.length())
		throw std::runtime_error("modLoc is out of bounds!");
	std::string ret = std::string(1, peptideSeq[modLoc]) + std::to_string(modNum + 1);
	
	return ret;
}

/**
\brief Align \p query to \p ref.

\param query String to align.
\param ref String to use as reference.
\param beg Set to beginning index of \p query in \p ref.
\param end Set to ending index of \p query in \p ref.

\return false if \p query is not in \p ref, true otherwise.
*/
bool utils::align(const std::string& query, const std::string& ref, size_t& beg, size_t& end)
{
	size_t match = ref.find(query);
	if(match == std::string::npos) return false;
	
	beg = match;
	end = match + query.length() - 1;
	
	return true;
}

/**
\brief Get \p n residues before \p query in \p ref. <br>

If \p n overruns \p ref, the maximum possible number of characters will be returned.

\param query String to search for.
\param ref String to search in.
\param n Number of residues in output.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return \p n residues before \p query.
*/
std::string utils::nBefore(const std::string& query, const std::string& ref, unsigned n, bool noExcept)
{
	size_t beg, end;
	if(!utils::align(query, ref, beg, end)){
		if(noExcept) return "";
		else throw std::out_of_range("query:\n\t" + query + "\nnot in ref:\n\t" + ref);
	}
	
	if(beg < n) n = beg;

	return ref.substr(beg - n, n);
}

/**
\brief Get \p n residues after \p query in \p ref. <br>

If \p n overruns \p ref, the maximum possible number of characters will be returned.

\param query String to search for.
\param ref String to search in.
\param n Number of residues in output.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return \p n residues after \p query.
*/
std::string utils::nAfter(const std::string& query, const std::string& ref, unsigned n, bool noExcept)
{
	size_t beg, end;
	if(!utils::align(query, ref, beg, end)){
		if(noExcept) return "";
		else throw std::out_of_range("query:\n\t" + query + "\nnot in ref:\n\t" + ref);
	}
	
	end += 1;
	if(end + n > ref.length())
		n = ref.length() - end;
	return ref.substr(end, n);
}

/**
\brief Get the index of the nth residue in \p query in \p ref. <br>

If \p query not found in \p ref, returns std::string::npos.

\param query String to search for.
\param ref String to search in.
\param n nth residue. If n is std::string::npos, the index of the last residue in query is returned.
\param noExcept Should an std::out_of_range be thrown if \p query is not in \p ref?

\throws std::out_of_range if \p query is not in \p ref and \p noExcept is false.

\return Residue index of nth residue in \p ref.
*/
size_t utils::indexN(const std::string& query, const std::string& ref, size_t n, bool noExcept)
{
	size_t beg, end;
	if(!utils::align(query, ref, beg, end)){
		if(noExcept) return std::string::npos;
		else throw std::out_of_range("query not in ref");
	}
	
	if(n == std::string::npos)
		n = query.length() - 1;
	if(beg + n > ref.length())
		return ref.length() - 1;
	return beg + n;
}

