
#include <iostream>
#include <vector>

#include <molecularFormula.hpp>

namespace Rcpp{
    typedef std::vector<double> NumericVector;
    typedef std::vector<std::string> StringVector;
}

Rcpp::NumericVector calcMass(const Rcpp::StringVector& sequences,
                             bool monoMass,
                             std::string residueAtoms)
{
    //get data file paths
    //std::string residueAtomsPath = residueAtoms.empty() ? _getPackageData("defaultResidueAtoms.txt") : residueAtoms;
    std::string residueAtomsPath = residueAtoms;
    char avg_mono = monoMass ? 'm' : 'a';

    //init residues
    utils::Residues residues(residueAtomsPath);
    if(!residues.initialize(false)) throw std::runtime_error("Error reading required files for calcMass!");

    size_t len = sequences.size();
    Rcpp::NumericVector ret(len);
    for(size_t i = 0; i < len; i++){
        ret[i] = residues.calcMass(std::string(sequences[i]), avg_mono);
    }

    return ret;
}


int main() {

    Rcpp::StringVector sequences;
    const char* seqs[] = {"SSGIHYGVITCEGCKGFFR*", "GFFR*R*SQQCNVAYSCTR", "R*SQQCNVAYSCTR*",
                           "QQNCPIDR*TSR", "QQNCPIDR*TSR*NR", "CLALGMSR*DAVK", "KQR*DSLHAEVQK",
                           "KQR*DSLHAEVQKQLQQQQQQEQVAK", "QR*DSLHAEVQK", "QR*DSLHAEVQKQLQQQQQQEQVAK",
                           "TPPAGSR*GADTLTYTLGLSDGQLPLGASPDLPEASACPPGLLR*", "TEVQGASCHLEYSPER*",
                           "TEVQGASCHLEYSPER*GK", "TEVQGASCHLEYSPER*GKAEGR", "TEVQGASCHLEYSPER*GKAEGR*DSIYSTDGQLTLGR",
                           "TEVQGASCHLEYSPER*GKAEGR*DSIYSTDGQLTLGR*", "GKAEGR*DSIYSTDGQLTLGR",
                           "GKAEGR*DSIYSTDGQLTLGR", "GKAEGR*DSIYSTDGQLTLGR*", "AEGR*DSIYSTDGQLTLGR",
                           "AEGR*DSIYSTDGQLTLGR", "AEGR*DSIYSTDGQLTLGR*", "AEGR*DSIYSTDGQLTLGR*CGLR",
                           "DSIYSTDGQLTLGR*", "DSIYSTDGQLTLGR*CGLR", "FEETR*HPELGEPEQGPDSH",
                           "SFR*ETCQLRLEDLLR", "LEDLLR*QR", "LEDLLR*QR*TNLFSR", "LEDLLR*QR*TNLFSR",
                           "LEDLLR*QR*TNLFSR*EEVTSYQR", "LEDLLR*QR*TNLFSR*EEVTSYQR", "LEDLLR*QR*TNLFSR*EEVTSYQR*",
                           "QRTNLFSR*EEVTSYQR", "QR*TNLFSR*EEVTSYQR", "QR*TNLFSR*EEVTSYQR",
                           "QR*TNLFSR*EEVTSYQR*", "QR*TNLFSR*EEVTSYQR*K", "QR*TNLFSR*EEVTSYQR*K",
                           "TNLFSREEVTSYQR*", "TNLFSR*EEVTSYQR", "TNLFSR*EEVTSYQR", "TNLFSR*EEVTSYQR*",
                           "TNLFSREEVTSYQR*K", "TNLFSR*EEVTSYQR*K", "TNLFSR*EEVTSYQR*K",
                           "KSMWEMWER*", "SMWEMWER*", "YGGVELFR*", "R*RVEHLQYNLELAFHHHLCK",
                           "R*R*VEHLQYNLELAFHHHLCK", "R*R*VEHLQYNLELAFHHHLCK", "R*RVEHLQYNLELAFHHHLCKTHR",
                           "RRVEHLQYNLELAFHHHLCKTHR*", "R*VEHLQYNLELAFHHHLCK", "R*VEHLQYNLELAFHHHLCKTHR*",
                           "R*VEHLQYNLELAFHHHLCKTHR*QGLLAK", "VEHLQYNLELAFHHHLCKTHR*", "LPPKGKLR*SLCSQHVEKLQIFQHLHPIVVQAAFPPLYK",
                           "LPPKGKLR*SLCSQHVEKLQIFQHLHPIVVQAAFPPLYK", "LR*SLCSQHVEKLQIFQHLHPIVVQAAFPPLYK",
                           "LR*SLCSQHVEKLQIFQHLHPIVVQAAFPPLYK", "KASWTWGPEGQGAILLVNCDR*",
                           "VYSKQDLQDMSQMILR*", "ILIGSSFPLSGGR*", "FLGEVHCGTNVR*R", "WELLQQVNTSTR*",
                           "LAILGYR*R*", "LAILGYR*R*"};

    for(auto s:seqs)
        sequences.push_back(s);

    Rcpp::NumericVector formulas = calcMass(sequences, true, "/Users/Aaron/local/envFinder/testFiles/cit_residue_atoms.txt");
    for(auto m: formulas)
        std::cout << m << std::endl;

    return 0;
}