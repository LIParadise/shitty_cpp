#ifndef BIG_NUM
#define BIG_NUM

#include <climits> // CHAR_BIT
#include <iostream>
#include <vector>

class big_num {
    private:
        using type_ul = unsigned char;
        std::vector<type_ul> num;
        static constexpr type_ul ALL_1_MASK =
            static_cast<type_ul>(~static_cast<type_ul>(0));

        static constexpr unsigned HALF_WIDTH_IN_BITS =
            ((static_cast<unsigned>(CHAR_BIT) / 2) * sizeof(type_ul));
        static constexpr type_ul LOW_1_HIGH_0_MASK =
            ALL_1_MASK >> HALF_WIDTH_IN_BITS;
        static constexpr type_ul HIGH_1_LOW_0_MASK = LOW_1_HIGH_0_MASK
                                                     << HALF_WIDTH_IN_BITS;
        static type_ul get_low_half(const type_ul t) {
            return t & LOW_1_HIGH_0_MASK;
        }
        static type_ul get_high_half(const type_ul t) {
            return (t & HIGH_1_LOW_0_MASK) >> HALF_WIDTH_IN_BITS;
        }
        static type_ul set_low_half(const type_ul objective,
                                    const type_ul new_bits) {
            type_ul ret = static_cast<type_ul>(0);
            ret |= HIGH_1_LOW_0_MASK & objective;
            ret |= LOW_1_HIGH_0_MASK & new_bits;
            return ret;
        }
        static type_ul set_high_half(const type_ul objective,
                                     const type_ul new_bits) {
            type_ul ret = static_cast<type_ul>(0);
            ret |= LOW_1_HIGH_0_MASK & objective;
            ret |= HIGH_1_LOW_0_MASK & new_bits;
            return ret;
        }
        void left_shift_by_half_n_times(const unsigned);
        void right_shift_by_half_n_times(const unsigned);

    public:
        big_num(const type_ul u) : num(1, u) {}
        big_num(const big_num &other) : num(other.num) {}
        big_num operator+(const big_num &) const;
        big_num &operator+=(const big_num &other) {
            (*this) = (*this) + other;
            return (*this);
        }
        bool operator<(const big_num &) const;
        bool operator==(const big_num &) const;
        big_num operator*(const big_num &) const;
        friend std::ostream &operator<<(std::ostream &, const big_num &);
};

#endif // BIG_NUM
