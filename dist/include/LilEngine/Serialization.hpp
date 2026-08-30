#pragma once


/*
Usage:



class Base {
public:
    int a;

    LIL_SERIALIZABLE()
};
LIL_SERIALIZE_NO_BASE(Base, a)

class Child : public Base {
public:
    int b;

    template <class Archive>
    void save( Archive & ar ) const {
        LIL_SAVE_BASE(Base)
        ar(b);
    }
        
    template <class Archive>
    void load( Archive & ar ) {
        LIL_LOAD_BASE(Base)
        ar(b);
    }
};
LIL_DISAMBIGUATE_LOAD_SAVE(Child)

class Grandkid : public Child {
public:
    int c;

    LIL_SERIALIZABLE()
};
LIL_SER_BEGIN(Grandkid)
LIL_SER_BASE(Child)
LIL_SER_FIELD(c)
LIL_SER_END()

int main() {
    {
    std::ofstream os("out.json");
    ArchiveOut a_out(os);

    Grandkid g;
    g.a = 3;
    a_out(g);
    }
    

    {
    std::ifstream is("out.json");
    ArchiveIn a_in(is);

    Grandkid g;
    a_in(g);
    std::cout << g.a << std::endl;
    }
}



*/



#include "CommonIncludes.hpp"
#include "stduuid/uuid.h"

#include "cereal/specialize.hpp"
#include "cereal/access.hpp"
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

#define LIL_SER_FIELD_OBJECT(object_name, field_name) \
ar(cereal::make_nvp(#field_name, object_name.field_name));

namespace cereal {

    template <class Archive>
    void save(Archive& ar, const uuids::uuid& id) {
        std::string uuid_str = uuids::to_string(id);
        ar(uuid_str);
    }
    
    template <class Archive>
    void load(Archive& ar, uuids::uuid& id) {
        std::string uuid_str;
        ar(uuid_str);
        auto result = uuids::uuid::from_string(uuid_str);
        if (result.has_value()) {
            id = result.value();
        } else {
            throw std::runtime_error("Invalid UUID: " + uuid_str);
        }
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector2& v) {
        LIL_SER_FIELD_OBJECT(v, x);
        LIL_SER_FIELD_OBJECT(v, y);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector3& v) {
        LIL_SER_FIELD_OBJECT(v, x);
        LIL_SER_FIELD_OBJECT(v, y);
        LIL_SER_FIELD_OBJECT(v, z);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector4& v) {
        LIL_SER_FIELD_OBJECT(v, x);
        LIL_SER_FIELD_OBJECT(v, y);
        LIL_SER_FIELD_OBJECT(v, z);
        LIL_SER_FIELD_OBJECT(v, w);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Color& c) {
        LIL_SER_FIELD_OBJECT(c, r);
        LIL_SER_FIELD_OBJECT(c, g);
        LIL_SER_FIELD_OBJECT(c, b);
        LIL_SER_FIELD_OBJECT(c, a);
    }


    template <class Archive>
    void serialize(Archive& ar, ::Transform& t) {
        LIL_SER_FIELD_OBJECT(t, translation);
        LIL_SER_FIELD_OBJECT(t, rotation);
        LIL_SER_FIELD_OBJECT(t, scale);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Matrix& m) {
        LIL_SER_FIELD_OBJECT(m, m0);
        LIL_SER_FIELD_OBJECT(m, m1);
        LIL_SER_FIELD_OBJECT(m, m2);
        LIL_SER_FIELD_OBJECT(m, m3);
        LIL_SER_FIELD_OBJECT(m, m4);
        LIL_SER_FIELD_OBJECT(m, m5);
        LIL_SER_FIELD_OBJECT(m, m6);
        LIL_SER_FIELD_OBJECT(m, m7);
        LIL_SER_FIELD_OBJECT(m, m8);
        LIL_SER_FIELD_OBJECT(m, m9);
        LIL_SER_FIELD_OBJECT(m, m10);
        LIL_SER_FIELD_OBJECT(m, m11);
        LIL_SER_FIELD_OBJECT(m, m12);
        LIL_SER_FIELD_OBJECT(m, m13);
        LIL_SER_FIELD_OBJECT(m, m14);
        LIL_SER_FIELD_OBJECT(m, m15);
    }
}

using ArchiveOut = cereal::JSONOutputArchive;
using ArchiveIn = cereal::JSONInputArchive;

CEREAL_REGISTER_ARCHIVE(ArchiveOut)
CEREAL_REGISTER_ARCHIVE(ArchiveIn)

#define LIL_DECLARE_SAVE() \
template <class Archive> \
void save(Archive& ar) const;

#define LIL_DECLARE_LOAD() \
template <class Archive> \
void load(Archive& ar);

#define LIL_SERIALIZABLE() \
friend class cereal::access;\
template <class Archive> \
void serialize(Archive& ar);

#define LIL_DISAMBIGUATE_SERIALIZE(typename) \
template <> \
struct cereal::specialize<ArchiveOut, typename, cereal::specialization::member_serialize> {}; \
template <> \
struct cereal::specialize<ArchiveIn, typename, cereal::specialization::member_serialize> {}; \

#define LIL_DISAMBIGUATE_LOAD_SAVE(typename) \
template <> \
struct cereal::specialize<ArchiveOut, typename, cereal::specialization::member_load_save> {}; \
template <> \
struct cereal::specialize<ArchiveIn, typename, cereal::specialization::member_load_save> {}; \


#define LIL_SERIALIZE_NO_BASE(typename, ...) \
LIL_DISAMBIGUATE_SERIALIZE(typename) \
template <class Archive> \
void typename::serialize( Archive & ar ) { ar(__VA_ARGS__); }

#define LIL_SAVE_BASE(base_typename) ar(cereal::base_class<base_typename>(this));
#define LIL_LOAD_BASE(base_typename) ar(cereal::base_class<base_typename>(this));

#define LIL_SER_REGISTER_POLYMORPHIC(typename) \
CEREAL_REGISTER_TYPE(typename)

#define LIL_SER_BEGIN(typename) \
LIL_SER_REGISTER_POLYMORPHIC(typename) \
LIL_DISAMBIGUATE_SERIALIZE(typename) \
template <class Archive> \
void typename::serialize( Archive & ar )  {

#define LIL_SER_BASE(base_typename) \
ar(cereal::base_class<base_typename>(this));

//LIL_REGISTER_POLYMORPHIC(base_typename, std::remove_pointer<decltype(this)>::type);

#define LIL_SER_FIELD(name) ar(cereal::make_nvp(#name, name));
#define LIL_SER_END() }