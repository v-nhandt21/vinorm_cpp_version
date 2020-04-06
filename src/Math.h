#pragma once
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include "unicode/ustream.h"
#include "unicode/schriter.h"
#include <bits/stdc++.h>
#include "ICUReadFile.h"
#include "ICUMapping.h"
#include "ICUConstant.h"
#include "ICUNumberConverting.h"
#include "ICUHelper.h"
using std::vector;
using std::cout;
using std::cerr;
using std::map;
using std::string;
using icu::UnicodeString;
using icu::RegexPattern;
using icu::RegexMatcher;
using icu::StringCharacterIterator;
using ICUConstant::DIGIT_ZERO;
using ICUConstant::PLUS_SIGN;
using ICUConstant::FULL_STOP;
using ICUConstant::COLON;
using ICUConstant::HYPEN_MINUS;
using ICUConstant::LEFT_PARENTHESIS;
using ICUConstant::RIGHT_PARENTHESIS;
using ICUConstant::LATIN_SMALL_LETTER_A;
using ICUConstant::LATIN_CAPITAL_LETTER_A;
using ICUConstant::VERTICAL_LINE;
using ICUConstant::COMMA;
using ICUConstant::REGEX_FOLDER;
using ICUConstant::MAPPING_FOLDER;
using ICUConstant::F_UNIT_MAPPING_CURRENCY;
using ICUConstant::F_UNIT_MAPPING_BASE;
class Math{
    private:
        // List of categories
        static constexpr int ROMAN_NUMBER= 0;
        static constexpr int MEASUREMENT= 1;
        static constexpr int MEASUREMENT_1= 2;
        static constexpr int NORMAL_NUMBER = 3;
        // File name contains patterns
        const string F_ROMAN_NUMBER = "RomanNumber.txt";
        const string F_MEASUREMENT  = "Measurement.txt";
        const string F_MEASUREMENT_1  = "Measurement_1.txt";
        const string F_NORMAL_NUMBER = "NormalNumber.txt";
        map<int, vector<UnicodeString>> patterns;
        
        ICUMapping unitCurrencyMapping;
        string unitCurrencyFile = MAPPING_FOLDER + "/" + F_UNIT_MAPPING_CURRENCY;
        ICUMapping unitBaseMapping;
        string unitBaseFile = MAPPING_FOLDER + "/" + F_UNIT_MAPPING_BASE;
        // Processing match by regex in each categories
        UnicodeString regexRomanNumber(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexNormalNumber(RegexMatcher* matcher, UErrorCode &status, int pattern);
        
        bool LocalisNumber(UnicodeString& unit);
        UnicodeString LocalhandleMeasurement(RegexMatcher* matcher, UErrorCode &status, bool UseBaseUnit, int pattern) ;
        UnicodeString stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status, int pattern);
        void loadPatterns(int categories, string filename);
    public:
        Math();
        
        UnicodeString normalizeText(const UnicodeString& input);
        ~Math();
};
