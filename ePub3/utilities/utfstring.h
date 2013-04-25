//
//  utfstring.h
//  ePub3
//
//  Created by Jim Dovey on 2012-11-22.
//  Copyright (c) 2012-2013 The Readium Foundation and contributors.
//  
//  The Readium SDK is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef __ePub3_xml_string__
#define __ePub3_xml_string__

#include <ePub3/utilities/basic.h>
#include <string>
#include <iterator>
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
#include <initializer_list>
#endif
#include <locale>
#include <vector>
#include REGEX_INCLUDE
#include <map>
#include <stdexcept>
#include <libxml/xmlstring.h>

#include <utf8/utf8.h>

// the GNU runtime hasn't updated std::string to the C++11 standard yet, so much of
// our `const_iterator` usage needs to be plain `iterator` to keep Android happy.
#if defined(CXX11_STRING_UNAVAILABLE) && CXX11_STRING_UNAVAILABLE
# define cxx11_const_iterator iterator
#else
# define cxx11_const_iterator const_iterator
#endif

EPUB3_BEGIN_NAMESPACE

class string;
typedef std::map<string, string>  NamespaceMap;

extern const size_t utf8_sizes[256];
#define UTF8CharLen(c) ePub3::utf8_sizes[static_cast<xmlChar>(c)]

/**
 @ingroup utilities
 */
class string
{
public:
    typedef std::string __base;
    
    typedef __base::size_type           size_type;
    typedef __base::difference_type     difference_type;
    typedef std::char_traits<char32_t>  traits_type;
    typedef char32_t                    value_type;
    typedef value_type&                 reference;
    typedef const value_type&           const_reference;
    typedef value_type*                 u4pointer;
    typedef const value_type*           const_u4pointer;
    
    static value_type utf8_to_utf32(const xmlChar * utf8);
    static value_type utf8_to_utf32(const __base::const_iterator p);
    
    template <typename _CharT>
    static __base utf8_of(_CharT ch) { return _Convert<_CharT>::toUTF8(ch); }
    
    static const string EmptyString;
    
    
    
    class InvalidUTF8Sequence : std::invalid_argument {
    public:
        InvalidUTF8Sequence(const std::string & str) : invalid_argument(str) {}
        InvalidUTF8Sequence(const char * str) : invalid_argument(str) {}
    };
    
    typedef utf8::iterator<__base::iterator>        iterator;
    typedef utf8::iterator<__base::const_iterator>  const_iterator;
    
    static const size_type npos;
    
#if 0
#pragma mark - Construction/Destruction
#endif
    
    // Standard
    string() : _base() {}
    string(const string &o) : _base(o._base) {}
    string(string &&o) : _base(std::move(o._base)) {}
    string(const string & s, size_type i, size_type n=npos) : _base(s._base, s.to_byte_size(i), s.to_byte_size(i,n)) {}
    
