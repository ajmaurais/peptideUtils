
#include <iostream>
#include <vector>
#include <stdexcept>

#include <fastaFile.hpp>
#include <molecularFormula.hpp>

namespace Rcpp{
    typedef std::vector<double> NumericVector;
    typedef std::vector<std::string> StringVector;
    typedef std::vector<const char*> CharacterVector;
    typedef std::vector<int> IntegerVector;
}

void digest(Rcpp::CharacterVector sequences, Rcpp::CharacterVector ids,
                  unsigned nMissedCleavages = 0, std::string cleavagePattern = "([RK])(?=[^P])",
                  bool mz_filter = true, std::string residueAtoms = "",
                  double minMz = 400, double maxMz = 1800,
                  int minCharge = 1, int maxCharge = 5,
                  size_t minLen = 6, size_t maxLen = 0)
{
    //get file paths for atom mass tables
    std::string residueAtomsPath;

    if(mz_filter){
        //residueAtomsPath = residueAtoms.empty() ? _getPackageData("defaultResidueAtoms.txt") : residueAtoms;
        residueAtomsPath = "/Users/Aaron/code/peptideUtils/peptideUtils/inst/defaultResidueAtoms.txt";
    }

    //check args
    if(ids.size() != sequences.size())
        throw std::runtime_error("sequences and ids must be the same length!");

    size_t _maxLen = maxLen == 0 ? std::string::npos : maxLen;

    size_t len = sequences.size();
    for(size_t i = 0; i < len; i++)
    {
        std::vector<std::string> peptides_temp;
        Rcpp::CharacterVector ret_temp;
        if(mz_filter)
        {
            //init residues
            utils::Residues residues(residueAtomsPath);
            if(!residues.initialize())
            throw std::runtime_error("Error reading required files for calcMass!");

            residues.digest(utils::removeWhitespace(std::string(sequences[i])), peptides_temp, nMissedCleavages, false, cleavagePattern,
                            minMz, maxMz, minCharge, maxCharge);

            std::sort(peptides_temp.begin(), peptides_temp.end(), utils::strLenCompare());
            for(auto it = peptides_temp.begin(); it != peptides_temp.end(); ++it)
            {
                size_t len_temp = it->length();
                if(len_temp >= minLen && (_maxLen == std::string::npos ? true : len_temp <= _maxLen))
                    ret_temp.push_back(it->c_str());
            }

        }//end if mz_filter
        else{
            utils::digest(utils::removeWhitespace(std::string(sequences[i])), peptides_temp,
                          nMissedCleavages, minLen, _maxLen, cleavagePattern);
            for(auto it = peptides_temp.begin(); it != peptides_temp.end(); ++it){
                ret_temp.push_back(it->c_str());
            }
        }
    }
}

void readFasta(std::vector<const char*>& ids, std::vector<const char*>& sequnces,
               std::string fastaPath = "", long n_entries = 0)
{
    //std::string _fastaPath = fastaPath.empty() ?
    //                         _getPackageData("extdata/Human_uniprot-reviewed_20171020.fasta") : fastaPath;

    std::string _fastaPath = fastaPath;

    //init FastaFile
    utils::FastaFile fasta(true, _fastaPath);
    if(!fasta.read()) throw std::runtime_error("Could not read fasta file!");

    size_t len = fasta.getSequenceCount();
    if(n_entries > len)
        throw std::runtime_error("n_entries more than the number of entries in file!");
    len = n_entries == 0 ? len : n_entries;

    for(size_t i = 0; i < len; i++)
    {
        ids.push_back(fasta.getIndexID(i).c_str());
        sequnces.push_back(fasta.at(i).c_str());
    }

    std::cout << "Read " << sequnces.size() << " sequences!\n";

    // return Rcpp::DataFrame::create(Rcpp::Named("id") = ids,
    //                                Rcpp::Named("sequence") = seqs,
    //                                Rcpp::Named("stringsAsFactors") = false);
}


int main() {

    utils::FastaFile fastaFile;
    std::string fname = "/Users/Aaron/Documents/School_Work/analysis/chatterjee_lab_PW_incorporation/percent_W_Y_in_proteome/data/human_proteome.fasta";
    fastaFile.read(fname);
    // std::string pfkl_seq = fastaFile.getSequence("P17858");

    std::vector<const char*> ids, sequences;
    readFasta(ids, sequences, fname, 10);

    digest(ids, sequences);


    return 0;
}