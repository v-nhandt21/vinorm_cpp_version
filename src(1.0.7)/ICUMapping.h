#pragma  once

/////////////////////////////////////////////////
//
// Load mapping from file and store it a map
// Words in mapping file have form "key#value",
// One word per line
//
/////////////////////////////////////////////////

#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
#include "unicode/ustream.h"
#include <bits/stdc++.h>
#include "ICUReadFile.h"
using icu::UnicodeString;
using std::map;
using std::cerr;
using std::cout;
using std::string;
class ICUMapping {
    private:
        // Mapping between unit and pronounciation
        map<UnicodeString, UnicodeString> mapping;
        string mappingName;
    public:
        ICUMapping();
        // Continous calling this functioc will add more words into the mapping
        bool loadMappingFile(const char* name);
        // Remove all current words in mapping
        void clearMapping();
        // Reture mapping of a word
        UnicodeString mappingOf(const UnicodeString& unit); 
        // Check whether a word has mapping
        bool hasMappingOf(const UnicodeString& input);
        // Print out all words in mapping
        void unitTest(); 
        ~ICUMapping();
};
