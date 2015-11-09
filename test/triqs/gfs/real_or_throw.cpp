#include "./common.hpp"
#include <triqs/gfs/functions/functions.hpp>

TEST(Gf, Base) {

 triqs::clef::placeholder<0> t_;
 double beta = 1;

 auto Gt = gf<imtime>{{beta, Fermion, 100}, {2, 2}};

 Gt(t_) << 8 * t_;
 auto gtr = real_or_throw(Gt);
 EXPECT_CLOSE_ARRAY(gtr.data(), Gt.data());

 Gt(t_) << 8_j * t_;
 EXPECT_THROW(real_or_throw(Gt), std::exception);
}
MAKE_MAIN;
