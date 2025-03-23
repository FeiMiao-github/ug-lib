#include <boost/lockfree/policies.hpp>
#include <chrono>
#include <cstddef>
#include <gtest/gtest.h>
#include <boost/lockfree/spsc_queue.hpp>
#include <atomic>
#include <optional>
#include <thread>
#include <type_traits>
#include <utility>

TEST(ug_lib_test, test_boost_spsc_queue) {
    // boost::lockfree::spsc_queue<int, boost::lockfree::capacity<1024>> q;
//     boost::lockfree::spsc_queue<int> q(1024);

//     std::atomic<int> counter(0);
//     auto consumer = [&]() {
//         int v = 0;
//         while (v >= 0) {
//             while (!q.pop(v))
//             {
//                 std::this_thread::yield();
//             }
//             std::cout << "consumer " << v << std::endl;
//         }
//         std::cout << "fetch add " << counter.fetch_add(1) << "\n";
//     };
//     auto producer = [&]() {
//         for (int i = 0; i < 1000000; ++i) {
//             while (!q.push(i))   
//             {
//                 std::this_thread::yield();
//             }
//         }
//         while (!q.push(-1))
//         {
//             std::this_thread::yield();
//         }
//         std::cout << "fetch add " << counter.fetch_add(1) << "\n";
//     };
//     std::thread th1(consumer);
//     std::thread th2(producer);
//     th1.detach();
//     th2.detach();
//     while (counter < 2) {
//         std::cout << "counter is " << counter << std::endl;
//         std::this_thread::yield();
//     }
}

#if 1

#pragma region Size
// Size
template <typename L>
struct Size_Imp {};

template <template <typename ...> typename L, typename ...T>
struct Size_Imp<L<T...>> {
    static constexpr size_t value = sizeof...(T);

    using Type = std::integral_constant<std::size_t, sizeof...(T)>;
};
#pragma endregion

template <typename L>
using Size = Size_Imp<L>::Type;

template <typename L>
using Empty = std::integral_constant<bool, Size<L>::value == 0>;

template <int I>
using Int = std::integral_constant<int, I>;

template <bool B>
using Bool = std::integral_constant<bool, B>;

using True = Bool<true>;
using False = Bool<false>;

template <std::size_t N>
using Size_T = std::integral_constant<std::size_t, N>;

template <typename T, T... I>
struct Int_Seq {};

#pragma region List
template<typename... T>
struct List
{
};

template <typename T, T... I>
using List_C = List<std::integral_constant<T, I>...>;

#pragma endregion

// If
template <bool C, class T, class ... E>
struct If_Imp {
};

template <class T, class ... E>
struct If_Imp<true, T, E...> {
    using Type = T;
};

template <class T, class E>
struct If_Imp<false, T, E> {
    using Type = E;
};

template <typename C, typename T, typename ...E>
using If = typename If_Imp<static_cast<bool>(C::value), T, E...>::Type;

template <bool C, typename T, typename ...E>
using If_C = typename If_Imp<C, T, E...>::Type;

#pragma region Void

struct Void
{
    using Type = Void;
};

template <typename T>
struct Is_Void
    : False
{
};

template <>
struct Is_Void<Void>
    : True
{
};

template <typename T>
struct Is_Not_Void
    : True
{
};

#pragma endregion

template <>
struct Is_Not_Void<Void>
    : False
{
};

template <typename S, typename F>
struct From_Seq_Imp;

template <template <typename T, T... I> typename S, typename U, U... J, typename F>
struct From_Seq_Imp<S<U, J...>, F> {
    using Type = List_C<U, (F::value + J)...>;
};

template <typename S, typename F=Int<0>>
using From_Seq = typename From_Seq_Imp<S, F>::Type;

template <bool C, typename T, typename E>
struct Iseq_If_C_Imp;

template <typename T, typename E>
struct Iseq_If_C_Imp<true, T, E>
{
    using Type = T;
};

template <typename T, typename E>
struct Iseq_If_C_Imp<false, T, E>
{
    using Type = E;
};

template <typename T>
struct Iseq_ID {
    using Type = T;
};

template <bool C, typename T, typename E>
using Iseq_If_C = typename Iseq_If_C_Imp<C, T, E>::Type;

template <typename S1, typename S2>
struct Append_Int_Seq;

template <typename T, T ...I, T ...J>
struct Append_Int_Seq<Int_Seq<T, I...>, Int_Seq<T, J...>> {
    using Type = Int_Seq<T, I..., (J + sizeof...(I))...>;
};

