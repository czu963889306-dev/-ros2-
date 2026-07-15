package_resource_indexer
========================

System for cataloging and referencing resources distributed by software packages.

Conceptual Overview
-------------------

This project provides two main things, the ability for packages to register types of resources they install and the ability to make queries about those resources at runtime.
A resource can represent any kind of asset or functionality provided by a package.
The queries might be anything from "Which packages are installed?" over "Which packages provide plugins of a certain type?" to "What messages and services does a specific package provide?".
This project does not aim to catalog and explicitly reference all individual resources, but rather to provide meta information about what resources are provided by what packages and then providing absolute paths to the prefix containing the package.
These are the design requirements:

- Prevent recursive crawling
  - Resources should be cataloged in away in which no recursive crawling of directories is required
- Autonomous Participation
  - Packages which register resources should be able to do so without invoking some other package (like this one)
- Avoid installed file collisions
  - Participation should not require a package to overwrite or modify an existing file when installing
  - This is useful when packaging for Linux package managers
- Do not try to capture all information
  - Many types of resources already provide mechanism for describing and referencing themselves, do not reinvent this
  - For example, if a system has plugins then it likely already has a mechanism for describing the plugins, this project should not try to capture all of that kind of meta information about individual resources
  - Stick to meta information about what resources are provided not meta information about the installed resources
- Support overlaying
  - If a package is installed into multiple prefixes on the system and those prefixes are ordered, return information based on that ordering
- Do not depend on externally defined environment variables or file formats
  - The `ROS_PACKAGE_PATH` and `CMAKE_PREFIX_PATH` environment variables
  - Parsing `package.xml`'s or `plugin.xml`'s

