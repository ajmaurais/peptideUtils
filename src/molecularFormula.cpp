//
//  molecularFormula.cpp
//  utils
// -----------------------------------------------------------------------------
// Copyright 2018 Aaron maurais
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

#include <molecularFormula.hpp>

//!Copy constructor
utils::Residue::Residue(const utils::Residue& rhs)
{
	//copy atomMassMap
	atomMassMap = new AtomMassMapType;
	*atomMassMap = *rhs.atomMassMap;
	
	//copy other members
	masses = rhs.masses;
	atomCountMap = rhs.atomCountMap;
	massesSupported = rhs.massesSupported;
}

//!Copy assignment
utils::Residue& utils::Residue::operator = (utils::Residue rhs)
{
	std::swap(atomMassMap, rhs.atomMassMap);

	//other vars
	masses = rhs.masses;
	atomCountMap = rhs.atomCountMap;
	massesSupported = rhs.massesSupported;

	return *this;
}

void utils::Residue::calcMasses()
{
	masses = utils::Species(0, 0);
	for(AtomCountMapType::const_iterator it = atomCountMap.begin(); it != atomCountMap.end(); ++it)
	{
		//check that query exists in atomMassMap
		if(atomMassMap->find(it->first) == atomMassMap->end())
			throw std::runtime_error(it->first + " not found in mass map!");
		
		//add mass * count to *this masses
		masses += ((*atomMassMap)[it->first] * it->second);
	}
}

//removes atoms from atomCountMap which have count of 0
void utils::Residue::removeZeros()
{
	AtomCountMapType::iterator it = atomCountMap.begin();
	while(it != atomCountMap.end())
	{
		if(it->second == 0){
			atomCountMap.erase(it++);
		} else {
			++it;
		}
	}
}

void utils::Residue::_checkIfMassesSupported() const
{
	if(!massesSupported)
		throw std::runtime_error("Atom mass file has not been initialized.");
}

void utils::Residue::initialize(const utils::HeaderType& _header,
								const std::vector<std::string>& _elems)
{
	//check that atom count std::vector is same len as header
	assert(_header.size() == _elems.size());
	
	//initialize atom count map
	size_t len = _header.size();
	for(size_t i = 0; i < len; i++)
		atomCountMap[_header[i]] = std::stoi(_elems[i]);
	removeZeros(); //remove 0 atom counts
	massesSupported = false;
}

void utils::Residue::initialize(utils::AtomMassMapType* _atomMassMap,
									const utils::HeaderType& _header,
									const std::vector<std::string>& _elems)
{
	initialize(_header, _elems);
	
	atomMassMap = new AtomMassMapType;
	atomMassMap = _atomMassMap;
	
	calcMasses(); //calculate residue mass
	
	massesSupported = true;
}

/**
 Get mass of residue.

 \param avg_mono 'a' for average mass, 'm' for monoisotopic mass
 \param checkIfMassesSupported Should a check be performed to see
 whether the atom mass table has been initialized?

 \return residue mass
*/
double utils::Residue::getMass(char avg_mono,
							   bool checkIfMassesSupported) const{
	switch(avg_mono){
		case 'a' :
			return getAvg(checkIfMassesSupported);
			break;
		case 'm' :
			return getMono(checkIfMassesSupported);
			break;
		default :
			throw std::runtime_error("only a or m are valid arguments!");
	}
}

/**
 Get monoisotopic mass for residue.

 \param checkIfMassesSupported Should a check be performed to see
 whether the atom mass table has been initialized?

 \return monoisotopic mass
*/
double utils::Residue::getMono(bool checkIfMassesSupported) const
{
	if(checkIfMassesSupported) _checkIfMassesSupported();
	return masses.getMono();
}

/**
 Get average mass for residue.

 \param checkIfMassesSupported Should a check be performed to see
 whether the atom mass table has been initialized?

 \return average mass
*/
double utils::Residue::getAvg(bool checkIfMassesSupported) const
{
	if(checkIfMassesSupported) _checkIfMassesSupported();
	return masses.getAvg();
}

/**
 Read atom count table. <br>

 Call this function to initialize only the atom count table, 
 when residue masses are not required.

 \param _atomCountTableLoc path to atom count table file
 \return true if IO was successful
*/
bool utils::Residues::readAtomCountTable(std::string _atomCountTableLoc)
{
	atomCountTableLoc = _atomCountTableLoc;
	return readAtomCountTable();
}

