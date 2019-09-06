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

