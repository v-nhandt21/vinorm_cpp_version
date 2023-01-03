#include <bits/stdc++.h>
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include "unicode/schriter.h"
#include "unicode/ustream.h"
#include "unicode/schriter.h"
#include "unicode/brkiter.h"
#include "ICUConstant.h"
#include "ICUHelper.h"
#include "ICUReadFile.h"
#include "ICUMapping.h"
#include "ICUDictionary.h"
#include "SpecialCase.h"
#include "Address.h"
#include "Math.h"
#include "DateTime.h"
using icu::BreakIterator;
using icu::CharacterIterator;
using icu::Locale;
using icu::RegexMatcher;
using icu::RegexPattern;
using icu::StringCharacterIterator;
using icu::UnicodeSet;
using icu::UnicodeString;
using ICUConstant::DICT_FOLDER;
using ICUConstant::F_ACRONYMS;
using ICUConstant::F_INPUT;
using ICUConstant::F_LETTER_SOUND_EN;
using ICUConstant::F_LETTER_SOUND_VN;
using ICUConstant::F_OUTPUT;
using ICUConstant::F_POPULAR;
using ICUConstant::F_SYMBOL;
using ICUConstant::F_TEENCODE;
using ICUConstant::MAPPING_FOLDER;
UnicodeString removeExtraWhitespace(UnicodeString &input)
{
    UErrorCode status = U_ZERO_ERROR;
    UParseError parseErr;
    RegexPattern *regexPattern = RegexPattern::compile("(\\(|\\)|\\s+)", parseErr, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in pattern: \"%s\" at position %d\n ",
                u_errorName(status), parseErr.offset);
        delete regexPattern;
        return input;
    }
    // Create a RegexMatcher from the input

    RegexMatcher *matcher = regexPattern->matcher(input, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                u_errorName(status));
        delete matcher;
        delete regexPattern;
        return input;
    }
    UnicodeString result = matcher->replaceAll(" ", status);
    delete matcher;
    delete regexPattern;
    result.trim();
    return result;
}
UnicodeString TokenizeSymbol(UnicodeString &input)
{
    UErrorCode status = U_ZERO_ERROR;
    UParseError parseErr;
    RegexPattern *regexPattern = RegexPattern::compile("[^\\w\\d\\s]", parseErr, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in pattern: \"%s\" at position   %d\n",
                u_errorName(status), parseErr.offset);
        return input;
    }

    RegexMatcher *matcher = regexPattern->matcher(input, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                u_errorName(status));
        delete matcher;
        return input;
    }

    UnicodeString result;
    while (matcher->find(status) && U_SUCCESS(status))
    {
        matcher->appendReplacement(result, " " + matcher->group(status) + " ", status);
    }
    matcher->appendTail(result);
    delete matcher;
    delete regexPattern;
    input = result.trim();
    return input;
}
UnicodeString removeNovoiceSymbol(UnicodeString &input, bool spaceReplace)
{
    UErrorCode status = U_ZERO_ERROR;
    UParseError parseErr;
    RegexPattern *regexPattern = RegexPattern::compile("(“|”|\\.\\.\\.|\"|\\{|\\}|\\[|\\]|-)", parseErr, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in pattern: \"%s\" at position %d\n",
                u_errorName(status), parseErr.offset);
        delete regexPattern;
        return input;
    }

    RegexMatcher *matcher = regexPattern->matcher(input, status);
    if (U_FAILURE(status))
    {
        fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                u_errorName(status));
        delete matcher;
        delete regexPattern;
        return input;
    }

    UnicodeString result;
    if (spaceReplace == 1)
    {
        result = matcher->replaceAll(" ", status);
    }
    else
    {
        result = matcher->replaceAll("", status);
    }
    delete matcher;
    delete regexPattern;
    result.trim();
    return result;
}
UnicodeString readLetterByLetter(const UnicodeString &word, ICUMapping &soundMapping)
{
    UnicodeString lowerd(word);
    lowerd.toLower();
    StringCharacterIterator iter(lowerd);
    UnicodeString result;
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32())
    {
        result += soundMapping.mappingOf(UnicodeString(c)) + " ";
    }
    return result;
}
bool isUpperCaseWord(const UnicodeString &word)
{
    UnicodeString upperCase(word);
    upperCase.toUpper();
    return upperCase == word;
}
bool containsOnlyLetter(const UnicodeString &word)
{
    UnicodeString lowered(word);
    lowered.toLower();
    ICUMapping letterSoundVN;
    string letterVNFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterSoundVN.loadMappingFile(letterVNFile.data());
    StringCharacterIterator iter(lowered);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32())
    {
        if (!letterSoundVN.hasMappingOf(c))
            return false;
    }
    return true;
}
bool containsVowel(const UnicodeString &word)
{
    UnicodeString lowerd(word);
    lowerd.toLower();
    UnicodeString vowels("aàảãáạăằẳẵâầẩẫấậeèẻẽéẹêềểễếệiìỉĩíịoòỏõóọôồổỗốộơỡớợuùủũúụưừửữứựyỳỷỹýỵ");
    StringCharacterIterator iter(vowels);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32())
    {
        if (lowerd.indexOf(c) != -1)
            return true;
    }
    return false;
}

