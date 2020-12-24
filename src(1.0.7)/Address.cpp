#include "Address.h"
void Address::loadPatterns(int categories, string filename) {
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
UnicodeString Address::stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status) {
    cerr << "[L] Using rule " << categories << " of Address\n";
    cerr << "[L] Match: " << matcher->group(status) << '\n';
    switch(categories) {
        case POLITICAL_DIVISION:
            return regexPoliticalDivision(matcher, status);
        case OFFICE:
            return regexOffice(matcher, status);
        case STREET:
            return regexStreet(matcher, status);
        case CODENUMBER:
            return regexCodenumber(matcher, status);
        default:
            cerr << "[E] Invalid category: " << categories << '\n';
    }
    return UnicodeString();
}
UnicodeString Address::normalizeText(const UnicodeString &input) {
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
                fprintf(stderr, "[E] Error in pattern: \"%s\" at position %d\n",
                        u_errorName(status), parseErr.offset);
                delete regexPattern;
                return UnicodeString();
            }
            // Create a RegexMatcher from the input
            RegexMatcher *matcher = regexPattern->matcher(preResult, status);
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
UnicodeString Address::expandPrefixPD(const UnicodeString &prefix) {
    if (prefix == "kp")
        return UnicodeString("khu phố");
    else if (prefix == "q")
        return UnicodeString("quận");
    else if (prefix == "p")
        return UnicodeString("phường");
    else if (prefix == "h")
        return UnicodeString("huyện");
    else if (prefix == "tx")
        return UnicodeString("thị xã");
    else if (prefix == "tp")
        return UnicodeString("thành phố");
    else if (prefix == "x")
        return UnicodeString("xã");
    cerr << "[E] Invalid prefix to expand for political division: " << prefix << '\n';
    return UnicodeString();
}
UnicodeString Address::regexPoliticalDivision(RegexMatcher* matcher, UErrorCode &status) {
    UnicodeString match = matcher->group(status);
    // The prefix in political division
    UnicodeString prefix = matcher->group(1, status);
    prefix.toLower();
    UnicodeString expandPrefix = expandPrefixPD(prefix);
    expandPrefix += " ";
    // Replace the prefix part (including following FULL_STOP)
    auto indexOfPoint = match.indexOf(FULL_STOP);
    if (indexOfPoint != -1)
        return expandPrefix + match.tempSubString(indexOfPoint + 1);
    else {
        return expandPrefix + match.tempSubString(prefix.length());
    }
}
UnicodeString Address::regexStreet(RegexMatcher* matcher, UErrorCode &status) {
    UnicodeString match = matcher->group(status);
    UnicodeString prefix = matcher->group(1, status);
    // The main part contains the digist and symbol for the street
    UnicodeString mainPart = match.tempSubString(prefix.length());
    //check continuous digits construct a whole number
    bool continuousDigits = false; 
    UnicodeString result;
    UnicodeString number;
    ConvertingNumber converter;
    ICUMapping letterSound;
    string letterSoundFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterSound.loadMappingFile(letterSoundFile.data());
    StringCharacterIterator iter(mainPart);
    
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9 ) {
            if (continuousDigits) {
                number += c;
            } else {
                if (DIGIT_ZERO ==c)
                {
                    continuousDigits = false;
                    result += "không ";
                    number = UnicodeString();
                }
                else {
                    number = c;
                    continuousDigits = true;
                }
            }
        } else if (c == SOLIDUS) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += "xuyệt ";
        } else if (c == HYPEN_MINUS) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += " , ";
        } else {
            if (continuousDigits) {
                continuousDigits  = false;
                result += converter.convertNumber(number) + " "; 
                number = UnicodeString();
            }
            result += letterSound.mappingOf(UnicodeString(c)) + " ";
        }
    }
    if (number.length() > 0) 
        result += converter.convertNumber(number);
    return prefix + " " + result;
}
UnicodeString Address::regexOffice(RegexMatcher* matcher, UErrorCode& status) {
    UnicodeString match = matcher->group(status);
    UnicodeString prefix = matcher->group(1, status);
    // The main part contains the digist and symbol for the office 
    UnicodeString mainPart = match.tempSubString(prefix.length());
    bool continuousDigits = false; 
    UnicodeString result;
    UnicodeString number;
    ConvertingNumber converter;
    ICUMapping letterSound;
    string letterSoundFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterSound.loadMappingFile(letterSoundFile.data());
    StringCharacterIterator iter(mainPart);
    
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9 ) {
            if (continuousDigits) {
                number += c;
            } else {
                if (DIGIT_ZERO == c)
                {
                    continuousDigits = false;
                    result += "không ";
                    number = UnicodeString();
                }
                else {
                    number = c;
                    continuousDigits = true;
                }
            }
        } else if (c == SOLIDUS) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += " ";
        } else if (c == HYPEN_MINUS) {
            if (continuousDigits) {
                continuousDigits = false;
                result += converter.convertNumber(number) + " ";
                number = UnicodeString();
            }
            result += " , ";
        } else {
            if (continuousDigits) {
                continuousDigits  = false;
                result += converter.convertNumber(number) + " "; 
                number = UnicodeString();
            }
            result += letterSound.mappingOf(UnicodeString(c)) + " ";
        }
    }
    if (number.length() > 0) 
        result += converter.convertNumber(number);
    return prefix + " " + result;
}
UnicodeString Address::regexCodenumber(RegexMatcher* matcher, UErrorCode& status) {
    UnicodeString match = matcher->group(status);
    UnicodeString result;
    UnicodeString number;
    UnicodeString PopWord;
    bool continuousDigits = false;
    bool continuous_Lowercase_Popular = false;
    ConvertingNumber converter;
    ICUMapping letterSound;
    ICUMapping letterVN;

    string letterVNFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterVN.loadMappingFile(letterVNFile.data());

    ICUDictionary popularWord;
    string popFile = DICT_FOLDER + "/" + F_POPULAR;
    popularWord.loadDictFile(popFile.data());
    
    string letterSoundFile = MAPPING_FOLDER + "/" + F_LETTER_SOUND_VN;
    letterSound.loadMappingFile(letterSoundFile.data());
    letterSoundFile = MAPPING_FOLDER + "/" + F_SYMBOL;
    letterSound.loadMappingFile(letterSoundFile.data());

    match.trim();
    StringCharacterIterator iter(match);
    
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        UnicodeString ctm = UnicodeString (c) ;
        if (DIGIT_ZERO <= c && c <= DIGIT_ZERO + 9 ) {
            if (continuousDigits) {
                number += c;
            } else {
                if (DIGIT_ZERO == c)
                {
                    continuousDigits = false;
                    result += "không ";
                    number = UnicodeString();
                }
                else {
                    number = c;
                    continuousDigits = true;
                }
            }
            if (continuous_Lowercase_Popular) {
                continuous_Lowercase_Popular  = false;
                if (popularWord.hasWord(PopWord))
                    result+=PopWord+" ";
                else{
                    StringCharacterIterator iterNum(PopWord);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += letterSound.mappingOf(UnicodeString(cn)) + " ";
                    }
                }
                PopWord = UnicodeString();
            }
        } else if (letterVN.hasMappingOf(ctm) && ctm.toUpper() != c ) { //TOI5
            if (continuousDigits) {
                continuousDigits  = false;
                if (number.length() <= 4)
                    result += converter.convertNumber(number) + " "; 
                else {
                    StringCharacterIterator iterNum(number);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += converter.convertNumber(cn) + " ";
                    }
                }
                number = UnicodeString();
            }
            if (continuous_Lowercase_Popular) {
                PopWord += c;
            } else {
                PopWord = c;
                continuous_Lowercase_Popular = true;
            }
        } else if (c == SOLIDUS) {
            if (continuousDigits) {
                continuousDigits  = false;
                if (number.length() <= 4)
                    result += converter.convertNumber(number) + " "; 
                else {
                    StringCharacterIterator iterNum(number);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += converter.convertNumber(cn) + " ";
                    }
                }
                number = UnicodeString();
            }
            if (continuous_Lowercase_Popular) {
                continuous_Lowercase_Popular  = false;
                if (popularWord.hasWord(PopWord))
                    result+=PopWord+" ";
                else{
                    StringCharacterIterator iterNum(PopWord);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += letterSound.mappingOf(UnicodeString(cn)) + " ";
                    }
                }
                PopWord = UnicodeString();
            }
            result += "xuyệt ";
        } else if (c == FULL_STOP ) {
            if (continuousDigits) {
                continuousDigits  = false;
                if (number.length() <= 4)
                    result += converter.convertNumber(number) + " "; 
                else {
                    StringCharacterIterator iterNum(number);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += converter.convertNumber(cn) + " ";
                    }
                }
                number = UnicodeString();
            }
            if (continuous_Lowercase_Popular) {
                continuous_Lowercase_Popular  = false;
                if (popularWord.hasWord(PopWord))
                    result+=PopWord+" ";
                else{
                    StringCharacterIterator iterNum(PopWord);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += letterSound.mappingOf(UnicodeString(cn)) + " ";
                    }
                }
                PopWord = UnicodeString();
            }
            result += "chấm ";
        } else if (c == HYPEN_MINUS) {
            if (continuousDigits) {
                continuousDigits  = false;
                if (number.length() <= 4)
                    result += converter.convertNumber(number) + " "; 
                else {
                    StringCharacterIterator iterNum(number);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += converter.convertNumber(cn) + " ";
                    }
                } 
                number = UnicodeString();
            }
            if (continuous_Lowercase_Popular) {
                continuous_Lowercase_Popular  = false;
                if (popularWord.hasWord(PopWord))
                    result+=PopWord+" ";
                else{
                    StringCharacterIterator iterNum(PopWord);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += letterSound.mappingOf(UnicodeString(cn)) + " ";
                    }
                }
                PopWord = UnicodeString();
            }
            result += " , ";
        } else {
            if (continuousDigits) {
                continuousDigits  = false;
                if (number.length() <= 4)
                    result += converter.convertNumber(number) + " "; 
                else {
                    StringCharacterIterator iterNum(number);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += converter.convertNumber(cn) + " ";
                    }
                }
                number = UnicodeString();
            }
            if (continuous_Lowercase_Popular) {
                continuous_Lowercase_Popular  = false;
                if (popularWord.hasWord(PopWord))
                    result+=PopWord+" ";
                else{
                    StringCharacterIterator iterNum(PopWord);
                    for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                        result += letterSound.mappingOf(UnicodeString(cn)) + " ";
                    }
                }
                PopWord = UnicodeString();
            }
            result += letterSound.mappingOf(UnicodeString(c)) + " ";
        }
    }
    if (number.length() > 0) {
        if (number.length() <= 4)
            result += converter.convertNumber(number) + " "; 
        else {
            StringCharacterIterator iterNum(number);
            for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
                result += converter.convertNumber(cn) + " ";
            }
        }
    }
    if (PopWord.length() > 0) {
        StringCharacterIterator iterNum(PopWord);
        for (auto cn = iterNum.first32(); cn != StringCharacterIterator::DONE; cn = iterNum.next32()){
            result += letterSound.mappingOf(UnicodeString(cn)) + " ";
        }
    }
    return result;
}
Address::Address() {
    loadPatterns(POLITICAL_DIVISION, F_POLITICAL_DIVISION);
    loadPatterns(STREET, F_STREET);
    loadPatterns(OFFICE, F_OFFICE);
    loadPatterns(CODENUMBER, F_CODENUMBER);
}
Address::~Address() {
    u_cleanup();
}
