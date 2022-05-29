#include "big_number.h"

big_num big_num::operator*(const big_num &other) const {
    const auto multiplicand = (other < *this) ? *this : other;
    const auto multiplier = (other < *this) ? other : *this;
    const unsigned multiplier_size_in_half_width =
        (get_high_half(multiplier.num.back())) ? 2 * multiplier.num.size()
                                               : 2 * multiplier.num.size() - 1;
    big_num lower_part_multiplicand(multiplicand);
    big_num higher_part_multiplicand(multiplicand);
    for (auto u = 0; u < multiplicand.num.size(); ++u) {
        lower_part_multiplicand.num[u] =
            set_high_half(lower_part_multiplicand.num[u], 0);
        higher_part_multiplicand.num[u] =
            set_low_half(higher_part_multiplicand.num[u], 0);
    }
    higher_part_multiplicand.left_shift_by_half_n_times(1);
    // shift s.t. information is in LSB s.t. multiplication is trivial
    // shall shift back later.
    big_num ret(0);

    for (unsigned u = 0; u < multiplier_size_in_half_width; ++u) {
        auto(*const fp) = (u % 2) ? get_high_half : get_low_half;
        const auto multiplier_at_u = fp(multiplier.num[u / 2]);
        auto l_mcand(lower_part_multiplicand);
        auto h_mcand(higher_part_multiplicand);
        for (auto &a : l_mcand.num) {
            a *= multiplier_at_u;
        }
        for (auto &a : h_mcand.num) {
            a *= multiplier_at_u;
        }
        h_mcand.right_shift_by_half_n_times(1);
        auto tmp = l_mcand + h_mcand;
        tmp.left_shift_by_half_n_times(u);
        ret += tmp;
    }

    return ret;
}

void big_num::left_shift_by_half_n_times(const unsigned u) {
    if (u == 0) {
        return;
    }
    const unsigned old_size_in_half_width =
        (get_high_half(num.back())) ? 2 * num.size() : 2 * num.size() - 1;
    const unsigned new_size_in_half_width = old_size_in_half_width + u;
    while (((new_size_in_half_width + 1) / 2) > num.size()) {
        num.emplace_back(0);
    }
    for (unsigned v = new_size_in_half_width - 1; v >= u; --v) {
        const unsigned paste_idx = v / 2;
        auto(*const paste_fp) = (v % 2) ? set_high_half : set_low_half;
        const auto shift = (v % 2) ? HALF_WIDTH_IN_BITS : 0;
        const unsigned copy_idx = (v - u) / 2;
        auto(*const copy_fp) = ((v - u) % 2) ? get_high_half : get_low_half;
        num[paste_idx] =
            paste_fp(num[paste_idx], (copy_fp(num[copy_idx]) << shift));
    }
    for (unsigned v = 0; v < u; ++v) {
        auto(*const fp) = (v % 2) ? set_high_half : set_low_half;
        num[v / 2] = fp(num[v / 2], 0);
    }
}

void big_num::right_shift_by_half_n_times(const unsigned u) {
    const unsigned old_size_in_half_width =
        (get_high_half(num.back())) ? 2 * num.size() : 2 * num.size() - 1;
    if (u >= old_size_in_half_width) {
        num = {0};
        return;
    } else if (u == 0) {
        return;
    } else {
        const unsigned new_size_in_half_width = old_size_in_half_width - u;
        for (unsigned v = 0; v < new_size_in_half_width; ++v) {
            const unsigned paste_idx = v / 2;
            auto(*const paste_fp) = (v % 2) ? set_high_half : set_low_half;
            const auto shift = (v % 2) ? HALF_WIDTH_IN_BITS : 0;
            const unsigned copy_idx = (v + u) / 2;
            auto(*const copy_fp) = ((v + u) % 2) ? get_high_half : get_low_half;
            auto(*const erase_copied_fp) =
                ((v + u) % 2) ? set_high_half : set_low_half;
            num[paste_idx] =
                paste_fp(num[paste_idx], (copy_fp(num[copy_idx]) << shift));
            num[copy_idx] = erase_copied_fp(num[copy_idx], 0);
        }
        auto rit = num.crbegin();
        while (rit != num.crend() && *rit == static_cast<type_ul>(0)) {
            ++rit;
        }
        if (rit != num.crbegin()) {
            num.erase((rit - 1).base() - 1, num.cend());
        }
    }
}

big_num big_num::operator+(const big_num &other) const {
    /*
     * According to [mathematics
     * stackexchange](https://math.stackexchange.com/a/1736991/595630)
     * Names of operands in addition is as such:
     * augend + addend = sum
     */
    auto augend_ret = (other < *this) ? (*this) : other;
    const auto &addend = (other < *this) ? other : (*this);
    type_ul carry_out = 0;
    for (auto s = 0; s < addend.num.size(); ++s) {
        type_ul overflow_sum = addend.num[s] + augend_ret.num[s] + carry_out;
        if ((carry_out && (overflow_sum <= addend.num[s] ||
                           overflow_sum <= augend_ret.num[s])) ||
            ((!carry_out) && (overflow_sum < addend.num[s] ||
                              overflow_sum < augend_ret.num[s]))) {
            carry_out = 1;
        } else {
            carry_out = 0;
        }
        augend_ret.num[s] = overflow_sum;
    }
    for (auto s = addend.num.size(); carry_out; ++s) {
        if (s < augend_ret.num.size()) {
            if (augend_ret.num[s] + 1 > augend_ret.num[s]) {
                carry_out = 0;
            }
            ++augend_ret.num[s];
        } else {
            augend_ret.num.emplace_back(1);
            carry_out = 0;
        }
    }
    return augend_ret;
}

bool big_num::operator<(const big_num &other) const {
    if (num.size() < other.num.size()) {
        return true;
    }
    if (num.size() == other.num.size()) {
        for (auto s = num.size() - 1; s < num.size(); --s) {
            if (num[s] < other.num[s]) {
                return true;
            }
        }
    }
    return false;
}

std::ostream &operator<<(std::ostream &os, const big_num &b) {
    for (auto itor = b.num.crbegin(); itor != b.num.crend(); ++itor) {
        os << static_cast<unsigned long long>(*itor);
        if (itor != b.num.crend() - 1) {
            os << ',';
        }
    }
    return os;
}

bool big_num::operator==(const big_num &other) const {
    return !(other < (*this) || (*this) < other);
}