bool utils::Residues::readAtomCountTable()
{
	if(atomCountTableLoc.empty())
		throw std::runtime_error("atomCountTableLoc must be specified");
	
	std::ifstream inF(atomCountTableLoc);
	if(!inF) return false;
	
	std::string line;
	std::vector<std::string> elems;
	while(utils::safeGetline(inF, line))
	{
		line = utils::trim(line);
		if(line.empty() || utils::isCommentLine(line))
			continue;
		
		line = line.substr(0, line.find(";"));
		utils::split(line, IN_DELIM, elems);
		utils::trimAll(elems);
		
		if(elems[0] == "H" || elems[0] == "R"){
			if(elems[0] == "H"){
				//remove first and second elements
				elems.erase(elems.begin(), elems.begin() + 2);
				atomCountHeader = elems;
			}
			else if(elems[0] == "R")
			{
				//get residue symbol
				std::string residue = elems[1];
				
				//remove first and second elements
				elems.erase(elems.begin(), elems.begin() + 2);
				
				if(atomCountHeader.size() != elems.size())
					throw std::runtime_error("bad atom count table");
				
				if(atomMassTalbeRead)
					residueMap[residue] = utils::Residue(&atomMassMap, atomCountHeader, elems);
				else residueMap[residue] = utils::Residue(atomCountHeader, elems);
			}
		}
	}
	atomCountTableRead = true;
	return true;
}

void utils::Residue::combineAtomCountMap(AtomCountMapType& _add) const
{
	for(AtomCountMapType::const_iterator it = atomCountMap.begin(); it != atomCountMap.end(); ++it)
	{
		if(_add.find(it->first) == _add.end())
			_add[it->first] = it->second;
		else _add[it->first] += it->second;
	}
}

bool utils::Residues::readAtomMassTable(std::string _massTableLoc)
{
	massTableLoc = _massTableLoc;
	return readAtomCountTable();
}

bool utils::Residues::readAtomMassTable()
{
	if(massTableLoc.empty())
		throw std::runtime_error("atomCountTableLoc must be specified");
	
	std::ifstream inF(massTableLoc);
	if(!inF) return false;
	
	std::string line;
	std::vector<std::string> elems;
	while(utils::safeGetline(inF, line))
	{
		line = utils::trim(line);
		if(line.empty() || utils::isCommentLine(line))
			continue;		
		
		utils::split(line, IN_DELIM, elems);
		utils::trimAll(elems);
		
		if(elems[0] == "H" || elems[0] == "A")
		{
			if(elems.size() != 4)
				throw std::runtime_error("Bad atom mass ");
		
			if(elems[0] == "H"){
				continue;
			}
			else if(elems[0] == "A"){
				atomMassMap[elems[1]] = utils::Species(std::stod(elems[2]),
															std::stod(elems[3]));
			}
		}//end if
	}//end while
	atomMassTalbeRead = true;
	return true;
}//end fxn

bool utils::Residues::initialize(std::string _atomCountTableLoc, std::string _massTableLoc)
{
	atomCountTableLoc = _atomCountTableLoc;
	massTableLoc = _massTableLoc;
	
	return initialize();
}

/**
\brief Initialise Residues with atom and residue mass data.
\param default Should the default residue mass files be used?
*/
bool utils::Residues::initialize(bool use_default)
{
	if(use_default){
		atomCountTableLoc = std::string(SHARE_DIR) + "/" + ATOM_COUNT_NAME;
		massTableLoc = std::string(SHARE_DIR) + "/" + MASS_TABLE_NAME;
	}

	bool gootAtomMassTable = readAtomMassTable();
	bool goodAtomCountTable = readAtomCountTable();
	
	return gootAtomMassTable && goodAtomCountTable;
}

