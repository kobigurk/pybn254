# pybn254

Accelerated operations for BN254 using libff and libfqfft. 

Supported operations:

* Multi-exponentiation in G1
* FFT
* Multiply polynomials

## Install for pip

* `pip install pybn254`

## Build

### Install python dependencies

Dependencies:
* py\_ecc

### Compile native dependencies

#### MacOS

* Install [Homebrew](https://brew.sh/)
* Install dependencies - `brew install gmp boost cmake openssl libomp`
* Go into `libff` and run `make all`

This should create `libff.dylib` in the root folder.

#### Ubuntu

* Install dependencies - `sudo apt-get install build-essential git libboost-all-dev cmake libgmp3-dev libssl-dev pkg-config`
* Go into `libff` and run `make all`

This should create `libff.so` in the root folder.

### Test

You can use the following script to test things out:

```python
import pybn254

from py_ecc.optimized_bn128 import add, multiply, pairing
from py_ecc.fields import optimized_bn128_FQ as FQ

p = [(FQ(1), FQ(2), FQ(1))]
e = [5]

p1 = [1, 2, 3]
p2 = [4, 5, 6]

print(pybn254.multiexp(p,e))

# run fft, not an inverse, not on a coset
print(pybn254.fft(p1, False, False))
print(pybn254.multiply_polynomials(p1, p2))
```
