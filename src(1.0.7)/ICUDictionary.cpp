#include "ICUDictionary.h"
ICUDictionary::ICUDictionary() {
}
bool ICUDictionary::loadDictFile(const char *name) {
    dictName = name;
    ICUReadFile dictFile(name);
    bool readStatus = dictFile.readFile();
    if (!readStatus) {
        cerr << "[E] Cannot load file " << name << " for dictionary\n";
        return false;
    }
    for (dictFile.nextLine(0); dictFile.getLineStart() < dictFile.getFileLength();
            dictFile.nextLine(dictFile.getLineEnd())) {
        UnicodeString line = UnicodeString(FALSE, dictFile.getContentUChar() + dictFile.getLineStart(), dictFile.getLineEnd()  - dictFile.getLineStart());
        line.trim();
        dict.insert(line);
    }
    cerr << "[L] Add words to dictionary " << name << " successfully" << '\n';
    return true;
}
bool ICUDictionary::hasWord(const UnicodeString& input) {
    UnicodeString word(input);
    word.trim();
    // Check original word
    if (dict.find(word) != dict.end())
        return true;
    // If original word does not exist in dictionary
    // Try to lowerize the word and rechec
    word.toLower();
    if (dict.find(word) != dict.end())
        return true;
    return false;
}
void ICUDictionary::clearDict() {
    dict.clear();
}
void ICUDictionary::unitTest() {
    for (auto& w : dict) {
        cout << w << '\n';
    }
}
ICUDictionary::~ICUDictionary() {
}
