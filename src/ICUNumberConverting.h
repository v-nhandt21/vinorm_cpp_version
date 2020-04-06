#pragma once
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ustream.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include "unicode/schriter.h"
#include <bits/stdc++.h>
#include "ICUReadFile.h"
#include "ICUConstant.h" 
using std::vector;
using std::cerr;
using icu::UnicodeString;
using icu::RegexPattern;
using icu::RegexMatcher;
using ICUConstant::DIGIT_ZERO;
using ICUConstant::LATIN_CAPITAL_LETTER_A;
using icu::StringCharacterIterator;
class ConvertingNumber{
    private:
        vector<UnicodeString> CHU_SO = {UnicodeString("không"), 
            UnicodeString("một"), UnicodeString("hai"),
            UnicodeString("ba"), UnicodeString("bốn"), 
            UnicodeString("năm"), UnicodeString("sáu"), 
            UnicodeString("bảy"), UnicodeString("tám"),
            UnicodeString("chín")};
        const UnicodeString MUOI_F{"mười"};
        const UnicodeString MUOI{"mươi"};
        const UnicodeString HUNDRED{"trăm"};
        const UnicodeString THOUSAND{"nghìn"};
        const UnicodeString MILLION{"triệu"};
        const UnicodeString BILLION{ "tỷ" };
        const UnicodeString MOTS{ "mốt" };
        const UnicodeString TUW{ "tư" };
        const UnicodeString LAWM{ "lăm" };
        const UnicodeString LINH{ "linh" };
        const UnicodeString SPACE{ " " };
        const UnicodeString COMMA{ ", " };
        const UnicodeString NGHIN{"nghìn"};
        const UnicodeString TRIEU{"triệu"};
        UnicodeString convertNumberLTHundred(const UnicodeString& num);
        UnicodeString convertNumberLTThousand(const UnicodeString& num); 
        UnicodeString convertNumberLTMillion(const UnicodeString& num); 
        UnicodeString convertNumberArbitrary(const UnicodeString& num);
        UnicodeString stripZeros(const UnicodeString& num, int32_t z = 0); 
        void printUnicodeString(const UnicodeString &s) {
            char charBuf[1000];
            s.extract(0, s.length(), charBuf, sizeof(charBuf)-1, 0);   
            charBuf[sizeof(charBuf)-1] = 0;          
            printf("%s", charBuf);
        }
    public:
        UnicodeString convertNumber(const UnicodeString &num);
        UnicodeString decimalToRoman(int number);
        UnicodeString romanToDecimal(const UnicodeString &roman);
        void unitTest();
};
