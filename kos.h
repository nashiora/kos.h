/// ======================================================================= ///
/// Kos - Simple C Base Library                                             ///
/// ----------------------------------------------------------------------- ///
/// Copyright (C) 2025  Local Atticus <contact@nashiora.com>                ///
///                                                                         ///
/// This software is provided 'as-is', without any express or implied       ///
/// warranty. In no event will the authors be held liable for any damages   ///
/// arising from the use of this software.                                  ///
///                                                                         ///
/// Permission is granted to anyone to use this software for any purpose,   ///
/// including commercial applications, and to alter it and redistribute it  ///
/// freely, subject to the following restrictions:                          ///
///                                                                         ///
/// 1. The origin of this software must not be misrepresented; you must not ///
///    claim that you wrote the original software. If you use this software ///
///    in a product, an acknowledgment in the product documentation would   ///
///    be appreciated but is not required.                                  ///
///                                                                         ///
/// 2. Altered source versions must be plainly marked as such, and must not ///
///    be misrepresented as being the original software.                    ///
///                                                                         ///
/// 3. This notice may not be removed or altered from any source            ///
///    distribution.                                                        ///
/// ======================================================================= ///

#ifndef KOS_H_
#define KOS_H_

#define _CRT_SECURE_NO_WARNINGS

/// ======================================================================= ///
/// Standard headers, for convenience                                       ///
/// ======================================================================= ///

#include <assert.h>
#include <inttypes.h>
#if __STDC_VERSION__ < 202311L
#    include <stdalign.h>
#endif
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// ======================================================================= ///
/// Platform and compiler identification                                    ///
/// ======================================================================= ///

#define KOS_WINDOWS 0
#define KOS_LINUX   0

#define KOS_GCC     0
#define KOS_CLANG   0
#define KOS_MSVC    0

#ifdef _WIN32
#    undef KOS_WINDOWS
#    define KOS_WINDOWS 1
#endif

#ifdef __linux__
#    undef KOS_LINUX
#    define KOS_LINUX 1
#endif

#ifdef __GNUC__
#    undef KOS_GCC
#    define KOS_GCC 1
#endif

#ifdef __clang__
#    undef KOS_CLANG
#    define KOS_CLANG 1
#endif

#if defined(_MSCVER) && !KOS_CLANG
#    undef KOS_MSVC
#    define KOS_MSVC 1
#endif

/// ======================================================================= ///
/// "Fancy" macros                                                          ///
/// ======================================================================= ///

#if __STDC_VERSION__ < 202311L || (defined(_MSC_VER) && !defined(__clang__))
#    define nullptr NULL
#endif

#define kos_cast(T) (T)
#define kos_discard (void)

#define kos_return_defer(Value) do { result = (Value); goto defer; } while (0)
#define kos_scope_defer(Expr) for (int kos_scope_defer_i_##__LINE__ = 0; kos_scope_defer_i_##__LINE__ == 0; ((kos_scope_defer_i_##__LINE__ = 1), (Expr))) do { } while (0)

#define kos_assert(Cond) assert(Cond)
#define kos_assert_message(Cond, Message) assert(Cond && "" Message "")

/// ======================================================================= ///
/// Primitive types                                                         ///
/// ======================================================================= ///

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef ptrdiff_t isize;
typedef size_t usize;

typedef float f32;
typedef double f64;

/// ======================================================================= ///
/// Fancy Utilities                                                         ///
/// ======================================================================= ///

void kos_hexdump(const char* data, isize count);

/// ======================================================================= ///
/// Allocators                                                              ///
/// ======================================================================= ///

typedef enum kos_allocator_action {
    KOS_ALLOC,
    KOS_REALLOC,
    KOS_REALLOC2,
    KOS_DEALLOC,
} kos_allocator_action;

typedef void* (*kos_allocator_function)(void* userdata, kos_allocator_action alloc_action, void* memory, isize size, isize previous_size, isize align);

typedef struct kos_allocator {
    kos_allocator_function allocator_function;
    void* userdata;
} kos_allocator;

typedef struct kos_arena_block {
    char* data;
    isize consumed;
} kos_arena_block;

typedef struct kos_arena {
    kos_allocator* allocator;
    kos_arena_block* data;
    isize capacity, count;
    isize block_size;
} kos_arena;

