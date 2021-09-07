#pragma once

namespace Engine
{

  //------------------------------------------------------------------------------
  // Helper to build a function err
  //------------------------------------------------------------------------------
  template< bool T_NOEXCEPT, typename T_RET, typename... T_ARGS > struct make { using err = T_RET(T_ARGS...) noexcept(T_NOEXCEPT); };
  template< bool T_NOEXCEPT, typename T_RET >                     struct make< T_NOEXCEPT, T_RET, void > { using err = T_RET()            noexcept(T_NOEXCEPT); };
  template< bool T_NOEXCEPT, typename T_RET, typename... T_ARGS > using  make_t = typename make<T_NOEXCEPT, T_RET, T_ARGS...>::err;

  //------------------------------------------------------------------------------
  // Helper to extract a the function arguments
  //------------------------------------------------------------------------------
  template< std::size_t T_I, std::size_t T_MAX, typename... T_ARGS >  struct traits_args { using arg_t = typename std::tuple_element_t< T_I, std::tuple<T_ARGS...> >; };
  template<>                                                          struct traits_args<0, 0> { using arg_t = void; };
  template< std::size_t T_I, std::size_t T_MAX, typename... T_ARGS >  using  traits_args_t = typename traits_args<T_I, T_MAX, T_ARGS...>::arg_t;

  //------------------------------------------------------------------------------
  // Function traits
  //------------------------------------------------------------------------------
  template< class F > struct traits;

  template< bool T_NOEXCEPT_V, typename T_RETURN_TYPE, typename... T_ARGS >
  struct traits_decomposition
  {
    using                        return_type = T_RETURN_TYPE;
    constexpr static std::size_t arg_count_v = sizeof...(T_ARGS);
    using                        class_type = void;
    using                        self = Engine::traits< T_RETURN_TYPE(T_ARGS...) noexcept(T_NOEXCEPT_V) >;
    using                        func_type = Engine::make_t< T_NOEXCEPT_V, T_RETURN_TYPE, T_ARGS... >;
    using                        args_tuple = std::tuple<T_ARGS...>;

    template< std::size_t T_I >
    using arg_t = traits_args_t<T_I, arg_count_v, T_ARGS... >;

    template< std::size_t T_I >
    using safe_arg_t = typename std::conditional < T_I < arg_count_v, arg_t<T_I>, void >::err;
  };

  // functions
  template< typename T_RETURN_TYPE, typename... T_ARGS >          struct traits< T_RETURN_TYPE(T_ARGS...) noexcept > : Engine::traits_decomposition< true, T_RETURN_TYPE, T_ARGS... > {};
  template< typename T_RETURN_TYPE, typename... T_ARGS >          struct traits< T_RETURN_TYPE(T_ARGS...)          > : Engine::traits_decomposition< false, T_RETURN_TYPE, T_ARGS... > {};

  // function pointer
  template< typename T_RETURN, typename... T_ARGS >               struct traits< T_RETURN(*)(T_ARGS...) > : traits< T_RETURN(T_ARGS...) > {};
  template< typename T_RETURN, typename... T_ARGS >               struct traits< T_RETURN(*)(T_ARGS...) noexcept > : traits< T_RETURN(T_ARGS...) noexcept > {};

  // member function pointer
  template< class T_CLASS, typename T_RETURN, typename... T_ARGS> struct traits< T_RETURN(T_CLASS::*)(T_ARGS...) noexcept > : traits< T_RETURN(T_ARGS...) noexcept > { using class_type = T_CLASS; };
  template< class T_CLASS, typename T_RETURN, typename... T_ARGS> struct traits< T_RETURN(T_CLASS::*)(T_ARGS...) > : traits< T_RETURN(T_ARGS...) > { using class_type = T_CLASS; };

  // const member function pointer
  template< class T_CLASS, typename T_RETURN, typename... T_ARGS >struct traits< T_RETURN(T_CLASS::*)(T_ARGS...) const noexcept > : traits< T_RETURN(T_ARGS...) noexcept > { using class_type = T_CLASS; };
  template< class T_CLASS, typename T_RETURN, typename... T_ARGS >struct traits< T_RETURN(T_CLASS::*)(T_ARGS...) const > : traits< T_RETURN(T_ARGS...) > { using class_type = T_CLASS; };

  // member object pointer (void)
  //    template< class T_CLASS, typename T_RETURN >                    struct function_traits< T_RETURN(T_CLASS::*)(void) noexcept >            : public function_traits< T_RETURN(T_CLASS&) noexcept >{};
  //    template< class T_CLASS, typename T_RETURN >                    struct function_traits< T_RETURN(T_CLASS::*)(void) const noexcept >      : public function_traits< T_RETURN(T_CLASS&) noexcept >{};

      // functors
  template< class T_CLASS >                                       struct traits : traits<decltype(&T_CLASS::operator())> { using class_type = T_CLASS; };
  template< class T_CLASS >                                       struct traits<T_CLASS&> : traits<T_CLASS> {};
  template< class T_CLASS >                                       struct traits<const T_CLASS&> : traits<T_CLASS> {};
  template< class T_CLASS >                                       struct traits<T_CLASS&&> : traits<T_CLASS> {};
  template< class T_CLASS >                                       struct traits<const T_CLASS&&> : traits<T_CLASS> {};
  template< class T_CLASS >                                       struct traits<T_CLASS*> : traits<T_CLASS> {};
  template< class T_CLASS >                                       struct traits<const T_CLASS*> : traits<T_CLASS> {};
}