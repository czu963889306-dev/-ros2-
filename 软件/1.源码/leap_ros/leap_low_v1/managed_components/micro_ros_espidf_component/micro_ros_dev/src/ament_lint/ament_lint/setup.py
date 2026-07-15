from setuptools import find_packages
from setuptools import setup

package_name = 'ament_lint'

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
    zip_safe=True,
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
    description='Providing common API for ament linter packages.',
    long_description="""\
Providing common API for ament linter packages, e.g. the `linter` marker for
pytest.""",
    license='Apache License, Version 2.0',
    entry_points={
        'pytest11': [
            'ament_lint = ament_lint.pytest_marker',
        ],
    },
)
