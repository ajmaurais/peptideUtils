
#include <iostream>
#include <vector>

#include <msInterface/mzXMLFile.hpp>
#include <msInterface/mzMLFile.hpp>
#include <msInterface/msScan.hpp>
#include <msInterface/ms2File.hpp>

// int main() {
//     std::string ifname = "/Volumes/Data/msData/ionFinder/another_another_bug/20190912_Thompson_PAD1_GlucTryp_t2.mzML";
//     //std::string ifname = "/Volumes/Data/msData/ionFinder/another_another_bug/20190917_Thompson_PAD62_GlucTryp_t4.ms2";
//     //std::string ifname = "/Volumes/Data/msData/ionFinder/another_another_bug/full_mzXML/20190912_Thompson_PAD1_GlucTryp_t1.mzXML";
//
//     utils::msInterface::MzMLFile inputFile;
//     //utils::MzXMLFile inputFile;
//     //utils::Ms2File inputFile;
//     utils::msInterface::Scan scan;
//     assert(inputFile.read(ifname));
//     size_t nScans = inputFile.getScanCount();
//     nScans = 1;
//
//     std::cout << "Found " << inputFile.getScanCount() << " scans total.\n";
//     std::cout << "First scan: " << inputFile.getFirstScan() << NEW_LINE;
//     std::cout << "Last scan: " << inputFile.getLastScan() << NEW_LINE;
//     int scanCount = 0;
//     for(size_t i = inputFile.getFirstScan(); i < inputFile.getLastScan(); i = inputFile.nextScan(i)){
//         if(scanCount > nScans)
//             break;
//         if(!inputFile.getScan(i, scan))
//             std::cout << "Fuck scan " << i << NEW_LINE;
//         scanCount++;
//     }
//
//     return 0;
// }

int main()
{
    std::string dir = "/Volumes/Data/msData/ionFinder/another_another_bug/";
    std::string ms2_fname = dir + "20190912_Thompson_PAD2_GlucTryp_t3.ms2";
    std::string mzml_fname = dir + "20190912_Thompson_PAD2_GlucTryp_t3.mzML";

    std::string mzmlTest = dir + "20190912_Thompson_PAD1_GlucTryp_t3.mzML";
    utils::msInterface::MzMLFile testFile(mzmlTest);
    assert(testFile.read());

    utils::msInterface::MzMLFile mzMlFile(mzml_fname);
    utils::msInterface::Ms2File ms2File(ms2_fname);
    assert(mzMlFile.read());
    assert(ms2File.read());

    std::string scanList [] = {"17602", "16730", "13003"};

    utils::msInterface::Scan ms2Scan, mzMLScan;
    //for(auto it = std::begin(scanList); it != std::end(scanList); ++it)
    for(size_t i = mzMlFile.getFirstScan(); i < mzMlFile.getLastScan(); i = mzMlFile.nextScan(i))
    {
        //size_t scan = std::stoul(*it);
        size_t scan = i;
        assert(mzMlFile.getScan(scan, mzMLScan));
        assert(ms2File.getScan(scan, ms2Scan));

        ms2Scan.setPolarity(mzMLScan.getPolarity());
        ms2Scan.getPrecursor().setScan(mzMLScan.getPrecursor().getScan());
        ms2Scan.getPrecursor().setFile(mzMLScan.getPrecursor().getFile());
        ms2Scan.getPrecursor().setRT(mzMLScan.getPrecursor().getRT());
        ms2Scan.getPrecursor().setActivationMethod(mzMLScan.getPrecursor().getActivationMethod());

        bool eq = (ms2Scan == mzMLScan);
        bool almostEq = ms2Scan.almostEqual(mzMLScan, 0.051);
        if(!almostEq) {
            std::cout << "Read scan " << scan << "... ";
            std::cout << (eq ? "equal" : "neq");
            std::cout << ", ";
            std::cout << (almostEq ? "almost eq" : "NOT ALMOST EQUAL!!!");
            std::cout << "\n";
        }
    }

    return 0;
}