extern kos_allocator kos_allocator_default;
extern kos_allocator kos_allocator_temp;

i64 kos_align_for_bytes(isize byte_count);
isize kos_align_padding(isize size, isize align);
isize kos_align_to(isize size, isize align);

void* kos_alloc(kos_allocator* allocator, isize size);
void* kos_alloc_aligned(kos_allocator* allocator, isize size, isize align);
void* kos_realloc(kos_allocator* allocator, void* memory, isize size);
void* kos_realloc_aligned(kos_allocator* allocator, void* memory, isize size, isize align);
void* kos_realloc2(kos_allocator* allocator, void* memory, isize previous_size, isize size);
void* kos_realloc2_aligned(kos_allocator* allocator, void* memory, isize previous_size, isize size, isize align);
void kos_dealloc(kos_allocator* allocator, void* memory);

isize kos_temp_mark();
void kos_temp_rewind(isize mark);
void kos_temp_clear();

void kos_arena_init(kos_arena* arena, kos_allocator* allocator, isize block_size);
void kos_arena_deinit(kos_arena* arena);
void* kos_arena_alloc(kos_arena* arena, isize size);
void* kos_arena_alloc_aligned(kos_arena* arena, isize size, isize align);
void* kos_arena_realloc2(kos_arena* arena, void* memory, isize previous_size, isize size);
void* kos_arena_realloc2_aligned(kos_arena* arena, void* memory, isize previous_size, isize size, isize align);
void kos_arena_clear(kos_arena* arena);

/// ======================================================================= ///
/// Container types                                                         ///
/// ======================================================================= ///

#define KOS_DA_INITIAL_CAPACITY 256

#define KOS_DYNAMIC_ARRAY_FIELDS(ElemType) kos_allocator* allocator; ElemType* data; isize capacity, count

#define kos_da_ensure_capacity(DynArr, MinCap) do {                                                     \
        kos_dynamic_array_ensure_capacity((DynArr)->allocator, kos_cast(isize) sizeof(*(DynArr)->data), \
            kos_cast(void**) &(DynArr)->data, &(DynArr)->capacity, (MinCap));                           \
    } while (0)

#define kos_da_push(DynArr, Elem) do {                         \
        kos_da_ensure_capacity((DynArr), (DynArr)->count + 1); \
        kos_assert((DynArr)->data != nullptr);                 \
        (DynArr)->data[(DynArr)->count++] = (Elem);            \
    } while (0)

#define kos_da_push_many(DynArr, Elems, ElemCount) do {                                         \
        kos_da_ensure_capacity((DynArr), (DynArr)->count + (ElemCount));                        \
        memcpy((DynArr)->data + (DynArr)->count, Elems, sizeof(*(DynArr)->data) * (ElemCount)); \
        (DynArr)->count += (ElemCount);                                                         \
    } while (0)

#define kos_da_pop(DynArr) (kos_assert((DynArr)->data != nullptr), (DynArr)->data[--(DynArr)->count])

#define kos_da_dealloc(DynArr) do { kos_dealloc((DynArr)->allocator, (DynArr)->data); } while (0)

void kos_dynamic_array_ensure_capacity(kos_allocator* allocator, isize element_size, void** da_data_ptr, i64* da_capacity_ptr, i64 required_capacity);

/// ======================================================================= ///
/// String types                                                            ///
/// ======================================================================= ///

#define KOS_STR_FMT "%.*s"
#define KOS_STR_ARG(Str) (int)(Str).count, (Str).data

#define KOS_SV_CONST(ConstStr) (kos_string_view){ .data = "" ConstStr "", .count = kos_cast(isize) ((sizeof(ConstStr) / sizeof(char)) - 1) }

typedef struct kos_string {
    KOS_DYNAMIC_ARRAY_FIELDS(char);
} kos_string;

typedef struct kos_string_view {
    const char* data;
    isize count;
} kos_string_view;

isize kos_cstrlen(const char* cstr);
isize kos_cstrnlen(const char* cstr, isize max_length);

isize kos_string_append_cstr(kos_string* str, const char* cstr);
isize kos_string_append_sv(kos_string* str, kos_string_view sv);
isize kos_string_sprintf(kos_string* str, const char* format, ...);
isize kos_string_vsprintf(kos_string* str, const char* format, va_list v);

