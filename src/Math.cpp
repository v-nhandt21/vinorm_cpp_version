#include "Math.h"
Math::Math() {
    
    
    loadPatterns(ROMAN_NUMBER, F_ROMAN_NUMBER);
    loadPatterns(MEASUREMENT, F_MEASUREMENT);
    loadPatterns(MEASUREMENT_1, F_MEASUREMENT_1);
    loadPatterns(NORMAL_NUMBER, F_NORMAL_NUMBER);
}
Math::~Math() {
    u_cleanup();
}
UnicodeString Math::stringForReplace(int categories, RegexMatcher* matcher, UErrorCode &status, int pattern) {
    cerr << "[L] Using rule " << categories << " of Math\n";
    switch(categories) {
        case ROMAN_NUMBER:
            return regexRomanNumber(matcher, status);
        case MEASUREMENT:
            return LocalhandleMeasurement(matcher,status,1,pattern);
        case MEASUREMENT_1:
            return LocalhandleMeasurement(matcher,status,0,pattern);
        case NORMAL_NUMBER:
            return regexNormalNumber(matcher, status, pattern);
            
        default:
            cerr << "[E] Invalid category: " << categories << '\n';
    }
    return UnicodeString();
}
void Math::loadPatterns(int categories, string filename) {
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
UnicodeString Math::normalizeText(const UnicodeString &input) {
    
    UnicodeString result;
    UnicodeString preResult(input);
    
    for (auto& rule : patterns) {
        int categ = rule.first;
        auto regexPatterns = rule.second;
        int noPattern=0;
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
                UnicodeString replaceString = stringForReplace(categ, matcher, status, noPattern);
                if (replaceString != "")
                {
                    matcher->appendReplacement(result," "+replaceString+" ", status);
                }
            }
            matcher->appendTail(result);
            delete matcher;
            delete regexPattern;
            preResult = result;
            result  = UnicodeString();
            noPattern+=1;
        }
    }
    return preResult;
}
UnicodeString Math::regexRomanNumber(RegexMatcher* outerMatcher, UErrorCode &outerStatus) {
    UnicodeString match = outerMatcher->group(outerStatus);
    UnicodeString romanPattern("[v|i|x|m|d|l|V|I|X|M|D|L]{1,5}");
    ConvertingNumber converter;
    UErrorCode status = U_ZERO_ERROR;   
    UParseError    parseErr;   
    RegexPattern  *rePat = RegexPattern::compile(romanPattern, parseErr, status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "[E] error in pattern: \"%s\" at position %d\n",
                u_errorName(status), parseErr.offset);
        delete rePat;
        return UnicodeString();
    }
    RegexMatcher *matcher = rePat->matcher(match, status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "[E]  error in creating RegexMatcher: \"%s\"\n",
                u_errorName(status));
        delete matcher;
        delete rePat;
        return UnicodeString();
    }
    UnicodeString result;
    while (matcher->find(status) && U_SUCCESS(status)) {
        UnicodeString found = matcher->group(status);
        cerr << "[L] Norman Number found : " << found << '\n';
        matcher->appendReplacement(result, converter.romanToDecimal(found) ,status);
    }
    matcher->appendTail(result);
    delete matcher;
    delete rePat;
    cerr<<"Roman  "<<result <<'\n';
    return result;
}
UnicodeString Math::regexNormalNumber(RegexMatcher* matcher, UErrorCode &status, int pattern) {
    UnicodeString match = matcher->group(status);
    UnicodeString sign="";
    if (matcher->group(1, status).trim() == "-"){
        sign+=" trừ ";
    }
    if (pattern==0 || pattern ==2)
        return sign + ICUHelper::readNumber(match,0);
    else
        return sign + ICUHelper::readNumber(match,1);
}
bool Math::LocalisNumber(UnicodeString& unit) {
    StringCharacterIterator iter(unit);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (c < DIGIT_ZERO || c > DIGIT_ZERO + 9)
            return false;
    }
    return true;
}
UnicodeString Math::LocalhandleMeasurement(RegexMatcher* matcher, UErrorCode &status, bool UseBaseUnit, int pattern) {
    int point;
    UnicodeString match = matcher->group(status);
    if(UseBaseUnit){
        if (pattern == 1 | pattern ==3){
            point =1;
        }
        else point=0;
        unitBaseMapping.loadMappingFile(unitBaseFile.data());
        UnicodeString result="";

        UnicodeString number = matcher->group(1, status);
        result+=" " +  ICUHelper::readNumber(number,point);

        if (!unitBaseMapping.hasMappingOf(matcher->group(2, status).trim())) {
            return "";
            //result+=" "+matcher->group(2, status)+" ";
        }
        else{
            result+=" "+unitBaseMapping.mappingOf(matcher->group(2, status).trim())+" ";
        }
        if(matcher->group(3, status)!=""){
            result+=" trên ";
            if (!unitBaseMapping.hasMappingOf(matcher->group(3, status).trim())) {
                return "";
                //result+=" "+matcher->group(3, status)+" ";
            }
            else{
                result+=" "+unitBaseMapping.mappingOf(matcher->group(3, status).trim())+" ";
            }
        }
        if (matcher->group(4, status).trim() == "-" )  {
            result += " đến ";
        }
        return result;
    }
    else
    {
        if (pattern == 1 | pattern ==3){
            point =1;
        }
        else point=0;
        bool haveUnit = matcher->groupCount() > 1;
        UnicodeString number = matcher->group(1, status).trim();
        UnicodeString unit;
        if (haveUnit) unit = matcher->group(2, status).trim();
        unitCurrencyMapping.loadMappingFile(unitCurrencyFile.data());
        UnicodeString next="";
        if (matcher->group(3, status).trim() == "-" )  {
            next = "đến";
        }
        if (!unitCurrencyMapping.hasMappingOf(unit)) {
            cerr << "[L] Mapping Unit of [" << unit << "] does not exist\n";
            return " " +  ICUHelper::readNumber(number,point) + " "  +next+" ";
        }
        else{
            return " " +  ICUHelper::readNumber(number,point) + " "  + unitCurrencyMapping.mappingOf(unit)+" "+next+" ";
        }
    }
    return UnicodeString();
}
