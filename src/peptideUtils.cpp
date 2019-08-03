//
//peptide_utils.cpp
//utils
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
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR  OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
//

#include <peptideUtils.hpp>

/**
\brief Perform a virtual protease digest of a protein.

\param seq Protein sequence.
\param peptides Populated with digested peptides.
\param missedCleavages Number of missed cleavages to allow.
\param minLen Minimum peptide length.
\param maxLen Maximum peptide length. Set to std::string::npos for no upper bound on length.
\param cleavagePattern RegEx for protease cleavage pattern.
*/
void utils::digest(std::string seq, std::vector<std::string>& peptides,
				 unsigned nMissedCleavages, size_t minLen, size_t maxLen,
				 std::string cleavagePattern)
{
	std::regex re(cleavagePattern);
	std::smatch m;
	std::vector<size_t> index_matches;
	peptides.clear();
		
	//Get indices of cleavage sites
	for(auto it = std::sregex_iterator(seq.begin(), seq.end(), re); it != std::sregex_iterator(); ++it){
		index_matches.push_back(it->position() + 1);
	}
	index_matches.push_back(0); //add beginning of sequence
	
	//Get unique values and sort
	utils::unique(index_matches);
	std::sort(index_matches.begin(), index_matches.end());

	size_t len = index_matches.size();
	for(int i = 0; i < len; i++)
	{
		for(int j = 0; j <= nMissedCleavages; j++)
		{
			size_t pLen;
			if((i + j + 1) >= len)
				pLen = seq.length() - index_matches[i];
			else
				pLen = index_matches.at(i+j+1) - index_matches.at(i);
			
			std::string seq_temp = seq.substr(index_matches[i], pLen);
			size_t len_temp = seq_temp.length();
			if(len_temp >= minLen && (maxLen == std::string::npos ? true : len_temp <= maxLen))
				peptides.push_back(seq_temp);
		}
	}
	
	//get unique values and sort by peptide length.
	utils::unique(peptides);
	std::sort(peptides.begin(), peptides.end(), utils::strLenCompare());
}

/**
\brief Record indices of occurrences of utils::MOD_CHARS in seq.

\param seq peptide sequence containing modifications
\param modLocs vector populated with locations of modifications

\return peptide sequence with mods removed
*/
std::string utils::getModLocs(std::string seq, std::vector<int>& modLocs)
{
	modLocs.clear();
	std::string ret = "";
	
	//check that n term is not a diff mod
	for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
	  if(seq[0] == *p)
	    throw std::runtime_error("Invalid peptide sequence: " + seq);
	
	for(size_t i = 0; i < seq.length(); i++)
	{
		for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
		{
			if(seq[i] == *p){
				seq.erase(i, 1);
				modLocs.push_back(int(ret.length() - 1));
				break;
			}
		}
		//exit loop if at end of sequence
		if(i >= seq.length())
			break;
		
		//Check that current char is letter
		if(!isalpha(seq[i]))
			throw std::runtime_error("Invalid peptide sequence: " + seq);
		
		//add new amino acid to ret
		ret.push_back(seq[i]);
	}
	return ret;
}

/**
\brief Convert from 1 letter amino acid codes to 3.

\param seq peptide sequence
\param sep_in delimiter between amino acids in input
\param sep_out delimiter between amino acids in output
\param n_term_out string to append to n terminus
\param c_term_out string to append to c terminus

\return Peptide with three letter amino acid codes
*/
std::string utils::oneLetterToThree(std::string seq,
									std::string sep_in, std::string sep_out,
									std::string n_term_out, std::string c_term_out)
{
seq = utils::removeSubstrs(sep_in, seq, false);
std::string ret_temp = "";
std::string add = "";

//check that n term is not a diff mod
for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
	if(seq[0] == *p)
		throw std::runtime_error("Invalid peptide sequence: " + seq);
	
	for(size_t i = 0; i < seq.length(); i++)
	{
		//deal with AA modifications
		for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
		{
			if(seq[i] == *p){
				ret_temp += seq[i];
				seq.erase(i, 1);
				break;
			}
		}
		//exit loop if at end of sequence
		if(i >= seq.length())
			break;
		
		try{
			add = _ONE_LETTER_TO_THREE_MAP.at(seq[i]);
		}catch(std::out_of_range e){
			throw std::out_of_range("Unknown amino acid: " + std::string(1, seq[i]) +
									", in sequence: " + seq);
		}
		utils::addChar(add, ret_temp, sep_out);
	}
	return ((n_term_out.empty() ? "" : n_term_out + sep_out) +
			ret_temp +
			(c_term_out.empty() ? "" : sep_out + c_term_out));
}

