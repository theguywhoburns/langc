#include <util.h>
#include <iconv.h>

size_t WideToUtf8(const char* buf, size_t buflen, const wchar_t* src, size_t srclen) {
    iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
    if(cd == (iconv_t)-1) {assert(false);}

    size_t len= wcslen(src);
    size_t in_bytes_left = len * sizeof(wchar_t);
    size_t out_bytes_left= len * 4; // TODO: wut???

    char* in  = (char*)src;
    char* out = (char*)malloc(out_bytes_left + 1);
    
    size_t ret = iconv(cd, &in, &in_bytes_left, &out, &out_bytes_left);
    iconv_close(cd);
    return ret;
}

FILE* OpenFileHandle(const char* path, const char* flags) {

}

FILE* OpenFileHandleW(const wchar_t* path, const wchar_t* flags) {

}