#pragma once

#include "buffer_concepts.hpp"
#include <cassert>
#include <utility>

namespace buffers::virtual_buffer
{

template <typename T, mb_concepts::BufferShape Shape, typename Underlying_Buffer>
struct virtual_buffer
{
    using value_type              = T;
    using index_t                 = std::size_t;
    using shape_t                 = Shape;
    using size_type               = typename shape_t::size_type;
    using underlying_buffer_t     = Underlying_Buffer;
    using underlying_buffer_ptr_t = underlying_buffer_t*;

    virtual_buffer(shape_t shape, underlying_buffer_t* ub_ptr) :
        shape_{ shape },
        underlying_buffer_(ub_ptr)
    {
    }

    constexpr virtual_buffer(virtual_buffer const&) noexcept                    = default;
    constexpr virtual_buffer(virtual_buffer&&) noexcept                         = default;
    constexpr auto operator=(virtual_buffer const&) noexcept -> virtual_buffer& = default;
    constexpr auto operator=(virtual_buffer&&) noexcept -> virtual_buffer&      = default;
    ~virtual_buffer() noexcept                                                  = default;

    [[nodiscard]]
    auto operator[](this auto&& self, index_t idx_y, index_t idx_x) noexcept
        -> decltype(auto)
    {
        assert(idx_y < std::forward<decltype(self)>(self).shape_.size_y());
        assert(idx_x < std::forward<decltype(self)>(self).shape_.size_x());
        return std::forward<decltype(self)>(self).underlying_buffer_->operator[](
            idx_y, idx_x
        );
    }

    shape_t                 shape_;
    underlying_buffer_ptr_t underlying_buffer_;
};

} // namespace buffers::virtual_buffer
