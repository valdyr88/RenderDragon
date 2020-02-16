#ifndef STRINGS_H
#define STRINGS_H

#include <cstdarg>
#include <string>
//#include <string.h>

//shader compiler ne podrzava include ni predaju defineova, no podrzava predavanje vise char* stringova. treba napraviti dobre string funkcije za ovo!

namespace str{

	/*
		void printwchar(wChar* Str1, int size, const wChar* Str2, ...)
		{
			//int a1 = strlenwchar(Str2);

			va_list argList;
			va_start(argList, Str2);

			//int a2 = strlenwchar(Str2);
			//swprintf_s
			//_vswprintf_c
			//vswprintf_s
			_vswprintf_c(Str1, size, Str2, argList);
			va_end(argList);
		}
	*/

	template<typename Type> inline size_t strleng(const Type* str){ if(str == NULL) return 0; size_t i = 0; for(i = 0; str[i] != 0; ++i); return i; }
	inline size_t cstrleng(const char* str){ if(str == NULL) return 0; size_t i = 0; for(i = 0; str[i] != 0; ++i); return i; }

	template<typename Type> inline ::std::string to_std_string(Type* str){

		size_t len = strleng(str);
		::std::string stdstr(len + 1, 0);

		for(size_t i = 0; i < len; ++i){

			char c = (char)str[i];
			stdstr[i] = c;
		}

		return stdstr;
	}

	template<typename Type1, typename Type2> inline bool copystr(const Type1* source, Type2* dest, size_t dest_len){
		if(source == NULL || dest == NULL) return false;

		size_t i = 0; for(; source[i] != 0 && i < dest_len - 1; ++i){ dest[i] = (Type2)source[i]; }
		dest[i] = 0;

		return true;
	}

	template<typename Type2, typename Type1> inline Type2* mkstr(const Type1* source)
	{
		if(source == NULL) return NULL;

		size_t size = strleng(source);
		Type2* outstr = __new Type2[size + 1];

		if(copystr(source, outstr, size + 1) == false){
			__release_array(outstr); return NULL;
		}

		return outstr;
	}

	template<typename Type2> inline Type2* mkstr(size_t size)
	{
		Type2* outstr = __new Type2[size + 1];
		zero_memory(outstr, size + 1);

		return outstr;
	}

	//writes \0 at first occurence of c in Str
	template<typename Type1, typename Type2> inline void discard_string_at_char(Type1* Str, Type2 c){
		if(!Str) return;
		for(size_t i = 0; Str[i] != 0; ++i){
			if(Str[i] == c){
				Str[i] = 0; return;
			}
		}
	}

	//writes \0 after first occurence of c in Str
	template<typename Type1, typename Type2> inline void discard_string_after_char(Type1* Str, Type2 c){
		if(!Str) return;
		for(size_t i = 0; Str[i] != 0; ++i){
			if(Str[i] == c && Str[i + 1] != 0){
				Str[i + 1] = 0; return;
			}
		}
	}

	template<typename Type1, typename Type2> inline Type2* find_first_occurence(Type1* String, Type2* MainString, size_t strlenString = 0)
	{
		if(MainString == NULL) return NULL;
		if(String == NULL) return MainString;

		size_t charsFound = 0; if(strlenString == 0) strlenString = strleng(String);

		for(size_t i = 0; MainString[i] != 0; ++i){

			if(MainString[i] == String[0]){
				Type2* ptr = &MainString[i];
				for(charsFound = 0; charsFound <= strlenString; ++charsFound){
					if(ptr[charsFound] != String[charsFound])
						break;
				}
				if(charsFound == strlenString) return ptr;
				else charsFound = 0;
			}
		}

		return NULL;
	}

