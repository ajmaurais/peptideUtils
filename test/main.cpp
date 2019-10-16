
#include <iostream>
#include <vector>

#include <molecularFormula.hpp>

namespace Rcpp{
    typedef std::vector<double> NumericVector;
    typedef std::vector<const char*> StringVector;
}

Rcpp::NumericVector calcMass(const Rcpp::StringVector& sequences,
                             bool monoMass = true,
                             std::string residueAtoms = "")
{
    //get data file paths
    //std::string residueAtomsPath = residueAtoms.empty() ? _getPackageData("defaultResidueAtoms.txt") : residueAtoms;
    char avg_mono = monoMass ? 'm' : 'a';

    //init residues
    utils::Residues residues(residueAtoms);
    if(!residues.initialize()) throw std::runtime_error("Error reading required files for calcMass!");

    size_t len = sequences.size();
    Rcpp::NumericVector ret(len);
    for(size_t i = 0; i < len; i++){
        ret[i] = residues.calcMass(std::string(sequences[i]), avg_mono);
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

    Rcpp::NumericVector masses = calcMass(sequences);
    for(auto m: masses)
        std::cout << m << std::endl;

    return 0;
}