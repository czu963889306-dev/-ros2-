from setuptools import find_packages
from setuptools import setup

package_name = 'ament_flake8'

setup(
    name=package_name,
    version='0.12.15',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['setuptools'],
    package_data={'': [
        'configuration/ament_flake8.ini',
    ]},
    zip_safe=False,
    author='D. Hood',
    author_email='dhood@osrfoundation.org',
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
    description='Check Python code style using flake8.',
    long_description="""\
The ability to check code for syntax and style conventions with flake8.""",
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ament_flake8 = ament_flake8.main:main',
        ],
        'pytest11': [
            'ament_flake8 = ament_flake8.pytest_marker',
        ],
    },
)
