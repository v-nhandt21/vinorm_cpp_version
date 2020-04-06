#include "DateTime.h"
void DateTime::loadPatterns(int categories, string filename) {
    // Open file, read line by line
    string file = REGEX_FOLDER + "/" + filename;
    ICUReadFile f(file.data());
    bool readStatus = f.readFile();
    if (!readStatus) {
        cerr << "[E] Error reading pattern file: " << filename << std::endl;
        return ;
    }
    for (f.nextLine(0); f.getLineStart() < f.getFileLength(); f.nextLine(f.getLineEnd())) {
        UnicodeString line = UnicodeString(FALSE, f.getContentUChar() + f.getLineStart(), 
                f.getLineEnd()  - f.getLineStart());
        line.trim();
        patterns[categories].push_back(line);
    }
}
UnicodeString DateTime::normalizeText(const UnicodeString &input) {
    UnicodeString result;
    UnicodeString preResult(input);
    for (auto& rule : patterns) {
        int categ = rule.first;
        auto regexPatterns = rule.second;
        for (auto& pattern : regexPatterns) {
            UErrorCode status = U_ZERO_ERROR;
            UParseError parseErr;
            RegexPattern *regexPattern = RegexPattern::compile(pattern, parseErr, status);
            if (U_FAILURE(status)) {
                fprintf(stderr, "ugrep:  error in pattern: \"%s\" at position %d\n",
                        u_errorName(status), parseErr.offset);
                delete regexPattern;
                return UnicodeString();
            }
            RegexMatcher *matcher = regexPattern->matcher(preResult, status);
            // Create a RegexMatcher from the input
            if (U_FAILURE(status)) {
                fprintf(stderr, "[E] Error in creating RegexMatcher: \"%s\"\n",
                        u_errorName(status));
                delete matcher;
                delete regexPattern;
                return UnicodeString();
            }
            while (matcher->find(status) && U_SUCCESS(status)) {
                matcher->appendReplacement(result," "+stringForReplace(categ, matcher, status)+" ", status);
            }
            matcher->appendTail(result);
            delete matcher;
            delete regexPattern;
            preResult = result;
            result  = UnicodeString();
            
        }
    }
    return preResult;
}
DateTime::DateTime() {
    loadPatterns(TIME, F_TIME);
    loadPatterns(DATE_1, F_DATE_1);
    loadPatterns(DATE_FROM_TO_1, F_DATE_FROM_TO_1);
    loadPatterns(DATE_FROM_TO_2, F_DATE_FROM_TO_2);
    loadPatterns(MONTH, F_MONTH);
    loadPatterns(DATE_3, F_DATE_3);
    loadPatterns(DATE_2, F_DATE_2);
}
DateTime::~DateTime() {
    u_cleanup();
}
UnicodeString DateTime::stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status) {
    cerr << "[L] Using rule " << categories << " of Date Time\n";
    switch(categories) {
        case TIME:
            return regexTime(matcher, status);
        case DATE_1:
            return regexDate1(matcher, status);
        case DATE_FROM_TO_1:
            return regexDateFromTo_1(matcher, status);
        case DATE_FROM_TO_2:
            return regexDateFromTo_2(matcher, status);
        case MONTH:
            return regexMonth(matcher, status);
        case DATE_3:
            return regexDate3(matcher, status);
        case DATE_2:
            return regexDate2(matcher, status);
        default:
            cerr << "[E] Invalid category: " << categories << '\n';
    }
    return UnicodeString();
}
UnicodeString DateTime::regexTime(RegexMatcher* matcher, UErrorCode &status) {
    UnicodeString match = matcher->group(status);
    UnicodeString next = matcher->group(1,status).trim();
    match.toLower();
    UnicodeString result;
    UnicodeString number;
    ConvertingNumber converter;
    bool continuousDigits = false;
    // offset to get Unicode of characters;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == LATIN_SMALL_LETTER_A + h_offset 
                ||c ==  LATIN_SMALL_LETTER_A + g_offset || c == COLON) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += "giờ ";
        } else if (c == LATIN_SMALL_LETTER_A) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += "ây em ";
        } else if (c == LATIN_SMALL_LETTER_A + p_offset) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += "bi em ";
        } else if (c == LATIN_SMALL_LETTER_A + m_offset) {
            
        } else {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += c;
        }
    }
    if (number.length() > 0)
        result += " " + converter.convertNumber(number) + " ";
    if (next == HYPEN_MINUS){
        result+="đến ";
    }
    return result;
}
UnicodeString DateTime::regexDate1(RegexMatcher* matcher, UErrorCode &status) {
    
    UnicodeString match = matcher->group(status);
    match.toLower();
    UnicodeString result;
    UnicodeString number;
    ConvertingNumber converter;
    bool continuousDigits = false;
    int checkDate = 1;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == SOLIDUS || c == FULL_STOP || c == HYPEN_MINUS) {
            if (checkDate == 1) {
                
                checkDate++;
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            } else if (checkDate == 2) {
                result += " tháng ";
                checkDate++;
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
        } else {
            if (continuousDigits) {
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
            result += c;
        }
    }
    result += " năm " + converter.convertNumber(number);
    return result;
}
UnicodeString DateTime::regexDateFromTo_1(RegexMatcher* matcher, UErrorCode &status) {
    UnicodeString match = matcher->group(status);
    match.toLower();
    ConvertingNumber converter;
    UnicodeString result;
    UnicodeString number;
    bool continuousDigits = false;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == SOLIDUS || c == FULL_STOP) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " tháng ";
            number = UnicodeString();
        } else if (c == HYPEN_MINUS) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " đến ";
            number = UnicodeString();
        }
        else {
            if (continuousDigits) {
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
            result += c;
        }
    }
    result += converter.convertNumber(number);
    return result;
}
UnicodeString DateTime::regexDateFromTo_2(RegexMatcher* matcher, UErrorCode &status) {
    UnicodeString match = matcher->group(status);
    match.toLower();
    ConvertingNumber converter;
    UnicodeString result;
    UnicodeString number;
    bool continuousDigits = false;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == SOLIDUS || c == FULL_STOP) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " năm ";
            number = UnicodeString();
        } else if (c == HYPEN_MINUS) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " đến tháng ";
            number = UnicodeString();
        }
        else {
            if (continuousDigits) {
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
            result += c;
        }
    }
    result += converter.convertNumber(number);
    return result;
}
UnicodeString DateTime::regexMonth(RegexMatcher* matcher, UErrorCode &status) { 
    UnicodeString match = matcher->group(status);
    match.toLower();
    ConvertingNumber converter;
    UnicodeString result;
    UnicodeString number;
    bool continuousDigits = false;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == SOLIDUS || c == FULL_STOP || c == HYPEN_MINUS) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " năm ";
            number = UnicodeString();
        } else {
            if (continuousDigits) {
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
            result += c;
        }
    }
    result += converter.convertNumber(number);
    return result;
}
UnicodeString DateTime::regexDate3(RegexMatcher* matcher, UErrorCode &status) { 
    UnicodeString match = matcher->group(status);
    match.toLower();
    ConvertingNumber converter;
    UnicodeString result;
    UnicodeString number;
    bool continuousDigits = false;
    int h_offset = 'h' - 'a';
    int g_offset = 'g' - 'a';
    int p_offset = 'p' - 'a';
    int m_offset = 'm' - 'a';
    StringCharacterIterator iter(match);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9) {
            if (continuousDigits) {
                number += c;
            } else {
                number = c;
                continuousDigits = true;
            }
        } else if (c == SOLIDUS || c == FULL_STOP || c == HYPEN_MINUS) {
            continuousDigits = false;
            result += converter.convertNumber(number);
            result += " tháng ";
            number = UnicodeString();
        } else {
            if (continuousDigits) {
                continuousDigits = 0;
                result += converter.convertNumber(number);
                number = UnicodeString();
            }
            result += c;
        }
    }
    result += converter.convertNumber(number);
    return result;
}
UnicodeString DateTime::regexDate2(RegexMatcher* matcher, UErrorCode &status) { 
    ConvertingNumber converter;
    UnicodeString result;
    UnicodeString match = matcher->group(status);
    UnicodeString romanPart = matcher->group(1,status);
    UnicodeString YearPart = matcher->group(2,status);
    UnicodeString checkRoman =converter.romanToDecimal(romanPart);
    if (checkRoman != romanPart && ICUHelper::isNumberLiteral(checkRoman)){
        result+=" "+ICUHelper::readNumber(checkRoman,0)+" ";
    }
    
    return result+"năm "+converter.convertNumber(YearPart);
}
