from setuptools import find_packages
from setuptools import setup

package_name = 'ament_copyright'

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
        'template/*',
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
    description='Check source files for copyright reference.',
    long_description="""\
The ability to check sources file for copyright and license information.""",
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
    entry_points={
        'ament_copyright.copyright_name': [
            'osrf = ament_copyright.copyright_names:osrf',
        ],
        'ament_copyright.license': [
            'apache2 = ament_copyright.licenses:apache2',
            'boost1 = ament_copyright.licenses:boost1',
            'bsd2 = ament_copyright.licenses:bsd2',
            'bsd_3clause = ament_copyright.licenses:bsd_3clause',
            'bsd_2clause = ament_copyright.licenses:bsd_2clause',
            'mit = ament_copyright.licenses:mit',
            'mit0 = ament_copyright.licenses:mit0',
            'gplv3 = ament_copyright.licenses:gplv3',
            'lgplv3 = ament_copyright.licenses:lgplv3',
        ],
        'console_scripts': [
            'ament_copyright = ament_copyright.main:main',
        ],
        'pytest11': [
            'ament_copyright = ament_copyright.pytest_marker',
        ],
    },
)