    // From char32_t (value_type)
    string(const_u4pointer s);   // NUL-delimited
    string(const_u4pointer s, size_type n);
    string(size_type n, value_type c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string(std::initializer_list<value_type> __il) : string(__il.begin(), __il.end()) {}
#endif
    
    // From char16_t (pure UTF-16)
    string(const char16_t* s);    // NUL-delimited
    string(const char16_t* s, size_type n);
    string(size_type n, char16_t c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string(std::initializer_list<char16_t> __il) : string(__il.begin(), __il.end()) {}
#endif
    
    // From std::string
    string(const __base &o) : _base(o) {}
    string(__base &&o) : _base(o) {}
    string(const __base &s, size_type i, size_type n=npos);
    
    // From char
    string(const char * s) : _base(s) {}
    string(const char * s, size_type n) : _base(s, n) {}
    string(size_type n, char c) : _base(n, c) {}
    
    // From xmlChar (unsigned char)
    string(const xmlChar * s) : _base(reinterpret_cast<const char *>(s)) {}
    string(const xmlChar * s, size_type n) : _base(reinterpret_cast<const char *>(s)) {}
    string(size_type n, xmlChar c) : _base(n, static_cast<char>(c)) {}
    
    template <class InputIterator>
    string(InputIterator begin, InputIterator end);
    /*
    template <typename... Args>
    string(const Args&... args) : _base(_Str(args...)) {}
    */
    ~string() {}
    
#if 0
#pragma mark - Length/Iteration/Indexing
#endif
    
    size_type size() const _NOEXCEPT;
    size_type length() const _NOEXCEPT { return size(); }
    size_type max_size() const _NOEXCEPT { return _base.max_size()/sizeof(value_type); }
    size_type capacity() const _NOEXCEPT { return _base.capacity(); }
    
    size_type utf8_size() const _NOEXCEPT { return _base.size(); }
    size_type utf8_length() const _NOEXCEPT { return utf8_size(); }
    
    void resize(size_type n, value_type c);
    void resize(size_type n);
    
    void reserve(size_type res_arg = 0) { return _base.reserve(res_arg*4); } // best guess
    void shrink_to_fit() { _base.shrink_to_fit(); }
    void clear() _NOEXCEPT { _base.clear(); }
    bool empty() const _NOEXCEPT { return _base.empty(); }
    
    iterator begin() _NOEXCEPT { return iterator(_base.begin(), _base.begin(), _base.end()); }
    const_iterator begin() const { return const_iterator(_base.begin(), _base.begin(), _base.end()); }
    const_iterator cbegin() const { return const_iterator(_base.begin(), _base.begin(), _base.end()); }
    iterator end() _NOEXCEPT { return iterator(_base.end(), _base.begin(), _base.end()); }
    const_iterator end() const { return const_iterator(_base.end(), _base.begin(), _base.end()); }
    const_iterator cend() const { return const_iterator(_base.end(), _base.begin(), _base.end()); }
    
    const value_type at(size_type pos) const;
    value_type at(size_type pos);
    
    const value_type operator[](size_type pos) const { return at(pos); }
    value_type operator[](size_type pos) { return at(pos); }
    
    const xmlChar * xmlAt(size_type pos) const;
    xmlChar * xmlAt(size_type pos);
    
    __base utf8At(size_type pos) const;
    
#if 0
#pragma mark - Splitting
#endif
    
    // courtesy of: http://stackoverflow.com/questions/9435385/split-a-string-using-c11
    inline std::vector<string> split(const REGEX_NS::regex& regex) const
    {
        // passing -1 as the submatch index parameter performs splitting
        REGEX_NS::sregex_token_iterator first(_base.begin(), _base.end(), regex, -1), last;
        return {first, last};
    }
    
#if 0
#pragma mark - Assign
#endif
    
    template <class InputIterator>
    string & assign(InputIterator first, InputIterator last);
    
    // standard
    string & assign(const string &o) { _base.assign(o._base); return *this; }
    string & assign(const string &o, size_type i, size_type n=npos);
    string & assign(string &&o) { _base.assign(std::move(o._base)); return *this; }
    string & operator=(const string & o) { return assign(o); }
    string & operator=(string &&o) { return assign(o); }
    
    // char32_t
    string & assign(const_u4pointer s, size_type n=npos);
    string & assign(size_type n, value_type c) { clear(); resize(n, c); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & assign(std::initializer_list<value_type> __il) { return assign(__il.begin(), __il.end()); }
#endif
    string & operator=(const_u4pointer s) { return assign(s, npos); }
    string & operator=(value_type c) { return assign(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator=(std::initializer_list<value_type> l) { return assign(l); }
#endif
    
    // char16_t
    string & assign(const char16_t* s, size_type n=npos);
    string & assign(size_type n, char16_t c) { clear(); resize(n, static_cast<value_type>(c)); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & assign(std::initializer_list<char16_t> __il) { return assign(__il.begin(), __il.end()); }
#endif
    string & operator=(const char16_t* s) { return assign(s, npos); }
    string & operator=(char16_t c) { return assign(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator=(std::initializer_list<char16_t> l) { return assign(l); }
#endif
    
    // std::string
    string & assign(const __base & o) { _base.assign(o); return *this; }
    string & assign(const __base & o, size_type i, size_type n=npos)
        { _base.assign(o, i, n); return *this; }
    string & assign(__base &&o) { _base.assign(o); return *this; }
    string & operator=(const __base &o) { return assign(o); }
    string & operator=(__base &&o) { return assign(o); }
    
    // char
    string & assign(const char * s, size_type n) { _base.assign(s, n); return *this; }
    string & assign(const char * s) { _base.assign(s); return *this; }
    string & assign(size_type n, char c) { _base.assign(n, c); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & assign(std::initializer_list<__base::value_type> __il) { _base.assign(__il); return *this; }
#endif
    string & operator=(const char * s) { return assign(s, __base::traits_type::length(s)); }
    string & operator=(char c) { return assign(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator=(std::initializer_list<__base::value_type> __il) { return assign(__il); }
#endif
    
    // xmlChar
    string & assign(const xmlChar * s, size_type n) { _base.assign(reinterpret_cast<const char *>(s), n); return *this; }
    string & assign(const xmlChar * s) { _base.assign(reinterpret_cast<const char *>(s), xmlStrlen(s)); return *this; }
    string & assign(size_type n, xmlChar c) { _base.assign(n, static_cast<char>(c)); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & assign(std::initializer_list<xmlChar> __il) { return assign(__il.begin(), __il.end()); }
#endif
    string & operator=(const xmlChar *s) { return assign(s, xmlStrlen(s)); }
    string & operator=(xmlChar c) { return assign(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator=(std::initializer_list<xmlChar> __il) { return assign(__il); }
#endif
    
#if 0
#pragma mark - Append
#endif
    
    template <class InputIterator>
    string & append(InputIterator first, InputIterator last);
    
    template <typename... Args>
    string & append(const Args&... args) { return append(string(args...)); }
    
    // standard
    string & append(const string &o) { _base.append(o._base); return *this; }
    string & append(const string &o, size_type i, size_type n=npos);
    string & append(string &&o) { _base.append(std::move(o._base)); return *this; }
    string & operator+=(const string & o) { return append(o); }
    string & operator+=(string &&o) { return append(o); }
    
    // char32_t
    string & append(const_u4pointer s, size_type n=npos);
    string & append(size_type n, value_type c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & append(std::initializer_list<value_type> __il) { return append(__il.begin(), __il.end()); }
#endif
    string & operator+=(const_u4pointer s) { return append(s, npos); }
    string & operator+=(value_type c) { return append(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator+=(std::initializer_list<value_type> __il) { return append(__il); }
#endif
    
    // char16_t
    string & append(const char16_t* s, size_type n=npos);
    string & append(size_type n, char16_t c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & append(std::initializer_list<char16_t> __il) { return append(__il.begin(), __il.end()); }
#endif
    string & operator+=(const char16_t* s) { return append(s, npos); }
    string & operator+=(char16_t c) { return append(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator+=(std::initializer_list<char16_t> __il) { return append(__il); }
#endif
    
    // std::string
    string & append(const __base & o) { _base.append(o); return *this; }
    string & append(const __base & o, size_type i, size_type n=npos) { _base.append(o, i, n); return *this; }
    string & append(__base &&o) { _base.append(o); return *this; }
    string & operator+=(const __base &o) { return append(o); }
    string & operator+=(__base &&o) { return append(o); }
    
    // char
    string & append(const char * s, size_type n) { _base.append(s, n); return *this; }
    string & append(const char * s) { _base.append(s); return *this; }
    string & append(size_type n, char c) { _base.append(n, c); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & append(std::initializer_list<__base::value_type> __il) { _base.append(__il); return *this; }
#endif
    string & operator+=(const char * s) { return append(s); }
    string & operator+=(char c) { return append(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator+=(std::initializer_list<__base::value_type> __il) { return append(__il); }
#endif
    
    // xmlChar
    string & append(const xmlChar * s, size_type n) { _base.append(reinterpret_cast<const char *>(s), n); return *this; }
    string & append(const xmlChar * s) { _base.append(reinterpret_cast<const char *>(s), xmlStrlen(s)); return *this; }
    string & append(size_type n, xmlChar c) { _base.append(n, static_cast<char>(c)); return *this; }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & append(std::initializer_list<xmlChar> __il) { return append(__il.begin(), __il.end()); }
#endif
    string & operator+=(const xmlChar *s) { return append(s, xmlStrlen(s)); }
    string & operator+=(xmlChar c) { return append(1, c); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & operator+=(std::initializer_list<xmlChar> __il) { return append(__il); }
#endif
    
#if 0
#pragma mark - Insertion
#endif
    
    template <class InputIterator>
    iterator insert(iterator p, InputIterator first, InputIterator last);
    
    template <typename... Args>
    string & insert(size_type p, const Args&... args) { return insert(p, string(args...)); }
    template <typename... Args>
    iterator insert(iterator p, const Args&... args) { return insert(p, string(args...)); }
    
    // standard
    string & insert(size_type p, const string &s, size_type b=0, size_type e=npos);
    iterator insert(iterator p, const string & s, size_type b=0, size_type e=npos);
    
    // char32_t
    string & insert(size_type p, const_u4pointer s, size_type e=npos);
    string & insert(size_type p, size_type n, value_type c);
    iterator insert(iterator p, const_u4pointer s, size_type e=npos);
    iterator insert(iterator p, size_type n, value_type c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    iterator insert(iterator p, std::initializer_list<value_type> __il) { return insert(p, __il.begin(), __il.end()); }
#endif
    
    // char16_t
    string & insert(size_type p, const char16_t* s, size_type e=npos);
    string & insert(size_type p, size_type n, char16_t c);
    iterator insert(iterator p, const char16_t* s, size_type e=npos);
    iterator insert(iterator p, size_type n, char16_t c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    iterator insert(iterator p, std::initializer_list<char16_t> __il) { return insert(p, __il.begin(), __il.end()); }
#endif
    
    // std::string
    string & insert(size_type p, const __base &s, size_type b=0, size_type e=npos);
    string & insert(size_type p, __base::iterator b, __base::iterator e);
    iterator insert(iterator p, const __base & s, size_type b=0, size_type e=npos);
    
    // char
    string & insert(size_type p, const char * s, size_type b=0, size_type e=npos);
    string & insert(size_type p, size_type n, char c);
    iterator insert(iterator p, const char * s, size_type b=0, size_type e=npos);
    iterator insert(iterator p, size_type n, char c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    iterator insert(iterator p, std::initializer_list<char> __il) { return insert(p, __il.begin(), __il.end()); }
#endif
    
    // xmlChar
    string & insert(size_type p, const xmlChar * s, size_type b=0, size_type e=npos)
        { return insert(p, reinterpret_cast<const char*>(s), b, e); }
    string & insert(size_type p, size_type n, xmlChar c) { return insert(p, n, static_cast<char>(c)); }
    iterator insert(iterator p, const xmlChar * s, size_type b=0, size_type e=npos)
        { return insert(p, reinterpret_cast<const char*>(s), b, e); }
    iterator insert(iterator p, size_type n, xmlChar c) { return insert(p, n, static_cast<char>(c)); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    iterator insert(iterator p, std::initializer_list<xmlChar> __il) { return insert(p, __il.begin(), __il.end()); }
#endif
    
#if 0
#pragma mark - Erasing
#endif
    
    string & erase(size_type pos=0, size_type n=npos);
    iterator erase(cxx11_const_iterator pos);
    iterator erase(cxx11_const_iterator first, cxx11_const_iterator last);
    
#if 0
#pragma mark - Replacements
#endif
    
    template <class InputIterator>
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, InputIterator j1, InputIterator j2);
    
    template <typename... Args>
    string & replace(size_type pos1, size_type n1, const Args&... args) {
        string __s(args...);
        return replace(pos1, n1, __s);
    }
    template <typename... Args>
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, Args&... args) {
        string __s(args...);
        return replace(i1, i2, __s);
    }
    
    // standard
    string & replace(size_type pos1, size_type n1, const string & str);
    string & replace(size_type pos1, size_type n1, const string & str, size_type pos2, size_type n2);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const string& str);
    
    // char32_t
    string & replace(size_type pos, size_type n1, const_u4pointer s, size_type n2);
    string & replace(size_type pos, size_type n1, const_u4pointer s);
    string & replace(size_type pos, size_type n1, size_type n2, value_type c);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const_u4pointer s, size_type n);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const_u4pointer s);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, value_type c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::initializer_list<value_type> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
#endif
    
    // char16_t
    string & replace(size_type pos, size_type n1, const char16_t* s, size_type n2);
    string & replace(size_type pos, size_type n1, const char16_t* s);
    string & replace(size_type pos, size_type n1, size_type n2, char16_t c);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char16_t* s, size_type n);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char16_t* s);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, char16_t c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::initializer_list<char16_t> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
#endif
    
    // std::string
    string & replace(size_type pos1, size_type n1, const __base & str);
    string & replace(size_type pos1, size_type n1, const __base & str, size_type pos2, size_type n2);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const __base & str);
    
    // char
    string & replace(size_type pos, size_type n1, const char * s, size_type n2);
    string & replace(size_type pos, size_type n1, const char * s);
    string & replace(size_type pos, size_type n1, size_type n2, char c);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char * s, size_type n);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const char * s);
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, char c);
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::initializer_list<char> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
#endif
    
    // xmlChar
    string & replace(size_type pos, size_type n1, const xmlChar * s, size_type n2)
        { return replace(pos, n1, reinterpret_cast<const char*>(s), n2); }
    string & replace(size_type pos, size_type n1, const xmlChar * s)
        { return replace(pos, n1, reinterpret_cast<const char*>(s)); }
    string & replace(size_type pos, size_type n1, size_type n2, xmlChar c)
        { return replace(pos, n1, n2, static_cast<char>(c)); }
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const xmlChar * s, size_type n)
        { return replace(i1, i2, reinterpret_cast<const char*>(s), n); }
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, const xmlChar * s)
        { return replace(i1, i2, reinterpret_cast<const char*>(s)); }
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, size_type n, xmlChar c)
            { return replace(i1, i2, n, static_cast<char>(c)); }
#if EPUB_COMPILER_SUPPORTS(CXX_INITIALIZER_LISTS)
    string & replace(cxx11_const_iterator i1, cxx11_const_iterator i2, std::initializer_list<xmlChar> __il) {
        return replace(i1, i2, __il.begin(), __il.end());
    }
#endif
    
#if 0
#pragma mark - Outputs
#endif
    
    size_type copy(u4pointer s, size_type n, size_type pos=0) const;
    size_type copy(char16_t* s, size_type n, size_type pos=0) const;
    size_type copyC(char * s, size_type n, size_type pos=0) const { return _base.copy(s, n, pos); }
    size_type copyXML(xmlChar * s, size_type n, size_type pos=0) const { return _base.copy(reinterpret_cast<char*>(s), n, pos); }
    
    string substr(size_type pos=0, size_type n=npos) const;
    
    void swap(string & str)
#ifdef _LIBCPP_STRING
    _NOEXCEPT(!__base::__alloc_traits::propagate_on_container_swap::value || std::__is_nothrow_swappable<__base::__alloc_traits>::value)
#endif
    {
        _base.swap(str._base);
    }
    
    std::u32string utf32string() const;
    inline const_u4pointer utf32() const { return utf32string().c_str(); }
    
    std::u16string utf16string() const;
    inline const char16_t* utf16() const { return utf16string().c_str(); }
    
    __base::const_pointer c_str() const _NOEXCEPT { return _base.c_str(); }
    __base::const_pointer data() const _NOEXCEPT { return _base.data(); }
    
    const __base& stl_str() const { return _base; }
    
    const xmlChar * utf8() const { return reinterpret_cast<const xmlChar *>(c_str()); }
    const xmlChar * xml_str() const { return reinterpret_cast<const xmlChar*>(c_str()); }
    
    __base::allocator_type get_allocator() const _NOEXCEPT { return _base.get_allocator(); }
    
    string& tolower(const std::locale& loc = std::locale(""));
    const string tolower(const std::locale& loc = std::locale("")) const;
    
    string& toupper(const std::locale& loc = std::locale(""));
    const string toupper(const std::locale& loc = std::locale("")) const;
    
#if 0
#pragma mark - Searching
#endif
    
    size_type find(const string& str, size_type pos=0) const _NOEXCEPT {
        return to_utf32_size(_base.find(str._base, to_byte_size(pos)));
    }
    size_type find(const __base& str, size_type pos=0) const _NOEXCEPT {
        return to_utf32_size(_base.find(str, to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(const _CharT * s, size_type pos, size_type n) const _NOEXCEPT {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(s, 0, n), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(const _CharT * s, size_type pos = 0) const _NOEXCEPT {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(s), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type find(_CharT c, size_type pos = 0) const _NOEXCEPT {
        return to_utf32_size(_base.find(_Convert<_CharT>::toUTF8(c), to_byte_size(pos)));
    }
    
    size_type rfind(const string& str, size_type pos=npos) const _NOEXCEPT {
        return to_utf32_size(_base.rfind(str._base, to_byte_size(pos)));
    }
    size_type rfind(const __base& str, size_type pos=npos) const _NOEXCEPT {
        return to_utf32_size(_base.rfind(str, to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(const _CharT * s, size_type pos, size_type n) const _NOEXCEPT {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(s, 0, n), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(const _CharT * s, size_type pos = npos) const _NOEXCEPT {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(s), to_byte_size(pos)));
    }
    template <typename _CharT>
    size_type rfind(_CharT c, size_type pos = npos) const _NOEXCEPT {
        return to_utf32_size(_base.rfind(_Convert<_CharT>::toUTF8(c), to_byte_size(pos)));
    }
    
    template <class _ForwardIterator1, class _ForwardIterator2, class _BinaryPredicate>
    _ForwardIterator1
    find_first_of(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
                  _ForwardIterator2 __first2, _ForwardIterator2 __last2, _BinaryPredicate __pred) const _NOEXCEPT
    {
        for (; __first1 != __last1; ++__first1)
            for (_ForwardIterator2 __j = __first2; __j != __last2; ++__j)
                if (__pred(*__first1, *__j))
                    return __first1;
        return __last1;
    }
    
    template <class _Traits>
    struct _LIBCPP_HIDDEN __traits_eq
    {
        typedef typename _Traits::char_type char_type;
        _LIBCPP_INLINE_VISIBILITY
        bool operator()(const char_type& __x, const char_type& __y) _NOEXCEPT {
            return _Traits::eq(__x, __y);
        }
    };
    
    size_type find_first_of(const string& str, size_type pos=0) const _NOEXCEPT {
        auto __r = find_first_of(const_iterator(_base, pos), end(), str.begin(), str.end(), __traits_eq<traits_type>());
        if ( __r == end() )
            return npos;
        return utf8::distance(begin().base(), __r.base());
    }
    size_type find_first_of(const __base& str, size_type pos=0) const {
        validate_utf8(str.substr(pos));
        auto __r = find_first_of(const_iterator(_base, pos), end(), const_iterator(str.begin(), str.begin(), str.end()), const_iterator(str.end(), str.begin(), str.end()), __traits_eq<traits_type>());
        if ( __r == end() )
            return npos;
        return utf8::distance(begin().base(), __r.base());
    }
    template <typename _CharT>
    size_type find_first_of(const _CharT * s, size_type pos, size_type n) const {
        return find_first_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_first_of(const _CharT * s, size_type pos = 0) const {
        validate_utf8(s+pos, npos);
        return find_first_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_first_of(_CharT c, size_type pos = 0) const _NOEXCEPT {
        auto __r = std::find_first_of(begin()+pos, end(), &c, ((&c) + sizeof(_CharT)));
        if ( __r == end() )
            return npos;
        return utf8::distance(begin().base(), __r.base());
    }
    
    size_type find_last_of(const string& str, size_type pos=npos) const _NOEXCEPT {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
        {
            size_type __r = str.find(*--__ps);
            if ( __r != npos )
                return utf8::distance(begin().base(), __ps.base());
        }
        return npos;
    }
    size_type find_last_of(const __base& str, size_type pos=npos) const {
        validate_utf8(str.substr(pos));
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
        {
            size_type __r = str.find((--__ps).utf8char());
            if ( __r != npos )
                return utf8::distance(begin().base(), __ps.base());
        }
        return npos;
    }
    template <typename _CharT>
    size_type find_last_of(const _CharT * s, size_type pos, size_type n) const {
        return find_last_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_last_of(const _CharT * s, size_type pos = npos) const {
        return find_last_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_last_of(_CharT c, size_type pos = npos) const _NOEXCEPT {
        return rfind(c, pos);
    }
    
    size_type find_first_not_of(const string& str, size_type pos=0) const _NOEXCEPT {
        size_type __sz = size();
        if ( pos < __sz )
        {
            const_iterator __p = begin();
            const_iterator __pe = end();
            for ( const_iterator __ps = __p + pos; __ps != __pe; ++__ps )
                if ( str.find(*__ps) == npos )
                    return utf8::distance(__p.base(), __ps.base());
        }
        return npos;
    }
    size_type find_first_not_of(const __base& str, size_type pos=0) const {
        validate_utf8(str.substr(pos));
        size_type __sz = size();
        if ( pos < __sz )
        {
            const_iterator __p = begin();
            const_iterator __pe = end();
            for ( const_iterator __ps = __p + pos; __ps != __pe; ++__ps )
                if ( str.find(__ps.utf8char()) == npos )
                    return utf8::distance(__p.base(), __ps.base());
        }
        return npos;
    }
    template <typename _CharT>
    size_type find_first_not_of(const _CharT * s, size_type pos, size_type n) const {
        return find_first_not_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_first_not_of(const _CharT * s, size_type pos = 0) const {
        return find_first_not_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_first_not_of(_CharT c, size_type pos = 0) const _NOEXCEPT {
        return find_first_not_of(_Convert<_CharT>::toUTF8(c), pos);
    }
    
    size_type find_last_not_of(const string& str, size_type pos=npos) const _NOEXCEPT {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
            if ( str.find(*--__ps) == npos )
                return utf8::distance(__p.base(), __ps.base());
        return npos;
    }
    size_type find_last_not_of(const __base& str, size_type pos=npos) const {
        size_type __sz = size();
        if ( pos < __sz )
            ++pos;
        else
            pos = __sz;
        const_iterator __p = begin();
        for ( const_iterator __ps = __p + pos; __ps != __p; )
            if ( str.find((--__ps).utf8char()) == npos )
                return utf8::distance(__p.base(), __ps.base());
        return npos;
    }
    template <typename _CharT>
    size_type find_last_not_of(const _CharT * s, size_type pos, size_type n) const {
        return find_last_not_of(_Convert<_CharT>::toUTF8(s, 0, n), pos);
    }
    template <typename _CharT>
    size_type find_last_not_of(const _CharT * s, size_type pos = npos) const {
        return find_last_not_of(_Convert<_CharT>::toUTF8(s), pos);
    }
    template <typename _CharT>
    size_type find_last_not_of(_CharT c, size_type pos = npos) const _NOEXCEPT {
        return find_last_not_of(_Convert<_CharT>::toUTF8(c), pos);
    }
    
#if 0
#pragma mark - Comparisons
#endif
    
    int compare(const string& str) const _NOEXCEPT {
        return _base.compare(str._base);
    }
    int compare(size_type pos1, size_type n1, const string& str) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), str._base);
    }
    int compare(size_type pos1, size_type n1, const string& str,
                size_type pos2, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1),
                             str._base, str.to_byte_size(pos2), str.to_byte_size(pos2, pos2+n2));
    }
    
    // there exist specializations for char32_t
    template <typename _CharT>
    int compare(const _CharT * s) const _NOEXCEPT {
        auto str(_Convert<_CharT>::toUTF8(s));
        return _base.compare(str);
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const _CharT * s) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(s));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const _CharT * s, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(s, 0, n2));
    }
    template <typename _CharT>
    int compare(const std::basic_string<_CharT> & s) const _NOEXCEPT {
        return _base.compare(_Convert<_CharT>::toUTF8(s));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const std::basic_string<_CharT>& str) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(str));
    }
    template <typename _CharT>
    int compare(size_type pos1, size_type n1, const std::basic_string<_CharT>& str,
                size_type pos2, size_type n2) const {
        return _base.compare(to_byte_size(pos1), to_byte_size(pos1, pos1+n1), _Convert<_CharT>::toUTF8(str, pos2, n2));
    }
    
    bool operator == (const string & str) const _NOEXCEPT { return compare(str) == 0; }
    bool operator != (const string & str) const _NOEXCEPT { return compare(str) != 0; }
    bool operator > (const string & str) const _NOEXCEPT { return compare(str) > 0; }
    bool operator >= (const string & str) const _NOEXCEPT { return compare(str) >= 0; }
    bool operator < (const string & str) const _NOEXCEPT { return compare(str) < 0; }
    bool operator <= (const string & str) const _NOEXCEPT { return compare(str) <= 0; }
    
    template <typename _CharT>
    bool operator == (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) == 0; }
    template <typename _CharT>
    bool operator != (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) != 0; }
    template <typename _CharT>
    bool operator > (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) > 0; }
    template <typename _CharT>
    bool operator >= (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) >= 0; }
    template <typename _CharT>
    bool operator < (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) < 0; }
    template <typename _CharT>
    bool operator <= (const _CharT * str) const _NOEXCEPT { return compare<_CharT>(str) <= 0; }
    
#ifdef _LIBCPP_VERSION
    bool __invariants() const { return _base.__invariants(); }
#endif
    
protected:
    __base      _base;
    
    void validate_utf8(const __base &s) const;
    void validate_utf8(const char *s, size_type sz) const;
    void validate_utf8(const xmlChar *s, size_type sz) const;
    
    void throw_unless_insertable(const __base &s, size_type b, size_type e) const;
    void throw_unless_insertable(const char * s, size_type b, size_type e) const;
    void throw_unless_insertable(const xmlChar * s, size_type b, size_type e) const;
    
    __base::size_type to_byte_size(size_type __n) const _NOEXCEPT;
    __base::size_type to_byte_size(size_type __b, size_type __e) const _NOEXCEPT;
    size_type to_utf32_size(__base::size_type __n) const _NOEXCEPT;
    size_type to_utf32_size(__base::size_type __b, __base::size_type __e) const _NOEXCEPT;
    static size_type utf32_distance(__base::const_iterator first, __base::const_iterator last) _NOEXCEPT;
    
    static inline constexpr __base::const_pointer _bchar(const xmlChar * c) _NOEXCEPT { return (__base::const_pointer)(c); }
    static inline constexpr __base::pointer _bchar(xmlChar * c) _NOEXCEPT { return (__base::pointer)(c); }
    
#if UTF_USE_ICU
    // ICU version, since GNU libstdc++ hasn't implemented wstring_convert or codecvt_utf8 yet
    template <class _CharT>
    class _Convert {
    public:
        typedef std::basic_string<char>     byte_string;
        typedef std::basic_string<_CharT>   wide_string;
        
        static byte_string toUTF8(const _CharT* p, size_type pos=0, size_type n=npos) {
            UErrorCode uerr = U_ZERO_ERROR;
            UChar* __up = __to_UChar(p, &pos, &n);
            UConverter* cvt = ucnv_open("utf-8", &uerr);
            byte_string __out;
            __out.resize(2*n);
            __up = __up + pos;
            
            __out.resize(ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), static_cast<int32_t>(__out.size()), __up, static_cast<int32_t>(n), &uerr));
            if ( uerr == U_BUFFER_OVERFLOW_ERROR )
                ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), static_cast<int32_t>(__out.size()), __up, static_cast<int32_t>(n), &uerr);
            
            ucnv_close(cvt);
            if ( (void*)__up != (void*)p )
                delete [] __up;
            
            return __out;
        }
        static byte_string toUTF8(const wide_string& s, size_type pos=0, size_type n=npos) {
            return ( toUTF8(s.c_str(), pos, n) );
        }
        static byte_string toUTF8(_CharT c, size_type n=1)
        {
            if ( n == 1 )
                return toUTF8(&c, 0, 1);
            _CharT* __buf = new _CharT[n];
            for ( size_type i = 0; i < n; i++ )
                __buf[i] = c;
            
            UErrorCode uerr = U_ZERO_ERROR;
            UChar* __up = __to_UChar(__buf, NULL, &n);
            
            UConverter* cvt = ucnv_open("utf-8", &uerr);
            byte_string __out;
            __out.resize(2*n);
            
            __out.resize(ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), static_cast<int32_t>(__out.size()), __up, static_cast<int32_t>(n), &uerr));
            if ( uerr == U_BUFFER_OVERFLOW_ERROR )
                ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), static_cast<int32_t>(__out.size()), __up, static_cast<int32_t>(n), &uerr);
            
            ucnv_close(cvt);
            if ( (void*)__up != (void*)__buf )
                delete [] __up;
            delete [] __buf;
            
            return __out;
        }
        
        static wide_string fromUTF8(const char * utf8, size_type pos=0, size_type n=npos) {
            if ( n == npos )
                n = std::char_traits<char>::length(utf8) - pos;
            UErrorCode uerr = U_ZERO_ERROR;
            UChar* __up = __to_UChar(utf8, &pos, &n);
            
            if ( sizeof(_CharT) == sizeof(UChar) )
            {
                wide_string __r(__up + pos, n);
                delete [] __up;
                return __r;
            }
            
            const char* name = "utf-16";
            switch ( sizeof(_CharT) )
            {
                case 4:
                    name = "utf-32";
                    break;
                default:
                    break;
            }
            
            UConverter* cvt = ucnv_open(name, &uerr);
            wide_string __out;
            __out.resize(n);
            
            __out.resize(ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), n, __up, n, &uerr));
            if ( uerr == U_BUFFER_OVERFLOW_ERROR )
                ucnv_fromUChars(cvt, const_cast<char*>(__out.data()), n, __up, n, &uerr);
            
            ucnv_close(cvt);
            delete [] __up;
            
            return __out;
        }
        static wide_string fromUTF8(const byte_string & utf8, size_type pos=0, size_type n=npos) {
            return fromUTF8(utf8.data(), pos, n);
        }
        
    private:
        static UChar* __to_UChar(const _CharT* p, size_type* pPos, size_type* pN) {
            const char* name = "utf-8";
            switch ( sizeof(_CharT) )
            {
                case 2:
                    name = "utf-16";
                    break;
                case 4:
                    return const_cast<UChar*>(reinterpret_cast<const UChar*>(p));
                    break;
                default:
                    break;
            }
            
            UErrorCode uerr = U_ZERO_ERROR;
            UConverter* cvt = ucnv_open(name, &uerr);
            
            int32_t len = ucnv_toUChars(cvt, NULL, 0, (const char*)p, *pN, &uerr);
            UChar* __out = new UChar[len+1];
            UChar* __p = __out;
            UChar* __e = __out + len;
            const char* __f = (const char*)p + *pPos;
            const char* __fe = (const char*)(p + *pPos + *pN);
            int32_t* offsets = new int32_t[*pN];
            
            ucnv_toUnicode(cvt, &__p, __e, &__f, __fe, offsets, TRUE, &uerr);
            *pN = offsets[*pN];
            *pPos = offsets[*pPos];
            
            return __out;
        }
    };
#else
    // non-ICU implementation for smaller Android builds
    template <class _CharT>
    class _Convert {
    public:
        typedef std::string byte_string;
        typedef std::basic_string<_CharT> wide_string;
        
        static byte_string toUTF8(const _CharT* p, size_type pos=0, size_type n=npos);
        static byte_string toUTF8(const wide_string& s, size_type pos=0, size_type n=npos);
        static byte_string toUTF8(_CharT c, size_type n=1);
        
        static wide_string fromUTF8(const char* utf8, size_type pos=0, size_type n=npos);
        static wide_string fromUTF8(const byte_string& s, size_type pos=0, size_type n=npos);
    };
#if 0

    // Pure C++11 implementation, works on libc++ and VC++2010
    template <class _CharT>
    class _Convert {
        typedef std::wstring_convert<std::codecvt_utf8<_CharT>, _CharT > _cvt;
    public:
        typedef typename _cvt::byte_string byte_string;
        typedef typename _cvt::wide_string wide_string;
        
        static byte_string toUTF8(const _CharT * p, size_type pos=0, size_type n=npos) {
            if ( n == npos )
                return _cvt().to_bytes(p+pos);
            return _cvt().to_bytes(p+pos, p+pos+n);
        }
        static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
            return _cvt().to_bytes(s.substr(pos,n));
        }
        static byte_string toUTF8(_CharT c, size_type n=1) {
            byte_string s = _cvt().to_bytes(c);
            if ( n == 1 )
                return s;
            else if ( s.length() == 1 )
                return byte_string(n, s[0]);
            byte_string r;
            r.reserve(n*s.size());
            for ( size_type i = 0; i < n; i++ )
                r.append(s);
            return r;
        }
        static wide_string fromUTF8(const char * utf8, size_type pos=0, size_type n=npos) {
            if ( n == npos )
                return _cvt().from_bytes(utf8+pos);
            return _cvt().from_bytes(utf8+pos, utf8+pos+n);
        }
        static wide_string fromUTF8(const byte_string & utf8, size_type pos=0, size_type n=npos) {
            return _cvt().from_bytes(utf8.substr(pos, n));
        }
    };
#endif
#endif
};

#if 0
#pragma mark - Template Specializations
#endif

template<>
class string::_Convert<char> {
public:
    typedef std::string byte_string;
    typedef std::string wide_string;
    static byte_string toUTF8(const char *p, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(p+pos);
        return std::string(p+pos, n);
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
    static byte_string toUTF8(char c, size_type n=1) {
        return std::string(n, c);
    }
    static wide_string fromUTF8(const char * utf8, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(utf8+pos);
        return std::string(utf8+pos, n);
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
};

template <>
class string::_Convert<xmlChar> {
public:
    typedef std::string byte_string;
    typedef std::string wide_string;
    static byte_string toUTF8(const xmlChar *p, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(reinterpret_cast<const char*>(p)+pos);
        return std::string(reinterpret_cast<const char*>(p)+pos, n);
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
    static byte_string toUTF8(xmlChar c, size_type n=1) {
        return std::string(n, static_cast<char>(c));
    }
    static wide_string fromUTF8(const xmlChar * utf8, size_type pos=0, size_type n=npos) {
        if ( n == std::string::npos )
            return std::string(reinterpret_cast<const char*>(utf8)+pos);
        return std::string(reinterpret_cast<const char*>(utf8)+pos, n);
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        return s.substr(pos, n);
    }
};

#if (!defined(UTF_USE_ICU) || UTF_USE_ICU == 0)
// ePub::string::_Convert is implemented for Unicode via template specializations here
template <>
class string::_Convert<char16_t> {
public:
    typedef std::string                 byte_string;
    typedef std::basic_string<char16_t> wide_string;
    
    static byte_string toUTF8(const char16_t *p, size_type pos=0, size_type n=npos) {
        byte_string __r;
        size_type len = (n == npos ? std::char_traits<char16_t>::length(p) : n);
        utf8::utf16to8(p+pos, p+len, std::back_inserter(__r));
        return __r;
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        byte_string __r;
        utf8::utf16to8(s.begin() + pos, (n == npos ? s.end() : s.begin() + n), std::back_inserter(__r));
        return __r;
    }
    static byte_string toUTF8(char16_t c, size_type n=1) {
        byte_string __t;
        utf8::utf16to8(&c, (&c)+1, std::back_inserter(__t));
        byte_string __r;
        for (int __i = 0; __i < n; __i++) {
            __r.append(__t);
        }
        return __r;
    }
    static wide_string fromUTF8(const char* p, size_type pos=0, size_type n=npos) {
        wide_string __r;
        size_type len = (n == npos ? std::char_traits<char>::length(p) : n);
        utf8::utf8to16(p+pos, p+len, std::back_inserter(__r));
        return __r;
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        wide_string __r;
        utf8::utf8to16(s.begin() + pos, (n == npos ? s.end() : s.begin() + n), std::back_inserter(__r));
        return __r;
    }
};

template <>
class string::_Convert<char32_t> {
public:
    typedef std::string                 byte_string;
    typedef std::basic_string<char32_t> wide_string;
    
    static byte_string toUTF8(const char32_t *p, size_type pos=0, size_type n=npos) {
        byte_string __r;
        size_type len = (n == npos ? std::char_traits<char32_t>::length(p) : n);
        utf8::utf32to8(p+pos, p+len, std::back_inserter(__r));
        return __r;
    }
    static byte_string toUTF8(const wide_string & s, size_type pos=0, size_type n=npos) {
        byte_string __r;
        utf8::utf32to8(s.begin() + pos, (n == npos ? s.end() : s.begin() + n), std::back_inserter(__r));
        return __r;
    }
    static byte_string toUTF8(char32_t c, size_type n=1) {
        byte_string __t;
        utf8::utf32to8(&c, (&c)+1, std::back_inserter(__t));
        byte_string __r;
        for (int __i = 0; __i < n; __i++) {
            __r.append(__t);
        }
        return __r;
    }
    static wide_string fromUTF8(const char* p, size_type pos=0, size_type n=npos) {
        wide_string __r;
        size_type len = (n == npos ? std::char_traits<char>::length(p) : n);
        utf8::utf8to32(p+pos, p+len, std::back_inserter(__r));
        return __r;
    }
    static wide_string fromUTF8(const byte_string & s, size_type pos=0, size_type n=npos) {
        wide_string __r;
        utf8::utf8to32(s.begin() + pos, (n == npos ? s.end() : s.begin() + n), std::back_inserter(__r));
        return __r;
    }
};
#endif

#if 0
#pragma mark - Helpers
#endif

// C++11 lets us define new literal types, so lets have "something"_xc be an xmlChar *, eh?
// Sadly, we can't define prefix forms. Boo...
constexpr inline const xmlChar * operator "" _xc(const char * __s, size_t __n) _NOEXCEPT {
    return (const xmlChar *)__s;
}
static inline constexpr const xmlChar * _xml(const char * __s) _NOEXCEPT {
    return (const xmlChar*)(__s);
}

static inline const xmlChar * xmlChars(const std::string & str) {
    return reinterpret_cast<const xmlChar*>(str.c_str());
}
static inline string xmlString(const char * str) {
    return string(reinterpret_cast<const xmlChar*>(str));
}
static inline string xmlString(const std::string & str) {
    return string(reinterpret_cast<const xmlChar*>(str.c_str()));
}
static inline std::string asciiString(const xmlChar * s) {
    return std::string(reinterpret_cast<const char *>(s));
}
static inline constexpr const char * ascii(const xmlChar * __x) {
    return (const char *)__x;
}

////////////////////////////////////////////////////////////////////////////
// some helpers

inline string operator + (const string & lhs, const string & rhs) {
    string s(lhs);
    s.append(rhs);
    return s;
}
inline string operator + (const string & lhs, const std::string & rhs) {
    string s(lhs);
    s.append(rhs);
    return s;
}
inline string operator * (const string & lhs, const string::value_type * rhs) {
    string s(lhs);
    s.append(rhs);
    return s;
}
inline string operator * (const string & lhs, string::value_type rhs) {
    string s(lhs);
    s.append(1, rhs);
    return s;
}
inline string operator + (const string & lhs, const char * rhs) {
    string s(lhs);
    s.append(rhs);
    return s;
}
inline string operator + (const string & lhs, char rhs) {
    string s(lhs);
    s.append(1, rhs);
    return s;
}
inline string operator + (const string & lhs, const xmlChar * rhs) {
    string s(lhs);
    s.append(rhs);
    return s;
}
inline string operator + (const string & lhs, xmlChar rhs) {
    string s(lhs);
    s.append(1, rhs);
    return s;
}

// to std::string (UTF-8, signed char)
inline std::string operator + (std::string& lhs, const xmlChar rhs) {
    return std::operator+(lhs, static_cast<const char>(rhs));
}
inline std::string operator + (std::string& lhs, const xmlChar *rhs) {
    return std::operator+(lhs, reinterpret_cast<const char*>(rhs));
}
inline std::string operator + (std::string &lhs, const string &rhs) {
    return std::operator+(lhs, reinterpret_cast<const char*>(rhs.c_str()));
}

template <class _CharT, class _Traits>
inline std::basic_ostream<_CharT, _Traits>&
operator<<(std::basic_ostream<_CharT, _Traits>& __os, const string& __str) {
    return __os << __str.stl_str();
}

EPUB3_END_NAMESPACE

#endif /* defined(__ePub3_xml_string__) */