kos_string_view kos_sv(const char* cstr, isize length);
kos_string_view kos_string_as_view(kos_string* str);
kos_string_view kos_cstr_as_view(const char* cstr);
kos_string_view kos_sv_slice(kos_string_view sv, isize offset, isize length);
bool kos_sv_equals(kos_string_view sv0, kos_string_view sv1);

#endif /* KOS_H_ */


#if defined(KOS_IMPLEMENTATION) || defined(KOS_IMPL)
#undef KOS_IMPLEMENTATION
#undef KOS_IMPL

/// ======================================================================= ///
/// Fancy Utilities                                                         ///
/// ======================================================================= ///

void kos_hexdump(const char* data, isize count) {
    fprintf(stderr, "          00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\n");
    for (isize i = 0; i < (count + 15) / 16; i++) {
        fprintf(stderr, "%08tX ", i * 16);

        int nleft = 49;
        for (isize j = 0; j < 16; j++) {
            isize index = i * 16 + j;
            if (index >= count) break;
            if (j == 0x08) {
                nleft--;
                fputc(' ', stderr);
            }
            nleft -= fprintf(stderr, " %02hhX", data[index]);
        }

        for (int i = 0; i < nleft; i++)
            fputc(' ', stderr);
        fprintf(stderr, "  |");

        nleft = 16;
        for (isize j = 0; j < 16; j++) {
            isize index = i * 16 + j;
            if (index >= count) break;
            char b = data[index];
            if (b < 32 || b >= 127) fputc('.', stderr);
            else fputc(b, stderr);
            nleft--;
        }

        for (int i = 0; i < nleft; i++)
            fputc(' ', stderr);
        fprintf(stderr, "|\n");
    }
}

/// ======================================================================= ///
/// Allocators                                                              ///
/// ======================================================================= ///

static void* kos_allocator_default_function(void* userdata, kos_allocator_action alloc_action, void* memory, isize size, isize previous_size, isize align);

kos_allocator kos_allocator_default = {
    .allocator_function = kos_allocator_default_function,
};

static void* kos_allocator_temp_function(void* userdata, kos_allocator_action alloc_action, void* memory, isize size, isize previous_size, isize align);

static char kos_allocator_temp_buffer[64 * 1024 * 1024];
static isize kos_allocator_temp_allocated = 0;

kos_allocator kos_allocator_temp = {
    .allocator_function = kos_allocator_temp_function,
};

isize kos_align_for_bytes(isize byte_count) {
    byte_count &= 0x7FFF;

    /// https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
    ///
    /// Devised by
    ///   - Sean Anderson, Sepember 14, 2001
    ///   - Peter Hart and William Lewis, February 14, 1997
    ///
    /// Computes the next power of two >= 'bytes'.
    isize align = byte_count - 1;
    align |= align >> 1;
    align |= align >> 2;
    align |= align >> 4;
    align |= align >> 8;
    return align + 1;
}

isize kos_align_padding(isize size, isize align) {
    return (align - (size % align)) % align;
}

isize kos_align_to(isize size, isize align) {
    return size + kos_align_padding(size, align);
}

void* kos_alloc(kos_allocator* allocator, isize size) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_ALLOC, nullptr, size, 0, 16);
}

void* kos_alloc_aligned(kos_allocator* allocator, isize size, isize align) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_ALLOC, nullptr, size, 0, align);
}

void* kos_realloc(kos_allocator* allocator, void* memory, isize size) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_REALLOC, memory, size, 0, 16);
}

void* kos_realloc_aligned(kos_allocator* allocator, void* memory, isize size, isize align) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_REALLOC, memory, size, 0, align);
}

void* kos_realloc2(kos_allocator* allocator, void* memory, isize previous_size, isize size) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_REALLOC2, memory, size, previous_size, 16);
}

void* kos_realloc2_aligned(kos_allocator* allocator, void* memory, isize previous_size, isize size, isize align) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    return allocator->allocator_function(allocator->userdata, KOS_REALLOC2, memory, size, previous_size, align);
}

void kos_dealloc(kos_allocator* allocator, void* memory) {
    if (allocator == nullptr) {
        allocator = &kos_allocator_default;
    }

    kos_discard allocator->allocator_function(allocator->userdata, KOS_DEALLOC, memory, 0, 0, 0);
}