template <typename T, T N>
struct Make_Int_Seq_Imp;

template <typename T, T N>
struct Make_Int_Seq_Imp_ {
private:
    static_assert( N >= 0, "make_integer_sequence<T, N>: N must not be negative" );

    static T const M = N / 2;
    static T const R = N % 2;

    using S1 = typename Make_Int_Seq_Imp<T, M>::Type;
    using S2 = typename Append_Int_Seq<S1, S1>::Type;
    using S3 = typename Make_Int_Seq_Imp<T, R>::Type;
    using S4 = typename Append_Int_Seq<S2, S3>::Type;

public:
    using Type = S4;
};

template <typename T, T N>
struct Make_Int_Seq_Imp:
    Iseq_If_C<
        N == 0,
        Iseq_ID<Int_Seq<T>>,
        Iseq_If_C<
            N == 1,
            Iseq_ID<Int_Seq<T, 0>>,
            Make_Int_Seq_Imp_<T, N>
        >
    >
{
};

template<class T, T N>
using Make_Int_Seq = Make_Int_Seq_Imp<T, N>::Type;

template <typename N, typename F = Int<0>>
using Iota = From_Seq<Make_Int_Seq<typename std::remove_const<decltype(N::value)>::type, N::value>, F>;

template <typename V, typename ...T>
constexpr std::size_t CX_Count()
{
    constexpr bool a[] = { false, std::is_same<T, V>::value...};
    std::size_t r = 0;
    for (std::size_t i = 1; i < sizeof...(T) + 1; ++i)
    {
        r += a[i];
    }
    return r;
}

template <typename L, typename V>
struct Count_Imp;

template <template <typename ...> typename L, typename ...T, typename V>
struct Count_Imp<L<T...>, V>
{
    using Type = Size_T<CX_Count<V, T...>()>;
};

template <typename L, typename V>
using Count = Count_Imp<L, V>::Type;

template <typename ...T>
struct Same_Imp;

template <>
struct Same_Imp<> {
    using Type = True;
};

template <typename T1, typename ...T>
struct Same_Imp<T1, T...>
{
    using Type = Bool<
            Count<List<T...>, T1>::value == sizeof...(T)
        >;
};

template <typename ...T>
using Same = Same_Imp<T...>::Type;

template <template <typename ...> typename F, typename ...L>
struct Transform_Imp
{
};

template <template <typename ...> typename F, template <typename ...>typename L, typename ...T>
struct Transform_Imp<F, L<T...>>
{
    using Type = L<F<T>...>;
};

template <template <typename ...> typename F, template <typename ...>typename L1, typename ...T1, template <typename ...>typename L2, typename ...T2>
struct Transform_Imp<F, L1<T1...>, L2<T2...>>
{
    using Type = L1<F<T1, T2>...>;
};

template<template<class...> class F, template<class...> class L1, class... T1, template<class...> class L2, class... T2, template<class...> class L3, class... T3>
struct Transform_Imp<F, L1<T1...>, L2<T2...>, L3<T3...>>
{
    using Type = L1<F<T1, T2, T3>...>;
};

struct List_Size_Mismatch {};

template <template <typename ...> typename F, typename ...L>
using Transform = typename If<Same<Size<L>...>, Transform_Imp<F, L...>, List_Size_Mismatch>::Type;


template <template <typename ...> typename F, typename ...T>
struct Valid_Imp {
    template<template<class...> class G, class = G<T...>>
    static std::integral_constant<bool, true> check(int);

    template<template<class...> class>
    static std::integral_constant<bool, false> check(...);

    using type = decltype(check<F>(0));
};

template <template <typename ...> typename F, typename ...T>
using Valid = Valid_Imp<F, T...>::type;

template <typename T>
struct ID {
    using Type = T;
};

template <typename ...T>
struct Inherit: T... {};

template <template <typename ...> typename F, typename ...T>
struct Defer_Imp {
    using Type = F<T...>;
};

template <typename K, typename V>
struct Map_Find_Imp;

template <template <typename ...> typename M, typename ...T, typename K>
struct Map_Find_Imp<M<T...>, K>{
    using U = Inherit<ID<T>...>;
    template <template <typename ...> typename L, typename ...U>
    static ID<L<K, U...>> f(ID<L<K, U...>>*);
    static ID<void> f(...);
    using Type = decltype( f( static_cast<U*>(0) ) )::Type;
};

