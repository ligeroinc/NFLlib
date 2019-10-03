#include <nfl.hpp>
#include <iostream>
#include <tuple>
#include <array>
#include <gmpxx.h>

// cipher text ring, have two moduli, i.e. 15361, 13313
using C = nfl::poly_from_modulus<uint16_t, 16, 28>;
// plain text ring, only have one modulus
using P = nfl::poly_from_modulus<uint16_t, 16, 14>;
using key = std::pair<C, C>;

std::pair<key, C> keygen(const nfl::gaussian<uint8_t, uint16_t, 2>& chi) {
    // first uniformly choose a
    C a(nfl::uniform{});
    // ignore sigma since only one here
    // gaussian dist
    C s(chi);
    C e(chi);

    C b = s * a + e;
    // return (public key, private key)
    return {{a, b}, s};
}

key encrypt(const nfl::gaussian<uint8_t, uint16_t, 2>& chi, const key& k, P& message) {
    C u(chi), v(chi), w(chi);

    // transform Q/P to polynomial
    auto qdivp = C(k.first.get_modulus(1));
    qdivp.invntt_pow_invphi();  // <- not sure about this
    // transform message from mod P to mod Q
    C message_q;
    auto arr = message.poly2mpz();
    message_q.mpz2poly(arr);

    C fst = u * k.first + v;
    C sec = u * k.second + w + qdivp * message_q ;

    return {fst, sec};
}

P decrypt(const key& cipher, const C& s) {
    auto R = std::pow(2, 80) * (2 * 3.19 * s.get_modulus(0) * 1 * 16);
    C r(nfl::uniform{});
    C dec = cipher.second - cipher.first * s + r;
    P ddd;
    ddd.mpz2poly(dec.poly2mpz());
    return ddd;
}


int main() {
    constexpr auto lambda = 80;
    constexpr auto degree = 16;  // simple one for now
    auto fg = nfl::FastGaussianNoise<uint8_t, uint16_t, 2>(3.2, lambda, degree);
    auto chi = nfl::gaussian<uint8_t, uint16_t, 2>(&fg);
    auto [k, s] = keygen(chi);
    auto test = nfl::poly_from_modulus<uint16_t, degree, 14>({15361, 15362});
    key enc = encrypt(chi, k, test);
    P d = decrypt(enc, s);
    d.ntt_pow_phi();  // not sure about evaluate ploynomial, do FFT here
    std::array<mpz_t, degree> dd = d.poly2mpz();  // convert poly to an array
    
    // and the result is incorrect for now
    for (auto it = dd.begin(); it != dd.end(); it++) {
        std::cout << *it << std::endl;
    }
    return 1;
}