isize kos_temp_mark() {
    return kos_allocator_temp_allocated;
}

void kos_temp_rewind(isize mark) {
    kos_allocator_temp_allocated = mark;
}

void kos_temp_clear() {
    memset(kos_allocator_temp_buffer, 0, kos_cast(usize) kos_allocator_temp_allocated);
    kos_allocator_temp_allocated = 0;
}


void kos_arena_init(kos_arena* arena, kos_allocator* allocator, isize block_size) {
    if (arena == nullptr) return;
    *arena = (kos_arena){
        .allocator = allocator,
        .block_size = block_size,
    };
}

void kos_arena_deinit(kos_arena* arena) {
    if (arena == nullptr) return;

    for (isize i = 0; i < arena->count; i++) {
        kos_dealloc(arena->allocator, arena->data[i].data);
    }

    kos_da_dealloc(arena);
    *arena = (kos_arena) {0};
}

void* kos_arena_alloc(kos_arena* arena, isize size) {
    return kos_arena_realloc2(arena, nullptr, 0, size);
}

void* kos_arena_alloc_aligned(kos_arena* arena, isize size, isize align) {
    return kos_arena_realloc2_aligned(arena, nullptr, 0, size, align);
}

void* kos_arena_realloc2(kos_arena* arena, void* memory, isize previous_size, isize size) {
    return kos_arena_realloc2_aligned(arena, memory, previous_size, size, 16);
}

void* kos_arena_realloc2_aligned(kos_arena* arena, void* memory, isize previous_size, isize size, isize align) {
    if (memory != nullptr && previous_size >= size) {
        return memory;
    }

    if (size == 0) {
        return nullptr;
    }

    kos_assert(arena != nullptr);
    kos_assert(arena->block_size > 0);
    kos_assert(align > 0);

    kos_arena_block* block = nullptr;

    for (isize i = 0; i < arena->count; i++) {
        kos_arena_block* check_block = &arena->data[i];
        if (arena->block_size - kos_align_to(check_block->consumed, align) >= size) {
            block = check_block;
            break;
        }
    }

    if (block == nullptr) {
        kos_da_push(arena, ((kos_arena_block){ .data = kos_alloc(arena->allocator, arena->block_size) }));
        block = &arena->data[arena->count - 1];
    }

    kos_assert(block != nullptr);

    block->consumed = kos_align_to(block->consumed, align);
    char* new_memory = block->data + block->consumed;
    block->consumed += size;

    isize zero_offset = 0;
    isize zero_length = size;

    if (memory != nullptr && previous_size > 0) {
        memcpy(new_memory, memory, kos_cast(usize) previous_size);
        memset(memory, 0, kos_cast(usize) previous_size);
        zero_offset += previous_size;
        zero_length -= previous_size;
    }

    memset(new_memory + zero_offset, 0, kos_cast(usize) zero_length);
    return new_memory;
}

void kos_arena_clear(kos_arena* arena) {
    if (arena == nullptr) return;

    for (isize i = 0; i < arena->count; i++) {
        arena->data[i].consumed = 0;
    }
}

static void* kos_allocator_default_function(void* userdata, kos_allocator_action alloc_action, void* memory, isize size, isize previous_size, isize align) {
    // TODO(local): Aligned allocation in the "default" libc allocator, if possible.
    switch (alloc_action) {
        default: kos_assert_message(false, "Unsupported allocator action in kos_allocator_default_function"); return nullptr;
        case KOS_ALLOC: return malloc(kos_cast(usize) size);
        case KOS_REALLOC:
        case KOS_REALLOC2: return realloc(memory, kos_cast(usize) size);
        case KOS_DEALLOC: free(memory); return nullptr;
    }
}

