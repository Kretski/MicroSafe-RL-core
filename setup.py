from setuptools import setup, find_packages

setup(
    name="microsafe-rl",
    version="0.1.0",
    packages=find_packages(),
    install_requires=[
        "numpy",
        "pandas",
        "gymnasium", # или gym
    ],
    author="Dimitar Kretski",
    description="Sub-microsecond adaptive safety layer for Edge AI",
)