	template<typename Type1, typename Type2> inline Type2* skip_string_in_string(Type1* StringToSkip, Type2* MainString, bool SearchJustFromBegining)
	{
		if(MainString == NULL) return NULL;
		if(StringToSkip == NULL) return MainString;

		Type2* outPtr = MainString; size_t strlenString = strleng(StringToSkip);
		Type2* firstOccurencePtr = find_first_occurence(StringToSkip, MainString);

		if(SearchJustFromBegining == true){
			if(firstOccurencePtr != MainString) return NULL;
			else return (firstOccurencePtr + strlenString + 1);
		}
		else{ return (firstOccurencePtr + strlenString + 1); }
	}

	template<typename Type> inline bool is_empty(Type* str){
		if(!str) return true;
		for(size_t i = 0; str[i] != 0; ++i){
			if(str[i] != ' ') return false;
		}
		return true;
	}

	//returns true if str contains char c
	template<typename Type1, typename Type2> inline bool string_contains_char(Type1* str, Type2 c){
		if(!str) return false;
		for(size_t i = 0; str[i] != 0; ++i) if(str[i] == (Type1)c) return true;
		return false;
	}

	//Copies next tag from string Line into outTag. Returns number of copied and skipped wChars. outLine is a pointer to wChar* that points to Line[copied + skipped] or NULL if copied + skipped > strlen(Line);
	template<typename Type> inline size_t get_next_tag_from_string(Type* Line, Type* Delimiters, Type* outTag, int lenTag, Type** outLine){
		if(Line == NULL) return -1;
		size_t l = 0; size_t i = 0; --lenTag; outTag[0] = 0; *outLine = NULL;

		for(i = 0; Line[i] != 0; ++i){

			if(!string_contains_char(Delimiters, Line[i]) && l < lenTag){
				outTag[l] = Line[i]; ++l; outTag[l] = 0;
			}
			else{
				size_t j = i;
				for(j = i; Line[j] != 0; ++j){
					if(!string_contains_char(Delimiters, Line[j])){
						*outLine = &Line[j]; return j;
					}
				}
				return j;
			}
		}

		return i;
	}

	//Copies next tag from string Line into outTag. Tags starting with StringDelimiter are treated as whole tags without the StringDelimiter removed. Returns number of copied and skipped wChars. outLine is a pointer to wChar* that points to Line[copied + skipped] or NULL if copied + skipped > strlen(Line);
	template<typename Type> inline size_t get_next_tag_from_string(Type* Line, Type* Delimiters, Type* StringDelimiters, Type* outTag, int lenTag, Type** outLine){
		if(Line == NULL) return -1;
		size_t l = 0; size_t i = 0; --lenTag; outTag[0] = 0; *outLine = NULL;
		bool StringDelimiterOpen = false;

		for(i = 0; Line[i] != 0; ++i){

			if(string_contains_char(StringDelimiters, Line[i])){
				StringDelimiterOpen = !StringDelimiterOpen;
			}
			if(StringDelimiterOpen || !string_contains_char(Delimiters, Line[i]) && l < lenTag){
				outTag[l] = Line[i]; ++l; outTag[l] = 0;
			}
			else{
				size_t j = i;
				for(j = i; Line[j] != 0; ++j){
					if(!string_contains_char(Delimiters, Line[j])){
						*outLine = &Line[j]; return j;
					}
				}
				return j;
			}
		}

		return i;
	}

	//returns 'a' - alpha, 'f' - float, 'i' - integer
	template<typename Type> inline char is_number(Type* Str){
		if(!Str) return '0';
		if(Str[0] == 0) return '0';
		char outType = 'i';

		uint start = 0;
		uint nof_dots = 0;
		uint nof_pn_signs = 0;
		uint nof_e_signs = 0;

		if((char)Str[0] == '.'){ start = 1; nof_dots = 1; outType = 'f'; }
		else if((char)Str[0] == '+' || (char)Str[0] == '-'){ start = 1; nof_pn_signs = 1; }
		if(Str[start] == 0 || string_contains_char("0123456789", Str[start]) == false){ return 'a'; }

		for(int i = start; Str[i] != 0; ++i){

			if(string_contains_char("0123456789", Str[i]) == true){

			}
			else{

				if((char)Str[i] == 'e'){
					outType = 'f'; nof_e_signs++; if(nof_e_signs > 1){ return 'a'; }
				}
				else if((char)Str[i] == '+' || (char)Str[i] == '-'){
					nof_pn_signs++; if(nof_e_signs == 0 && nof_pn_signs > 1 || nof_pn_signs > 2){ return 'a'; }
				}
				else if((char)Str[i] == '.'){
					outType = 'f'; ++nof_dots; if(nof_dots > 1){ return 'a'; }
				}
				else if((char)Str[i] == 'f' && (char)Str[i + 1] == 0){
					return 'f';
				}
				else{
					return 'a';
				}
			}

		}

		return outType;
	}

