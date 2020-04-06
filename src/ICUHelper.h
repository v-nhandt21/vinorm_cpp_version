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
using ICUConstant::COMMA;
using ICUConstant::REGEX_FOLDER;
using ICUConstant::MAPPING_FOLDER;
using ICUConstant::F_UNIT_MAPPING_BASE;
using ICUConstant::F_PREFIX_UNIT;
class ICUHelper{
    private:
    public:
        static UnicodeString removeExtraWhitespace(UnicodeString &input) {
            UErrorCode status = U_ZERO_ERROR;
            UParseError parseErr;
            RegexPattern *regexPattern = RegexPattern::compile("\\s+", parseErr, status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "[E] Error in pattern: \"%s\" at position %d\n",
                        u_errorName(status), parseErr.offset);
                return input;
            }
            
            
            
            RegexMatcher *matcher = regexPattern->matcher(input, status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                        u_errorName(status));
                return input;
            }
            UnicodeString result =  matcher->replaceAll(" ", status);
            delete matcher;
            delete regexPattern;
            result.trim();
            return result;
        }
        static bool isNumberLiteral(UnicodeString s) {
            StringCharacterIterator iter(s);
            for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
                if (c < DIGIT_ZERO || c > DIGIT_ZERO + 9)
                    return false;
            }
            return true;
        }
        static vector<UnicodeString> splitFractionUnit(UnicodeString unit) {
            vector<UnicodeString> result;
            int idx = unit.indexOf("/");
            if (idx == -1)
                result.push_back(unit);
            else {
                result.push_back(UnicodeString(unit, 0, idx));
                result.push_back(UnicodeString(unit, idx +1));
            }
            return result;
        }
        static UnicodeString  splitCompositeUnit(UnicodeString& unit) {
            ICUMapping baseUnit;
            string baseUnitPath = MAPPING_FOLDER + "/" + F_UNIT_MAPPING_BASE;
            baseUnit.loadMappingFile(baseUnitPath.data());
            ICUMapping prefixDict;
            string prefixUnitPath = MAPPING_FOLDER + "/" + F_PREFIX_UNIT;
            prefixDict.loadMappingFile(prefixUnitPath.data());
            UnicodeString normalizedText="";
            if (unit.length() < 2 || baseUnit.hasMappingOf(unit)) {
                return " " + baseUnit.mappingOf(unit) + " ";
            }
            
            
            int prefixLength = 0;
            UnicodeString prefix = unit.tempSubString(0, 2);
            if (prefixDict.hasMappingOf(prefix)) {
                prefixLength = 2;
            }
            
            if (prefixLength == 0) {
                prefix = unit.tempSubString(0, 1);
                if (prefixDict.hasMappingOf(prefix))
                    prefixLength = 1;
            }
            if (prefixLength != 0) 
                normalizedText += prefixDict.mappingOf(prefix) + " ";
            UnicodeString mainUnit = unit.tempSubString(prefixLength);
            vector<UnicodeString> result;
            if (prefixLength != 0)
                result.push_back(prefix);
            int start = 0;
            int end = 0;
            while (start < mainUnit.length() && end <= mainUnit.length()) {
                end++;
                UnicodeString currentUnit = mainUnit.tempSubString(start, end-start);
                if (baseUnit.hasMappingOf(currentUnit) || isNumberLiteral(currentUnit)) {
                    if (isNumberLiteral(currentUnit)) {
                        if (currentUnit == "2") {
                            normalizedText += " vuông ";
                        } else if (currentUnit == "3") {
                            normalizedText += " khối ";
                        } else {
                            ConvertingNumber converter;
                            normalizedText += " mũ " + converter.convertNumber(currentUnit) + " "; 
                        }
                    } else {
                        if (baseUnit.hasMappingOf(currentUnit))
                            normalizedText += baseUnit.mappingOf(currentUnit) + " "; 
                        else 
                            normalizedText += currentUnit + " "; 
                    }
                    start =  end;
                    end++;
                }
            }
            if (normalizedText == "")
                return unit;
            else
            {
                return normalizedText;
            }
            
        }
        static UnicodeString normalizeUnit(UnicodeString unit) {
            UnicodeString normalizedText = "";
            auto compositeUnits = splitFractionUnit(unit);
            for (int i = 0;  i < compositeUnits.size(); i++) {
                cerr << "[L] Unit: " << compositeUnits[i] << '\n';
                normalizedText += " "+splitCompositeUnit(compositeUnits[i])+" ";
                if (compositeUnits.size() > 1 && i != (int) compositeUnits.size() - 1) {
                    normalizedText += " trên ";
                }
            }
            return removeExtraWhitespace(normalizedText);
            
        }
        static UnicodeString readNumber(UnicodeString literalNumber, int point) {
            UnicodeString match = literalNumber;
            ConvertingNumber converter;
            
            float_t floatIdx = -1;
            if (point==0){
                 floatIdx = match.indexOf(COMMA);
            }
            else if(point==1){
                 floatIdx = match.indexOf(FULL_STOP);
            }
            else{
                StringCharacterIterator iter(literalNumber);
                for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
                    if (c==COMMA) {
                        floatIdx = match.indexOf(COMMA);
                    } 
                    if (c==FULL_STOP)
                    {
                        floatIdx = match.indexOf(FULL_STOP);
                    }
                }
            }
            UnicodeString integerPart, fractionalPart;
            UnicodeString Zerofrac="";
            bool checkZF=0;
            if (floatIdx != -1) {
                fractionalPart = match.tempSubString(floatIdx+1).trim();
                integerPart = match.tempSubStringBetween(0, floatIdx).trim();
                
                UnicodeString fractionalPartProcessed;
                StringCharacterIterator iter(fractionalPart);
                for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
                    if (DIGIT_ZERO == c && checkZF == 0)
                    {
                        Zerofrac+=" không ";
                        
                        continue;
                    }
                    if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
                        fractionalPartProcessed += c;
                        checkZF=1;
                    } 
                }
            } else {
                integerPart = match;
            }
            
            UnicodeString integerPartProcessed;
            StringCharacterIterator iter(integerPart);
            for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
                if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
                    integerPartProcessed += c;
                } 
            }
            UnicodeString result;
            if (floatIdx == -1)
                return " " + converter.convertNumber(integerPartProcessed) + " ";
            else
            {
                
                return " " + converter.convertNumber(integerPartProcessed) + " phẩy " + Zerofrac
                    + converter.convertNumber(fractionalPart.trim()) + " ";
            }
        }
};
