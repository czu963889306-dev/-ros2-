from setuptools import find_packages
from setuptools import setup

package_name = 'tracetools_trace'

setup(
    name=package_name,
    version='4.1.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['setuptools'],
    maintainer=(
        'Christophe Bedard, '
        'Ingo Luetkebohle'
    ),
    maintainer_email=(
        'bedard.christophe@gmail.com, '
        'ingo.luetkebohle@de.bosch.com'
    ),
    author='Christophe Bedard',
    author_email='fixed-term.christophe.bourquebedard@de.bosch.com',
    url='https://gitlab.com/ros-tracing/ros2_tracing',
    keywords=[],
    description='Tools for setting up tracing sessions.',
    long_description=(
        'This package provides tools for setting up tracing sessions. '
        'It also provides basic utilities to provide CLI functionality.'
    ),
    entry_points={
        'console_scripts': [
            f'trace = {package_name}.trace:main',
        ],
    },
    license='Apache 2.0',
    tests_require=['pytest'],
)
