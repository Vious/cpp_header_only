/* reference to https://github.com/adishavit/argh,
thanks for their contributions */
#pragma once

#include <string>
#include <vector>
#include <cassert>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <initializer_list>

namespace argh
{

#if !defined(__GNUC__) || (__GNUC__ >= 5)
    using StringStream = std::istringstream;
#else
    // Until GCC 5, istringstream did not have a move constructor.
    // stringstream_proxy is used instead, as a workaround.
    class StringStreamProxy {
    public:
        StringStreamProxy() = default;

        ~StringStreamProxy() = default;
        
        // construct with given value
        StringStreamProxy(std::string const& value) : m_stream(value) 
        {}

        // Copy constructor
        StringStreamProxy(const StringStreamProxy &other)
          :m_stream(other.m_stream.str()) {
            m_stream.setstate(other.m_stream.rdstate());
        }

        void setstat(std::ios_base::iostate state) {
            m_stream.setstate(state);
        }
        
        // Stream out the value of the parameter.
        // If the conversion was not possible, the stream will enter the fail state,
        // and operator bool will return false.
        template<typename T>
        StringStreamProxy& operator >> (T& thing) {
            m_stream >> thing;
            return *this;
        }


        // get string value
        std::string str() const {
            return m_stream.str();
        }

        std::stringbuf* rdbuf() const {
            return m_stream.rdbuf();
        }

        explicit operator bool() const {
            return !!m_stream;
        }

    private:
        // class member var, named m_xxx;
        std::istringstream m_stream;
    };
    
    using StringStream = StringStreamProxy;
#endif

class MultiMapIterationWrapper {
public:
    using container_t = std::multimap<std::string, std::string>;

    using iterator_t = container_t::const_iterator;
    using difference_t = container_t::difference_type;
    
    explicit MultiMapIterationWrapper(const iterator_t& lb, const iterator_t& ub) 
        : m_lb(lb), m_ub(ub) {}

    iterator_t begin() const {
        return m_lb;
    }

    iterator_t end() const {
        return m_ub;
    }

    difference_t size() const {
        return std::distance(m_lb, m_ub);
    }

private:
    iterator_t m_lb;
    iterator_t m_ub;
};

class Parser {
public:
    enum Mode {
        PREFER_FLAG_FOR_UNREG_OPTION = 0x01,
        PREFER_PARAM_FOR_UNREG_OPTION = 0x10,
        NO_SPLIT_ON_EQUALSIGN = 0x100,
        SINGLE_DASH_IS_MULTIFLAG = 0x1000,
    };

    Parser() = default;

    Parser(std::initializer_list<char const* const> pre_reg_names) {
        addParams(pre_reg_names);
    }

    Parser(const char* const argv[], int mode = PREFER_FLAG_FOR_UNREG_OPTION) {
        doParse(argv, mode);
    }

    Parser(int argc, const char* const argv[], int mode = PREFER_FLAG_FOR_UNREG_OPTION) {
        doParse(argc, argv, mode);
    }


    void addParam(std::string const& name);
    void addParam(std::initializer_list<char const* const> init_list);

    void addParams(std::string const& name);
    void addParams(std::initializer_list<char const* const> init_list);

    void doParse(const char* const argv[], int mode = PREFER_FLAG_FOR_UNREG_OPTION);
    void doParse(int argc, const char* const argv[], int mode = PREFER_FLAG_FOR_UNREG_OPTION);

    std::multiset<std::string> const& getFlags() const {
        return m_flags;
    }

    std::multimap<std::string, std::string> const getParams() const {
        return m_params;
    }

    MultiMapIterationWrapper getParams(std::string const& name) const;

    std::vector<std::string> const& getPosArgs() const {
        return m_posArgs;
    }

    // begin() and end() for using range-for over positional args.
    std::vector<std::string>::const_iterator begin() const {
        return m_posArgs.cbegin();
    }

    std::vector<std::string>::const_iterator end() const {
        return m_posArgs.cend();
    }

    size_t size() const {
        return m_posArgs.size();
    }

    //////////////////////////////////////////////////////////////////////////
    // Accessors

    // flag (boolean) accessors: return true if the flag appeared, otherwise false.
    bool operator[](std::string const& name) const;

    // multiple flag (boolean) accessors: return true if at least one of the flag appeared, otherwise false.
    bool operator[](std::initializer_list<char const* const> init_list) const;

    // returns positional arg string by order. Like argv[] but without the options
    std::string const& operator[](size_t idx) const;

    // returns a std::istream that can be used to convert a positional arg to a typed value.
    StringStream operator()(size_t idx) const;

    // same as above, but with a default value in case the arg is missing (index out of range).
    template<typename T>
    StringStream operator()(size_t idx) const;

    // parameter accessors, give a name get an std::istream that can be used to convert to a typed value.
    // call .str() on result to get as string
    StringStream operator()(std::string const& name) const;

    // accessor for a parameter with multiple names, give a list of names, get an std::istream that can be used to convert to a typed value.
    // call .str() on result to get as string
    // returns the first value in the list to be found.
    StringStream operator()(std::initializer_list<char const* const> init_list) const;

    // same as above, but with a default value in case the param was missing.
    // Non-string def_val types must have an operator<<() (output stream operator)
    // If T only has an input stream operator, pass the string version of the type as in "3" instead of 3.
    template<typename T>
    StringStream operator()(std::string const& name, T&& def_val) const;


    // same as above but for a list of names. returns the first value to be found.
    template<typename T>
    StringStream operator()(std::initializer_list<char const* const> init_list, T&& def_val) const;


private:
    StringStream badStream() const;
    std::string trimLeadingDashes(std::string const& name) const;
    bool isNumber(std::string const& arg) const;
    bool isOption(std::string const& arg) const;
    bool gotFlag(std::string const& name) const;
    bool isParam(std::string const& name) const;


private:
    std::multiset<std::string> m_flags;
    std::multimap<std::string, std::string> m_params;
    std::vector<std::string> m_posArgs;
    std::vector<std::string> m_Args;
    std::set<std::string> m_registeredParams;
    std::string m_empty;

};






} // namespace argh