static void* kos_allocator_temp_function(void* userdata, kos_allocator_action alloc_action, void* memory, isize size, isize previous_size, isize align) {
    kos_allocator_temp_allocated = kos_align_to(kos_allocator_temp_allocated, align);
    switch (alloc_action) {
        default: kos_assert_message(false, "Unsupported allocator action on temp memory."); return nullptr;
        case KOS_ALLOC: {
            if (size == 0) return nullptr;

            char* new_memory = kos_allocator_temp_buffer + kos_allocator_temp_allocated;
            kos_allocator_temp_allocated += size;

            memset(new_memory, 0, kos_cast(usize) size);
            return new_memory;
        }

        case KOS_REALLOC2: {
            if (memory != nullptr && previous_size >= size) return memory;
            if (size == 0) return nullptr;

            char* new_memory = kos_allocator_temp_buffer + kos_allocator_temp_allocated;
            kos_allocator_temp_allocated += size;

            isize zero_offset = 0;
            isize zero_length = size;

            if (memory != nullptr && previous_size > 0) {
                memcpy(new_memory, memory, kos_cast(usize) previous_size);
                memset(memory, 0, kos_cast(usize) previous_size);
                zero_offset += previous_size;
                zero_length -= previous_size;
            }

            memset(new_memory + zero_offset, 0, kos_cast(usize) zero_length);
            return new_memory;
        }

        case KOS_DEALLOC: return nullptr;
    }
}

/// ======================================================================= ///
/// Container types                                                         ///
/// ======================================================================= ///

void kos_dynamic_array_ensure_capacity(kos_allocator* allocator, isize element_size, void** da_data_ptr, isize* da_capacity_ptr, isize required_capacity) {
    kos_assert(element_size > 0);
    if (*da_capacity_ptr >= required_capacity) {
        return;
    }

    isize old_capacity = *da_capacity_ptr;
    isize new_capacity = *da_capacity_ptr;

    if (new_capacity == 0) new_capacity = KOS_DA_INITIAL_CAPACITY;
    while (new_capacity < required_capacity) new_capacity *= 2;

    kos_assert(new_capacity > 0);

    *da_capacity_ptr = new_capacity;
    *da_data_ptr = kos_realloc2(allocator, *da_data_ptr, old_capacity * element_size, new_capacity * element_size);
}

/// ======================================================================= ///
/// String types                                                            ///
/// ======================================================================= ///

isize kos_cstrlen(const char* cstr) {
    isize length = 0;
    while (cstr[length] != '\0') {
        length++;
    }

    return length;
}

isize kos_cstrnlen(const char* cstr, isize max_length) {
    isize length = 0;
    while (length < max_length && cstr[length] != '\0') {
        length++;
    }

    return length;
}

isize kos_string_append_cstr(kos_string* str, const char* cstr) {
    isize cstr_length = kos_cstrlen(cstr);
    kos_da_push_many(str, cstr, cstr_length);
    return cstr_length;
}

isize kos_string_append_sv(kos_string* str, kos_string_view sv) {
    kos_da_push_many(str, sv.data, sv.count);
    return sv.count;
}

isize kos_string_sprintf(kos_string* str, const char* format, ...) {
    va_list v;
    va_start(v, format);
    isize result_count = kos_string_vsprintf(str, format, v);
    va_end(v);
    return result_count;
}

isize kos_string_vsprintf(kos_string* str, const char* format, va_list v) {
    va_list v0;
    va_copy(v0, v);
    isize check_append_count = vsnprintf(NULL, 0, format, v0);
    va_end(v0);

    kos_da_ensure_capacity(str, str->count + check_append_count + 1);
    kos_assert(str->data != nullptr);

    va_list v1;
    va_copy(v1, v);
    isize append_count = vsnprintf(str->data + str->count, kos_cast(usize) check_append_count + 1, format, v1);
    kos_assert(append_count == check_append_count);
    va_end(v1);

    str->count += append_count;
    return append_count;
}

kos_string_view kos_sv(const char* cstr, isize length) {
    return (kos_string_view) {
        .data = cstr,
        .count = length,
    };
}

kos_string_view kos_string_as_view(kos_string* str) {
    return kos_sv(str->data, str->count);
}

kos_string_view kos_cstr_as_view(const char* cstr) {
    return kos_sv(cstr, kos_cstrlen(cstr));
}

kos_string_view kos_sv_slice(kos_string_view sv, isize offset, isize length) {
    return kos_sv(sv.data + offset, length);
}

bool kos_sv_equals(kos_string_view sv0, kos_string_view sv1) {
    if (sv0.count != sv1.count) {
        return false;
    }

    for (isize i = 0; i < sv0.count; i++) {
        if (sv0.data[i] != sv1.data[i])
            return false;
    }

    return true;
}

#endif /* KOS_IMPLEMENTATION */