template <typename K, typename V>
using Map_Find = typename Map_Find_Imp<K, V>::Type;

template <typename L>
struct Second_Imp;

template <template <typename ...> typename L, typename T1, typename T2, typename ...T>
struct Second_Imp<L<T1, T2, T...>> {
    using Type = T2;  
};

template <typename L>
using Second = typename Second_Imp<L>::Type;

struct No_Type {};

template <template <typename ...> typename F, typename ...T>
using Defer = If<Valid<F, T...>, Defer_Imp<F, T...>, No_Type>;

template <typename T, template <typename ...> typename B>
struct Rename_Imp{};

template <template <typename ...> typename L, typename ... T, template<typename ...> typename B>
struct Rename_Imp<L<T...>, B>
    : Defer<B, T...>
{
};

template <typename L, template <typename ...> typename B>
using Rename = typename Rename_Imp<L, B>::Type;

template <typename Q, typename L>
using Apply_Q = typename Rename_Imp<L, Q::template func>::Type;

template <typename L, std::size_t I> struct At_C_Imp;
template <typename L, std::size_t I>
struct At_C_Imp {
    using Map = Transform<List, Iota<Size<L> >, Rename<L, List>>;
    using Type = Second<Map_Find<Map, Size_T<I> > >;
};

template <typename L, std::size_t I>
using At_C = typename If_C<(I < Size<L>::value), At_C_Imp<L, I>, void>::Type;

template <typename L>
struct Front_Imp {};

template <template <typename ...> typename L, typename T1, typename ...T>
struct Front_Imp<L<T1, T...>> {
    using Type = T1;  
};

template <typename L>
using Front = typename Front_Imp<L>::Type;

template <typename L>
struct Pop_Front_Imp {};

template <template <typename ...> typename L, typename T1, typename ...T>
struct Pop_Front_Imp<L<T1, T...>> {
    using Type = L<T...>;
};

template <typename L>
using Pop_Front = typename Pop_Front_Imp<L>::Type;

struct Def {};

template <typename Tag, typename Predicate_ = Def>
struct Opt {
    using Key_Type = Tag;
    using Predicate = Predicate_;
};

template <typename T>
struct Is_Opt 
    : std::integral_constant<bool, false>
{
};

template <typename Tag, typename Predicate>
struct Is_Opt<Opt<Tag, Predicate>>
    : std::integral_constant<bool, true>
{
};

template <typename Tag, typename Predicate_ = Def>
struct Req {
    using Key_Type = Tag;
    using Predicate = Predicate_;
};

template <typename T>
struct Is_Req
    : std::integral_constant<bool, false>
{
};

template <typename Tag, typename Predicate>
struct Is_Req<Req<Tag, Predicate>>
    : std::integral_constant<bool, true>
{
};

struct Tag_Template_Keyword_Arg
{
    template <typename K, typename T>
    struct Apply
    {
        // typedef ::boost::parameter::template_keyword<K,T> type;
    };

    // template <typename K, typename T>
    // using fn = ::boost::parameter::template_keyword<K,T>;
};

template <typename SpecSeq, typename ...Arg>
struct Make_Param_Spec_Items_Helper;

template <typename SpecSeq>
struct Make_Param_Spec_Items_Helper<SpecSeq>
{
    using Type = Void;
};

template <typename SpecSeq, typename ...Args>
using Make_Param_Spec_Items = If<
    Empty<SpecSeq>,
    ID<Void>,
    Make_Param_Spec_Items_Helper<SpecSeq, Args...>>;

template <typename SpecSeq>
struct Make_Deduced_List;

template <typename Tag>
struct Deduced {
    using Key_Type = Tag;  
};

template <typename T>
struct Is_Deduced0_Imp
    : std::integral_constant<bool, false>
{
    using Base_Type = std::integral_constant<bool, false>;
    using Type = Base_Type::type;
};

template <typename Tag>
struct Is_Deduced0_Imp<Deduced<Tag>>
    : std::integral_constant<bool, true>
{
    using Base_Type = std::integral_constant<bool, false>;
    using Type = Base_Type::type;
};

template <typename T>
struct Is_Deduced0
    : Is_Deduced0_Imp<typename T::Key_Type>::Type
{
};

template <typename T>
using Is_Deduced = If<
    If<
        Is_Opt<T>,
        std::integral_constant<bool, true>,
        Is_Req<T>
    >,
    Is_Deduced0<T>,
    std::integral_constant<bool, false>
