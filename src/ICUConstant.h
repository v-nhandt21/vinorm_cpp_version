#pragma once
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include <bits/stdc++.h>
using std::string;
namespace ICUConstant {
    const UChar32 PLUS_SIGN = 43;
    const UChar32 LEFT_PARENTHESIS = 40;
    const UChar32 RIGHT_PARENTHESIS = 41;
    const UChar32 FULL_STOP = 46;
    const UChar32 HYPEN_MINUS = 45;
    const UChar32 COLON = 58;
    const UChar32 DIGIT_ZERO = 48; 
    const UChar32 LATIN_CAPITAL_LETTER_A = 65;
    const UChar32 LATIN_SMALL_LETTER_A = 97;
    const UChar32 LATIN_CAPITAL_LETTER_U = 85;
    const UChar32 LATIN_SMALL_LETTER_U = 117;
    const UChar32 VERTICAL_LINE = 124;
    const UChar32 SOLIDUS = 47;
    const UChar32 COMMA = 44;
    // Folder contains rules
    const string REGEX_FOLDER = "RegexRule";
    // Folder contains mapping
    const string MAPPING_FOLDER = "Mapping";
    // Folder contains dictionary
    const string DICT_FOLDER = "Dict";
    // Mapping file
    const string F_LETTER_SOUND_EN = "LetterSoundEN.txt";
    const string F_LETTER_SOUND_VN = "LetterSoundVN.txt";
    const string F_NUMBER_SOUND= "Number.txt";
    const string F_UNIT_MAPPING_CURRENCY = "CurrencyUnit.txt";
    const string F_UNIT_MAPPING_BASE = "BaseUnit.txt";
    const string F_SYMBOL = "Symbol.txt";
    const string F_PREFIX_UNIT = "PrefixUnit.txt";
    // Dictionary File
    const string F_ACRONYMS = "Acronyms_shorten.txt";
    const string F_TEENCODE = "Teencode.txt";
    const string F_POPULAR = "Popular.txt";
    // Input file contains text to normalize
    const string F_INPUT = "input.txt";
    // Ouput file contains normalized text
    const string F_OUTPUT = "output.txt";
        
};
