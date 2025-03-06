namespace bm_utils
{

inline static auto escape(void* p) noexcept -> void
{
    asm volatile("" : : "g"(p) : "memory");
}

inline static auto clobber() noexcept -> void
{
    asm volatile("" : : : "memory");
}

}; // namespace bm_utils
