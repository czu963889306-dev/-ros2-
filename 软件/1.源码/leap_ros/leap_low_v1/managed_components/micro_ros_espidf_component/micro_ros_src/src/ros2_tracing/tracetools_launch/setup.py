import glob

from setuptools import find_packages
from setuptools import setup

package_name = 'tracetools_launch'

setup(
    name=package_name,
    version='4.1.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/launch', glob.glob('launch/*.launch.*')),
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
    description='Launch integration for tracing.',
    long_description=(
        'This package provides a trace action to '
        'launch tracing through a launch file.'
    ),
    license='Apache 2.0',
    tests_require=['pytest'],
    entry_points={
        'launch.frontend.launch_extension': [
            'tracetools_launch = tracetools_launch',
        ],
    },
)
