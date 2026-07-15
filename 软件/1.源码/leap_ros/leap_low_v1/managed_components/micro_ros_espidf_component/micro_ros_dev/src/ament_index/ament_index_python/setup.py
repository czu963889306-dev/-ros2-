from setuptools import find_packages
from setuptools import setup

package_name = 'ament_index_python'

setup(
    name=package_name,
    version='1.4.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + '/environment', [
            'completion/ament_index-argcomplete.bash',
            'completion/ament_index-argcomplete.zsh'
        ]),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    author='Dirk Thomas',
    author_email='dthomas@osrfoundation.org',
    maintainer='Audrow Nash',
    maintainer_email='audrow@openrobotics.org',
    url='https://github.com/ament/ament_index',
    download_url='https://github.com/ament/ament_index/releases',
    keywords=['ROS'],
    classifiers=[
        'Intended Audience :: Developers',
        'License :: OSI Approved :: Apache Software License',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description='Python API to access the ament resource index.',
    long_description="""\
A Python API to find resources based on their type in the ament resource index
and get the content of individual resources.""",
    license='Apache License, Version 2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'ament_index = ament_index_python.cli:main',
        ],
    },
)
