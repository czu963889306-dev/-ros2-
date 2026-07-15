from setuptools import find_packages
from setuptools import setup

package_name = 'ament_clang_format'

setup(
    name=package_name,
    version='0.12.15',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['setuptools', 'pyyaml'],
    package_data={'': [
        'configuration/.clang-format',
    ]},
    zip_safe=False,
    author='Dirk Thomas',
    author_email='dthomas@osrfoundation.org',
    maintainer='Michael Jeronimo, Michel Hidalgo',
    maintainer_email='michael.jeronimo@openrobotics.org, michel@ekumenlabs.com',
    url='https://github.com/ament/ament_lint',
    download_url='https://github.com/ament/ament_lint/releases',
    keywords=['ROS'],
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description='Check C++ code style using clang-format.',
    long_description="""\
The ability to check code against style conventions using clang-format
and generate xUnit test result files.""",
    license='Apache License, Version 2.0, BSD',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ament_clang_format = ament_clang_format.main:main',
        ],
    },
)
