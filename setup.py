#!/bin/env python
import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
    name="pyvtd",
    version="0.1.0",
    author='Philipp Rothenhäusler'
    author_email='philipp.rothenhaeusler@gmail.com'
    description="A Python interface to VTD.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/prothen/pyvtd",
    packages=setuptools.find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: Other/Proprietary License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.5',
)