>;

template <
typename ParameterSpec
, typename Tail_ = Void
>
struct Deduced_Item
{
    using Spec = ParameterSpec;
    using Tail = Tail_;
};

template <typename Spec, typename Tail>
struct Make_Deduced_Item {
    using Type = Deduced_Item<Spec, typename Tail::Type>;
};

template <typename Spec, typename Tail>
using Make_Deduced_Items = If<
    std::is_same<Spec, Void>,
    ID<Void>,
    If<
        Is_Deduced<Spec>,
        Make_Deduced_Item<Spec, Tail>,
        Tail
    >
>;

template <typename SpecSeq>
struct Make_Deduced_List_Not_Empty:
    Make_Deduced_Items<
        Front<SpecSeq>,
        Make_Deduced_List<
            Pop_Front<SpecSeq>
        >
    >
{
};

template <typename SpecSeq>
struct Make_Deduced_List
    : If<
        Empty<SpecSeq>,
        ID<Void>,
        Make_Deduced_List_Not_Empty<SpecSeq>
        >
{
};

template <
typename List_
, typename DeducedArgs
, typename TagFn
, typename IsPositional
, typename DeducedSet
, typename ArgumentPack
, typename Error
, typename EmitsErrors
>
struct Make_Arg_List0 {
    using Arg_ = List_::Arg;
    using Arg_Type = typename std::remove_const<typename std::remove_reference<Arg_>>::type;
};

template <
typename List_
, typename DeducedArgs
, typename TagFn
, typename IsPositional
, typename DeducedSet
, typename ArgumentPack
, typename Error
, typename EmitsErrors
>
struct Make_Arg_List_Imp:
    If<
        std::is_same<List_, Void>,
        ID<List<ArgumentPack, Error>>,
        Make_Arg_List0<List_, DeducedArgs, TagFn, IsPositional, DeducedSet, ArgumentPack, Error, EmitsErrors>
    >
{
};

template <typename K, typename V>
struct Def_ {
    constexpr Def_(V& v)
        : v_(v)
    {
    }

    V v_;
};

template <typename K, typename V>
struct Def_R_ {
    constexpr Def_R_(V&& v)
        : v_(std::forward<V>(v))
    {
    }

    V&& v_;
};

struct Empty_Arg_List {
    // TODO:
    template <typename ...Args>
    constexpr Empty_Arg_List(Args ... args)
    {
    }

    template <typename K, typename Default>
    constexpr Default& operator[](Def_<K, Default> x) const
    {
        return x.v_;
    }

    template <typename K, typename Default>
    constexpr Default&& operator[](Def_R_<K, Default> x) const
    {
        return std::forward<Default>(x.v_);
    }
};

template <
    typename Tagged_Arg,
    typename Next = Empty_Arg_List,
    typename Emits_Errors = True
>
class Arg_List
    : public Next
{

};

template <typename ...ArgTuples>
struct Arg_List_Cons;

template <>
struct Arg_List_Cons<>
{
    using Type = Empty_Arg_List;
};

template <typename ArgTuple0, typename ...Tuples>
struct Arg_List_Cons<ArgTuple0,Tuples...>
{
    using Type = Arg_List<
        typename ArgTuple0::Tagged_Arg,
        typename Arg_List_Cons<Tuples...>::Type,
        typename ArgTuple0::Emits_Errors
    >;
};

template <
    typename Keyword
    , typename TaggedArg
    , typename EmitsErrors = True
>
struct Flat_Like_Arg_Tuple
{
    using Tagged_Arg = TaggedArg;
    using Emits_Errors = EmitsErrors;
};

template <typename ...ArgTuples>
class Flat_Like_Arg_List
    : public Arg_List_Cons<ArgTuples...>::Type
{};

template <>
class Flat_Like_Arg_List<>
    : public Empty_Arg_List
{
    using Base_Type = Empty_Arg_List;
public:
    template <typename ...Args>
    constexpr Flat_Like_Arg_List(Args&&... args)
        : Base_Type(std::forward<Args>(args)...)
    {
    }

    template <typename Tagged_Arg>
    constexpr Flat_Like_Arg_List<
        Flat_Like_Arg_Tuple<
            typename Tagged_Arg::Base_Type::Key_Type,
            typename Tagged_Arg::Base_Type
        >
    > operator,(Tagged_Arg const& x) const {
        return Flat_Like_Arg_List<
            Flat_Like_Arg_Tuple<
                typename Tagged_Arg::Base_Type::Key_Type,
                typename Tagged_Arg::Base_Type
            >
        >(
            static_cast<typename Tagged_Arg::Base_Type const&>(x),
            static_cast<Base_Type const&>(*this)
        );
    }
};

