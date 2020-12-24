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
using ICUConstant::SOLIDUS;
using ICUConstant::COLON;
using ICUConstant::HYPEN_MINUS;
using ICUConstant::LEFT_PARENTHESIS;
using ICUConstant::RIGHT_PARENTHESIS;
using ICUConstant::LATIN_SMALL_LETTER_A;
using ICUConstant::LATIN_CAPITAL_LETTER_A;
using ICUConstant::VERTICAL_LINE;
using ICUConstant::REGEX_FOLDER;
using ICUConstant::MAPPING_FOLDER;
using ICUConstant::F_LETTER_SOUND_EN;
using ICUConstant::F_LETTER_SOUND_VN;
using ICUConstant::F_SYMBOL;
using ICUConstant::F_NUMBER_SOUND;
class SpecialCase{
    private:
        // List of categories   
        static constexpr int PHONE_NUMBER = 0;
        static constexpr int FOOTBALL_UNDER = 1;
        static constexpr int FOOTBALL_OTHER = 2;
        static constexpr int EMAIL = 3;
        static constexpr int WEBSITE = 4;
        // File name contains patterns
        const string F_PHONE_NUMBER = "PhoneNumber.txt";
        const string F_FOOTBALL_UNDER= "FootballUnder.txt";
        const string F_FOOTBALL_OTHER= "FootballOther.txt";
        const string F_WEBSITE= "Website.txt";
        const string F_EMAIL= "Email.txt";
        
        map<int, vector<UnicodeString>> patterns;
        // Processing match by regex in each categories
        UnicodeString regexPhoneNumber(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexFootballUnder(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexFootballOther(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexWebsite(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexEmail(RegexMatcher* matcher, UErrorCode &status);
        // Wrapper function to return string for replacing match by regex
        UnicodeString stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status);
        void loadPatterns(int categories, string filename);
    public:
        SpecialCase();
        
        UnicodeString normalizeText(const UnicodeString& input);
        ~SpecialCase();
};
