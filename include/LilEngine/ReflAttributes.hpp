#pragma once

#include "Reflection.hpp"
#include <span>

struct Attribute : refl::attr::usage::field {}; 
struct ModelKeyAttribute : Attribute {};
struct TextureKeyAttribute : Attribute {};
struct ContainerNoAddAttribute : Attribute {};
struct ContainerNoEraseAttribute : Attribute {};

struct EnumAttrItem {
    std::string_view label;
    int value;
};

struct EnumAttribute : Attribute {
    static inline constexpr std::size_t MAX_ITEMS = 32;

    std::string_view labels[MAX_ITEMS]{};
    int values[MAX_ITEMS]{};
    std::size_t count = 0;

    constexpr EnumAttribute(std::initializer_list<EnumAttrItem> list) {
        if (list.size() > MAX_ITEMS) {
            throw std::out_of_range("Too many items passed to EnumAttribute");
        }
        for (const auto& item : list) {
            labels[count] = item.label;
            values[count] = item.value;
            ++count;
        }
    }

    constexpr std::span<const std::string_view> get_labels() const noexcept { 
        return {labels, count}; 
    }
    
    constexpr std::span<const int> get_values() const noexcept { 
        return {values, count}; 
    }
};