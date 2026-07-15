from setuptools import find_packages
from setuptools import setup

package_name = 'ros2trace'

setup(
    name=package_name,
    version='4.1.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['ros2cli'],
    zip_safe=True,
    maintainer=(
        'Christophe Bedard, '
        'Ingo Lütkebohle'
    ),
    maintainer_email=(
        'bedard.christophe@gmail.com, '
        'ingo.luetkebohle@de.bosch.com'
    ),
    author='Christophe Bedard',
    author_email='fixed-term.christophe.bourquebedard@de.bosch.com',
    url='https://gitlab.com/ros-tracing/ros2_tracing',
    keywords=[],
    description='The trace command for ROS 2 command line tools.',
    long_description=(
        'The package provides the trace command '
        'for the ROS 2 command line tools.'
    ),
    license='Apache 2.0',
    tests_require=['pytest'],
    entry_points={
        'ros2cli.command': [
            f'trace = {package_name}.command.trace:TraceCommand',
        ],
    }
)
