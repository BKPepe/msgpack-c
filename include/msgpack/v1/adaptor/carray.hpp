//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2016 KONDO Takatoshi
//
//    Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//    http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef MSGPACK_V1_TYPE_CARRAY_HPP
#define MSGPACK_V1_TYPE_CARRAY_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/object_fwd.hpp"
#include "msgpack/adaptor/adaptor_base.hpp"
#include "msgpack/adaptor/check_container_size.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template <typename T, std::size_t N>
struct convert<T[N]> {
    msgpack::object const& operator()(msgpack::object const& o, T* v) const {
        if (o.type != msgpack::type::ARRAY) { throw msgpack::type_error(); }
        if (o.via.array.size > N) { throw msgpack::type_error(); }
        msgpack::object* p = o.via.array.ptr;
        msgpack::object* const pend = o.via.array.ptr + o.via.array.size;
        do {
            p->convert(*v);
            ++p;
            ++v;
        } while(p < pend);
        return o;
    }
};

template <std::size_t N>
struct convert<char[N]> {
    msgpack::object const& operator()(msgpack::object const& o, char(&v)[N]) const {
        switch (o.type) {
        case msgpack::type::BIN:
            if (o.via.bin.size > N) { throw msgpack::type_error(); }
            std::memcpy(v, o.via.bin.ptr, o.via.bin.size);
            break;
        case msgpack::type::STR:
            if (o.via.str.size + 1 > N) { throw msgpack::type_error(); }
            std::memcpy(v, o.via.str.ptr, o.via.str.size);
            v[o.via.str.size] = '\0';
            break;
        default:
            throw msgpack::type_error();
            break;
        }
        return o;
    }
};

template <std::size_t N>
struct convert<unsigned char[N]> {
    msgpack::object const& operator()(msgpack::object const& o, unsigned char(&v)[N]) const {
        switch (o.type) {
        case msgpack::type::BIN:
            if (o.via.bin.size > N) { throw msgpack::type_error(); }
            std::memcpy(v, o.via.bin.ptr, o.via.bin.size);
            break;
        case msgpack::type::STR:
            if (o.via.str.size + 1 > N) { throw msgpack::type_error(); }
            std::memcpy(v, o.via.str.ptr, o.via.str.size);
            v[o.via.str.size] = '\0';
            break;
        default:
            throw msgpack::type_error();
            break;
        }
        return o;
    }
};


template <typename T, std::size_t N>
struct pack<T[N]> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const T(&v)[N]) const {
        o.pack_array(N);
        const T* ptr = v;
        for (; ptr != &v[N]; ++ptr) o.pack(*ptr);
        return o;
    }
};

template <std::size_t N>
struct pack<char[N]> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const char(&v)[N]) const {
        char const* p = v;
        uint32_t size = checked_get_container_size(std::strlen(p));
        o.pack_str(size);
        o.pack_str_body(p, size);
        return o;
    }
};

template <std::size_t N>
struct pack<const char[N]> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const char(&v)[N]) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.pack_str(size);
        o.pack_str_body(v, size);
        return o;
    }
};

template <std::size_t N>
struct pack<unsigned char[N]> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const unsigned char(&v)[N]) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.pack_bin(size);
        o.pack_bin_body(reinterpret_cast<char const*>(p), size);
        return o;
    }
};

template <std::size_t N>
struct pack<const unsigned char[N]> {
    template <typename Stream>
    msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, const unsigned char(&v)[N]) const {
        unsigned char const* p = v;
        uint32_t size = checked_get_container_size(N);
        o.pack_bin(size);
        o.pack_bin_body(reinterpret_cast<char const*>(p), size);
        return o;
    }
};

template <typename T, std::size_t N>
struct object_with_zone<T[N]> {
    void operator()(msgpack::object::with_zone& o, const T(&v)[N]) const {
        o.type = msgpack::type::ARRAY;
        msgpack::object* ptr = static_cast<msgpack::object*>(o.zone.allocate_align(sizeof(msgpack::object) * N));
        o.via.array.ptr = ptr;
        o.via.array.size = N;
        const T* pv = v;
        for (; pv != &v[N]; ++pv) {
            *ptr++ = msgpack::object(*pv, o.zone);
        }
    }
};

template <std::size_t N>
struct object_with_zone<char[N]> {
    void operator()(msgpack::object::with_zone& o, const char(&v)[N]) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msgpack::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size));
        o.via.str.ptr = ptr;
        o.via.str.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object_with_zone<const char[N]> {
    void operator()(msgpack::object::with_zone& o, const char(&v)[N]) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msgpack::type::STR;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size));
        o.via.str.ptr = ptr;
        o.via.str.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object_with_zone<unsigned char[N]> {
    void operator()(msgpack::object::with_zone& o, const unsigned char(&v)[N]) const {
        uint32_t size = checked_get_container_size(N);
        o.type = msgpack::type::BIN;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size));
        o.via.bin.ptr = ptr;
        o.via.bin.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object_with_zone<const unsigned char[N]> {
    void operator()(msgpack::object::with_zone& o, const unsigned char(&v)[N]) const {
        uint32_t size = checked_get_container_size(N);
        o.type = msgpack::type::BIN;
        char* ptr = static_cast<char*>(o.zone.allocate_align(size));
        o.via.bin.ptr = ptr;
        o.via.bin.size = size;
        std::memcpy(ptr, v, size);
    }
};

template <std::size_t N>
struct object<char[N]> {
    void operator()(msgpack::object& o, const char(&v)[N]) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msgpack::type::STR;
        o.via.str.ptr = v;
        o.via.str.size = size;
    }
};

template <std::size_t N>
struct object<const char[N]> {
    void operator()(msgpack::object& o, const char(&v)[N]) const {
        uint32_t size = checked_get_container_size(std::strlen(v));
        o.type = msgpack::type::STR;
        o.via.str.ptr = v;
        o.via.str.size = size;
    }
};


} // namespace adaptor

/// @cond
}  // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

}  // namespace msgpack

#endif // MSGPACK_V1_TYPE_CARRAY_HPP