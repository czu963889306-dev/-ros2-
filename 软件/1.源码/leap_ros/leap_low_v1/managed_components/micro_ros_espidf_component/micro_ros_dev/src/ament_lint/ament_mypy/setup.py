from setuptools import find_packages
from setuptools import setup

package_name = 'ament_mypy'

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
        'configuration/ament_mypy.ini',
    ]},
    zip_safe=False,
    author='Ted Kern',
    author_email='ted.kern@canonical.com',
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
    description='Check Python static typing using mypy.',
    long_description="""\
The ability to check code for user specified static typing with mypy.""",
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ament_mypy = ament_mypy.main:main',
        ],
        'pytest11': [
            'ament_mypy = ament_mypy.pytest_marker',
        ],
    },
)
