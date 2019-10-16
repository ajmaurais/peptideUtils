
#include <iostream>
#include <vector>

#include <molecularFormula.hpp>

namespace Rcpp{
    typedef std::vector<double> NumericVector;
    typedef std::vector<std::string> StringVector;
}

Rcpp::StringVector calcFormula(const Rcpp::StringVector& sequences,
                               bool subscripts = false,
                               std::string residueAtomsPath = "")
{
    //get data file paths
    //std::string residueAtomsPath = residueAtoms.empty() ? _getPackageData("defaultResidueAtoms.txt") : residueAtoms;

    //init residues
    utils::Residues residues;
    if(!residues.initialize())
        throw std::runtime_error("Error reading required files for calcFormula!");

    size_t len = sequences.size();
    Rcpp::StringVector ret(len);
    for(size_t i = 0; i < len; i++){
        ret[i] = residues.calcFormula(std::string(sequences[i]), subscripts);
    }

    return ret;
}

int main() {

    Rcpp::StringVector sequences;
    const char* seqs[] = {"ACLLPETVNMEEYPYDAEY", "ALCAEFK", "AQCPIVER", "CTGGEVGATSALAPK",
                     "IVSNASCTTNCLAPLAK", "LDADIHTNTCR", "LLYVSCNPR", "LPACVVDCGTGYTK",
                     "MIVECVMNNATCTR", "SEGLPSECR", "TPCGEGSK"};

    for(auto s:seqs)
        sequences.push_back(s);

    Rcpp::StringVector formulas = calcFormula(sequences);
    for(auto m: formulas)
        std::cout << m << std::endl;

    return 0;
}