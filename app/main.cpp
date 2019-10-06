#include <nfl.hpp>
#include <iostream>
#include <tuple>
#include <array>
#include <gmpxx.h>

// cipher text ring, have two moduli, i.e. 15361, 13313
using T = uint64_t;

template <typename _T>
struct Para;

template <>
struct Para<uint16_t> {
    constexpr static auto p = 1;
    constexpr static auto q = 2;
    constexpr static auto degree = 16;
};

template <>
struct Para<uint32_t> {
    constexpr static auto p = 10;
    constexpr static auto q = 11;
    constexpr static auto degree = 128;
};

template <>
struct Para<uint64_t> {
    constexpr static auto p = 10;
    constexpr static auto q = 11;
    constexpr static auto degree = 1 << 15;
};

template <typename _T>
struct pair {
    _T x;
    _T y;
};

struct C {
    using poly = nfl::poly<T, Para<T>::degree, Para<T>::q>;
    using poly_ptr = nfl::poly_p<T, Para<T>::degree, Para<T>::q>;
};

struct P {
    using poly = nfl::poly<T, Para<T>::degree, Para<T>::p>;
    using poly_ptr = nfl::poly_p<T, Para<T>::degree, Para<T>::p>;
};


static nfl::FastGaussianNoise<uint16_t, T, 2> fg(8, 80, Para<T>::degree);
static nfl::gaussian<uint16_t, T, 2> chi(&fg);

// plain text ring, only have one modulus

using Cipher = std::pair<C::poly_ptr, C::poly_ptr>;
using Keys = std::tuple<C::poly_ptr, C::poly_ptr, C::poly_ptr>;

using Chi = nfl::gaussian<uint16_t, T, 2>;

void roundNearest(mpz_t out, mpz_t in, mpz_t div) {
    mpz_t q, r;
    mpz_init(q);
    mpz_init(r);
    mpz_set_ui(q, 0);
    mpz_set_ui(r, 0);

    mpz_tdiv_qr(q, r, in, div);
    mpz_mul_ui(r, r, 2);
    if (mpz_cmp(r, div) >= 0) {
        mpz_add_ui(q, q, 1);
    }
    mpz_mul(out, q, div);
}

Keys genKeys() {


    // first uniformly choose a
    C::poly a(nfl::uniform{});
    // ignore sigma since only one here
    // gaussian dist
    C::poly s(nfl::uniform{});
    C::poly e(nfl::uniform{});

    // keep everything in FFT domain
    a.ntt_pow_phi();
    s.ntt_pow_phi();
    e.ntt_pow_phi();

    C::poly b = s * a + e;
    // return (public Cipher, private Cipher)
    C::poly_ptr A(a), B(b), S(s);
    return {A, B, S};
}


Cipher encrypt(Cipher k, P::poly_ptr message) {
    C::poly u(chi), v(chi), w(chi);
    C::poly message_q(0);
    //C u{}, v{}, w{};  // no noise now

    // keep everything in FFT domain
    u.ntt_pow_phi();
    v.ntt_pow_phi();
    w.ntt_pow_phi();

    // transform Q/P to polynomial
    auto qdivp = C::poly(std::get<0>(k).get_modulus(1));
    qdivp.ntt_pow_phi();
    // transform message from mod P to mod Q
    //auto arr = message.poly2mpz();

    C::poly_ptr fst = C::poly_ptr(u) * std::get<0>(k) + C::poly_ptr(v);
    C::poly_ptr sec = C::poly_ptr(u) * std::get<1>(k) + C::poly_ptr(w) ;

    return {fst, sec};
}

C::poly_ptr decrypt(Cipher cipher, C::poly_ptr s) {

    C::poly_ptr dec = std::get<1>(cipher) - std::get<0>(cipher) * s;
    return dec;
}

void fuck(Cipher c, P::poly_ptr m) {
    return;
}


int main() {
 
    //genKeys();
    P::poly test{1,2,3,4,5,6,7};     // in fact this one already made invalid memory access
    std::make_pair<P::poly, P::poly>(P::poly{0}, P::poly{1});  // and this one always trigger seg fault
    //P::poly_ptr tp{test};

    //std::pair<C::poly_ptr, C::poly_ptr> kaj = {a, b};
    
    //Cipher enc = encrypt(kaj, tp);
/*     C::poly_ptr d = decrypt(enc, s);

    // get coefficient
    d.invntt_pow_invphi();
    std::array<mpz_t, Para<T>::degree> tt = d.poly2mpz();  // convert poly to an array

    // get Q/P
    auto m = d.get_modulus(1);
    mpz_t mm;
    mpz_init(mm);
    mpz_set_ui(mm, m);

    for (auto it = tt.begin(); it != tt.end(); it++) {
        roundNearest(*it, *it, mm);
    }

    d.mpz2poly(tt);

    // finally evaluate the polynomial
    d.ntt_pow_phi();
    tt = d.poly2mpz();
    
    // and the result is incorrect for now
    for (auto it = tt.begin(); it != tt.begin() + 8; it++) {
        mpz_tdiv_q(*it, *it, mm);
        std::cout << *it << std::endl;
    } */
    return 1;
}