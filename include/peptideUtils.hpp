//
//  peptide_utils.hpp
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

#ifndef peptide_utils_hpp
#define peptide_utils_hpp

#include <iostream>
#include <regex>
#include <stdexcept>
#include <map>

#include <utils.hpp>

namespace utils{

	//!Characters representing dynamic modifications
	const std::string MOD_CHARS = "*";

	//!Maps 1 letter amino acids to 3.
	const std::map<char, std::string> _ONE_LETTER_TO_THREE_MAP = {{'A', "Ala"},
																  {'C', "Cys"},
																  {'D', "Asp"},
																  {'E', "Glu"},
																  {'F', "Phe"},
																  {'G', "Gly"},
																  {'H', "His"},
																  {'I', "Ile"},
																  {'K', "Lys"},
																  {'L', "Leu"},
																  {'M', "Met"},
																  {'N', "Asn"},
																  {'P', "Pro"},
																  {'Q', "Gln"},
																  {'R', "Arg"},
																  {'S', "Ser"},
																  {'T', "Thr"},
																  {'U', "Sec"},
																  {'V', "Val"},
																  {'W', "Trp"},
																  {'Y', "Tyr"}};

	//!Maps 3 letter amino acids to 1.
	const std::map<std::string, char> _THREE_LETTER_TO_ONE_MAP = {{"Ala", 'A'},
																  {"Cys", 'C'},
																  {"Asp", 'D'},
																  {"Glu", 'E'},
																  {"Phe", 'F'},
																  {"Gly", 'G'},
																  {"His", 'H'},
																  {"Ile", 'I'},
																  {"Lys", 'K'},
																  {"Leu", 'L'},
																  {"Met", 'M'},
																  {"Asn", 'N'},
																  {"Pro", 'P'},
																  {"Gln", 'Q'},
																  {"Arg", 'R'},
																  {"Ser", 'S'},
																  {"Thr", 'T'},
																  {"Sec", 'U'},
																  {"Val", 'V'},
																  {"Trp", 'W'},
																  {"Tyr", 'Y'}};

void digest(std::string seq, std::vector<std::string>& peptides,
	unsigned nMissedCleavages = 0, size_t minLen = 6, size_t maxLen = std::string::npos,
	std::string cleavagePattern = "([RK])([^P])");

std::string getModLocs(std::string seq, std::vector<int>& modLocs);

std::string oneLetterToThree(std::string seq,
							 std::string sep_in = "",
							 std::string sep_out = "",
							 std::string n_term_out = "",
							 std::string c_term_out = "");

std::string threeLetterToOne(std::string seq,
							 std::string sep_in = "",
							 std::string sep_out = "",
							 std::string n_term_out = "",
							 std::string c_term_out = "");

}//end namespace utils

#endif
