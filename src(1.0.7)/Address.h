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
#include "ICUDictionary.h"
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
using ICUConstant::SOLIDUS;
using ICUConstant::REGEX_FOLDER;
using ICUConstant::MAPPING_FOLDER;
using ICUConstant::DICT_FOLDER;
using ICUConstant::F_POPULAR;
using ICUConstant::F_LETTER_SOUND_EN;
using ICUConstant::F_LETTER_SOUND_VN;
using ICUConstant::F_SYMBOL;
class Address{
    private:
        // List of categories
        static constexpr int POLITICAL_DIVISION= 0;
        static constexpr int STREET= 1;
        static constexpr int OFFICE= 2;
        static constexpr int CODENUMBER= 3;
        // File name contains patterns
        const string F_POLITICAL_DIVISION = "PoliticalDivision.txt";
        const string F_STREET = "Street.txt";
        const string F_OFFICE = "Office.txt";
        const string F_CODENUMBER = "Codenumber.txt";
        map<int, vector<UnicodeString>> patterns;
        // Processing match by regex in each categories
        UnicodeString regexPoliticalDivision(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexStreet(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexOffice(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexCodenumber(RegexMatcher* matcher, UErrorCode &status);
        // Handle prefix in political division
        UnicodeString expandPrefixPD(const UnicodeString &prefix);
        // Wrapper function to return string for replacing match by regex
        UnicodeString stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status);
        void loadPatterns(int categories, string filename);
    public:
        Address();
        
        UnicodeString normalizeText(const UnicodeString& input);
        ~Address();
};
 