double utils::Residues::calcMass(std::string _seq, char avg_mono, bool _nterm, bool _cterm) const
{
	//check that required files have been read
	if(!atomCountTableRead)
		throw std::runtime_error("Atom count table has not been read in!");
	if(!atomMassTalbeRead)
		throw std::runtime_error("Atom mass table has not been read in!");

	double mass = 0;
	size_t len = _seq.length();
	ResidueMapType::const_iterator it;
	
	if(_nterm)
	{
		it = residueMap.find(N_TERM_STR);
		if(it == residueMap.end())
			throw std::runtime_error(N_TERM_STR + " not found in residueMap");
		mass += it->second.getMass(avg_mono);
	}
	
	for(size_t i = 0; i < len; i++)
	{
		std::string aaTemp = std::string(1, _seq[i]);
		it = residueMap.find(aaTemp);
		if(it == residueMap.end())
			return -1;
		mass += it->second.getMass(avg_mono);
	}
	
	if(_cterm)
	{
		it = residueMap.find(C_TERM_STR);
		if(it == residueMap.end())
			throw std::runtime_error(C_TERM_STR + " not found in residueMap");
		mass += it->second.getMass(avg_mono);
	}
	
	return mass;
}

std::string utils::Residues::calcFormula(std::string _seq, bool unicode,
											  bool _nterm, bool _cterm) const
{
	//check that required files have been read
	if(!atomCountTableRead)
		throw std::runtime_error("Atom count table has not been read in!");

	std::string formula = "";
	AtomCountMapType atomCounts;
	ResidueMapType::const_iterator resMapIt;
	if(_nterm)
	{
		resMapIt = residueMap.find(N_TERM_STR);
		if(resMapIt == residueMap.end())
			throw std::runtime_error(N_TERM_STR + " not found in residueMap");
		resMapIt->second.combineAtomCountMap(atomCounts);
	}
	for(std::string::iterator it = _seq.begin(); it != _seq.end(); ++it)
	{
		std::string aaTemp = std::string(1, *it);
		resMapIt = residueMap.find(aaTemp);
		if(resMapIt == residueMap.end())
			return BAD_AMINO_ACID;
		resMapIt->second.combineAtomCountMap(atomCounts);
	}
	if(_cterm)
	{
		resMapIt = residueMap.find(C_TERM_STR);
		if(resMapIt == residueMap.end())
			throw std::runtime_error(C_TERM_STR + " not found in residueMap");
		resMapIt->second.combineAtomCountMap(atomCounts);
	}
	
	return getFormulaFromMap(atomCounts, unicode);
}

std::string utils::getFormulaFromMap(const utils::AtomCountMapType& atomCountMap, bool unicode)
{
	std::string formula;
	
	//make atom count map which can keep track of already printed atoms
	typedef std::pair<int, bool> PairType;
	typedef std::map<std::string, PairType> AtomCountGraphType;
	AtomCountGraphType atomCountGraph;
	for(AtomCountMapType::const_iterator it = atomCountMap.begin(); it != atomCountMap.end(); ++it)
		atomCountGraph[it->first] =  PairType(it->second, false);
	
	//first print atoms in FORMULA_RESIDUE_ORDER
	for(size_t i = 0; i < FORMULA_RESIDUE_ORDER_LEN; i++)
	{
		if(atomCountGraph[FORMULA_RESIDUE_ORDER[i]].first == 0)
		{
			atomCountGraph[FORMULA_RESIDUE_ORDER[i]].second = true;
			continue;
		}
		else if(atomCountGraph[FORMULA_RESIDUE_ORDER[i]].first == 1) {
			formula += FORMULA_RESIDUE_ORDER[i];
		}
		else {
			if(unicode){
				//formula += molFormula::symbolToUnicode(FORMULA_RESIDUE_ORDER[i]);
				formula += FORMULA_RESIDUE_ORDER[i];
				formula += utils::toSubscript(atomCountGraph[FORMULA_RESIDUE_ORDER[i]].first);
			}
			else {
				formula += FORMULA_RESIDUE_ORDER[i];
				formula += std::to_string(atomCountGraph[FORMULA_RESIDUE_ORDER[i]].first);
			}
		}
		atomCountGraph[FORMULA_RESIDUE_ORDER[i]].second = true;
	}
	
	//next itterate through atomCountGraph and print atoms not added to formula
	for(AtomCountGraphType::iterator it = atomCountGraph.begin();
		it != atomCountGraph.end(); ++it)
	{
		if(it->second.second) //check if atom has already been added to formula
			continue;
		
		formula += it->first;
		if(it->second.first > 1) {
			if(unicode)
				formula += utils::toSubscript(it->second.first);
			else formula += std::to_string(it->second.first);
		}
		it->second.second = true;
	}
	
	return formula;
}





