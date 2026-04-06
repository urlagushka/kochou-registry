// kochou
template < requirement_concept T, ktl::u32 ID >
struct ensure
{
    ensure(kochou::context const & _ctx) noexcept
    {
        ktl::errc rc = T::apply(_ctx, ID);
        if (rc != ktl::errc::no_error)
        {
            kochou::log_fatal(std::format("ensure failed, id={}, requirement={}, rc={}", ID, T::string(), rc));
        }
    }
};

template < requirement_concept T, ktl::u32 ID >
struct should
{
    should(kochou::context const & _ctx) noexcept
    {
        std::ignore = T::apply(_ctx, ID);
    }
};

template < ktl::u32 ID >
struct allowed final
{
    bool operator()(kochou::context const & _ctx) noexcept
    {
        return _ctx.allowed(ID);
    }
}

// kochou-registry
template < typename T >
concept compoment_interface = requires {
    { T(kochou::context const &) -> T; }  
};

template < compoment_interface COMPONENT >
struct component_meta< crossplatform_window > final
{
    static constexpr ktl::u32 id = 0;
    static constexpr struct {
        ktl::u32 major = 0;
        ktl::u32 minor = 0;
        ktl::u32 patch = 0;
    } version;
    static constexpr ktl::flat_set< ktl::u32 > deps = {};
};

// crossplatform window
template < >
struct component_meta< crossplatform_window > final
{
    static constexpr ktl::u32 id = KOCHOU_REGISTRY_LOCAL_SPEC_ID;
    static constexpr struct {
        ktl::u32 major = KOCHOU_REGISTRY_LOCAL_SPEC_VERSION_MAJOR;
        ktl::u32 minor = KOCHOU_REGISTRY_LOCAL_SPEC_VERSION_MINOR;
        ktl::u32 patch = KOCHOU_REGISTRY_LOCAL_SPEC_VERSION_PATCH;
    } version;
    static constexpr ktl::flat_set< ktl::u32 > deps = KOCHOU_REGISTRY_LOCAL_SPEC_DEPS;
};

struct window_metal
    : kochou::ensure< ktl::api::khr_metal_surface, component_meta< crossplatform_window >::id >
    : kochou::ensure< ktl::api::khr_surface, component_meta< crossplatform_window >::id >
{
    using window_handle = void;
    using native_handle = void;
    using create_fn     = void;
    // other functions
    using destroy_fn    = void;
    
    window_metal(kochou::context const & _ctx) noexcept
        : kochou::ensure< ktl::api::khr_metal_surface, component_meta< crossplatform_window >::id >(_ctx)
        , kochou::ensure< ktl::api::khr_surface, component_meta< crossplatform_window >::id >(_ctx)
    {}
}

class crossplatform_window final
{
private:
    using active = ktl::reflection::selector<
        CONDITION,
        metal_window,
        wayland_window,
        win32_window
    >;
    using create_fn     = active::create_fn;
    // other functions
    using destroy_fn    = active::destroy_fn;

public
    using window_handle = active::window_handle;
    using native_handle = active::native_handle;
    

public:
    crossplatform_window(kochou::context const & _ctx)
        : active_(_ctx)
        : window_handle_(window_handle_default_value)
        , native_handle_(native_handle_default_value)
    {}
    
    // other methods ...

private:
    active active_;
    window_handle window_handle_;
    native_handle native_handle_;
}

// main.cpp
int main(int argc, char ** argv)
{
    kochou::context ctx; // can be thread local value
    kochou::registry::crossplatform_window window(cxt);
    bool is_available = kochou::allowed< kochou::registry::component_meta< crossplatform_window >::id >(ctx);
}