bool to_bool(const std::string& x) {
  assert(x == "0" || x == "1");
  return x == "1";
}

int main(int argc, char *argv[])
{
    bool punc = false; // if true: keep do not replace with dot and coma
    bool unknown = false; // if true: discard word undefine and do not contain vowel, do not spell word with vowel
    bool lower = false; // if false: Just get normalization without lowercase
    bool rule = false; // if true: Just get normalization wit Regex, not using Dictionary Checking

    for (int i = 1; i < argc; i++)
    {  
        if (strcmp(argv[i], "-punc") == 0)
        {                 
            punc = true;
            continue;
        }
        if (strcmp(argv[i], "-unknown") == 0)
        {                 
            unknown = true;
            continue;
        }
        if (strcmp(argv[i], "-lower") == 0)
        {                 
            lower = true;
            continue;
        }
        if (strcmp(argv[i], "-rule") == 0)
        {                 
            rule = true;
            continue;
        }
    }

    // Regex Rules
    SpecialCase specialCase;
    Address address;
    Math math;
    DateTime dateTime;

    // Mapping files
    ICUMapping acronym;
    string acronymFile = MAPPING_FOLDER + "/" + F_ACRONYMS;
    acronym.loadMappingFile(acronymFile.data());
    ICUMapping teenCode;
    string teencodeFile = MAPPING_FOLDER + "/" + F_TEENCODE;
    teenCode.loadMappingFile(teencodeFile.data());
    ICUMapping symbol;
    string symbolFile = MAPPING_FOLDER + "/" + F_SYMBOL;
    symbol.loadMappingFile(symbolFile.data());
    ICUMapping letterSoundVN;
    string letterVNFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterSoundVN.loadMappingFile(letterVNFile.data());
    ICUMapping letterSoundEN;
    string letterENFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_EN;
    letterSoundEN.loadMappingFile(letterENFile.data());
    // Dictionary Files
    ICUDictionary popularWord;
    string popFile = DICT_FOLDER + "/" + F_POPULAR;
    popularWord.loadDictFile(popFile.data());
    // Read input from file
    ICUReadFile inputFile(F_INPUT.data());
    inputFile.readFile();
    // Write result to file
    std::ofstream fout(F_OUTPUT);
    // Result text
    UnicodeString normalizedText;
    // Read input file line by line
    for (inputFile.nextLine(0); inputFile.getLineStart() < inputFile.getFileLength();
        inputFile.nextLine(inputFile.getLineEnd()))
    {
        UnicodeString line = UnicodeString(false, inputFile.getContentUChar() + inputFile.getLineStart(), inputFile.getLineEnd() - inputFile.getLineStart());
        line = removeExtraWhitespace(line);
        // Regex rules processing
        normalizedText = removeExtraWhitespace(line);
        normalizedText = specialCase.normalizeText(normalizedText);
        normalizedText = dateTime.normalizeText(normalizedText);
        normalizedText = math.normalizeText(normalizedText);
        normalizedText = address.normalizeText(normalizedText);

        normalizedText = removeExtraWhitespace(normalizedText);
        
        if (rule == true){
            fout << normalizedText << "#line#";
            continue;
        }
        
        UnicodeString result;
        UnicodeString match;
        UErrorCode status = U_ZERO_ERROR;
        UParseError parseErr;
        RegexPattern *regexPattern = RegexPattern::compile("[^\\s]+", parseErr, status);
        if (U_FAILURE(status))
        {
            fprintf(stderr, "[E] Error in pattern: \"%s\" at position %d\n",
                    u_errorName(status), parseErr.offset);
            return 1;
        }
        RegexMatcher *matcher = regexPattern->matcher(normalizedText, status);
        if (U_FAILURE(status))
        {
            fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                    u_errorName(status));
            return 1;
        }
        while (matcher->find(status) && U_SUCCESS(status))
        {
            match = matcher->group(status);
            UnicodeString word(match);
            word.trim();
            UnicodeString resultAdd;
            while (true)
            {

                UnicodeString normalized_word(word);
                UnicodeString TMsymbol;

                normalized_word = removeNovoiceSymbol(normalized_word, 0);

                normalized_word.trim();
                UErrorCode statusAdd = U_ZERO_ERROR;
                UParseError parseErrAdd;

                RegexPattern *regexPatternAdd = RegexPattern::compile("(\\b[^\\s]+)(\\;$|\\:$|\\!$|\\?$|\\,$|\\.$|\\b)", parseErrAdd, statusAdd);
                if (U_FAILURE(statusAdd))
                {
                    fprintf(stderr, "ugrep:  error in pattern: \"%s\" at position %d\n",
                            u_errorName(statusAdd), parseErrAdd.offset);
                }
                RegexMatcher *matcherAdd = regexPatternAdd->matcher(normalized_word, statusAdd);
                if (U_FAILURE(statusAdd))
                {
                    fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                            u_errorName(statusAdd));
                }
                if (matcherAdd->find(statusAdd) && U_SUCCESS(statusAdd))
                {
                    TMsymbol = matcherAdd->group(2, statusAdd);
                    normalized_word = matcherAdd->group(1, statusAdd);
                    if (punc == false ){
                        if (TMsymbol == "." || TMsymbol == "!" || TMsymbol == ":" || TMsymbol == "?")
                            TMsymbol = ".";
                        else if (TMsymbol == "," || TMsymbol == ";")
                            TMsymbol = ",";
                    }
                }
                delete matcherAdd;
                delete regexPatternAdd;
                if (normalized_word.length() == 0)
                {

                    resultAdd = " " + normalized_word + " " + TMsymbol + " ";
                    break;
                }

                if (popularWord.hasWord(normalized_word))
                {
                    resultAdd = " " + normalized_word + " " + TMsymbol + " ";
                    break;
                }
                if (acronym.hasMappingOf(normalized_word))
                {
                    resultAdd = " " + acronym.mappingOf(normalized_word) + " " + TMsymbol + " ";
                    break;
                }

                if (teenCode.hasMappingOf(normalized_word))
                {
                    resultAdd = " " + teenCode.mappingOf(normalized_word) + " " + TMsymbol + " ";
                    break;
                }
                normalized_word = word;

                normalized_word = removeNovoiceSymbol(normalized_word, 1);

                normalized_word = TokenizeSymbol(normalized_word);
                UErrorCode statusToken = U_ZERO_ERROR;
                BreakIterator *boundary = BreakIterator::createWordInstance(Locale::getDefault(), statusToken);
                boundary->setText(normalized_word);
                resultAdd = "";

                auto start = boundary->first();
                for (auto end = boundary->next(); end != BreakIterator::DONE;
                    start = end, end = boundary->next())
                {

                    UnicodeString token = UnicodeString(normalized_word, start, end - start);
                    UnicodeString wordAdd(token);

                    if (popularWord.hasWord(wordAdd))
                    {
                        resultAdd += " " + wordAdd + " ";
                        continue;
                    }
                    if (acronym.hasMappingOf(wordAdd))
                    {
                        resultAdd += " " + acronym.mappingOf(wordAdd) + " ";
                        continue;
                    }

                    if (teenCode.hasMappingOf(wordAdd))
                    {
                        resultAdd += " " + teenCode.mappingOf(wordAdd) + " ";
                        continue;
                    }

                    if (wordAdd == "." || wordAdd == "!" || wordAdd == ":" || wordAdd == "?")
                    {
                        if (punc == false)
                            resultAdd += " . ";
                        else
                            resultAdd += " " + wordAdd + " ";
                        continue;
                    }
                    if (wordAdd == "," || wordAdd == ";" || wordAdd == "/")
                    {
                        if (punc == false)
                            resultAdd += " , ";
                        else
                            resultAdd += " " + wordAdd + " ";
                        continue;
                    }
                    if (symbol.hasMappingOf(wordAdd))
                    {
                        resultAdd += " " + symbol.mappingOf(wordAdd) + " ";
                        continue;
                    }
                    if (containsOnlyLetter(wordAdd))
                    {
                        
                        
                        if (isUpperCaseWord(wordAdd))
                        {
                            ConvertingNumber converter;
                            UnicodeString checkRoman = converter.romanToDecimal(wordAdd);
                            if (checkRoman != wordAdd && ICUHelper::isNumberLiteral(checkRoman))
                            {
                                resultAdd += " " + ICUHelper::readNumber(checkRoman, 0) + " ";
                                continue;
                            }
                            if (unknown == true)
                                resultAdd += " " + wordAdd + " ";
                            else
                                resultAdd += " " + readLetterByLetter(wordAdd, letterSoundEN) + " ";
                            continue;
                        }
                        
                        else if (unknown == false){
                            if (!containsVowel(wordAdd))
                            {
                                resultAdd += " " + readLetterByLetter(wordAdd, letterSoundVN) + " ";
                                continue;
                            }
                            else
                            {
                                resultAdd += " " + wordAdd + " ";
                            }
                        }
                        else resultAdd += " " + wordAdd + " ";
                    }
                    else
                    {
                        resultAdd += " ";
                    }
                }
                break;
            }
            result += resultAdd;
        }
        if (lower == true)
        {
            result.toLower();
        }
        result = removeNovoiceSymbol(result, 1);
        result = removeExtraWhitespace(result);
        fout << result << "#line#";
        delete matcher;
        delete regexPattern;
    }

    return 0;
}