	//Checks if Str begins and finishes with \". Does not skip leading spaces or tabs.
	template<typename Type> inline bool is_string(Type* Str){
		if(!Str) return 0;
		int len = strleng(Str);
		if(len == 0) return 0;
		return Str[0] == (Type)'"' && Str[len - 1] == (Type)'"';
	}

	//Checks if Str begins and finishes with delimiter. Does not skip leading spaces or tabs.
	template<typename Type> inline bool is_string(Type* Str, Type delimiter){
		if(!Str) return 0;
		int len = strleng(Str);
		if(len == 0) return 0;
		return Str[0] == delimiter && Str[len - 1] == delimiter;
	}

	//Removes begining and finishing delimiter
	template<typename Type> inline Type* remove_string_delimiters(Type* Str, Type delimiter){
		if(!Str) return 0;
		bool strDelimiterStart = 0;
		for(int i = 0; Str[i] != 0; ++i){

			if(Str[i] == delimiter){ strDelimiterStart = !strDelimiterStart; }

			if(strDelimiterStart){
				if(Str[i + 1] != delimiter) Str[i] = Str[i + 1];
				else Str[i] = Str[i + 2];
			}
			if(Str[i] == 0) break;
		}
		return Str;
	}

	inline int to_int(char* string){
		return (int)atoi(string);
	}
	inline int to_int(wchar_t* string){
		return (int)_wtoi(string);
	}
	inline float to_float(char* string){
		return (float)atof(string);
	}
	inline float to_float(wchar_t* string){
		return (float)_wtof(string);
	}
	inline double to_double(char* string){
		return atof(string);
	}
	inline double to_double(wchar_t* string){
		return _wtof(string);
	}


	inline void to_string(int value, char* string, size_t string_len){
		sprintf_s(string, string_len, "%d", value);
	}
	inline void to_string(double value, char* string, size_t string_len){
		sprintf_s(string, string_len, "%f", value);
	}

	/*
	inline void to_string(float value, wchar_t* string){
		wsprintf(string, L"%f", value);
	}
	inline void to_string(int value, wchar_t* string, size_t string_len){
		wsprintf(string, L"%d", value);
	}
	inline void to_string(double value, wchar_t* string, size_t string_len){
		wsprintf(string, L"%f", value);
	}*/

	//Gets array of floats from Str into fArray. Returns number of floats read (which is smaller than maxElements).
	template<typename Type> inline int get_float_array_from_string(Type* Str, float* fArray, int maxElements, Type* Delimiters, Type* StrDelimiters){
		if(!Str) return 0;
		Type CBuffer[128]; int i = 0;
		do{
			get_next_tag_from_string(Str, Delimiters, StrDelimiters, CBuffer, 128, &Str); if(CBuffer[0] == 0) continue;

			if(is_number(CBuffer) != 'a'){
				fArray[i] = (float)to_float(CBuffer);
				++i;
			}

		} while(Str != NULL && i < maxElements);
		return i;
	}

	template<typename Type1, typename Type2> inline int difference_between_strings_ci(Type1* Str1, Type2* Str2){

		int difference = 0;

		int i = 0;
		for(; Str1[i] && Str2[i]; ++i){

			if(Str1[i] != Str2[i]){
				difference = ((int)Str1[i]) - ((int)Str2[i]);
				if(abs(difference) != 0x20) return difference;
			}

		}
		if(Str1[i] != Str2[i]) return -257;//razlicite su duljine stringovi

		return 0;
	}

