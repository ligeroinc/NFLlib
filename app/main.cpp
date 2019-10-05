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

std::pair<key, C> keygen(const nfl::gaussian<uint8_t, uint16_t, 2>& chi) {
    // first uniformly choose a
    C a(nfl::uniform{});
    // ignore sigma since only one here
    // gaussian dist
    C s(chi);
    C e(chi);

    // keep everything in FFT domain
    a.ntt_pow_phi();
    s.ntt_pow_phi();
    e.ntt_pow_phi();

    C b = s * a + e;
    // return (public key, private key)
    return {{a, b}, s};
}

key encrypt(const nfl::gaussian<uint8_t, uint16_t, 2>& chi, const key& k, P& message) {
    //C u(chi), v(chi), w(chi);
    C u{}, v{}, w{};  // no noise now

    // keep everything in FFT domain
    u.ntt_pow_phi();
    v.ntt_pow_phi();
    w.ntt_pow_phi();

    // transform Q/P to polynomial
    auto qdivp = C(k.first.get_modulus(1));
    qdivp.ntt_pow_phi();
    // transform message from mod P to mod Q
    C message_q;
    auto arr = message.poly2mpz();
    message_q.mpz2poly(arr);

    C fst = u * k.first + v;
    C sec = u * k.second + w + qdivp * message_q ;

    return {fst, sec};
}

C decrypt(const key& cipher, const C& s) {
    C dec = cipher.second - cipher.first * s;
    return dec;
}


int main() {
    constexpr auto lambda = 80;
    constexpr auto degree = 16;  // simple one for now
    auto fg = nfl::FastGaussianNoise<uint8_t, uint16_t, 2>(3.2, lambda, degree);
    auto chi = nfl::gaussian<uint8_t, uint16_t, 2>(&fg);
    auto [k, s] = keygen(chi);
    auto test = nfl::poly_from_modulus<uint16_t, degree, 14>({1, 2, 3, 4});
    
    key enc = encrypt(chi, k, test);
    C d = decrypt(enc, s);

    // get coefficient
    d.invntt_pow_invphi();
    std::array<mpz_t, degree> tt = d.poly2mpz();  // convert poly to an array

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
    for (auto it = tt.begin(); it != tt.end(); it++) {
        std::cout << *it << std::endl;
    }
    return 1;
}