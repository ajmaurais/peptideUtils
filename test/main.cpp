
#include <iostream>
#include <vector>
#include <stdexcept>

#include <fastaFile.hpp>

namespace Rcpp{
    typedef std::vector<double> NumericVector;
    typedef std::vector<std::string> StringVector;
    typedef std::vector<const char*> CharacterVector;
    typedef std::vector<int> IntegerVector;
}

void readFasta(std::string fastaPath = "", long n_entries = 0)
{
    //std::string _fastaPath = fastaPath.empty() ?
    //                         _getPackageData("extdata/Human_uniprot-reviewed_20171020.fasta") : fastaPath;

    std::string _fastaPath = fastaPath;

    //init FastaFile
    utils::FastaFile fasta(true, _fastaPath);
    if(!fasta.read()) throw std::runtime_error("Could not read fasta file!");

    Rcpp::CharacterVector ids, seqs;

    size_t len = fasta.getSequenceCount();
    if(n_entries > len)
        throw std::runtime_error("n_entries more than the number of entries in file!");
    len = n_entries == 0 ? len : n_entries;

    for(size_t i = 0; i < len; i++)
    {
        ids.push_back(fasta.getIndexID(i).c_str());
        seqs.push_back(fasta.at(i).c_str());
    }

    std::cout << "Read " << seqs.size() << " sequences!\n";

    // return Rcpp::DataFrame::create(Rcpp::Named("id") = ids,
    //                                Rcpp::Named("sequence") = seqs,
    //                                Rcpp::Named("stringsAsFactors") = false);
}

void transpose_sequence(const Rcpp::StringVector& peptide_sequences,
                        const Rcpp::NumericVector& quantification,
                        const std::string& protein_seq)
{
    if(peptide_sequences.size() != quantification.size())
        throw std::runtime_error("peptide_sequences and quantification must be the same length!");

    std::vector<char> residues;
    Rcpp::IntegerVector numbers;
    Rcpp::NumericVector quantifications;

    size_t n_seq = peptide_sequences.size();
    size_t begin, end;
    for(size_t i = 0; i < n_seq; i++)
    {
        if(!utils::align(peptide_sequences[i], protein_seq, begin, end))
            throw std::runtime_error("Peptide sequence '" + peptide_sequences[i] + "' does not exist in protein_seq!");

        size_t pep_len = peptide_sequences[i].size();
        for(size_t pep_begin = 0; pep_begin < pep_len; pep_begin++) {
            residues.push_back(peptide_sequences[i][pep_begin]);
            numbers.push_back(pep_begin + begin);
            quantifications.push_back(quantification[i]);
        }
    }

    // return Rcpp::DataFrame::create(Rcpp::Named("residue") = residues,
    //                                Rcpp::Named("number") = numbers,
    //                                Rcpp::Named("quant") = quantifications);
}

int main() {

    utils::FastaFile fastaFile;
    std::string fname = "/Users/Aaron/Documents/School_Work/analysis/chatterjee_lab_PW_incorporation/percent_W_Y_in_proteome/data/human_proteome.fasta";
    fastaFile.read(fname);
    // std::string pfkl_seq = fastaFile.getSequence("P17858");

    readFasta(fname);

    // const std::string seqs [] = {"AAAYNLVQHGITNLCVIGGDGSLTGANIFR",
    //     "AAAYNLVQHGITNLCVIGGDGSLTGANIFR", "AAAYNLVQHGITNLCVIGGDGSLTGANIFR",
    //     "AIGVLTSGGDAQGMNAAVR", "AIGVLTSGGDAQGMNAAVR", "AIGVLTSGGDAQGMNAAVR",
    //     "AIGVLTSGGDAQGMNAAVR", "AMDDKRFDEATQLR", "AMDDKRFDEATQLR", "AMDDKRFDEATQLR"};

    // const double ratios [] = {0.05, 0.05, 0.05, 0.997914597815293, 1.96910994764398,
    //     0.997914597815293, 1.9673647469459, 1.57885220125786,
    //     0.402500267122556, 1.61120882635114};

    // std::vector <std::string> seqs_v(seqs, seqs + 10);
    // std::vector <double> ratios_v(ratios, ratios + 10);

    // transpose_sequence(seqs_v, ratios_v, pfkl_seq);

    return 0;
}