/**
\title Convert from 3 letter amino acid codes to 1

\param seq peptide sequence
\param sep_in delimiter between amino acids in input
\param sep_out delimiter between amino acids in output
\param n_term_out string to append to n terminus
\param c_term_out string to append to c terminus

\return Peptide with one letter amino acid codes
*/
std::string utils::threeLetterToOne(std::string seq,
									std::string sep_in, std::string sep_out,
									std::string n_term_out, std::string c_term_out)
{
	seq = utils::removeSubstrs(sep_in, seq, false);
	std::string ret_temp = "";
	std::string add = "";
	
	//check that n term is not a diff mod
	for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
		if(seq[0] == *p)
			throw std::runtime_error("Invalid peptide sequence: " + seq);
	
	for(size_t i = 0; i < seq.length(); i += 3)
	{
	//deal with AA modifications
	for(auto p = utils::MOD_CHARS.begin(); p != utils::MOD_CHARS.end(); p++)
	{
		if(seq[i] == *p){
			ret_temp += seq[i];
			seq.erase(i, 1);
			break;
		}
	}
	//exit loop if at end of sequence
	if(i >= seq.length())
		break;
	
	try{
		std::string temp = seq.substr(i, 3);
		add = _THREE_LETTER_TO_ONE_MAP.at(seq.substr(i, 3));
	}catch(std::out_of_range e){
		throw std::out_of_range("Unknown amino acid: " + std::string(1, seq[i]) +
								", in sequence: " + seq);
	}
	utils::addChar(add, ret_temp, sep_out);
	}
	return ((n_term_out.empty() ? "" : n_term_out + sep_out) +
			ret_temp +
			(c_term_out.empty() ? "" : sep_out + c_term_out));
}

/**
\brief Align \p query to \p ref.

\param query String to align.
\param ref String to use as reference.
\param beg Set to beginning index of \p query in \p ref.
\param end Set to ending index of \p query in \p ref.

\return false if \p query is not in \p ref, true otherwise.
*/
bool utils::allign(const std::string& query, const std::string& ref, size_t& beg, size_t& end)
{
	size_t match = ref.find(query);
	if(match == std::string::npos) return false;
	
	beg = match;
	end = match + query.length() - 1;
	
	return true;
}

/**
\brief Get \p n residues before \p query in \p ref. <br>

If \p n overruns \p ref, the maximum number of characters will be returned.

\param query String to search for.
\param ref String to search in.
\param n Number of residues in output.
\param noExcept Should an std::runtime_error be thrown if \p query is not in \p ref?

\throw std::runtime_error if \p query is not in \p ref and \p noExcept is false.

\return \p n residues before \p query.
*/
std::string utils::nBefore(const std::string& query, const std::string& ref, unsigned n, bool noExcept)
{
	size_t beg, end;
	if(!utils::allign(query, ref, beg, end)){
		if(noExcept) return "";
		else throw std::runtime_error("query not in ref");
	}
	
	if(beg < n) n = beg;

	return ref.substr(beg - n, n);
}

/**
\brief Get \p n residues after \p query in \p ref. <br>

If \p n overruns \p ref, the maximum number of characters will be returned.

\param query String to search for.
\param ref String to search in.
\param n Number of residues in output.
\param noExcept Should an std::runtime_error be thrown if \p query is not in \p ref?

\throw std::runtime_error if \p query is not in \p ref and \p noExcept is false.

\return \p n residues after \p query.
*/
std::string utils::nAfter(const std::string& query, const std::string& ref, unsigned n, bool noExcept)
{
	size_t beg, end;
	if(!utils::allign(query, ref, beg, end)){
		if(noExcept) return "";
		else throw std::runtime_error("query not in ref");
	}
	
	end += 1;
	if(end + n > ref.length())
		n = ref.length() - end;
	return ref.substr(end, n);
}

