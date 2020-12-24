#include "ICUMapping.h"
ICUMapping::ICUMapping() {
}
bool ICUMapping::loadMappingFile(const char *name) {
    mappingName = string(name);
    ICUReadFile mappingFile(name);
    bool readStatus = mappingFile.readFile();
    if (!readStatus) {
        cerr << "[E] Cannot load file " << name << " for mapping\n";
        return false;
    }
    for (mappingFile.nextLine(0); mappingFile.getLineStart() < mappingFile.getFileLength();
            mappingFile.nextLine(mappingFile.getLineEnd())) {
        UnicodeString line = UnicodeString(FALSE, mappingFile.getContentUChar() + mappingFile.getLineStart(), mappingFile.getLineEnd()  - mappingFile.getLineStart());
        auto separatorPosition = line.indexOf(UnicodeString("#"));
        UnicodeString unit = UnicodeString(line, 0, separatorPosition);
        UnicodeString pronoun = UnicodeString(line, separatorPosition+ 1);
        unit.trim();
        pronoun.trim();
        mapping[unit] = pronoun;
    }
    cerr << "[L] Add words to map " << name << " successfully" << '\n';
    return true;
}
UnicodeString ICUMapping::mappingOf(const UnicodeString &unit) {
    UnicodeString unitTrim(unit);
    unitTrim.trim();
    if (unitTrim.length() == 0)
        return unit;
    
    // Try original first
    if (mapping.find(unitTrim) != mapping.end()) {
        return mapping[unitTrim];
    }
    if ( mappingName == "Mapping/Teencode.txt" )
    {
        UnicodeString upperCase(unitTrim);
        upperCase.toUpper();
        if ( unitTrim == upperCase )
        {
            return UnicodeString();
        }
    }
    // Try lowerized
    unitTrim.toLower();
    if (mapping.find(unitTrim) != mapping.end())
        return mapping[unitTrim];
    cerr << "[L] Word not found in mapping " << mappingName << " : " << unit << '\n';
    return UnicodeString();
}
bool ICUMapping::hasMappingOf(const UnicodeString& input) {
    UnicodeString word(input);
    word.trim();
    
    if (mapping.find(word) != mapping.end())
        return true;
    
    if ( mappingName == "Mapping/Teencode.txt" )
    {
        UnicodeString upperCase(word);
        upperCase.toUpper();
        if ( word == upperCase )
        {
            return false;
        }
    }

    word.toLower();
    if (mapping.find(word) != mapping.end())
        return true;
    
    return false;
}
void ICUMapping::clearMapping() {
    mapping.clear();
}
void ICUMapping::unitTest() {
    for (auto& p : mapping) {
        cout << p.first << " ----- " << p.second << '\n'; 
    }
}
ICUMapping::~ICUMapping() {
}