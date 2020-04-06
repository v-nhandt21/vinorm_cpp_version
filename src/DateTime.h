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
using ICUConstant::SOLIDUS;
using ICUConstant::REGEX_FOLDER;
class DateTime{
    private:
        // List of categories
        static constexpr int TIME = 0;
        static constexpr int DATE_1 = 1;
        static constexpr int DATE_FROM_TO_1= 2;
        static constexpr int DATE_FROM_TO_2= 3;
        static constexpr int MONTH = 4;
        static constexpr int DATE_3 = 5;
        static constexpr int DATE_2 = 6;
        // File name contains patterns
        const string F_TIME = "Time.txt";
        const string F_DATE_FROM_TO_1 = "Date_From_To_1.txt";
        const string F_DATE_FROM_TO_2 =  "Date_From_To_2.txt";
        const string F_MONTH = "Month.txt";
        const string F_DATE_1= "Date_1.txt";
        const string F_DATE_2= "Date_2.txt";
        const string F_DATE_3= "Date_3.txt";
        map<int, vector<UnicodeString>> patterns;
        // Processing match by regex in each categories
        UnicodeString regexTime(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexDate1(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexDateFromTo_1(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexDateFromTo_2(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexMonth(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexDate3(RegexMatcher* matcher, UErrorCode &status);
        UnicodeString regexDate2(RegexMatcher* matcher, UErrorCode &status);
        // Wrapper function to return string for replacing match by regex
        UnicodeString stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status);
        void loadPatterns(int categories, string filename);
    public:
        DateTime();
        
        UnicodeString normalizeText(const UnicodeString& input);
        ~DateTime();
};
