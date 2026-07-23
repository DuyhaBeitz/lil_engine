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
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>

namespace cereal {

    template <class Archive>
    void save(Archive& archive, const uuids::uuid& id) {
        std::string uuid_str = uuids::to_string(id);
        archive(uuid_str);
    }
    
    template <class Archive>
    void load(Archive& archive, uuids::uuid& id) {
        std::string uuid_str;
        archive(uuid_str);
        auto result = uuids::uuid::from_string(uuid_str);
        if (result.has_value()) {
            id = result.value();
        } else {
            throw std::runtime_error("Invalid UUID: " + uuid_str);
        }
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector2& v) {
        ar(v.x, v.y);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector3& v) {
        ar(v.x, v.y, v.z);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Vector4& v) {
        ar(v.x, v.y, v.z, v.w);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Transform& t) {
        ar(t.translation, t.rotation, t.scale);
    }

    template <class Archive>
    void serialize(Archive& ar, ::Matrix& m) {
        ar(
            m.m0,
            m.m1,
            m.m2,
            m.m3,
            m.m4,
            m.m5,
            m.m6,
            m.m7,
            m.m8,
            m.m9,
            m.m10,
            m.m11,
            m.m12,
            m.m13,
            m.m14,
            m.m15
        );
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

#define LIL_SER_BEGIN(typename) \
CEREAL_REGISTER_TYPE(typename) \
LIL_DISAMBIGUATE_SERIALIZE(typename) \
template <class Archive> \
void typename::serialize( Archive & ar )  {

#define LIL_SER_BASE(base_typename) \
ar(cereal::base_class<base_typename>(this));

//LIL_REGISTER_POLYMORPHIC(base_typename, std::remove_pointer<decltype(this)>::type);

#define LIL_SER_FIELD(name) ar(name);
#define LIL_SER_END() }