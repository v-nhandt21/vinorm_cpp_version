#include "ICUReadFile.h"
ICUReadFile::ICUReadFile() {
    lineStart = lineEnd = lineNum = fileLen = 0;
    fileName = NULL;
    ucharBuf = NULL;
    charBuf = NULL;
}
ICUReadFile::ICUReadFile(const char* name) {
    lineStart = lineEnd = lineNum =  0;
    fileName = NULL;
    ucharBuf = NULL;
    charBuf = NULL;
    //  Initialize global file variables
    fileName = name;
    fileLen  = 0;     
    // zero length prevents processing in case of errors. 
}
UChar* ICUReadFile::getContentUChar() {
    return ucharBuf;
}
bool ICUReadFile::readFile() {
    //  Open the file and determine its size.
    FILE *file = fopen(fileName, "rb");
    if (file == 0 ) {
        fprintf(stderr, "[E] Could not open file \"%s\"\n", fileName);
        return false;
    }
    fseek(file, 0, SEEK_END);
    int rawFileLen = ftell(file);
    fseek(file, 0, SEEK_SET);
     //   Read in the file  
    charBuf    = (char *)realloc(charBuf, rawFileLen+1);   
    int t = static_cast<int>(fread(charBuf, 1, rawFileLen, file));
    if (t != rawFileLen)  {
        fprintf(stderr, "[E] Error reading file \"%s\"\n", fileName);
        fclose(file);
        return false;
    }
    charBuf[rawFileLen]=0;
    fclose(file);
    
    // Look for a Unicode Signature (BOM) in the data
    
    int32_t        signatureLength;
    const char *   charDataStart = charBuf;
    UErrorCode     status        = U_ZERO_ERROR;
    const char*    encoding      = ucnv_detectUnicodeSignature(
            charDataStart, rawFileLen, &signatureLength, &status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "[E] ICU Error \"%s\" from ucnv_detectUnicodeSignature()\n",
                u_errorName(status));
        return false;
    }
    if(encoding!=NULL ){
        charDataStart  += signatureLength;
        rawFileLen     -= signatureLength;
    }
    
    // Open a converter to take the file to UTF-16
    
    UConverter* conv;
    conv = ucnv_open(encoding, &status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "[E] ICU Error \"%s\" from ucnv_open()\n", u_errorName(status));
        return false;
    }
    
    // Convert the file data to UChar.
    //  Preflight first to determine required buffer size.
    
    uint32_t destCap = ucnv_toUChars(conv,
            NULL,           
            0,              
            charDataStart,
            rawFileLen,
            &status);
    if (status != U_BUFFER_OVERFLOW_ERROR) {
        fprintf(stderr, "[E] ucnv_toUChars: ICU Error \"%s\"\n", u_errorName(status));
        return false;
    };
    status = U_ZERO_ERROR;
    ucharBuf = (UChar *)realloc(ucharBuf, (destCap+1) * sizeof(UChar));
    ucnv_toUChars(conv,
            ucharBuf,           
            destCap+1,
            charDataStart,
            rawFileLen,
            &status);
    if (U_FAILURE(status)) {
        fprintf(stderr, "[E] ucnv_toUChars: ICU Error \"%s\"\n", u_errorName(status));
        return false;
    };
    ucnv_close(conv);
    
    //Successful conversion.  Set the global size variables so that
    //     the rest of the processing will proceed for this file.
    
    
    fileLen = destCap;
    return true;
}
void ICUReadFile::nextLine(int  startPos) {
    if (startPos == 0) {
        lineNum = 0;
    } else {
        lineNum++;
    }
    lineStart = lineEnd = startPos;
    for (;;) {
        if (lineEnd >= fileLen) {
            return;
        }
        UChar c = ucharBuf[lineEnd];
        lineEnd++;
        if (c == 0x0a   ||       // Line Feed
                c == 0x0c   ||       // Form Feed
                c == 0x0d   ||       // Carriage Return
                c == 0x85   ||       // Next Line
                c == 0x2028 ||       // Line Separator
                c == 0x2029)         // Paragraph separator
        { 
            break;
        }
    }
    // Check for CR/LF sequence, and advance over the LF if we're in the middle of one.

    if (lineEnd < fileLen           &&
            ucharBuf[lineEnd-1] == 0x0d &&
            ucharBuf[lineEnd]   == 0x0a) 
    {
        lineEnd++;
    }
}
ICUReadFile::~ICUReadFile() {
    free(ucharBuf);
    free(charBuf);
}
