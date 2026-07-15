#!/usr/bin/env python

from setuptools import setup

package_name = 'domain_coordinator'

setup(
    name=package_name,
    version='0.10.0',
    packages=[
        'domain_coordinator',
    ],
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    author='Pete Baughman',
    author_email='pete.baughman@apex.ai',
    maintainer='Michel Hidalgo',
    maintainer_email='michel@ekumenlabs.com',
    url='https://github.com/ros2/ament_cmake_ros',
    download_url='https://github.com/ros2/ament_cmake_ros/releases',
    keywords=['ROS'],
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description='A tool to coordinate unique ROS_DOMAIN_IDs across multiple processes',
    long_description=(
        'This package provides the functionality used by ament_cmake_pytest_isolated, '
        'ament_cmake_gtest_isolated, and launch_testing to select unique ROS_DOMAIN_IDs '
        'that allow tests to run in parallel without interfering with each other.'
    ),
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
)
