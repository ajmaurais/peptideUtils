// 
// peptide_utils.hpp
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

#ifndef peptide_utils_hpp
#define peptide_utils_hpp

#include <iostream>
#include <stdexcept>
#include <map>
#include <thread>

#include <utils.hpp>
#include <molecularFormula.hpp>
#include <fastaFile.hpp>

namespace utils{

    typedef std::map<std::string, std::vector<std::string> > SeqListType;
    typedef std::pair<size_t, size_t> SizePair;

    //!Characters representing dynamic modifications
    const std::string MOD_CHARS = "*";

    //! Trypsin regex pattern
    const std::string TRYPSIN_RE = "([RK])(?=[^P])";

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

    void _digest_all_len(unsigned int begin, unsigned int end,
        const utils::FastaFile& fasta, SeqListType& seqs,
        unsigned nMissedCleavages = 0, size_t minLen = 6, size_t maxLen = std::string::npos,
        std::string cleavagePattern = TRYPSIN_RE);

    void _digest_all_res(unsigned int begin, unsigned int end,
        const utils::FastaFile& fasta, const utils::Residues& residues, SeqListType& seqs,
        unsigned nMissedCleavages = 0, bool length_filter = true,
        std::string cleavagePattern = TRYPSIN_RE,
        double minMz = 400, double maxMz = 1800, int minCharge = 1, int maxCharge = 5);

    void digest_all(const utils::FastaFile& fasta, const utils::Residues& residues, SeqListType& peptides, unsigned int nThread = 0,
        unsigned nMissedCleavages = 0, bool length_filter = true,
        std::string cleavagePattern = TRYPSIN_RE,
        double minMz = 400, double maxMz = 1800, int minCharge = 1, int maxCharge = 5);

    void digest_all(const utils::FastaFile& fasta, SeqListType& peptides, unsigned int nThread = 0,
        unsigned nMissedCleavages = 0, size_t minLen = 6, size_t maxLen = std::string::npos,
        std::string cleavagePattern = TRYPSIN_RE);

    std::string getModLocs(std::string seq, std::vector<int>& modLocs);
    void seqToIons(const std::string& s, std::map<std::string, SizePair>& ions,
                   bool addB = true, bool addY = true);

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
