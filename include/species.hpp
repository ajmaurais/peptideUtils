//
//  species.hpp
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

#ifndef UTILS_SPECIES_HPP
#define UTILS_SPECIES_HPP

namespace utils{

    class Species;

    class Species{
    private:
        double mono;
        double avg;
    public:
        Species(){
            avg = 0; mono = 0;
        }
        Species(double _avg, double _mono){
            avg = _avg; mono = _mono;
        }
        ~Species() = default;

        void operator += (const Species& _add){
            avg += _add.avg; mono += _add.mono;
        }
        void operator = (const Species& _cp){
            avg = _cp.avg; mono = _cp.mono;
        }
        template<class _Tp> Species operator * (_Tp mult){
            Species ret;
            ret.avg = avg * mult;
            ret.mono = mono * mult;
            return ret;
        }

        double getAvg() const{
            return avg;
        }
        double getMono() const{
            return mono;
        }
    };

}//end of namespace utils

#endif //UTILS_SPECIES_HPP
