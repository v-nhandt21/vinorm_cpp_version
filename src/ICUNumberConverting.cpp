#include "ICUNumberConverting.h"
UnicodeString ConvertingNumber::convertNumberLTHundred(const UnicodeString& num) {
    // less than 10
    if (num.length() == 1)
        return CHU_SO[num.char32At(0) - DIGIT_ZERO];
    UnicodeString result;
    // between 10 and 19
    if (num.char32At(0) == DIGIT_ZERO + 1)
        result = MUOI_F;
    // between 20 and 99
    else {
        result = CHU_SO[num.char32At(0)  - DIGIT_ZERO] + SPACE + MUOI;
    }
    // num is 10, 20, ...
    if (num.char32At(1) == DIGIT_ZERO)
        return result;
    result.append(SPACE);
    if (num.char32At(1) == DIGIT_ZERO + 1 && num.char32At(0) != DIGIT_ZERO +1)
        result.append(MOTS);
    else if (num.char32At(1) == DIGIT_ZERO + 4 && num.char32At(0) != DIGIT_ZERO + 1)
        result.append(TUW);
    else if (num.char32At(1) == DIGIT_ZERO + 5)
        result.append(LAWM);
    else
        result.append(CHU_SO[num.char32At(1) - DIGIT_ZERO]);
    return result;
}
UnicodeString ConvertingNumber::convertNumberLTThousand(const UnicodeString& num) {
    if (num.length() < 3)
        return convertNumberLTHundred(num);
    UnicodeString result;
    result += CHU_SO[num.char32At(0) - DIGIT_ZERO] + SPACE + HUNDRED;
    // 000, 100, 200, ..., 900
    if (num.char32At(1) == DIGIT_ZERO && num.char32At(2) ==  DIGIT_ZERO) {
        if (num.char32At(0) == DIGIT_ZERO)
            return UnicodeString();
        else
            return result;
    }
    // [1-9]0[1-9]
    if (num.char32At(1) == DIGIT_ZERO)
        return result + SPACE + LINH + SPACE + CHU_SO[num.char32At(2) - DIGIT_ZERO];
    return result + SPACE + convertNumberLTHundred(num.tempSubStringBetween(1));
}
UnicodeString ConvertingNumber::convertNumberLTMillion(const UnicodeString& num) {
    if (num.length() < 4)
        return convertNumberLTThousand(num);
    auto splitIndex = num.length() % 3;
    if (splitIndex == 0)
        splitIndex = 3;
    UnicodeString left = convertNumberLTMillion(num.tempSubStringBetween(0, splitIndex));
    UnicodeString right  = convertNumberLTMillion(num.tempSubStringBetween(splitIndex));
    if (left.isEmpty() && right.isEmpty())
        return UnicodeString();
    int hangIndex = (num.length() - splitIndex)/3 - 1;
    UnicodeString hang;
    if (hangIndex == 0)
        hang = NGHIN;
    else
        hang = TRIEU;
    if (left.isEmpty())
        return CHU_SO[0] + SPACE + hang + SPACE + right;
    if (right.isEmpty())
        return left + SPACE + hang;
    return left + SPACE + hang + SPACE + right;
}
UnicodeString ConvertingNumber::convertNumberArbitrary(const UnicodeString& num) {
    if (num.length() < 10)
        return convertNumberLTMillion(num);
    auto splitIndex =  num.length() % 9;
    if (splitIndex == 0 )
        splitIndex = 9;
    auto left =  convertNumberLTMillion(num.tempSubStringBetween(0, splitIndex));
    auto right = convertNumberArbitrary(num.tempSubStringBetween(splitIndex));
    if (left.isEmpty())
        return right;
    auto hang = BILLION;
    for (auto i = (num.length() - splitIndex) / 9 -1; i > 0; i--) {
        hang += SPACE + BILLION;
    }
    if (right.isEmpty())
        return left + SPACE + hang;
    return left + SPACE + hang + COMMA + right;
}
UnicodeString ConvertingNumber::stripZeros(const UnicodeString& num, int32_t z) {
    while (z < num.length() && num.char32At(z) == DIGIT_ZERO) z++;
    if (num.char32At(z-1) == DIGIT_ZERO)
        return UnicodeString(num, z);
    else
        return num;
}
UnicodeString ConvertingNumber::convertNumber(const UnicodeString &num) {
    if (num == UnicodeString("0"))
        return UnicodeString("không");
    
    // if num contains non digit characters, return empty string
    StringCharacterIterator iter(num);
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        if (c < DIGIT_ZERO || c > DIGIT_ZERO + 9)
            return UnicodeString();
    }
    UnicodeString result = stripZeros(num);
    if (result.length()>15)
    {
        UnicodeString longRe =UnicodeString("");
	StringCharacterIterator iter(result);
    	for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32()) {
        	longRe=longRe +UnicodeString(" ")+convertNumber(c);
    	}
        return longRe;
    }
    if (result.isEmpty())
        return UnicodeString();
    else
        result = convertNumberArbitrary(result);
    result.findAndReplace("không nghìn ", "");
    result.findAndReplace("không triệu ", "");
    if (result.length() < 60) {
        result.findAndReplace("tỷ,", "tỷ");
    }
    return result;
}
UnicodeString ConvertingNumber::decimalToRoman(int number) {
    UnicodeString result;
    vector<int> decimal{1000, 900, 500, 400, 100, 90, 50, 40, 10, 9, 5, 4, 1};
    vector<UnicodeString> symbol{
        "M", "CM", "D", "CD", "C", "XC", "L", "XL", "X", "IX", "V", "IV", "I"
    };
    
    int i = 0;;
    while (number > 0) {
        while (number/decimal[i] > 0) {
            result += symbol[i];
            number -= decimal[i];
        }
        i++;
    }
    return result;
}
UnicodeString ConvertingNumber::romanToDecimal(const UnicodeString &roman) {
    vector<int> a(20);
    auto l = roman.length();
    StringCharacterIterator iter(roman);
    int i_offset = 'i' - 'a';
    int v_offset = 'v' - 'a';
    int x_offset = 'x' - 'a';
    int i = 0;
    for (auto c = iter.first32(); c != StringCharacterIterator::DONE; c = iter.next32(), i++) {
        if (c == LATIN_CAPITAL_LETTER_A + i_offset) {
            a[i] = 1;
        }  else if (c == LATIN_CAPITAL_LETTER_A + v_offset) {
            a[i] = 5;
        } else if (c == LATIN_CAPITAL_LETTER_A + x_offset) {
            a[i] = 10;
        }
    }
    int k = a[l-1];
    for (int  i = l -1; i > 0; i--) {
        if (a[i] > a[i-1]) {
            k -= a[i-1];
        } else if (a[i] <= a[i-1]) {
            k += a[i-1];
        }
    }
    // Since the deicalToRoman return result in uppercase
    // We need to upperize the reuslt before comparing
    UnicodeString romanUpper(roman);
    romanUpper.toUpper();
    if (romanUpper == decimalToRoman(k)) {
        UnicodeString result;
        while (k > 0) {
            int d = k % 10;
            result.insert(0, DIGIT_ZERO + d);
            k /= 10;
        }
        return result;
    }
    return roman;
}