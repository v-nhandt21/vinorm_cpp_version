#pragma  once
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include "unicode/ustream.h"
#include <bits/stdc++.h>
#include "ICUReadFile.h"
using icu::UnicodeString;
using std::set;
using std::cerr;
using std::cout;
using std::string;
class ICUDictionary{
    private:
        // Set of words in dictionary
        set<UnicodeString> dict;
        string dictName;
    public:
        ICUDictionary();
        // Continous calling this functioc will add more words into the mapping
        bool loadDictFile(const char* name);
        // Remove all current words in mapping
        void clearDict();
        // Check whether a word exists in dictionary
        bool hasWord(const UnicodeString& input);
        // Print out all words in mapping
        void unitTest(); 
        ~ICUDictionary();
};