template <
    typename List_,
    typename DeducedArgs,
    typename TagFn,
    typename EmitsErrors = std::integral_constant<bool, true>
>
using Make_Arg_List = Make_Arg_List_Imp<
    List_,
    DeducedArgs,
    TagFn,
    std::integral_constant<bool, true>,
    List<>,
    Flat_Like_Arg_List<>,
    Void,
    EmitsErrors
>;

template <typename ...Specs>
struct Params
{
    using Param_Spec = List<Specs...>;
    using Deduced_List = Make_Deduced_List<Param_Spec>::Type;

    template <typename ...Args>
    struct Bind {
        using Type =
            At_C<
                typename Make_Arg_List<
                    typename Make_Param_Spec_Items<Param_Spec, Args...>::Type,
                    Deduced_List,
                    Tag_Template_Keyword_Arg
                >::Type,
                0
            >;
    };

    // Empty_Arg_List operator()() const {
    //     return Empty_Arg_List();
    // }
};

namespace Tag {
    struct Cap;
}

template <int N>
struct Arg;

template <typename T>
struct Is_PlaceHolder
    : std::integral_constant<bool, false>
{
};

template <int I>
struct Is_PlaceHolder<Arg<I>>
    : std::integral_constant<bool, true>
{
};

template <typename T>
struct Is_11_PlaceHolder
    : std::integral_constant<bool, false>
{
};

template <size_t I>
struct Is_11_PlaceHolder<Arg<I>>
    : std::integral_constant<bool, true>
{
};

template <typename Parameters, typename KeyWord, typename Default = Void>
struct Bind0 {
    using Type = Apply_Q<typename Parameters::Bind, List<KeyWord, Default, True>>;
};

template <typename Placeholder, typename KeyWord, typename Default = Void>
struct Bind1 {
    using Type = Apply_Q<Placeholder, List<KeyWord, Default, True>>;
};

template <typename Parameters, typename KeyWord, typename Default = Void>
struct Bind {
    using Type =
        If<
            Is_PlaceHolder<Parameters>,
            ID<int>,
            If<
                Is_11_PlaceHolder<Parameters>,
                Bind1<Parameters, KeyWord, Default>,
                Bind0<Parameters, KeyWord, Default>
            >
        >::Type;
};

template <typename Bound_Args, typename Tag_Type>
struct Has_Arg {
    using Type = Bind<Bound_Args, Tag_Type, Void>::Type;
    static constexpr bool value = Is_Not_Void<Type>::value;
};

template <typename Bound_Args>
struct Ext_Capacity {
    static const bool Has_Capacity = Has_Arg<Bound_Args, Tag::Cap>::value;
};

using Ring_Buffer_Sign = Params<Opt<Tag::Cap>>;

struct Runtime_Sized_Ring_Buffer {
    void ShowLog()
    {
        std::cout << "Runtime_Sized_Ring_Buffer\n";
    }
};

struct Compile_Time_Sized_Ring_Buffer {
    void ShowLog()
    {
        std::cout << "Compile_Time_Sized_Ring_Buffer\n";
    }
};

template <typename ...Options>
struct Make_Ring_Buffer
{
    using Bound_Args = Ring_Buffer_Sign::Bind<Options...>::Type;

    using Ext_Capacity_Type = Ext_Capacity<Bound_Args>;

    static constexpr bool Runtime_Sized = !Ext_Capacity_Type::Has_Capacity;

    using Ring_Buffer_Type = std::conditional_t<Runtime_Sized,
        Runtime_Sized_Ring_Buffer,
        Compile_Time_Sized_Ring_Buffer>;
};

template <typename ...Options>
class My_Spsc_Queue: public Make_Ring_Buffer<Options...>::Ring_Buffer_Type
{
    using Base_Type = typename Make_Ring_Buffer<Options...>::Ring_Buffer_Type;

public:
    void ShowLog()
    {
        Base_Type::ShowLog();
    }
};

#endif

// 主函数
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    std::cout << "hello world !\n";

    // My_Spsc_Queue().ShowLog();

    return RUN_ALL_TESTS();
}