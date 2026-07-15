from setuptools import find_packages
from setuptools import setup

setup(
    name='rosidl_cli',
    version='3.1.9',
    packages=find_packages(exclude=['test']),
    extras_require={
        'completion': ['argcomplete'],
        'test': [
            'pytest',
        ],
    },
    data_files=[
        ('share/ament_index/resource_index/packages', [
            'resource/rosidl_cli',
        ]),
        ('share/rosidl_cli', [
            'package.xml',
            'resource/package.dsv',
        ]),
        ('share/rosidl_cli/environment', [
            'completion/rosidl-argcomplete.bash',
            'completion/rosidl-argcomplete.zsh'
        ]),
    ],
    zip_safe=False,
    author='Michel Hidalgo',
    author_email='michel@ekumenlabs.com',
    maintainer='Michel Hidalgo, Shane Loretz',
    maintainer_email='michel@ekumenlabs.com, sloretz@openrobotics.org',
    url='https://github.com/ros2/rosidl/tree/master/rosidl_cli',
    download_url='https://github.com/ros2/rosidl/releases',
    keywords=[],
    classifiers=[
        'Environment :: Console',
        'Intended Audience :: Developers',
        'Programming Language :: Python',
    ],
    description='Command line tools for ROS interface generation.',
    long_description="""\
The tooling provides a single command line script for ROS interface source code generation.""",
    license='Apache License, Version 2.0',
    entry_points={
        'console_scripts': [
            'rosidl = rosidl_cli.cli:main',
        ],
    }
)
