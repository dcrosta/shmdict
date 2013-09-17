#!/usr/bin/env python
from setuptools import setup, find_packages
from distutils.core import Extension

# work around a bug in nose setuptools integration
from multiprocessing import util

setup(
    name='shmdict',
    version='0.1.0',
    description='A dict-like interface to shared memory',
    author='Dan Crosta',
    author_email='dcrosta@late.am',
    url='https://github.com/dcrosta/shmdict',
    packages=find_packages(),
    ext_modules=[
        Extension('_shmdict', sources=['_shmdict.c']),
    ],
)


