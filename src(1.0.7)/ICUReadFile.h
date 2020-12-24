#pragma  once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bits/stdc++.h>
#include "unicode/utypes.h"
#include "unicode/ustring.h"
#include "unicode/regex.h"
#include "unicode/ucnv.h"
#include "unicode/uclean.h"
using icu::UnicodeString;
class ICUReadFile {
    private: 
        //  Info regarding the line currently being processed
        int lineStart;     // Index of first char of the current line in the file buffer
        int lineEnd;       // Index of char following the new line sequence for the current line
        int lineNum;
        //
        //  Info regarding the file currently being processed
        //
        const char *fileName; 
        int fileLen;              // Length, in UTF-16 Code Units.  
        UChar *ucharBuf;         // Buffer, holds converted file.  (Simple minded program, always reads
        //the whole file at once.
        char *charBuf;          // Buffer, for original, unconverted file data.
    public:
        ICUReadFile();
        ICUReadFile(const char* name);
        UChar* getContentUChar();
        bool readFile();     
        void nextLine(int  startPos);
        int getLineStart() {return lineStart;}
        int getLineEnd() {return lineEnd;}
        int getFileLength() {return fileLen;}
        ~ICUReadFile();
};