These requirements come from experience with the resource discovery system in [ROS](https://wiki.ros.org/), where packages were located anywhere recursively under one of several paths in the `ROS_PACKAGE_PATH` environment variable.
This decision has lead to things like `rospack` caching information, which can get out of date, and then lead to subtle bugs for the users.
The catkin build system also requires recursive crawling on the `share` directory for each install prefix in order to find packages.
This can be slow even though the crawling stops when a `package.xml` is discovered, because in cases like `/usr` the contents of `share` can be broad and folders with no `package.xml` can be deep.

Design
------

After some discussion we decided to implement a system which "registers" meta information about the resources installed by packages using a file system.
The benefit of this type of system is two fold.
First, there is no crawling because there is a well known location under each prefix (`/usr`, `/opt/ros/indigo`, etc...) in which to look and the contents of that location are defined and finite.
Second, it can be implemented in a way which does not cause file collisions on installation nor the running or updating of a database at distribution time, which eases the use of this in Linux packaging.
This design has the added benefit of being easy to make use of at run time using any programming language because it does not require parsing of files.

### File System Index Layout

The general concept is that we define a well known location, `<prefix>/share/ament_index/resource_index`, which is reserved and in which all packages can install files.
We'll call this the "resource index".
In this "resource index" is a file system structure which is setup as a set of folders, each of which represent a "resource type".
In each of those "resource type" folders every package which provides a resource of that type can install a file named for the package, called a "marker file".

Let's look at an example.
Each package wants to make the fact that it is installed available to others.
In this case the resource is rather a piece of information than a specific asset or functionality.
The resource type `packages` is being used to indicate that a package with the name of the resource is installed.
The simplest case is that each package (`foo`, `bar`, and `baz`) provides an empty file with its package name within the resource type subfolder.
That would look like this:

```
<prefix>
    `-- share
        `-- ament_index
            `-- resource_index
                `-- packages
                    `-- foo  # empty file
                    `-- bar  # empty file
                    `-- baz  # empty file
```

So now the operations to answer "Which packages are installed?" is just (Python):

```python
import os
return os.listdir(os.path.join(prefix, 'share', 'ament_index', 'resource_index', 'packages'))
```

This can be used to catalog other types of resources with varying degrees of precision.
There is a trade-off between the number of files installed and the number of ways things are categorized, but run time search is unaffected by the number of categories.
For example, we could keep track of which packages have plugins for `rviz`'s display system:

```
<prefix>
    `-- share
        `-- ament_index
            `-- resource_index
                |-- packages
                |   `-- foo
                |   `-- bar
                |   `-- baz
                |-- plugins.rviz.display
                    `-- foo
```

Answering the question "Which packages have plugins for rviz's display system?" is now simply:

```python
import os
return os.listdir(os.path.join(prefix, 'share', 'ament_index', 'resource_index', 'plugins.rviz.display'))
```

In both examples the resource was just an empty file.
But each resource could also store arbitrary content.
This could e.g. be used to store a list of messages and services a package does provide.
Please read below for recommendations on how to store information in each resource / how to use the information within a resource to reference additional external information.

Currently in ROS, this requires that all packages are discovered first and then each manifest file for each package must be parsed (`package.xml` or `manifest.xml`) and then for each package zero to many `plugin.xml` files must be parsed.
For this example system, the `package.xml` file and `plugin.xml` files may still need to be parsed, but instead of discovering all packages, and parsing all `package.xml` files this system can already narrow down the packages which need to be considered.
In the above example that only saves us from parsing two out of three packages, but in the presence of hundreds of packages, this could be parsing one or two package manifests versus parsing hundreds.
It also prevents us from parsing additional, unrelated, `plugin.xml` files, so the win for this type of system with respect to plugin discovery is potentially huge.
For other resources, the speed up at this point is probably minimal, but other examples might include "Which packages have defined message files?" or "Which packages have launch files?".
These types of queries can potentially speed up command line tools considerably.

### Resource Index

Each prefix which contains any packages should contain a resource index folder located at `<prefix>/share/ament_index/resource_index`.
In this context a "prefix" is a FHS compliant file system and typically will be listed in an environment variable as a list of paths, e.g. `ROS_PACKAGE_PATH` or `CMAKE_PREFIX_PATH`, and will contain the system and user "install spaces", e.g. `/usr`, `/usr/local`, `/opt/ros/indigo`, `/home/user/workspace/install`, etc...

Any implementation which allows queries should consider multiple prefixes.
Consider a set of prefixes in this order: `/home/user/workspace/install`, `/opt/ros/indigo`, and `/usr`.
Also consider that the package `foo` is installed into `/home/user/workspace/install` and `/usr`.
Then consider the possible answers to the query "List the location of rviz plugin files." where `foo` provides plugins for `rviz` but no other package does:

```python
{'foo': ['/home/user/workspace/install/share/foo/plugin.xml', '/usr/share/foo/plugin.xml']}  # This is OK

{'foo': ['/usr/share/foo/plugin.xml', '/home/user/workspace/install/share/foo/plugin.xml']}  # This is Bad

['/home/user/workspace/share/foo/plugin.xml']  # This is also OK

['/usr/share/foo/plugin.xml']  # This is bad!
```

Where possible the implementation should give the user the option to get multiple responses to a question if multiple locations are available, but when the user is asking for one answer, then the first matched prefix, according to the prefix path ordering, should be returned.
Note that when returning the multiple results, that they should be organized by package so that it is clear that they are overlaying each other.
Also note that when returning multiple results the prefix based ordering should be preserved.
Other data structures are possible, but in any case make sure that overlaid results are not presented as peers and that prefix based ordering is preserved in all cases.

### Resource Types

Resource types are represented as folders in the resource index and should be shallow, i.e. there should only be marker files within the resource type folders.
This means that there is no nesting of resource types, which keeps the file system flat, and makes answering "What resource types are in this `<prefix>`?" as easy as simply listing the directories in the resource index folder.
Any folders within the resource type folders, and any folders/files starting with a dot (`.`), should be ignored while listing the directories in the resource index folder.
Instead of nesting resource type folders, the convention of using prefixes and suffixes separated by periods should be used.

Resource type names should be agreed on a priori by the packages utilizing them.

The `packages` resource type is reserved and every software package should place a marker file in the `packages` resource type folder.
Additionally, anything with a marker file in the `packages` resource type should have any corresponding FHS compliant folders and files located relatively to that marker file within this prefix.
For example if `<prefix>/share/ament_index/resource_index/packages/foo` exists then architecture independent files, like a CMake config file or a `package.xml`, should be located relatively from that file in the package's `share` folder, i.e. `../../../foo`.

Spaces in the resource type names are allowed, but underscores should be preferred, either way be consistent.

### Marker Files

The contents of these files will remain unspecified, but may be used by other systems which utilize this convention to make them more efficient.

Consider the plugin example, for each marker file discovered, you must parse a `package.xml` file to get the location of one or more `plugin.xml` files and then parse them.
You could conceivably put the locations of the those `plugin.xml` files into the marker files to prevent the need for parsing the `package.xml` at runtime, potentially saving you some time.
The nice thing about this is that if you don't want to parse the marker file, then you can still parse the `package.xml` file and find the `plugin.xml` files that way.
This is a good model to follow, feel free to optimize by placing domain specific information into the marker files, but you should avoid making it required to get the information.

Implementations should consider that spaces are allowed in marker file names, but it would be a good idea to follow the package naming guidelines for catkin packages: http://www.ros.org/reps/rep-0127.html#name

### Integration with Other Systems

You should strive to avoid having other systems depend on this system.
That is to say, rather than describing your plugins in the marker file you place in the resource index, have the existence of that marker file imply the existence of another file in the share folder for your package.
To make that concrete you could do this:

```
<prefix>
    `-- share
        |-- foo
        |   `-- ...  # Other, non-plugin related, stuff
        |-- ament_index
            |-- resource_index
                |-- packages
                |   `-- foo
                |   `-- ...
                |-- plugins.rviz
                |   `-- foo  # Contains XML describing the rviz plugin
                |-- plugins.rqt
                    `-- foo  # Contains XML describing the rqt plugin
```

But in that case if someone just looks at the share folder for `foo` and doesn't have knowledge of this system, then they don't see that you have any plugins.
Instead you should do it like this:

```
<prefix>
    `-- share
        |-- foo
        |   `-- ...  # Other, non-plugin related, stuff
        |   `-- rviz_plugins.xml
        |   `-- rqt_plugins.xml
        |-- ament_index
            |-- resource_index
                |-- packages
                |   `-- foo
                |   `-- ...
                |-- plugins.rviz
                |   `-- foo  # Contains nothing, or the relative path `../../../foo/rviz_plugins.xml`
                |-- plugins.rqt
                    `-- foo  # Contains nothing, or the relative path `../../../foo/rqt_plugins.xml`
```

That way your package has all its required information in its `share` folder and the files in `share/ament_index/resource_index` are simply used as an optimization.

While there are no restrictions about content or format of the marker files, you should try to keep them simple.

Implementation
--------------

The Design description above should be sufficient for anyone to implement a version of this or implement a client to interact with the resource index.
Hopefully by clearly describing the layout of the above system, creating tools to facilitate creation of these files in different build systems should be simple.
Additionally, the simple file system based layout should make it relatively easy to interact with and query the resource index from any programming language.

### Registering with the Resource Index

Registering that your package installs a resource of a particular type should follow these steps:

- Do the `<prefix>/share/ament_index/resource_index/<resource_type>` folders exist?
 - No: Create them.
- Does the `<prefix>/share/ament_index/resource_index/<resource_type>/<package_name>` file exist?
 - Yes: At best, error registration collision, otherwise overwrite (bad behavior, but it may not be possible to detect)
 - No: Create it, with any content you want (keep it simple)

This prescription should be relatively easy to follow for any build system.

It is recommended that the interface provided to the user follow something like this (CMake in this example):

```cmake
# register_package_resource(<package_name> <resource_type> [CONTENT <content>])
register_package_resource(${PROJECT_NAME} "plugin.rviz.display" CONTENT "../../../${PROJECT_NAME}/plugin.xml")
# register_package(<package_name>)
register_package(${PROJECT_NAME})
# register_package(...) is functionally equivalent to:
# register_package_resource(${PROJECT_NAME} "packages")
```

### Querying the Resource Index

Querying the resource index should be relatively simple, only requiring the listing of directories to answer all queries.
Optionally, the marker files can have information in the content, but at most an implementation of this would only need to return the content of this file and at least just return the path to the file.

There are some obvious queries which any implementation should provide.
First consider this function (in Python as a demonstration since it is simple):

```python
def list_prefix_of_packages_by_resource(resource_type, prefixes):
    ...
```

This function should take a resource type name as a string and a list of prefixes as strings.
It should silently pass if any of the prefixes do not exist, or if any of them do not have a resource index within them.
It should return a data structure like this:


```python
{
    '<package name>': '/path/to/first-matched-prefix',
    ...
}
```

Perhaps in C a linked list struct representing a "match" would work better:

```C
typedef struct PackageResource_Match {
    char *package_name;
    char *package_prefix;
    struct PackageResource_Match *next;
} PackageResource_Match;
```

Either way the answer should not only be a list of which packages matched, but the associated prefix in which the package was found so that the user doesn't assume which prefix was matched.

Another useful, but probably not required query is this one:

```python
def list_all_prefixes_of_packages_by_resource(resource_type, prefixes):
    ...
```

This one will return all matching prefixes, not just the first matched one, but preserving the prefix ordering.
Equivalent data types in Python:

```Python
{
    '<package name>': ['/path/to/first-matched-prefix', ...],
    ...
}
```

And the C equivalent:

```C
typedef struct PackageResource_PrefixList {
    char *package_prefix;
    struct PackageResource_PrefixList *next;
} PackageResource_PrefixList;

typedef struct PackageResource_Match {
    char *package_name;
    struct PackageResource_PrefixList *package_prefixes;
    struct PackageResource_Match *next;
} PackageResource_Match;
```

If desired, there can be a syntactic sugar version for listing packages:

```python
def list_prefix_of_packages(prefixes):
    return list_prefix_of_packages_by_resource('packages', prefixes)
```

And:

```python
def list_all_prefixes_of_packages(prefixes):
    return list_all_prefixes_of_packages_by_resource('packages', prefixes)
```

Additionally, functions which find the prefix(es) for a particular package are probably useful:

```python
def get_prefix_for_package(package_name, prefixes):
    ...

def get_all_prefixes_for_package(package_name, prefixes):
    ...
```

These functions should have some error state (raise or throw or return None/NULL) if the package is not found in any of the prefixes.

### Locating Resources

This project does not aim to index all resources for every package, but simply index meta information about what kinds of resources packages have, in order to narrow down searches and prevent recursive crawling.
So, if you wanted to locate a particular file, let's say a particular launch for a given package, then you would need to know where that file is installed to, with respect to the install prefix.
In this case, this project is only useful in finding which prefix to find it in.
So first, you would find which prefix the given package is in by calling the `get_prefix_for_package` function and from there you can append the FHS defined folders like `bin`, `lib`, `share/<package name>`, etc...
Let's say you know that the launch file is in the `share/<package name>` folder because it is not architecture specific and furthermore that it is in the `launch` folder in the `share/<package name>` folder and finally that the name of the launch file is `demo.launch`.
From that information, and the prefix you got from `get_prefix_for_package`, you can construct the path to the launch file.

Let's take another example, you are looking for the location of a shared library for a particular plugin, called `llama_display` of resource type `plugin.rviz.display`, so that you can call `dlopen` on it, but you don't know which package it is (weird case, but instructional):

```python
# First you can narrow down which packages might have the plugin
packages = list_prefix_of_packages_by_resource('plugin.rviz.display', list_of_prefixes)
# Now you can search for the plugin in this considerably shorter list of packages
for package_name, prefix in packages.items():
    package_xml_path = os.path.join(prefix, 'share', package_name, 'package.xml')
    # Use something to parse the package.xml
    package_obj = catkin_pkg.package.parse_package(package_xml_path)
    # Use some other system to get the plugins with the package.xml
    plugins = pluginlib.get_plugins_from_package_manifest(package_obj)
    for plugin in plugins:
        if plugin.name == 'llama_display':
            return plugin.shared_library_path
```

Again, this is a scenario in which this project does not find the plugin for you, but instead makes it more efficient to find the plugin.
