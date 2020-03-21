#include <vector>
#include <iostream>

#include <libff/algebra/curves/alt_bn128/alt_bn128_init.hpp>
#include <libff/algebra/curves/alt_bn128/alt_bn128_g1.hpp>
#include <libff/algebra/scalar_multiplication/multiexp.hpp>
#include <libff/common/utils.hpp>
#include <libfqfft/evaluation_domain/get_evaluation_domain.hpp>

using namespace libff;
using namespace std;

vector<alt_bn128_G1> gs;
vector<alt_bn128_Fr> rs;

extern "C" void random_elements(int num_points) {
  init_alt_bn128_params();
  for (int i = 0; i< num_points; i++) {
    alt_bn128_G1 g = alt_bn128_G1::random_element();
    gs.push_back(g);
    alt_bn128_Fr r = alt_bn128_Fr::random_element();
    rs.push_back(r);
  }
}

extern "C" void multiexp_random() {
	const size_t chunks = 8;
  alt_bn128_G1 evaluation = libff::multi_exp_with_mixed_addition<alt_bn128_G1, alt_bn128_Fr, libff::multi_exp_method_BDLO12>(gs.begin(), gs.end(), rs.begin(), rs.end(), chunks);
}

template<typename T>
void fp_to_str(T src, char* str) {
	mpz_t mpz_x;
	mpz_init(mpz_x);
	src.as_bigint().to_mpz(mpz_x);

  mpz_export(str, NULL, -1, 1, -1, 0, mpz_x);
}

extern "C" void multiexp(char** exponents, int exponents_length, char** points_x, char** points_y, char** points_z, int points_length, char* res_x, char* res_y, char* res_z) {
  init_alt_bn128_params();
  std::cout << "multiexp for " << exponents_length << " exponents and " << points_length << " points" << std::endl;
	vector<alt_bn128_Fr> es;
	for (int i = 0; i < exponents_length; i++) {
    mpz_t z;
    mpz_init(z);
    mpz_import(z, 32, -1, 1, -1, 0, exponents[i]);

		bigint<alt_bn128_r_limbs> e(z);
		alt_bn128_Fr f(e);
		es.push_back(f);
	}
	vector<alt_bn128_G1> ps;
	for (int i = 0; i < points_length; i++) {
    mpz_t xx;
    mpz_init(xx);
    mpz_import(xx, 32, -1, 1, -1, 0, points_x[i]);

    mpz_t yy;
    mpz_init(yy);
    mpz_import(yy, 32, -1, 1, -1, 0, points_y[i]);

    mpz_t zz;
    mpz_init(zz);
    mpz_import(zz, 32, -1, 1, -1, 0, points_z[i]);

		alt_bn128_Fq x = alt_bn128_Fq(bigint<alt_bn128_q_limbs>(xx));
		alt_bn128_Fq y = alt_bn128_Fq(bigint<alt_bn128_q_limbs>(yy));
		alt_bn128_Fq z = alt_bn128_Fq(bigint<alt_bn128_q_limbs>(zz));
		ps.push_back(alt_bn128_G1(
				x*z,
				y*z*z,
				z
			)
		);
	}

	const size_t chunks = 8;
  alt_bn128_G1 evaluation = libff::multi_exp_with_mixed_addition<alt_bn128_G1, alt_bn128_Fr, libff::multi_exp_method_BDLO12>(ps.begin(), ps.end(), es.begin(), es.end(), chunks);

	fp_to_str(evaluation.X*evaluation.Z, res_x);
	fp_to_str(evaluation.Y, res_y);
	fp_to_str(evaluation.Z*evaluation.Z*evaluation.Z, res_z);
}

extern "C" void fft(char** elements, int elements_length, char* res, int domain_size, bool inverse, bool coset) {
  init_alt_bn128_params();
  std::cout << "fft for " << elements_length << " elements and domain size " << domain_size << " and (inverse, coset) = (" << inverse << ", " << coset << ")" << std::endl;
  const std::shared_ptr<libfqfft::evaluation_domain<alt_bn128_Fr> > domain = libfqfft::get_evaluation_domain<alt_bn128_Fr>(domain_size);

	vector<alt_bn128_Fr> es;
	for (int i = 0; i < elements_length; i++) {
    mpz_t z;
    mpz_init(z);
    mpz_import(z, 32, -1, 1, -1, 0, elements[i]);

		bigint<alt_bn128_r_limbs> e(z);
		alt_bn128_Fr f(e);
		es.push_back(f);
	}
  if (inverse && coset) {
    domain->icosetFFT(es, alt_bn128_Fr::multiplicative_generator);
  } else if (!inverse && coset) {
    domain->cosetFFT(es, alt_bn128_Fr::multiplicative_generator);
  } else if (inverse && !coset) {
    domain->iFFT(es);
  } else {
    domain->FFT(es);
  }
	for (int i = 0; i < elements_length; i++) {
    fp_to_str(es[i], res + 32*i);
  }
}

extern "C" void root_of_unity(int domain_size, bool coset, char* res) {
  init_alt_bn128_params();
   const std::shared_ptr<libfqfft::evaluation_domain<alt_bn128_Fr> > domain = libfqfft::get_evaluation_domain<alt_bn128_Fr>(domain_size);

  alt_bn128_Fr element;
  if (coset) {
    element = domain->get_domain_element(1) * alt_bn128_Fr::multiplicative_generator;
  } else {
    element = domain->get_domain_element(1);
  }
  fp_to_str(element, res);
}
