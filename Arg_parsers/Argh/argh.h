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
    

};
    
} // namespace argh

