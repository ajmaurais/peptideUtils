// 
// peptide_utils.cpp
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
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR  OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// -----------------------------------------------------------------------------
// 

#include <sequenceUtils.hpp>

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

//!This function should not be called directly.
void utils::_digest_all_res(unsigned int begin, unsigned int end,
                            const utils::FastaFile& fasta, const utils::Residues& residues, utils::SeqListType& seqs,
                            unsigned nMissedCleavages, bool length_filter,
                            std::string cleavagePattern,
                            double minMz, double maxMz, int minCharge, int maxCharge)
{
    for(unsigned int i = begin; i < end; i++)
    {
        std::vector<std::string> seq_temp;
        residues.digest(fasta.at(i), seq_temp,
                        nMissedCleavages, length_filter, cleavagePattern,
                        minMz, maxMz, minCharge, maxCharge);
        seqs[fasta.getIndexID(i)] = seq_temp;
    }
}

//!This function should not be called directly.
void utils::_digest_all_len(unsigned int begin, unsigned int end,
                            const utils::FastaFile& fasta, utils::SeqListType& seqs,
                            unsigned nMissedCleavages, size_t minLen, size_t maxLen,
                            std::string cleavagePattern)
{
    for(unsigned int i = begin; i < end; i++)
    {
        std::vector<std::string> seq_temp;
        utils::digest(fasta.at(i), seq_temp, nMissedCleavages, minLen, maxLen, cleavagePattern);
        seqs[fasta.getIndexID(i)] = seq_temp;
    }
}

/**
\brief Digest all proteins in a FastaFile. <br>

By default, the digest is performed in parallel on all logical cores on the system.

\param fasta Initialized    FastaFile to pull sequences from.
\param peptides Populated with digested peptides for each protein in \p fasta. 
\param nThread Number of threads to use. If 0, \p std::thread::hardware_concurrency() threads are used.<br>

Additional arguments are passed to utils::digest.

\param missedCleavages Number of missed cleavages to allow.
\param minLen Minimum peptide length.
\param maxLen Maximum peptide length. Set to std::string::npos for no upper bound on length.
\param cleavagePattern RegEx for protease cleavage pattern.
*/
void utils::digest_all(const utils::FastaFile& fasta, utils::SeqListType& peptides, unsigned int nThread,
                       unsigned nMissedCleavages, size_t minLen, size_t maxLen,
                       std::string cleavagePattern)
{
    typedef utils::SeqListType ValsType;
    
    const unsigned int _nThread = nThread == 0 ? std::thread::hardware_concurrency() : nThread;
    size_t const nVals = fasta.getSequenceCount();
    size_t perThread = nVals / _nThread;
    if(nVals % _nThread != 0)
        perThread += 1;
    
    std::vector<std::thread> threads;
    
    ValsType* split_vals = new ValsType[_nThread];
    size_t begNum, endNum ;
    unsigned int threadIndex = 0;
    for(size_t i = 0; i < nVals; i += perThread)
    {
        begNum = i;
        endNum = (begNum + perThread > nVals ? nVals : begNum + perThread);
        
        //spawn thread
        assert(threadIndex < _nThread);
        threads.push_back(std::thread(_digest_all_len, begNum, endNum,
                                      std::ref(fasta), std::ref(split_vals[threadIndex]),
                                      nMissedCleavages, minLen, maxLen, cleavagePattern));
        
        threadIndex++;
    }
    
    //join threads
    for(auto it = threads.begin(); it != threads.end(); ++it){
        it->join();
    }
    
    peptides.clear();
    for(unsigned int i = 0; i < nThread; i++){
        peptides.insert(split_vals[i].begin(), split_vals[i].end());
    }
    
    delete [] split_vals;
}

/**
\brief Digest all proteins in a FastaFile. <br>

By default, the digest is performed in parallel on all logical cores on the system.

\param fasta Initialized FastaFile to pull sequences from.
\param residues Initialized Residues object to use for \p m/z filter.
\param peptides Populated with digested peptides for each protein in \p fasta. 
\param nThread Number of threads to use. If 0, \p std::thread::hardware_concurrency() threads are used.<br>

Additional arguments are passed to Residues::digest.

\param missedCleavages number of missed cleavages to allow.
\param length_filter Should peptides with less than 6 amino acids be automatically excluded?
\param cleavagePattern RegEx for protease cleavage pattern.
\param minMz Minimum m/z to allow in \p peptides.
\param maxMz Maximum m/z to allow in \p peptides. Set to 0 for no upper bound on m/z.
\param minCharge Minimum charge to consider when calculating m/z.
\param maxCharge Maximum charge to consider when calculating m/z.
*/
void utils::digest_all(const utils::FastaFile& fasta, const utils::Residues& residues, utils::SeqListType& peptides, unsigned int nThread,
                       unsigned nMissedCleavages, bool length_filter,
                       std::string cleavagePattern,
                       double minMz, double maxMz, int minCharge, int maxCharge)
{
    typedef utils::SeqListType ValsType;
    
    const unsigned int _nThread = nThread == 0 ? std::thread::hardware_concurrency() : nThread;
    size_t const nVals = fasta.getSequenceCount();
    size_t perThread = nVals / _nThread;
    if(nVals % _nThread != 0)
        perThread += 1;
    
    std::vector<std::thread> threads;
    
    ValsType* split_vals = new ValsType[_nThread];
    size_t begNum, endNum ;
    unsigned int threadIndex = 0;
    for(size_t i = 0; i < nVals; i += perThread)
    {
        begNum = i;
        endNum = (begNum + perThread > nVals ? nVals : begNum + perThread);
        
        //spawn thread
        assert(threadIndex < _nThread);
        threads.push_back(std::thread(_digest_all_res, begNum, endNum,
                                      std::ref(fasta), std::ref(residues), std::ref(split_vals[threadIndex]),
                                      nMissedCleavages, length_filter, cleavagePattern,
                                      minMz, maxMz, minCharge, maxCharge));
        
        threadIndex++;
    }
    
    //join threads
    for(auto it = threads.begin(); it != threads.end(); ++it){
        it->join();
    }
    
    peptides.clear();
    for(unsigned int i = 0; i < nThread; i++){
        peptides.insert(split_vals[i].begin(), split_vals[i].end());
    }
    
    delete [] split_vals;
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
        }catch(std::out_of_range const& e){
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
    }catch(std::out_of_range const& e){
        throw std::out_of_range("Unknown amino acid: " + std::string(1, seq[i]) +
                                ", in sequence: " + seq);
    }
    utils::addChar(add, ret_temp, sep_out);
    }
    return ((n_term_out.empty() ? "" : n_term_out + sep_out) +
            ret_temp +
            (c_term_out.empty() ? "" : sep_out + c_term_out));
}