	template<typename Type1, typename Type2> inline bool compare_strings_ci(Type1* Str1, Type2* Str2){
		return difference_between_strings_ci(Str1, Str2) == 0;
	}

	template<typename Type1, typename Type2> inline int difference_between_strings(Type1* Str1, Type2* Str2){

		int difference = 0;

		int i = 0;
		for(; Str1[i] && Str2[i]; ++i){

			if(Str1[i] != Str2[i]){
				difference = ((int)Str1[i]) - ((int)Str2[i]);
				if(difference != 0) return difference;
			}

		}
		if(Str1[i] != Str2[i]) return -257;//razlicite su duljine stringovi

		return 0;
	}

	template<typename Type1, typename Type2> inline bool compare_strings(Type1* Str1, Type2* Str2){
		return difference_between_strings(Str1, Str2) == 0;
	}

	template<typename Type1, typename Type2> inline bool string_has_substring(Type1* string, Type2* substring){
		return find_first_occurence(substring, string) != NULL;
	}


	/*
	void printwchar(wChar* Str1, int size, const wChar* Str2, ...)
	{
	va_list argList;
	va_start(argList, Str2);

	_vswprintf_c(Str1, size, Str2, argList);
	va_end(argList);
	}
	*/
	inline bool print_to_string(char* out_string, size_t len_out_string, char* format, ...){

		if(out_string == NULL) return false; if(len_out_string == 0) return false;

		va_list argList;
		va_start(argList, format);

		vsprintf_s(out_string, len_out_string, format, argList);

		va_end(argList);

		return true;
	}

	inline bool print_to_string(wchar_t* out_string, size_t len_out_string, wchar_t* format, ...){

		if(out_string == NULL) return false; if(len_out_string == 0) return false;

		va_list argList;
		va_start(argList, format);

		_vswprintf_c(out_string, len_out_string, format, argList);

		va_end(argList);

		return true;
	}

	//fills string with character c. if len parameter is left out, it is assumed that out_string is a zero terminating string and string is filled until the zero terminating character.
	template<typename Type1, typename Type2> inline void fill_string(Type1* out_string, Type2 c, size_t len = (size_t)-1){
		for(size_t i = 0; out_string[i] != 0 && (i < len); ++i){
			out_string[i] = (Type1)c;
		}
	}

	template<typename Type> inline bool shift_string(Type* string, size_t size, int location, int shift){
		if(string == NULL) return false;
		int start = location + shift;
		if(start < 0){ return false; }

		int j = location; int i = start;
		for(; string[i] && string[j]; ++i, ++j){
			string[i] = string[j];
		}
		string[i] = 0;

		return true;
	}

	template<typename Type1, typename Type2> bool container_has_string(::std::list<Type1>& container, Type2* string){
		if(string == NULL) return false;

		for(uint i = 0; i < container.size(); ++i){
			Type1* container_string = &container[i];
			if(str::compare_strings(container_string, string) == true){ return true; }
		}
		return false;
	}

	template<typename Type1, typename Type2> inline size_t strleng(const Type1* str, Type2* delimiters){
		size_t i = 0; for(i = 0; str[i] != NULL && string_contains_char(delimiters, str[i]) == false; ++i); return i;
	}

	template<typename Type1, typename Type2> inline void get_file_extension(const Type1* path, Type2* dest_str, size_t dest_leng){
		if(dest_str == NULL || path == NULL || dest_leng == 0) return;
		int strlength = (int)strleng(path);

		for(int i = strlength - 1; i >= 0; --i){
			if(path[i] == '.'){
				copystr(&path[i], dest_str, dest_leng); return;
			}
		}

		dest_str[0] = 0; return;
	}

};
#endif //STRINGS_H