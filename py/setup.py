from distutils.core import setup

setup(name='GItools',
    version='1.0',
    description='Tools for GI library',
    author='Marco Ortolani, Enrico Casella',
    author_email='marco.ortolani@unipa.it',
    package_data={'GItools': ['gi_gipy.so']},
    include_package_data=True,
    py_modules=['GItools'],
    )
