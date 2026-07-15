from setuptools import find_packages
from setuptools import setup

package_name = 'ament_package'

setup(
    name=package_name,
    version='0.14.2',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/' + package_name, ['package.xml']),
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
    ],
    zip_safe=True,
    author='Dirk Thomas',
    author_email='dthomas@osrfoundation.org',
    maintainer='Audrow Nash',
    maintainer_email='audrow@openrobotics.org',
    url='https://github.com/ament/ament_package/wiki',
    download_url='https://github.com/ament/ament_package/releases',
    keywords=['ROS'],
    classifiers=[
        'Intended Audience :: Developers',
        'Programming Language :: Python',
        'Topic :: Software Development',
    ],
    description='Ament is a build system for federated packages.',
    long_description="""\
Ament defines metainformation for packages, their dependencies,
and provides tooling to build these federated packages together.""",
    license='Apache License, Version 2.0',
    extras_require={
        'test': [
            'pytest', 'flake8',
        ],
    },
    package_data={
        'ament_package': [
            'template/environment_hook/*',
            'template/isolated_prefix_level/*',
            'template/package_level/*',
            'template/prefix_level/*',
        ],
    },
)
