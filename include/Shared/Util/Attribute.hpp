#ifndef SHARED_UTIL_ATTRIBUTE_HPP
#define SHARED_UTIL_ATTRIBUTE_HPP

#include <string>
#include <stdexcept>

namespace Util
{
    class IAttribute
    {
    public:

        virtual std::string getAttributeName() const = 0;
        virtual std::string getValueStr() const = 0;

    };

    /*
        A more pain to use but perhaps easier to maintain way to replace constants, defines and enums.
        Due to the inherent performance flaws its meant to be used for resource creation and initialization.
    */
    template<typename base_type> class Attribute : public IAttribute
    {
    protected:

        base_type attrib_value;

    public:

        Attribute() :
            attrib_value()
        {
        }

        virtual Attribute& operator =(const Attribute& rh)
        {
            this->attrib_value = rh.attrib_value;
            return *this;
        }

        virtual Attribute& operator =(const base_type& rh)
        {
            this->attrib_value = rh;
            return *this;
        }

        virtual base_type& getValue()
        {
            return this->attrib_value;
        }

        virtual const base_type& getValue() const
        {
            return this->attrib_value;
        }

        virtual operator base_type& ()
        {
            return this->getValue();
        }

        virtual operator const base_type& () const
        {
            return this->getValue();
        }
        
        virtual std::string getAttributeName() const override
        {
            return "UnknownAttribute";
        }

        virtual std::string getValueStr() const override
        {
            throw std::runtime_error("Non overridden or specialized call");
        }
    };

    template<> std::string Attribute<bool>::getValueStr() const { return attrib_value ? "true" : "false"; }
    template<> std::string Attribute<char>::getValueStr() const { return std::string(1, attrib_value); }
    template<> std::string Attribute<unsigned char>::getValueStr() const { return std::string(1, attrib_value); }
    template<> std::string Attribute<int16_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<int32_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<int64_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<uint16_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<uint32_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<uint64_t>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<float>::getValueStr() const { return std::to_string(attrib_value); }
    template<> std::string Attribute<double>::getValueStr() const { return std::to_string(attrib_value); }

    template<typename AttribType, typename IterableStorageType> AttribType* findAttribute(IterableStorageType attrib_list)
    {
        AttribType* ret = nullptr;
        for (size_t i = 0; attrib_list[i] != nullptr; ++i)
        {
            ret = dynamic_cast<AttribType>(attrib_list[i]);
            if (ret != nullptr)
            {
                return ret;
            }
        }
    }

    template<typename AttribType, typename IterableStorageType> AttribType* findAttribute(IterableStorageType attrib_list, size_t count)
    {
        AttribType* ret = nullptr;
        for (size_t i = 0; i != count; ++i)
        {
            ret = dynamic_cast<AttribType>(attrib_list[i]);
            if (ret != nullptr)
            {
                return ret;
            }
        }
    }
}

#endif /* SHARED_UTIL_ATTRIBUTE_HPP */