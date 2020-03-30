from setuptools import setup, Distribution

from os import path
this_directory = path.abspath(path.dirname(__file__))
with open(path.join(this_directory, 'README.md'), encoding='utf-8') as f:
    long_description = f.read()

class BinaryDistribution(Distribution):
    def has_ext_modules(foo):
        return True


setup(
    name='pybn254',
    version='1.0.5',
    description='Accelerated operations for BN254',
    packages=['pybn254'],
    package_data={
        'pybn254': ['libff.dylib', 'libff.so'],
    },
    install_requires=['py_ecc'],
    distclass=BinaryDistribution,
    long_description=long_description,
    long_description_content_type='text/markdown'
)
